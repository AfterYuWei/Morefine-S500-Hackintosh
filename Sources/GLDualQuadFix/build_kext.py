"""Build the standalone GL repair candidate; no EFI or system writes."""
from pathlib import Path
import subprocess,json,plistlib,hashlib,shutil,os
ROOT=Path(__file__).resolve().parent
BASE=Path(os.environ.get('GLFIX_DEPS', str(ROOT.parent/'Dependencies/source'))).resolve()
CLANG=Path(os.environ.get('GLFIX_CLANG') or subprocess.check_output(['xcrun','--find','clang'],text=True).strip())
SDK=Path(os.environ.get('GLFIX_SDK') or subprocess.check_output(['xcrun','--show-sdk-path'],text=True).strip())
OUT=Path(os.environ.get('GLFIX_BUILD', str(ROOT/'build'))).resolve();OUT.mkdir(parents=True,exist_ok=True)
PACK=OUT/'package'
CONTENTS=PACK/'Kexts/GLDualQuadFix.kext/Contents'
(CONTENTS/'MacOS').mkdir(parents=True,exist_ok=True)
(PACK/'Validation').mkdir(parents=True,exist_ok=True)
common=[str(CLANG),'-arch','x86_64','-mkernel','-fno-common','-fstack-protector-strong',
    '-mno-red-zone','-fno-builtin','-fno-asynchronous-unwind-tables','-mmacosx-version-min=10.14',
    '-O3','-Wall','-Wextra','-Werror','-Wno-unused-parameter','-Wno-c23-extensions',
    '-isysroot',str(SDK),'-DKERNEL=1','-DKERNEL_PRIVATE=1',
    '-DPRODUCT_NAME=GLDualQuadFix','-DMODULE_VERSION=0.2.0',
    '-I',str(BASE/'Lilu/Lilu'),'-I',str(BASE/'MacKernelSDK/Headers'),'-I',str(ROOT)]
cpp=['-std=c++23','-fapple-kext','-fno-rtti','-fno-exceptions','-fvisibility=hidden','-fvisibility-inlines-hidden']
results=[]
def run(command):
    r=subprocess.run(command,capture_output=True,text=True)
    results.append({'command':command,'returncode':r.returncode,'diagnostics':r.stdout+r.stderr})
    (PACK/'Validation/build.json').write_text(json.dumps(results,indent=2))
    if r.returncode:raise RuntimeError(r.stdout+r.stderr)
for source,name in [(ROOT/'GLDualQuadFix.cpp','GLDualQuadFix'),(BASE/'Lilu/Lilu/Library/plugin_start.cpp','plugin_start')]:
    run(common+cpp+['-c',str(source),'-o',str(OUT/(name+'.o'))])
metadata=OUT/'vers.c'
metadata.write_text('''#include <mach/mach_types.h>
#include <mach/kmod.h>
extern kern_return_t _start(kmod_info_t*,void*);
extern kern_return_t _stop(kmod_info_t*,void*);
extern kern_return_t GLDualQuadFix_kern_start(kmod_info_t*,void*);
extern kern_return_t GLDualQuadFix_kern_stop(kmod_info_t*,void*);
KMOD_EXPLICIT_DECL(local.xian.GLDualQuadFix,"0.2.0",_start,_stop)
__attribute__((visibility("hidden"))) kmod_start_func_t *_realmain=GLDualQuadFix_kern_start;
__attribute__((visibility("hidden"))) kmod_stop_func_t *_antimain=GLDualQuadFix_kern_stop;
''')
run(common+['-c',str(metadata),'-o',str(OUT/'vers.o')])
binary=CONTENTS/'MacOS/GLDualQuadFix'
run([str(CLANG),'-arch','x86_64','-mkernel','-nostdlib','-mmacosx-version-min=10.14',
    '-Wl,-kext','-Wl,-static',str(OUT/'GLDualQuadFix.o'),str(OUT/'plugin_start.o'),str(OUT/'vers.o'),
    '-L',str(BASE/'MacKernelSDK/Library/x86_64'),'-lkmod','-o',str(binary)])
run(common+cpp+['--analyze',str(ROOT/'GLDualQuadFix.cpp'),'-o',str(OUT/'analysis.plist')])
findings=plistlib.loads((OUT/'analysis.plist').read_bytes()).get('diagnostics',[])
if findings:raise RuntimeError(f'Static analyzer reported {len(findings)} findings')
info={'CFBundleDevelopmentRegion':'English','CFBundleExecutable':'GLDualQuadFix','CFBundleIdentifier':'local.xian.GLDualQuadFix',
 'CFBundleInfoDictionaryVersion':'6.0','CFBundleName':'GLDualQuadFix','CFBundlePackageType':'KEXT',
 'CFBundleShortVersionString':'0.2.0','CFBundleVersion':'0.2.0','OSBundleCompatibleVersion':'0.2.0',
 'OSBundleRequired':'Root','IOKitPersonalities':{'local.xian.GLDualQuadFix':{
 'CFBundleIdentifier':'local.xian.GLDualQuadFix','IOClass':'GLDualQuadFix','IOMatchCategory':'GLDualQuadFix',
 'IOProviderClass':'IOResources','IOResourceMatch':'IOKit','XianCandidateRevision':3,'XianMetalGlobalRevision':1,'XianApplicationFilter':'none'}},
 'OSBundleLibraries':{'com.apple.iokit.IOPCIFamily':'2.0.0','as.vit9696.Lilu':'1.7.2','com.apple.kpi.bsd':'12.0.0',
 'com.apple.kpi.iokit':'12.0.0','com.apple.kpi.libkern':'12.0.0','com.apple.kpi.mach':'12.0.0','com.apple.kpi.unsupported':'12.0.0'}}
(CONTENTS/'Info.plist').write_bytes(plistlib.dumps(info))
run(['/usr/bin/codesign','--force','--sign','-','--timestamp=none',str(CONTENTS.parent)])
run(['/usr/bin/codesign','--verify','--deep','--strict','--verbose=2',str(CONTENTS.parent)])
summary={'binary_sha256':hashlib.sha256(binary.read_bytes()).hexdigest(),'static_analysis_findings':len(findings),
 'signature':'ad-hoc verified','status':'offline candidate only; not installed or GPU-validated'}
(PACK/'Validation/build-summary.json').write_text(json.dumps(summary,indent=2)+'\n')
print(json.dumps(summary,indent=2))
