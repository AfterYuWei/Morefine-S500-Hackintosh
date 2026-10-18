#!/usr/bin/env python3
"""Read-only current-build check. Reports stay local; no GPU submissions."""
from pathlib import Path
from datetime import datetime
import json,re,subprocess,sys
ROOT=Path(__file__).resolve().parent;META=json.loads((ROOT/'clean-build.json').read_text())
out=ROOT/'Reports'/datetime.now().strftime('%Y%m%d-%H%M%S');out.mkdir(parents=True)
def run(name,args,pattern=None):
    p=subprocess.run(args,capture_output=True,text=True,timeout=45);s=p.stdout
    if pattern:s='\n'.join(x for x in s.splitlines() if re.search(pattern,x,re.I))+'\n'
    (out/(name+'.txt')).write_text(s);return p.returncode,s
krc,kexts=run('graphics-kexts',['/usr/bin/kmutil','showloaded','--list-only'],r'NootedRed|GLDualQuadFix|Lilu')
brc,args=run('boot-args',['/usr/sbin/sysctl','kern.bootargs'])
mrc,m=run('metal',[str(ROOT/'metal-global-inspect')]);grc,g=run('gl',[str(ROOT/'gl-inspect')])
core=any('org.ChefKiss.NootedRed' in x and META['NootedRed_clean_uuid'] in x for x in kexts.splitlines())
global_loaded=any('local.xian.GLDualQuadFix (0.2.0)' in x and META['GLDualQuadFix_uuid'] in x for x in kexts.splitlines())
passed=core and global_loaded and not any((krc,brc,mrc,grc))
status='CLEAN_CORE_AND_GLOBAL_PATCHED' if passed else 'GLOBAL_PATCHED_CLEAN_CORE_PENDING_REBOOT' if global_loaded and not mrc and not grc else 'NOT_CONFIRMED'
summary={'status':status,'clean_core_loaded':core,'global_module_loaded':global_loaded,'Metal':m.strip(),'GL':g.strip(),'application_stability':'Requires actual use; machine-code inspection alone does not verify rendering stability.'}
(out/'result.json').write_text(json.dumps(summary,ensure_ascii=False,indent=2))
print(m+g);print('清理版核心与全局修补均已加载。' if passed else '清理版尚未完整确认；刚部署未重启时属于预期状态。')
print('报告：'+str(out));sys.exit(0 if passed else 10)
