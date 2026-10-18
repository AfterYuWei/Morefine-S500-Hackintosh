# GLDualQuadFix 0.2.0

本机的 GL + Metal 全应用修补。文件名沿用最初的 GL 插件名称，现在通过同一个 `cs_validate_page` 接口处理 GL 一页和 Metal 共享缓存两页；不按应用进程名称筛选。

原生页面验证先运行；有效、未污染、可执行页面还必须匹配文件路径、文件页偏移与完整 4096 字节内容。只支持当前 Darwin 25.6、单一 `1002:15e7` 核显及记录中的共享缓存。其他版本或内容拒绝修补。本插件不调用进程签名放行接口，不改写磁盘上的系统文件。

`global_page_data.hpp` 是实际编译的数据；`active-payloads.json` 和 `generate_pages.py` 仅保留当前使用的四段载荷。旧的进程白名单、COW 授权、Scratch、VCN 实验源码不在此目录。生成器只读本机系统页，并允许识别当前内存文件缓存中已修补的已知状态。

2026-10-18 已在清理前核心组合中重启确认 GL 与 Metal 均 `PATCHED`，用户反馈 QQ、Codex 无异常。清理版 NootedRed 核心需要单独重启验证，不能把此前反馈记为新核心已通过。

构建前先解压 `../NootedRed-Clean-Source.zip`，然后设置：

```sh
GLFIX_DEPS=/path/to/NootedRed-Clean/source \
GLFIX_CLANG=/path/to/clang \
GLFIX_SDK=/path/to/MacOSX.sdk \
python3 build_kext.py
```

输出在 `build/package/Kexts/`，不会安装 EFI。代码页回归用 `test_global_pages.cpp`，既有 GL 指令载荷回归用 `test_gl_blend.c`。常规和 UBSan 页面检查已通过；本机 ASan 在进入 main 前出现 libpthread 初始化错误，未记录为通过。原作者及衍生数据许可证见根目录 `LICENSES/`。
