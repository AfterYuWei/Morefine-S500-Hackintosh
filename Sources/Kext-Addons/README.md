# 官方驱动来源与下载记录

本次新增三个驱动均为官方发布二进制，没有本地修改、重新编译或重新签名。其内容与本机已部署并重启加载的同版文件逐字节核对。依赖已有 Lilu / VirtualSMC，加载和验证边界见 [驱动说明](../../docs/kext-addons.md)。

| 文件 | 固定发布 / 源码 | 许可 |
| --- | --- | --- |
| NVMeFix.kext | [acidanthera 1.1.3 RELEASE](https://github.com/acidanthera/NVMeFix/releases/tag/1.1.3) / [源码](https://github.com/acidanthera/NVMeFix/tree/1.1.3) | [完整原文](../../LICENSES/acidanthera-NVMeFix-LICENSE.txt) |
| AMDRyzenCPUPowerManagement.kext | [trulyspinach 0.7.2f1](https://github.com/trulyspinach/SMCAMDProcessor/releases/tag/0.7.2f1) / [源码](https://github.com/trulyspinach/SMCAMDProcessor/tree/0.7.2f1) | [BSD 原文](../../LICENSES/trulyspinach-SMCAMDProcessor-LICENSE.txt) |
| SMCAMDProcessor.kext | 同一 0.7.2f1 发布及源码 | 同上 |

发布包版本与内部 BundleVersion 不同：AMDRyzenCPUPowerManagement 为 0.7.2，SMCAMDProcessor 为 1；这是上游原文件的元数据，不是混装版本。

[upstream-assets.json](upstream-assets.json) 记录固定下载 URL、下载包 SHA-256，以及是否与上游 API digest 核对。NVMeFix 有官方 digest 并已比对；AMD 发布包没有提供该字段，仅记录本次官方 HTTPS 下载文件的本地散列，不能把它称作与官方摘要核验。

NVMeFix 上游原始二进制未签名，保持原样；两个 AMD kext 的原始签名校验通过。配套 AMD Power Gadget 的官方下载链接和散列也包含在记录中；它是单独应用，不放进 EFI。

没有将本机自动写盘脚本、分区标识、SMBIOS、原始日志或私有备份放入本目录。公开工具仅在用户主动运行时采集本地状态。
