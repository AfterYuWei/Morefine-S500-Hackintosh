# Morefine S500+ Hackintosh

适用于 Morefine S500+ 的 macOS Tahoe EFI，基于 OpenCore 1.0.8 RELEASE，包含 AMD 核显、Intel AX200 无线、USB 映射及电源管理配置。

## 硬件与系统

| 项目 | 配置 |
| --- | --- |
| 主机 | Morefine S500+ |
| 处理器 | AMD Ryzen 7 5825U，8 核 16 线程 |
| 核显 | AMD 集成显卡，PCI ID `1002:15e7` |
| 无线网卡 | Intel AX200 |
| 存储 | NVMe SSD |
| SMBIOS | `iMacPro1,1` |
| 验证系统 | macOS 26.7.1 / 25G241，Darwin 25.6.0 |
| 引导版本 | OpenCore 1.0.8 RELEASE |

## 功能状态

| 功能 | 状态与说明 |
| --- | --- |
| 固态独立启动 | 已验证 |
| 图形引导菜单 | 已配置 OpenCanopy / GoldenGate，显示效果待确认 |
| 核显驱动 | NootedRed 0.9.0 本地修订已通过重启加载验证 |
| OpenGL / Metal 修补 | GLDualQuadFix 0.2.0 已加载，目标代码检查为 PATCHED |
| Wi-Fi | 已配置 AX200 对应 AirportItlwm 2.4.1 |
| 蓝牙 | 驱动已加载；双无线接收器同时使用时存在音频断续 |
| USB | 已配置 USBToolBox 与本机 UTBMap |
| NVMe 电源管理 | NVMeFix 已验证加载，已观察到 APST 与 NVMePMProxy |
| CPU 电源与监控 | AMD 电源驱动及 SMC 插件已验证加载，可读取监控数据 |
| 3.5mm 音频 | 未配置 |
| 睡眠唤醒、硬件视频编解码、DRM、连续互通 | 未验证 |

蓝牙音频在 ATK 与 Logitech 无线接收器同时使用时曾出现明显断续，单接收器对照基本顺畅。存在此问题时可使用有线外设或单接收器组合。CPU 监控读数精度、实际节能效果及长期稳定性尚未完成验证。

## 主要组件

| 组件 | 版本 | 用途 |
| --- | --- | --- |
| Lilu | 1.7.2 | 补丁框架 |
| VirtualSMC | 1.3.8 | SMC 兼容 |
| NootedRed-Tahoe | 0.9.0，本地修订 | AMD 核显兼容 |
| GLDualQuadFix | 0.2.0 | OpenGL / Metal 渲染修补 |
| AMFIPass | 1.4.1 | Tahoe 签名兼容 |
| RestrictEvents | 1.1.6 | 系统兼容与 CPU 名称配置 |
| AppleMCEReporterDisabler | 1.2 | AMD 平台兼容 |
| AirportItlwm-Tahoe | 2.4.1 | AX200 Wi-Fi |
| IntelBluetoothFirmware | 2.5.1 | Intel 蓝牙固件 |
| IntelBTPatcher | 2.5.1 | Intel 蓝牙兼容 |
| BlueToolFixup | 2.7.2 | 蓝牙框架兼容 |
| LucyRTL8125Ethernet | 1.2.3 | Realtek RTL8125 网卡驱动 |
| RealtekRTL8111 | 2.4.2 | Realtek RTL8111 网卡驱动 |
| USBToolBox | 1.2.0 | USB 映射框架 |
| UTBMap | 1.1 | 本机 USB 端口映射 |
| NVMeFix | 1.1.3 | NVMe 电源管理 |
| AMDRyzenCPUPowerManagement | 0.7.2 | AMD CPU 电源管理与数据采集 |
| SMCAMDProcessor | 1.0.1 | AMD CPU 的 SMC 传感器插件 |

AX200 Wi-Fi 使用 jzrodriguez98/itlwm `1.1.0` 发布包，蓝牙组件使用 lshbluesky 维护分支 `2.5.1`。各组件的固定来源见 [第三方组件说明](Sources/Kext-Addons)。

## 图形适配

NootedRed 提供核显兼容，GLDualQuadFix 提供应用通用的 OpenGL / Metal 渲染修补。图形方案适用于本机单核显 `1002:15e7` 与已核验的 Darwin 25.6 驱动代码；更新 macOS 后需要重新核验修补状态。

本机系统另有 CoreMedia `allowMetalTransferSession=false` 设置，该设置位于系统卷，不包含在 EFI 中。实现与兼容范围见 [图形说明](docs/graphics.md)，验证记录见 [状态摘要](docs/current-validation.json)。

## 使用方法

1. 下载或克隆本仓库，使用 `EFI/` 目录，并保留原有可启动 EFI 的备份。
2. 使用 GenSMBIOS 等工具为 `iMacPro1,1` 生成自己的身份信息，在 `EFI/OC/config.plist` 的 `PlatformInfo/Generic` 中填写 SystemSerialNumber、MLB、SystemUUID 和六字节 ROM。仓库中的这些字段已留空或置零。
3. 根据实际硬件核对驱动、USB 映射和内核补丁，使用 OpenCore 1.0.8 对应的 `ocvalidate` 校验配置。
4. 将 `EFI/` 复制到测试 U 盘的 EFI 分区，验证启动后再部署到目标磁盘。
5. 进入系统后运行 [检查当前EFI.command](Tools/GLDualQuadFix/检查当前EFI.command)，检查驱动版本与图形修补状态。报告保存在工具目录下的 `Reports/`，由 Git 忽略。

本配置以以上硬件与系统版本为验证范围。其他机型、显卡或 macOS 构建需要独立适配。

## 目录说明

| 目录 | 内容 |
| --- | --- |
| [EFI](EFI) | OpenCore、ACPI、UEFI 驱动、kext、图形资源及配置 |
| [Sources](Sources) | 本地修订源码、构建脚本与组件来源 |
| [Tools](Tools) | 图形修补与附加驱动检查工具 |
| [docs](docs) | 图形、电源管理说明与验证摘要 |
| [LICENSES](LICENSES) | 第三方许可证与来源说明 |

## 参考与致谢

本 EFI 使用以下项目的组件和维护成果，原有署名与许可证保留于对应源码及许可文件中。

| 项目 / 作者 | 在本配置中的作用 |
| --- | --- |
| [acidanthera/OpenCorePkg](https://github.com/acidanthera/OpenCorePkg) | OpenCore、OpenRuntime、macserial、ocvalidate 与配置文档 |
| [acidanthera/OcBinaryData](https://github.com/acidanthera/OcBinaryData) | OpenCore 相关二进制驱动与资源参考 |
| [AMD-OSX/AMD_Vanilla](https://github.com/AMD-OSX/AMD_Vanilla) | AMD 内核补丁；感谢 algrey、XLNC、CaseySJ、Visual、Zormeister、Goldfish64、laobamac 等贡献者，具体署名保留于配置注释 |
| [ChefKissInc/NootedRed](https://github.com/ChefKissInc/NootedRed) | AMD APU 核显支持及问题讨论 |
| [steve12311/NootedRed](https://github.com/steve12311/NootedRed) | NootedRed 本地修订的直接上游与 `pre-release.4` 基础 |
| [acidanthera/Lilu](https://github.com/acidanthera/Lilu)、[VirtualSMC](https://github.com/acidanthera/VirtualSMC)、[RestrictEvents](https://github.com/acidanthera/RestrictEvents) | 基础补丁框架、SMC 与兼容性处理 |
| [acidanthera/MacKernelSDK](https://github.com/acidanthera/MacKernelSDK) | 本地驱动构建所需 SDK 头文件与库 |
| [acidanthera/NVMeFix](https://github.com/acidanthera/NVMeFix) | 第三方 NVMe 电源管理 |
| [trulyspinach/SMCAMDProcessor](https://github.com/trulyspinach/SMCAMDProcessor) 及其贡献者 | AMDRyzenCPUPowerManagement、SMCAMDProcessor 与 AMD Power Gadget |
| [OpenIntelWireless/itlwm](https://github.com/OpenIntelWireless/itlwm) | Intel Wi-Fi / AirportItlwm |
| [OpenIntelWireless/IntelBluetoothFirmware](https://github.com/OpenIntelWireless/IntelBluetoothFirmware) | Intel 蓝牙固件与补丁 |
| [lshbluesky/IntelBluetoothFirmware](https://github.com/lshbluesky/IntelBluetoothFirmware) | 当前 2.5.1 蓝牙维护分支 |
| [jzrodriguez98/itlwm](https://github.com/jzrodriguez98/itlwm) | 当前 Tahoe AirportItlwm 发布分支 |
| [acidanthera/BrcmPatchRAM](https://github.com/acidanthera/BrcmPatchRAM) | BlueToolFixup |
| [dortania/OpenCore-Legacy-Patcher](https://github.com/dortania/OpenCore-Legacy-Patcher) / dhinakg | AMFIPass 1.4.1 包来源与相关签名兼容工作 |
| [USBToolBox/kext](https://github.com/USBToolBox/kext)、[USBToolBox/tool](https://github.com/USBToolBox/tool) | USB 映射框架与工具 |
| [Mieze/RTL8111_driver_for_OS_X](https://github.com/Mieze/RTL8111_driver_for_OS_X)、[Mieze/LucyRTL8125Ethernet](https://github.com/Mieze/LucyRTL8125Ethernet) | Realtek 有线网卡驱动 |
| XLNC | 当前 AppleMCEReporterDisabler 包内保留的作者署名 |
| [corpnewt/GenSMBIOS](https://github.com/corpnewt/GenSMBIOS) | SMBIOS 身份生成工具 |
| [Dortania OpenCore Install Guide](https://dortania.github.io/OpenCore-Install-Guide/) | OpenCore / AMD 配置参考资料 |

## 许可证与源码

各组件遵循各自的原始许可证，详见 [LICENSES](LICENSES)。NootedRed 派生部分遵循 **Thou Shalt Not Profit License 1.5**，包含非商业分发条件，并保留 AMD 固件声明。完整维护源码与固定依赖见 [Sources](Sources)。
