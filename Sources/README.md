# 本地派生源码与维护

`NootedRed-Init2-Source.zip` 包含本次本地修改的完整源码、固定版本的 Lilu / MacKernelSDK 依赖、源码差异、构建说明与初始化表审计。原作者版权和许可保留于源码树中。

直接上游为 [steve12311/NootedRed](https://github.com/steve12311/NootedRed)，提交 `1983f326ebedda9e5596ee6e8ba47af52420ec11`，基于 [ChefKissInc/NootedRed](https://github.com/ChefKissInc/NootedRed)。

本地变更主要是：

1. 在完整核验缓存、代码与页面后，为指定进程私有 Metal 页提供短时、精确进程与线程范围的授权；不猜测 proc 结构偏移。
2. 修正手动链接的初始化顺序，先初始化 KernelVersion，再构造依赖它的硬件模块；构建结束后检查 `__mod_init_func` 首项。

已验证二进制由历史 Auth1 构建修正初始化表、更新 UUID 和说明字段后重新 ad-hoc 签名得到；代码段与原 Auth1 相同。源码包中的更新构建脚本体现相同初始化约束，但没有声称新编译的文件逐字节重现该二进制。新编译或更换工具链后必须重新验证。

已验证驱动 SHA-256：

```text
332f4723f8b874ecb5a9c51873de023a1657a2f69ade6686a6bc55847c922edc
```

依赖提交：

- Lilu：`0515f40b7f2a096adc85e832a4c6104fbd07f936`
- MacKernelSDK：`7af1933c27aefcbdf4809ee44478829aad30f9c1`

构建说明见源码 ZIP 内 `构建说明.md`。SDK / 编译器本身不随本仓库分发。

`ACPI/SSDT-AXPWR.dsl` 是本机有效 AX200 电源适配的维护源码。其他原有 AML 保留自初始 EFI，当前没有未经核实地重写它们。
