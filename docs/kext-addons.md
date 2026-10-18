# 存储与 AMD CPU 驱动：2026-10-18

本次将三个官方驱动和加载项加入仓库既有公开 EFI。对应二进制已在本机 SSD 的 Tahoe 配置重启后加载，监控能显示读数；公开 EFI 的其余组件保持仓库原版本，没有把本机 106 文件 SSD EFI 整体覆盖到仓库。因此，下面是组件验证结果，不能称为当前公开组合的完整重启测试。

## 组件与配置

| 组件 | 版本 | 用途 |
| --- | --- | --- |
| NVMeFix | 1.1.3 RELEASE | NVMe 电源管理与兼容性处理 |
| AMDRyzenCPUPowerManagement | 0.7.2，官方发布包 0.7.2f1 | CPU 数据读取与默认电源管理 |
| SMCAMDProcessor | 1，同一发布包 | 将 CPU 数据提供给 VirtualSMC |
| AMD Power Gadget | 同一官方发布 | 配套监控应用，按需从上游下载并放入应用程序目录 |

三者均限定 Darwin 25.0.0–25.99.99。NVMeFix 在 Lilu 后；AMD 部分依次为 VirtualSMC、AMDRyzenCPUPowerManagement、SMCAMDProcessor。只新增这三个 Kernel/Add 条目，其余既有配置、启动参数、设备属性未修改。

**AMD 驱动包含默认电源控制，不是纯只读监控。** 官方源码中的 `pmRyzen_init` 注册 PM dispatch、初始化 P-state，计时器初始化还写 C-state MSR 和 HWCR。本机明确接受这些行为后才安装测试；未添加 `-amdpnopchk`、强制 ASPM 或手动调频参数。

## 重启后的实际结果

2026-10-18，5825U、macOS 26.7.1 / 25G241 / Darwin 25.6.0：

- NVMeFix 1.1.3、AMDRyzenCPUPowerManagement 0.7.2、SMCAMDProcessor 1 均在实际加载列表中。
- AMD 核心服务 registered / matched / active，SMC 插件服务存在且 active。
- NVMe 控制器 `apst=Yes`，电源树实际存在 `NVMePMProxy` 节点，支持电源管理初始化已完成的判断；尚未做功耗下降的对照测量。
- AMD Power Gadget 显示 8 核 16 线程，以及频率、温度、功耗历史曲线；截图约为平均 3.76GHz、最高 4.40GHz、74.65°C、15.63W。截图负载未知，不能将这些数字当作待机指标，也未独立校准精度。
- 原有 GLDualQuadFix 0.1.1 仍为 PATCHED。这只确认机器码，原硬件加速的网易云歌单及持续播放仍待验证。

`Motherboard: Unknown` 是未返回 OEM 名称；`Model 05h` 来自原版型号字段拼接显示错误，系统实际 CPUID 是 Family 19h / Model 50h。本机没有为这些显示字段修改 BIOS 或 SMBIOS。

尚未验证：睡眠唤醒、长时间负载、功耗/温度前后对照、传感器精度、其他读取软件是否正确获得 VirtualSMC 键。原始诊断和完整桌面截图保存在本机，仓库提供 [脱敏摘要](kext-addons-validation.json)。

## 使用与停用

在填写自己的 SMBIOS 并部署 EFI 后正常重启，运行 [检查驱动加载.command](../Tools/KextAddons/检查驱动加载.command)。报告只写本地 `Tools/KextAddons/Reports/`，已加入 Git 忽略规则，不会自动上传。

AMD Power Gadget 从 [官方 0.7.2f1](https://github.com/trulyspinach/SMCAMDProcessor/releases/tag/0.7.2f1) 下载。安装到 `/Applications/AMD Power Gadget.app` 后运行 [打开AMD监控.command](../Tools/KextAddons/打开AMD监控.command)。保持默认设置，先检查读数和日常稳定性。

只停用 AMD 部分：将两个 AMD kext 的 `Enabled` 一起改为 false，使用 OpenCore 1.0.8 的 ocvalidate 校验并重启；可保留 NVMeFix。需要同时停用 NVMeFix 时再关闭其条目。无法启动时使用自己的完整 EFI 备份，不要热卸载这些内核驱动。

`已验证EFI.zip` 保留更新前的历史配置，不包含本轮三个驱动或 OpenGL 候选。当前公开 `EFI/` 的完整文件清单见 [EFI-SHA256.txt](EFI-SHA256.txt)。

## 来源

- [NVMeFix 1.1.3 发布](https://github.com/acidanthera/NVMeFix/releases/tag/1.1.3)、[固定源码](https://github.com/acidanthera/NVMeFix/tree/1.1.3)
- [AMD 0.7.2f1 发布](https://github.com/trulyspinach/SMCAMDProcessor/releases/tag/0.7.2f1)、[固定源码](https://github.com/trulyspinach/SMCAMDProcessor/tree/0.7.2f1)
- 来源散列与完整许可入口见 [Sources/Kext-Addons](../Sources/Kext-Addons)。
