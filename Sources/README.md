# 当前源码与固定来源

- `NootedRed-Clean-Source.zip`：当前清理版 NootedRed 核心、手动构建脚本、打包脚本、初始化审计、固定 Lilu / MacKernelSDK 依赖与原始许可证。旧 Metal 进程授权、Scratch、VCN 实验模块已从源码和编译单元中删除。
- `GLDualQuadFix/`：实际使用的全应用 GL + Metal 0.2.0，含有效载荷、精确页面生成器、构建脚本、只读检查工具源码和 CPU 回归。
- `ACPI/`：当前 AX200 电源适配的 DSL。
- `Kext-Addons/`：未修改的第三方新增驱动固定下载来源。

NootedRed 直接上游是 [steve12311/NootedRed](https://github.com/steve12311/NootedRed)，固定提交 `1983f326ebedda9e5596ee6e8ba47af52420ec11`，派生自 [ChefKissInc/NootedRed](https://github.com/ChefKissInc/NootedRed)。保留正常 GPU / 固件支持、已有 compute 虚表槽位修正，以及 KernelVersion 必须先初始化的链接约束。25 个 C++ 编译单元完成构建，两个实际改动单元的静态分析无发现；已停用 VCN 的新旧引擎分配/能力函数经 CPU/UBSan 对照保持原行为。

解压源码 ZIP 后设置 `CLANG` 与 `SDKROOT`，运行 `build-local.py`，再运行 `package-kext.py`。打包使用随附 `bundle-info.plist`，不会写入 EFI。SDK / 编译器不分发。新核心尚待重启验证。

依赖提交保持为 Lilu `0515f40b7f2a096adc85e832a4c6104fbd07f936`、MacKernelSDK `7af1933c27aefcbdf4809ee44478829aad30f9c1`。
