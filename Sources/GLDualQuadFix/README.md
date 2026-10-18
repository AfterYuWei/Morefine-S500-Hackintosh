# GLDualQuadFix 0.2.0

GLDualQuadFix 提供应用通用的 OpenGL / Metal 渲染修补，通过同一个 `cs_validate_page` 接口处理一个 GL 代码页和两个 Metal 共享缓存代码页。修补范围由驱动页面确定，与应用进程名称无关。

原生页面验证先运行；有效、未污染、可执行页面还必须匹配文件路径、文件页偏移与完整 4096 字节内容。只支持当前 Darwin 25.6、单一 `1002:15e7` 核显及记录中的共享缓存。其他版本或内容拒绝修补。本插件不调用进程签名放行接口，不改写磁盘上的系统文件。

`global_page_data.hpp` 为编译使用的页面数据；`active-payloads.json` 记录四段载荷，`generate_pages.py` 用于生成对应头文件。生成器读取本机系统页，支持识别文件缓存中已修补的已知状态。

2026-10-18 的重启检查已确认当前 NootedRed 核心与 GLDualQuadFix 0.2.0 加载，GL 与 Metal 均 `PATCHED`。应用回归和长期稳定性需分别验证，详见 [验证摘要](../../docs/current-validation.json)。

构建前先解压 `../NootedRed-Clean-Source.zip`，然后设置：

```sh
GLFIX_DEPS=/path/to/NootedRed-Clean/source \
GLFIX_CLANG=/path/to/clang \
GLFIX_SDK=/path/to/MacOSX.sdk \
python3 build_kext.py
```

输出在 `build/package/Kexts/`，不会安装 EFI。代码页回归用 `test_global_pages.cpp`，既有 GL 指令载荷回归用 `test_gl_blend.c`。常规和 UBSan 页面检查已通过；本机 ASan 在进入 main 前出现 libpthread 初始化错误，未记录为通过。原作者及衍生数据许可证见根目录 `LICENSES/`。
