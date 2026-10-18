# 图形适配

NootedRed 0.9.0 本地修订负责 AMD 核显兼容及 compute 提交桥接，GLDualQuadFix 0.2.0 提供应用通用的 OpenGL / Metal 渲染修补。

## 修补机制与范围

GLDualQuadFix 使用 `cs_validate_page` 入口，覆盖一个 OpenGL 代码页和两个 Metal 代码页。每页 4096 字节，只有文件路径、页偏移、完整页面内容及原生验证结果均满足要求时才应用对应指令修改。原生验证输出保持原值，修补作用于运行内存。

适用范围为 Darwin 25.6、单一 AMD `1002:15e7` 核显及已记录的驱动和共享缓存。应用进程名称不参与匹配；系统更新后需要重新核验页面。

## 运行状态

2026-10-18 重启后的检查结果为 `CLEAN_CORE_AND_GLOBAL_PATCHED`：NootedRed UUID 与发布构建一致，GLDualQuadFix 0.2.0 已加载，GL 和 Metal 两页均 PATCHED。

QQ、Codex 已有此前核心组合的正常使用反馈；当前核心的应用回归与持续负载尚待补充。页面状态检查仅验证代码是否生效，应用稳定性、睡眠唤醒、硬件视频编解码及 DRM 需分别验证。

## 系统兼容设置

本机使用 AMFIPass 1.4.1，并在系统卷设置 CoreMedia `allowMetalTransferSession=false`。CoreMedia 设置不随 EFI 部署，需与图形驱动配置分开维护。

源码见 [GLDualQuadFix](../Sources/GLDualQuadFix)，状态和构建指纹见 [验证摘要](current-validation.json)。
