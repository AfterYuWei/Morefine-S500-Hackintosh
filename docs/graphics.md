# 当前图形方案

NootedRed Clean Core 1只承担基础核显兼容与现有compute桥接；已删除旧按进程Metal/Scratch/COW签名授权与实验VCN模块。全局渲染交给独立GLDualQuadFix0.2.0，其二进制本次未改。

GL与Metal按精确驱动页面匹配，不依赖应用名称。原生页面签名验证先执行，输出保持原值；三张目标页面须通过路径、偏移、整页内容检查。所有处理只修改运行内存，没有重签磁盘系统缓存。

此前组合已重启，GL及Metal两页PATCHED，QQ/Codex用户初步使用无异常。新清理核心未在该反馈时运行，仍需后续重启验证。网易云原硬件加速操作、其他应用、长期运行和睡眠未因此自动通过。

本机仍保留AMFIPass和CoreMedia allowMetalTransferSession=false等既有兼容设置，不声称全系统签名例外已撤销。源码与固定页面在Sources/GLDualQuadFix。
