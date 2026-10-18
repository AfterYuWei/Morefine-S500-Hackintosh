# Morefine S500+ Hackintosh — 当前 Tahoe EFI

适用本机 Morefine S500+ / Ryzen 7 5825U（8C/16T）、AMD 核显 `1002:15e7`、Intel AX200，当前系统 macOS 26.7.1 / 25G241 / Darwin 25.6.0，OpenCore 1.0.8 RELEASE。

**本次以实际 SSD 的 106 文件 EFI 为基线整体整理。** 已删除停用的旧进程 Metal/Scratch/签名授权与实验 VCN 模块，删除不再适用的旧 TSC 条目，保留已验证的全应用 GL/Metal 修补。目录与 [EFI.zip](EFI.zip) 为同一份脱敏配置；旧 EFI ZIP 与旧 Init2 源码包已从当前树移除，历史仍可在 Git 提交中查阅。

**清理版 NootedRed 已重新编译，部署后仍需重启验证。** 此前 GLDualQuadFix 0.2.0 已确认 GL 和 Metal 均 PATCHED，用户反馈 QQ、Codex 无异常；这份反馈属于清理前核心组合。全局修补二进制本次逐字节保持不变，不能把此前反馈当成新核心已经重启通过。

**使用前生成自己的身份信息。** 两份公开配置均清空序列号、MLB、SystemUUID，并将 ROM 置零；需要自行填写。私人完整备份、分区标识、原始日志和截图没有上传。

## 当前组件

| 组件 | 当前版本 / 用途 |
| --- | --- |
| AMDRyzenCPUPowerManagement.kext | 0.7.2 |
| AMFIPass.kext | 1.4.1 |
| AirportItlwm-Tahoe.kext | 2.4.1 |
| AppleMCEReporterDisabler.kext | 1.2 |
| BlueToolFixup.kext | 2.7.2 |
| GLDualQuadFix.kext | 0.2.0，GL + Metal 全应用精确页面 |
| IntelBTPatcher.kext | 2.5.1 |
| IntelBluetoothFirmware.kext | 2.5.1 |
| Lilu.kext | 1.7.2 |
| LucyRTL8125Ethernet.kext | 1.2.3 |
| NVMeFix.kext | 1.1.3 |
| NootedRed-Tahoe.kext | 0.9.0，Clean Core 1 |
| RealtekRTL8111.kext | 2.4.2 |
| RestrictEvents.kext | 1.1.6 |
| SMCAMDProcessor.kext | 1.0.1 |
| USBToolBox.kext | 1.2.0 |
| UTBMap.kext | 1.1 |
| VirtualSMC.kext | 1.3.8 |

保留本机 USB 映射、三个 SSDT、当前 AMD 内核补丁、图形菜单资源及 UEFI Shell。没有删除已启用的必要驱动，也没有顺带升级其他组件。当前有效内核补丁集合保持一致，旧条目仅覆盖 Darwin 25.3 及以前。

## 图形修补

- NootedRed Clean Core 1 保留 GPU / 固件基础支持、compute 提交虚表槽位修正与 KernelVersion 优先初始化；旧白名单、进程私有 COW/签名授权、Scratch 和 VCN 实验实现直接删除。
- `GLDualQuadFix 0.2.0` 统一处理 GL 一页和 Metal 两页，按原生验证结果、文件路径、页偏移、完整 4KB 指令匹配，不按应用名称筛选。
- 仅支持本机单 GPU `1002:15e7` 与当前精确驱动页面；macOS 更新后需要重新核验。没有启用 Scratch/VCN 参数，也没有删除已有 AMFIPass 设置。
- 运行 [检查当前EFI.command](Tools/GLDualQuadFix/检查当前EFI.command)，核对清理版核心 UUID、全局模块、GL/Metal 页面。检查不提交 GPU 工作，报告保存在本地且被 Git 忽略。

详见 [图形方案](docs/graphics.md)、[当前验证摘要](docs/current-validation.json) 与 [维护源码](Sources)。网易云原硬件加速歌单和持续播放尚无本轮成功反馈，不能借 QQ/Codex 结果宣称所有应用已根治。

## 无线、蓝牙与电源

AX200 当前使用 jzrodriguez98/itlwm `1.1.0` 发布包的 AirportItlwm（kext 2.4.1），蓝牙为 lshbluesky 维护分支 `2.5.1`，BlueToolFixup 2.7.2。它们是当前实际 SSD 组件，不能套用旧 README 的“仅 Sequoia/未启用 Tahoe”说明。

蓝牙 2.5.1 已加载，但单靠升级未消除音箱断续。用户对照显示两个无线接收器同时使用时明显卡顿，单独使用任一接收器基本顺畅；当前采用一只有线的绕过。射频/USB 调度尚未严格区分，不能宣称蓝牙驱动根治。

NVMeFix、AMDRyzenCPUPowerManagement、SMCAMDProcessor 已有重启加载与初步读数验证。AMD 原版组件含默认电源管理行为，不只是监控；实际节能、读数精度、睡眠唤醒和长期稳定性未完成验证。见 [附加驱动说明](docs/kext-addons.md)。

CoreMedia `allowMetalTransferSession=false` 是当前系统卷保留的既有绕过，**不会随 EFI 自动应用**；不把它等同于关闭整个 GPU 加速。3.5mm 音频未配置，硬件视频编解码、DRM、连续互通等不在已验证范围内。

## 使用与检查

1. 保留自己的完整 EFI 备份，先用可恢复的 U 盘测试。
2. 为 `iMacPro1,1` 生成自己的 SystemSerialNumber、MLB、SystemUUID 和六字节 ROM，填写 `EFI/OC/config.plist`。不要用空身份配置直接覆盖自己的可用 EFI。
3. 用 OpenCore 1.0.8 对应的 `ocvalidate` 校验，再将配置放到正确 EFI 分区。
4. 正常重启后运行检查工具，确认清理版核心及 GL/Metal 修补生效，再逐个验证常用应用。

公开仓库不提供自动覆盖用户磁盘的安装脚本。源码构建和本机编译验证不等于所有机器可启动。本次完整回退备份只保存在本机，公开 ZIP 是脱敏发布包。

## 本次清理与限制

25 个 C++ 单元构建、初始化表审计、改动单元静态分析、签名与配置检查通过；已停用 VCN 路径的新旧函数经 CPU/UBSan 对照一致。compute 槽位回归使用实际保留的桥接代码，未新做独立系统驱动文件虚表审计（当前系统不单独提供该 Mach-O）。全局页面回归已有常规/UBSan 记录，ASan 在本机进入 main 前发生 libpthread 初始化错误，未记为通过。

详见 [EFI 清理说明](docs/efi-cleanup.md)。长期负载、睡眠唤醒、所有显示接口和全部应用仍待验证；不外推至 Sequoia、其他显卡或其他 macOS 构建。

## 参考与致谢

感谢下列项目的作者、维护者和社区贡献者。本仓库的基础能力来自这些项目，本机整理与实验修改不替代上游署名：

| 项目 / 作者 | 在本配置中的作用 |
| --- | --- |
| [acidanthera/OpenCorePkg](https://github.com/acidanthera/OpenCorePkg) | OpenCore、OpenRuntime、macserial、ocvalidate 与配置文档 |
| [acidanthera/OcBinaryData](https://github.com/acidanthera/OcBinaryData) | OpenCore 相关二进制驱动与资源参考 |
| [AMD-OSX/AMD_Vanilla](https://github.com/AMD-OSX/AMD_Vanilla) | AMD 内核补丁；感谢 algrey、XLNC、CaseySJ、Visual、Zormeister、Goldfish64、laobamac 等贡献者，具体署名保留于配置注释 |
| [ChefKissInc/NootedRed](https://github.com/ChefKissInc/NootedRed) | AMD APU 核显支持及问题讨论 |
| [steve12311/NootedRed](https://github.com/steve12311/NootedRed) | 本地 Tahoe 渲染实验的直接上游与 `pre-release.4` 基础 |
| [acidanthera/Lilu](https://github.com/acidanthera/Lilu)、[VirtualSMC](https://github.com/acidanthera/VirtualSMC)、[RestrictEvents](https://github.com/acidanthera/RestrictEvents) | 基础补丁框架、SMC 与兼容性处理 |
| [acidanthera/MacKernelSDK](https://github.com/acidanthera/MacKernelSDK) | 本地驱动构建所需 SDK 头文件与库 |
| [acidanthera/NVMeFix](https://github.com/acidanthera/NVMeFix) | 第三方 NVMe 电源管理 |
| [trulyspinach/SMCAMDProcessor](https://github.com/trulyspinach/SMCAMDProcessor) 及其贡献者 | AMDRyzenCPUPowerManagement、SMCAMDProcessor 与 AMD Power Gadget |
| [OpenIntelWireless/itlwm](https://github.com/OpenIntelWireless/itlwm) | Intel Wi-Fi / AirportItlwm |
| [OpenIntelWireless/IntelBluetoothFirmware](https://github.com/OpenIntelWireless/IntelBluetoothFirmware) | Intel 蓝牙固件与补丁 |
| [lshbluesky/IntelBluetoothFirmware](https://github.com/lshbluesky/IntelBluetoothFirmware) | 当前 2.5.1 蓝牙维护分支 |
| [jzrodriguez98/itlwm](https://github.com/jzrodriguez98/itlwm) | 当前 Tahoe AirportItlwm 发布分支 |
| [acidanthera/BrcmPatchRAM](https://github.com/acidanthera/BrcmPatchRAM) | BlueToolFixup |
| [dortania/OpenCore-Legacy-Patcher](https://github.com/dortania/OpenCore-Legacy-Patcher) / dhinakg | 本次使用的 AMFIPass 1.4.1 包来源与相关签名兼容工作 |
| [USBToolBox/kext](https://github.com/USBToolBox/kext)、[USBToolBox/tool](https://github.com/USBToolBox/tool) | USB 映射框架与工具 |
| [Mieze/RTL8111_driver_for_OS_X](https://github.com/Mieze/RTL8111_driver_for_OS_X)、[Mieze/LucyRTL8125Ethernet](https://github.com/Mieze/LucyRTL8125Ethernet) | Realtek 有线网卡驱动 |
| XLNC | 当前 AppleMCEReporterDisabler 包内保留的作者署名 |
| [corpnewt/GenSMBIOS](https://github.com/corpnewt/GenSMBIOS) | SMBIOS 身份生成工具 |
| [Dortania OpenCore Install Guide](https://dortania.github.io/OpenCore-Install-Guide/) | OpenCore / AMD 配置参考资料 |

初始 EFI 来自本机既有配置；无法核实的更早整包来源不冒认原创。欢迎原作者提供可核实的来源以补充署名。

## 许可证与源码

各驱动、固件、工具及文档分别遵循其原有许可证；本仓库不使用一个统一许可证覆盖第三方作品。许可原文与来源说明见 [LICENSES](LICENSES)。

NootedRed 派生部分保留 **Thou Shalt Not Profit License 1.5** 及 AMD 固件声明，包含非商业分发条件。本地实验修改已经明确标记，完整维护源码与固定依赖见 [Sources](Sources)。请在使用和再分发前阅读相应许可；致谢不表示上游作者为这份本机配置背书。
