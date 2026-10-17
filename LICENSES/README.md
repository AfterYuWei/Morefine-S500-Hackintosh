# 第三方许可与来源

本仓库是多组件 EFI 配置集合，没有一个统一许可证可以替代各组件的原始许可。组件、修改和再分发应遵循各自许可与固件声明。

- NootedRed 及本地派生版本：见 `NootedRed-TSNPL-1.5.txt`、`NootedRed-FIRMWARE-LICENSE.txt` 及 `Sources/NootedRed-Init2-Source.zip`。本地派生版本保留 TSNPL 1.5，含非商业分发条件；AMD 固件声明另行保留。
- OpenCore、Lilu、VirtualSMC、RestrictEvents、BlueToolFixup、Intel 无线与蓝牙：本目录保留取得的上游许可原文。具体分发版本和依赖版本见根目录 README 与源码说明。
- GLDualQuadFix 0.1.1：本地实验代码及修改说明位于 `Sources/GLDualQuadFix/`；链接使用 Lilu 的 `plugin_start.cpp`，保留 `acidanthera-Lilu-LICENSE.txt` 的 BSD 许可与上游署名。Lilu / MacKernelSDK 固定依赖见源码说明。生成的 Apple GL 原页面及函数匹配数据不被重新声明为本地原创或赋予其他许可。
- RealtekRTL8111 / LucyRTL8125Ethernet：感谢 Mieze；GPL 声明见上游源码，本目录保留对应 GPL 原文和来源摘录。
- USBToolBox、AMD_Vanilla、OcBinaryData、AppleMCEReporterDisabler：按上游仓库、包内署名及各文件声明使用，不自行赋予这些组件一个未经上游声明的许可证。
- AMFIPass 1.4.1 的实际取得来源是 [OpenCore-Legacy-Patcher payload](https://github.com/dortania/OpenCore-Legacy-Patcher/blob/main/payloads/Kexts/Acidanthera/AMFIPass-v1.4.1-RELEASE.zip)，保留包内原始署名。OCLP 的许可证不自动替代其第三方 payload 的许可。

各组件的对应源码仓库链接在根目录“参考与致谢”中。本地 NootedRed 修改的完整源码、差异和构建说明已经随仓库提供；二进制包保留其作者信息。
