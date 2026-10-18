#!/usr/bin/env python3
"""Generate exact pages using only active payloads; read system files without writes."""
from pathlib import Path
import hashlib,json
ROOT=Path(__file__).resolve().parent
meta=json.loads((ROOT/'active-payloads.json').read_text())
cache=Path('/System/Volumes/Preboot/Cryptexes/OS/System/Library/dyld/dyld_shared_cache_x86_64h')
with cache.open('rb') as f:
    f.seek(88);assert f.read(16).hex()==meta['cache_uuid'],'Unknown shared cache; refuse generation'
def fmt(name,data):
    lines=[','.join(f'0x{x:02x}' for x in data[i:i+16]) for i in range(0,len(data),16)]
    return 'inline constexpr unsigned char '+name+'[] = {\n'+',\n'.join(lines)+'\n};\n'
header='// Generated from native signed system pages; never hand-edit.\n#pragma once\nnamespace GlobalPageData {\n'
header+='inline constexpr unsigned PageSize=4096;\nstruct Entry { const char* const* paths; unsigned pathCount; unsigned long long fileOffset; const unsigned char* original; const unsigned char* patched; bool metal; unsigned long long sharedOffset; };\n'
for i,row in enumerate(meta['pages']):
    with Path(row['paths'][0]).open('rb') as f:f.seek(row['file_offset']);data=f.read(4096)
    assert len(data)==4096
    h=hashlib.sha256(data).hexdigest()
    assert h in (row['original_sha256'],row['patched_sha256']),'Unknown page bytes'
    original=bytearray(data)
    if h==row['patched_sha256']:
        for e in row['edits']:
            o=e['in_page'];a=bytes.fromhex(e['original']);b=bytes.fromhex(e['patched'])
            assert original[o:o+len(b)]==b;original[o:o+len(a)]=a
    assert hashlib.sha256(original).hexdigest()==row['original_sha256']
    patched=bytearray(original)
    for e in row['edits']:
        o=e['in_page'];a=bytes.fromhex(e['original']);b=bytes.fromhex(e['patched'])
        assert len(a)==len(b) and original[o:o+len(a)]==a;patched[o:o+len(b)]=b
    assert hashlib.sha256(patched).hexdigest()==row['patched_sha256']
    header+=fmt(f'Original{i}',original)+fmt(f'Patched{i}',patched)
    header+=f'inline constexpr const char* Paths{i}[] = {{'+','.join(json.dumps(x) for x in row['paths'])+'};\n'
header+='inline constexpr Entry Entries[] = {\n'
for i,row in enumerate(meta['pages']):
    header+=f'{{Paths{i},{len(row["paths"])},0x{row["file_offset"]:x}ULL,Original{i},Patched{i},{str(row["kind"]=="Metal").lower()},0x{row["shared_offset"]:x}ULL}},\n'
header+='};\ninline constexpr unsigned Count=sizeof(Entries)/sizeof(Entries[0]);\n}\n'
(ROOT/'global_page_data.hpp').write_text(header)
print('Generated only the active GL and two Metal pages; exact original/replacement hashes verified.')
