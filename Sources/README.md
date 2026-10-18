# 源码与构建

| 路径 | 内容 |
| --- | --- |
| `NootedRed-Clean-Source.zip` | NootedRed 本地修订、构建与打包脚本、初始化审计、固定依赖及许可证 |
| [GLDualQuadFix](GLDualQuadFix) | OpenGL / Metal 0.2.0 的修补数据、页面生成器、构建脚本、检查工具源码与 CPU 回归 |
| [ACPI](ACPI) | AX200 电源适配 DSL |
| [Kext-Addons](Kext-Addons) | 第三方组件的固定发布来源 |

## NootedRed

直接上游为 [steve12311/NootedRed](https://github.com/steve12311/NootedRed)，固定提交 `1983f326ebedda9e5596ee6e8ba47af52420ec11`，派生自 [ChefKissInc/NootedRed](https://github.com/ChefKissInc/NootedRed)。本地修订包含 compute 提交桥接和 KernelVersion 优先初始化约束。

解压源码 ZIP 后设置 `CLANG` 与 `SDKROOT`，运行 `build-local.py`，再运行 `package-kext.py`。打包脚本使用随附的 `bundle-info.plist`，输出构建产物；编译器及系统 SDK 需自行准备。

固定依赖为 Lilu `0515f40b7f2a096adc85e832a4c6104fbd07f936` 与 MacKernelSDK `7af1933c27aefcbdf4809ee44478829aad30f9c1`。构建、签名、源码重建和运行时结果见 [验证摘要](../docs/current-validation.json)。
