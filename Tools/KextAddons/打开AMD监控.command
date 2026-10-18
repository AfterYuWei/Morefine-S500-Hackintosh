#!/bin/sh
set -eu
PACK=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
if ! /usr/sbin/ioreg -r -c AMDRyzenCPUPowerManagement -l -w0 | /usr/bin/grep -q 'AMDRyzenCPUPowerManagement'; then
  printf '%s\n' 'AMD 驱动尚未形成服务。若刚安装，请先正常重启，再运行检查驱动加载.command。'
  exit 1
fi
if [ -d '/Applications/AMD Power Gadget.app' ]; then
  /usr/bin/open '/Applications/AMD Power Gadget.app'
else
  printf '%s\n' '请从官方 0.7.2f1 发布下载 AMD Power Gadget，并放入应用程序目录：' 'https://github.com/trulyspinach/SMCAMDProcessor/releases/tag/0.7.2f1'
  exit 1
fi
