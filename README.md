# Morefine S500 Hackintosh

记录 Morefine S500+（Ryzen 7 5825U）上的 OpenCore 配置、AX200 无线适配，以及 NootedRed 在 Tahoe 下的 Chrome / Electron 渲染实验和网易云 OpenGL 花屏候选修补。

本仓库基于现有 EFI 和多个社区项目整理，包含本机适配与本地实验修改。主要驱动和补丁的功劳属于各上游作者；参考与致谢见文末。

> **使用前先生成自己的 SMBIOS 信息。** `EFI/OC/config.plist` 与 `已验证EFI.zip` 均已移除本机的序列号、MLB、SystemUUID 和 ROM，不能直接使用这些空值启动。实机验证使用的是脱敏前、具有有效本机身份信息的配置。

> **OpenGL 更新状态：** `EFI/` 新增实验性 `GLDualQuadFix 0.1.1`，面向本机 `1002:15e7`、Darwin `25.6` 的精确 Apple OpenGL 驱动。已通过离线检查并部署到本机 SSD；重启后 `PATCHED`、网易云硬件加速歌单操作及持续稳定性尚未确认。`已验证EFI.zip` 保留此前不含此插件的历史版本。详情见 [OpenGL 候选说明](docs/opengl-candidate.md)。

## 本机配置与验证范围

| 项目 | 配置 / 状态 |
| --- | --- |
| 机器 | Morefine S500+，5825U 版本 |
| CPU | AMD Ryzen 7 5825U，8 核 16 线程 |
| 核显 | AMD Radeon Vega / Renoir，PCI ID `1002:15e7` |
| 内存 | 32 GB |
| 核显显存 | 本机报告 2 GB，Metal 3 |
| 无线 / 蓝牙 | Intel AX200 |
| SMBIOS | `iMacPro1,1`，身份字段需自行生成 |
| OpenCore | 1.0.8 |
| Sequoia | macOS 15.8.1，原有基础功能及无线配置已实测 |
| Tahoe | macOS 26.7.1 / `25G241` / Darwin `25.6.0`，独立测试卷实测 |
| 图形测试 | DELL U2422HX，1920 × 1080 / 60 Hz |

验证日期：2026-10-17。基础配置及下述 Chrome / VS Code 结果来自此前的引导介质测试。OpenGL 0.1.1 后续已部署到本机 SSD，仓库本次仅同步这一插件、加载条目及维护资料；其余组件保留原仓库版本，不能把历史应用通过结果视为新候选通过。

## 处理了哪些问题

### 1. AMD CPU 与不同内核版本的启动适配

- 使用 [AMD-OSX/AMD_Vanilla](https://github.com/AMD-OSX/AMD_Vanilla) 的内核补丁体系；核心数补丁按本机 **8 个物理核心**配置。
- Tahoe 配置包含适用于 Darwin 25 的补丁范围，以及参考 [PR #215](https://github.com/AMD-OSX/AMD_Vanilla/pull/215) 的 Darwin 25.4+ TSC 补丁调整。
- 保留本机 CPU / EC / USB ACPI 配置与 `RestrictEvents` 兼容参数。它们不等于其他型号或其他 BIOS 可直接复用。

### 2. Sequoia 下 AX200 原生 Wi-Fi 与蓝牙

- 使用 `AirportItlwm` 的 Sequoia 预览构建，在系统 Wi-Fi 菜单连接网络；不依赖 HeliPort。
- 使用 `IntelBluetoothFirmware`、`IntelBTPatcher` 和 `BlueToolFixup` 配套处理蓝牙加载与初始化，已验证搜索或连接设备。
- 保留本机的 `SSDT-AXPWR.aml`、GPP4 电源方法相关适配与 PCIe ASPM 设置；维护源码见 [Sources/ACPI](Sources/ACPI)。
- 已验证 2.4 GHz 和另一接入点的 5 GHz / 80 MHz 连接。原接入点的部分 5 GHz / 160 MHz 组合仍有高延迟，**没有证明仅由 160 MHz 带宽导致，也没有宣称彻底修复该互操作问题**。

本 EFI 的无线、蓝牙条目设置了内核版本范围。**Tahoe 下未启用这组 AX200 驱动，当前使用有线网络测试。**

### 3. Tahoe 下 Chrome / Electron 闪屏、卡顿、GPU Reset

原始现象是 Chrome 或 VS Code 使用 GPU 渲染后出现闪屏、严重卡顿；若未及时退出应用，最终可能只能强制重启。旧采集出现过实际 `GFX GPU Reset`，涉及 Dawn 渲染任务。

本机通过的组合为：

- [steve12311/NootedRed](https://github.com/steve12311/NootedRed) `v0.9.0-pre-release.4`，上游提交 `1983f326ebedda9e5596ee6e8ba47af52420ec11`。
- 本地 **Auth1 + Init2** 修正版：对经过完整核验的私有 Metal 代码页加入局部进程授权，并修正本地手动链接时的全局初始化顺序。
- 启动参数 **`-NRedLegacyBlend`**。本版本不启用 `-NRedComputeScratch` 或 `-NRedVCN`。
- Tahoe 专用 `AMFIPass 1.4.1` 与 `-amfipassbeta`；`csr-active-config` 为 `00000000`，实机 SIP 状态显示 enabled。

局部授权仅面向经过核验的 WindowServer、Chrome Helper 与 Code Helper；本地构建的 KernelVersion 初始化必须先于依赖它的硬件模块。此前手动构建漏掉这个依赖，会选择错误的硬件字段偏移并导致启动失败。源码包包含修正后的构建顺序和链接后审计。

**2026-10-17 本轮验证结果：**

| 检查 | 结果 |
| --- | --- |
| Init2 驱动实际加载 | 已确认，UUID `86E597A9-1A9C-5BE4-AA4F-D11370076911` |
| LegacyBlend / WindowServer 补丁实际应用 | 已确认 |
| Chrome | `155.0.8059.85`，用户实测不卡顿 |
| Chrome 渲染后端 | `GraphiteDawnMetal` / `ANGLE_METAL` |
| 合成、栅格化、WebGL、WebGPU | Hardware accelerated |
| Chrome GPU 进程崩溃计数 | 0 |
| VS Code | 用户确认滚动、切换窗口、终端操作正常；该测试用户未启用禁用硬件加速的 argv 设置 |
| 本轮采集的新 GPU Reset / WindowServer 崩溃 | 未发现 |

本轮是约四分钟的启动与应用操作验证，不能替代长期稳定性验证。**修复结果仅在上述 Tahoe 环境中确认；Sequoia 15.8.1 的 Electron 问题没有因此被宣称修复。** 原始应用进程、UUID、设备序列号等私人诊断没有公开上传，仓库只保留不含本机身份的验证摘要。

这是本地实验性派生版本，不是 NootedRed 官方正式发布。SIP 显示 enabled 也不代表签名机制完全未作例外处理；当前仍使用 AMFIPass 与局部授权例外。修改说明、固定版本源码及依赖见 [Sources](Sources)。

### 4. Tahoe 首次设置 Data & Privacy 卡顿

这属于单独的像素转换 / Metal compute 问题。测试卷保留了参考 [NootedRed issue #235](https://github.com/ChefKissInc/NootedRed/issues/235) 的系统偏好：

```sh
sudo defaults write /Library/Preferences/com.apple.coremedia allowMetalTransferSession -bool false
sudo chmod 644 /Library/Preferences/com.apple.coremedia.plist
```

这是 **macOS 卷内的设置，不会随 EFI 文件自动应用**。只在遇到对应问题、确认目标为测试系统时考虑设置。本次 Chrome 报告仍显示 Metal 硬件渲染，不能将这项偏好等同于关闭整个 GPU 加速。

### 5. USB 与配置整理

- 使用 `USBToolBox.kext` 与本机 `UTBMap.kext`；保留现有端口映射，不在发布时重新猜测 USB 端口。
- 清理已停用的旧 `itlwm`、临时调试驱动及过时配置条目。
- 分别限定 Sequoia 与 Tahoe 的 NootedRed、无线和蓝牙驱动加载范围，避免跨版本误加载。
- 使用 OpenCore 1.0.8 的 `ocvalidate`、plist 检查及文件散列核对。

### 6. 网易云歌单触发 OpenGL GPU Reset：候选 0.1.1

点击“我喜欢的歌单”会出现花屏或桌面卡顿，退出网易云可恢复。已有报告显示网易云 GPU Helper 在 Apple OpenGL 提交路径发生 GPU 重置；软件渲染启动后原操作正常。此前的 Metal 修补不能直接覆盖这条 OpenGL 路径。

新增独立 Lilu 插件 `GLDualQuadFix.kext`，仅对本机 PCI `15e7` 的 `UpdateHwCbDualQuadMode` 设置 `DISABLE_DUAL_QUAD`，保留 GPU 加速。候选通过 `cs_validate_page` 在原生验证成功后，核对精确驱动路径、偏移与完整 4KB 原页面，再修改内存中的 142 字节函数。

首版 0.1.0 重启后仍为 `NATIVE`。0.1.1 修正了插件的 IOKit 匹配阶段和页面验证入口，原渲染载荷不变。新入口 32,768 组 CPU / UBSan 检查、编译、静态分析和签名检查通过；原载荷的 786,432 组对照及 4096 字节变异拒绝检查记录随源码提供。**这些检查不等于 GPU 实测，当前尚未证实花屏根因或根治。**

重启后先运行 [检查驱动修补.command](Tools/GLDualQuadFix/检查驱动修补.command)，出现 0.1.1 服务及 `PATCHED` 后，才正常打开网易云测试原歌单。完整限制与回退方法见 [候选说明](docs/opengl-candidate.md)，源码及构建说明见 [Sources/GLDualQuadFix](Sources/GLDualQuadFix)。

## 主要组件

| 组件 | 当前版本 | 范围 / 用途 |
| --- | --- | --- |
| Lilu | 1.7.2 | 补丁框架 |
| VirtualSMC | 1.3.8 | SMC 模拟 |
| RestrictEvents | 1.1.6 | 兼容性处理 |
| NootedRed | 0.8.10 | Darwin ≤ 24.99.99，Sequoia 原有核显驱动 |
| NootedRed-Tahoe | 0.9.0，Auth1 + Init2 本地版 | Darwin 25.0.0–25.99.99 |
| GLDualQuadFix | 0.1.1，本机实验候选 | Darwin 25.6.0–25.6.99；精确 GL 驱动及 PCI 15e7 |
| AMFIPass | 1.4.1 | 仅 Darwin 25，签名兼容 |
| AirportItlwm | 2.4.0 预览构建 | Darwin 24.2.0–24.99.99 |
| IntelBluetoothFirmware / IntelBTPatcher | 2.5.0 | 仅 Darwin 24 |
| BlueToolFixup | 2.7.2 | 仅 Darwin 24 |
| RealtekRTL8111 | 2.4.2 | RTL8111 系列有线网卡驱动 |
| LucyRTL8125Ethernet | 1.2.300 | RTL8125 系列有线网卡驱动 |
| USBToolBox / UTBMap | 1.2.0 / 1.1 | 本机 USB 映射 |
| AppleMCEReporterDisabler | 1.2 | AMD / SMBIOS 兼容 |

保留驱动不等于所有对应硬件功能均经过独立验证。当前没有随意升级这些已测试组件。

## 使用方法

1. 确认硬件与内核版本匹配，先保留自己的完整 EFI 备份和可用 U 盘引导。
2. 使用 [GenSMBIOS](https://github.com/corpnewt/GenSMBIOS) 为 `iMacPro1,1` 生成自己的 `SystemSerialNumber`、`MLB`、`SystemUUID`，并配置自己的 6 字节 `ROM`。不要使用其他人的身份信息。
3. 修改 `EFI/OC/config.plist`，用 **OpenCore 1.0.8 对应的 ocvalidate** 校验。压缩包中的同名配置也是脱敏版本，不能用它覆盖已经填好的身份。
4. 先把 `EFI` 放到测试 U 盘的正确位置，从 BIOS 的 UEFI 启动项进入。请自行核对目标设备，仓库不提供自动覆盖固态 EFI 的脚本。
5. Tahoe 启动后确认正确驱动与补丁实际应用，再在 Chrome 的 `chrome://gpu` 核对硬件加速，测试平时会触发问题的操作。

需要恢复渲染修复前的行为时，可在已有有效身份信息的配置中只删除 `-NRedLegacyBlend`，保留其余已知可启动设置，再校验和重启。若整个 EFI 已损坏，应使用自己的完整备份。

## 未解决或未完整验证

- 长时间负载、睡眠唤醒、所有显示接口 / 分辨率及其他 Electron 应用尚未完成验证。
- OpenGL 0.1.1 尚待重启后确认 `PATCHED`、原生硬件加速的网易云歌单复现和持续播放；历史 Chrome / VS Code 通过不代表此候选已通过。
- Tahoe 的 AX200 Wi-Fi / 蓝牙没有在本配置中启用。
- 原接入点的部分 5 GHz 连接高延迟仍需进一步定位。
- 后续关机阶段存在独立 `shutdownStall` 记录，原因尚未确定，不能声称整机所有稳定性问题已解决。
- 硬件视频编解码、DRM、音频、AirDrop、连续互通、iMessage / FaceTime 没有本仓库的完整实测结论。
- 更新 macOS、Lilu、NootedRed 或应用后，需要重新核对；不能把单一系统版本的结果外推到其他版本或机器。

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
| [OpenIntelWireless/itlwm](https://github.com/OpenIntelWireless/itlwm) | Intel Wi-Fi / AirportItlwm |
| [OpenIntelWireless/IntelBluetoothFirmware](https://github.com/OpenIntelWireless/IntelBluetoothFirmware) | Intel 蓝牙固件与补丁 |
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
