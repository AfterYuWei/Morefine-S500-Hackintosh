"""Prepare an exact-version GL driver candidate; never modifies system files."""
from pathlib import Path
import hashlib
import json
import struct
import subprocess
import sys
import os

ROOT = Path(__file__).resolve().parent
CLANG = Path(os.environ.get('GLFIX_CLANG') or subprocess.check_output(['xcrun','--find','clang'],text=True).strip())
DRIVER = Path('/System/Library/Extensions/AMDRadeonX5000GLDriver.bundle/Contents/MacOS/AMDRadeonX5000GLDriver')
data = DRIVER.read_bytes()
if hashlib.sha256(data).hexdigest() != 'ce205f063627a4b82ee8ccee04ab7963a3efb8dec488cb3c53ed6c6719434f80':
    raise ValueError('Unverified Apple GL driver version; stopped')
BASE = Path(os.environ.get('GLFIX_DEPS', str(ROOT.parent/'Dependencies/source'))).resolve()
sys.path.insert(0, str(BASE/'NootedRed/tools'))
from metal_cache import macho
_, sections, _ = macho(lambda a, n: data[a:a+n], 0)
text = next(s for s in sections if s[0] == '__text')
start, end = 0x819e, 0x822c
original = data[text[3]+start-text[1]:text[3]+end-text[1]]
assert len(original) == 142
assert hashlib.sha256(original).hexdigest() == '2e6f3173c900c04b4f88bcdab671c64cbb270a8eb2a4f75661acb8467dab6b6a'
assert original[:6] == bytes.fromhex('554889e55350')
assert original[-7:] == bytes.fromhex('4883c4085b5dc3')
subprocess.run([str(CLANG), '-arch', 'x86_64', '-c', str(ROOT/'gl_blend_replacement.s'),
                '-o', str(ROOT/'gl_blend_replacement.o')], check=True)
obj = (ROOT/'gl_blend_replacement.o').read_bytes()
_, sect, _ = macho(lambda a,n: obj[a:a+n], 0)
s = next(s for s in sect if s[0] == '__text')
replacement = obj[s[3]:s[3]+s[2]]
if len(replacement)>len(original):
    raise ValueError(f'Replacement {len(replacement)} > original {len(original)}')
replacement += b'\x90'*(len(original)-len(replacement))
assert len(replacement)==len(original)
orig_call = original.index(bytes.fromhex('e8bc730500'))
call = next(i for i in range(len(replacement)-4)
            if replacement[i]==0xe8 and start+i+5+struct.unpack_from('<i',replacement,i+1)[0]==0x5f5ae)
(ROOT/'gl-original.bin').write_bytes(original)
(ROOT/'gl-replacement.bin').write_bytes(replacement)
def array(name, b):
    return f'static const unsigned char {name}[] = {{\n'+',\n'.join(
        '    '+', '.join(f'0x{x:02x}' for x in b[i:i+16]) for i in range(0,len(b),16))+'\n};\n'
(ROOT/'gl_blend_bytes.h').write_text(array('Original',original)+array('Replacement',replacement)+
    f'static const unsigned OriginalCallOffset = {orig_call};\nstatic const unsigned ReplacementCallOffset = {call};\n')
page_offset=(text[3]+start-text[1])&~4095
page=data[page_offset:page_offset+4096]
assert len(page)==4096 and page_offset==0x8000 and start-page_offset==0x19e
assert page[0x19e:0x19e+142]==original
(ROOT/'gl_page_data.hpp').write_text('#pragma once\nnamespace GLPageData {\n'+
    'static constexpr const char* Path="'+str(DRIVER)+'";\n'+
    'static constexpr unsigned long long FilePageOffset=0x8000;\n'+
    'static constexpr unsigned FunctionInPage=0x19e;\n'+
    array('Original',original)+array('Replacement',replacement)+array('Page',page)+'}\n')
result={'system_driver':str(DRIVER),'driver_sha256':hashlib.sha256(data).hexdigest(),
        'function':'_glrAMD_GFX9_UpdateHwCbDualQuadMode','rva':hex(start),'size':len(original),
        'original_sha256':hashlib.sha256(original).hexdigest(),
        'replacement_sha256':hashlib.sha256(replacement).hexdigest(),
        'page_sha256':hashlib.sha256(page).hexdigest(),
        'original_call_offset':orig_call,'replacement_call_offset':call,'submit_target_rva':'0x5f5ae',
        'scope':'hardware PCI device 1002:15e7 only; other device IDs retain native expression',
        'intent':'force CB_COLOR_CONTROL.DISABLE_DUAL_QUAD for this APU, preserve native command emission and flush',
        'status':'offline candidate, no GPU validation or installation'}
(ROOT/'candidate-code-audit.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(result,indent=2))
