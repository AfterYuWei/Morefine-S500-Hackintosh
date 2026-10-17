#!/bin/sh
set -eu
PACK=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
printf '%s\n' '插件服务与版本：'
/usr/sbin/ioreg -r -c GLDualQuadFix -l -w0
set +e
"$PACK/gl-inspect"
rc=$?
set -e
case "$rc" in
  0) printf '%s\n' '已确认苹果 OpenGL 驱动中的候选机器码。可正常打开网易云测试原歌单，不使用软件渲染启动器。' ;;
  10) printf '%s\n' '仍是原生代码：候选未安装、尚未重启，或页面补丁未生效。请采集状态；不要把此状态记为修复成功。' ;;
  *) printf '%s\n' '检查未通过，请保留输出并采集状态。' ;;
esac
exit "$rc"
