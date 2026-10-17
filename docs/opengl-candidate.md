# GLDualQuadFix 0.1.1：OpenGL 歌单花屏候选

记录日期：2026-10-17。此版本已部署到本机 SSD，并完成安装文件及备份核验；尚无 0.1.1 重启后的 PATCHED 或原操作 GPU 实测结论。公开 EFI 的 SMBIOS 信息继续脱敏。

## 现象与假设

网易云点击“我喜欢的歌单”时花屏、桌面卡顿，退出应用可恢复。三组 GPU Helper 报告与 OpenGL GPU 重置对应；改用软件渲染后用户确认原操作正常。这定位了图形路径，没有证明某一寄存器或 shader 是唯一根因。

候选在 Apple `AMDRadeonX5000GLDriver` 的 `_glrAMD_GFX9_UpdateHwCbDualQuadMode` 中，针对上下文报告的 PCI `15e7` 设置 `CB_COLOR_CONTROL.DISABLE_DUAL_QUAD`。其他 PCI ID 保留原生判断，命令写入和缓冲区刷新流程保留。该模式与核显能力不匹配仍是研究假设，没有完整实现所有 APU 拓扑，也不能推断解决其他 Metal、睡眠或计算问题。

## 精确范围

| 项目 | 值 |
| --- | --- |
| 系统 | 本机 macOS 26.7.1 / 25G241 / Darwin 25.6.0 |
| OpenCore 条目范围 | 25.6.0–25.6.99，x86_64；不在其他 macOS 启用 |
| 核显 | AMD 1002:15e7 |
| GL 驱动 SHA-256 | `ce205f063627a4b82ee8ccee04ab7963a3efb8dec488cb3c53ed6c6719434f80` |
| 文件路径 | `/System/Library/Extensions/AMDRadeonX5000GLDriver.bundle/Contents/MacOS/AMDRadeonX5000GLDriver` |
| 函数 RVA / 大小 | `0x819e` / 142 字节 |
| 文件页偏移 | `0x8000`，函数在页内 `0x19e` |
| 插件二进制 SHA-256 | `43964c3db7a6aff5eeb0212e3c264fd81feb36035d855bc5b278b88d36d125bd` |
| 替换载荷 SHA-256 | `669daa1f7b96afc89b12aa977ddc2be31962a3a47bae41dcfaa3be716bd542b4` |

插件先调用原生 `cs_validate_page`，保留全部七个参数及 validated、tainted、NX 输出；仅在原生验证通过、taint / NX 为零、驱动路径与完整原 4KB 页面逐字节匹配时替换内存函数。其他页面保持原行为。该插件不改写系统驱动文件，也不提供系统共享缓存的持久化修改。

## 两版状态及检查

0.1.0 已在 22:39 重启，但检查仍为 NATIVE；内核模块存在，未观察到对应 IORegistry 服务。审查发现插件错误使用 IOBSD 匹配阶段，并且只接入 `cs_validate_range`。没有捕获 TooLate 返回码，因此不能将注册过晚当作运行日志已经直接证明的原因。

0.1.1 改用 IOKit、接入本机内核普通页实际调用的 `cs_validate_page`。142 字节渲染载荷与首版相同。

- 新页面桥接：32,768 组 CPU / UBSan 用例通过，验证原生调用顺序、参数与输出保持，以及不合格页面拒绝。
- 未变化载荷：保留首版 786,432 组机器码对照及全部 4096 个单字节页面变异拒绝的有效检查记录。
- 构建、静态分析（0 项）、严格 ad-hoc 签名及 OpenCore 配置检查通过。
- 本机 22:53 部署完成，现场 98 文件与两份安装前备份均核验；这些是部署结果，未计作 GPU 通过。

## 使用与回退

1. 保留自己的可启动 EFI 备份，生成 SMBIOS 身份，按 OpenCore 1.0.8 校验配置并部署。
2. 正常重启后运行 `Tools/GLDualQuadFix/检查驱动修补.command`。工具仅加载库并读取机器码，不创建绘图上下文或提交 GPU 命令。应同时看到 0.1.1 服务及 `PATCHED`。`NATIVE` / `UNKNOWN` 均不算通过，先停止 GPU 复现并检查加载状态。
3. PATCHED 后普通打开网易云，不使用 `--disable-gpu` 软件渲染启动器，测试原歌单。若花屏，退出应用并记录结果，停止重复触发。
4. 单次通过后仍需持续播放、其他应用回归及 GPU 重置检查；PATCHED 本身不是根治证明。

要只停用此插件，可在 `Kernel -> Add` 将 `GLDualQuadFix.kext` 的 `Enabled` 改为 false，校验后重启；也可使用临时启动参数 `-glquadfixoff`。无法启动时使用自己的完整 EFI 备份。不要热卸载内核插件。

`EFI/` 是原仓库 EFI 加上该候选，未整体替换成本机最新 SSD 配置；`已验证EFI.zip` 为提交前不含此候选的历史归档，两者不能混作同一次验证结果。

## 依据与维护

参数与生命周期核对 [Apple XNU](https://github.com/apple-oss-distributions/xnu/blob/main/bsd/kern/ubc_subr.c)、[Lilu](https://github.com/acidanthera/Lilu/blob/master/Lilu/Library/plugin_start.cpp) 及本机 Darwin 25.6 内核调用点。寄存器定义参考 [Linux AMD GFX9 头文件](https://github.com/torvalds/linux/blob/v6.17/drivers/gpu/drm/amd/include/asic_reg/gc/gc_9_0_sh_mask.h)，相关混合状态参考 [Mesa radeonsi](https://chromium.googlesource.com/chromiumos/third_party/mesa/+/refs/heads/main/src/gallium/drivers/radeonsi/si_state.c)。这些资料提供机制依据，不能替代本机 GPU 验证。

维护源码与可配置路径的构建工具见 [Sources/GLDualQuadFix](../Sources/GLDualQuadFix)。本次未上传本机 SMBIOS、磁盘分区标识、原始诊断或私人安装备份。
