#!/bin/sh
set -eu
PACK=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
stamp=$(TZ=Asia/Shanghai date '+%Y%m%d-%H%M%S')
OUT="$PACK/Reports/$stamp"
mkdir -p "$OUT"
/usr/sbin/sysctl kern.boottime kern.osrelease > "$OUT/启动时间.txt" 2>&1 || true
/usr/bin/kmutil showloaded --list-only > "$OUT/已加载驱动.txt" 2> "$OUT/驱动读取错误.txt" || true
/usr/sbin/ioreg -r -c IONVMeController -l -w0 > "$OUT/NVMe状态.txt" 2>&1 || true
/usr/sbin/ioreg -p IOPower -l -w0 > "$OUT/电源状态.txt" 2>&1 || true
/usr/sbin/ioreg -r -c AMDRyzenCPUPowerManagement -l -w0 > "$OUT/AMD监控服务.txt" 2>&1 || true
/usr/sbin/ioreg -r -c SMCAMDProcessor -l -w0 > "$OUT/SMC监控服务.txt" 2>&1 || true
set +e
"$PACK/../GLDualQuadFix/gl-inspect" > "$OUT/OpenGL机器码.txt" 2>&1
glrc=$?
set -e
printf '%s\n' "$glrc" > "$OUT/OpenGL返回码.txt"
printf '%s\n' '相关驱动：'
/usr/bin/grep -E 'Lilu|VirtualSMC|NootedRed|GLDualQuadFix|NVMeFix|AMDRyzen|SMCAMD' "$OUT/已加载驱动.txt" || true
if /usr/bin/grep -q 'org.acidanthera.NVMeFix' "$OUT/已加载驱动.txt"; then
  printf '%s\n' 'NVMeFix 已加载。APST、待机功耗及睡眠效果还需结合状态和实际使用核对。'
else
  printf '%s\n' '未观察到 NVMeFix；若刚安装，请先正常重启。'
fi
if /usr/bin/grep -q 'wtf.spinach.AMDRyzenCPUPowerManagement' "$OUT/已加载驱动.txt" && /usr/bin/grep -q 'wtf.spinach.SMCAMDProcessor' "$OUT/已加载驱动.txt"; then
  printf '%s\n' '两个 AMD 驱动均在加载列表；还需核对服务、监控读数和实际稳定性。'
else
  printf '%s\n' '尚未观察到完整的 AMD 驱动组合；若刚安装，请正常重启。'
fi
cat "$OUT/OpenGL机器码.txt"
printf '\n报告已保存：%s\n' "$OUT"
