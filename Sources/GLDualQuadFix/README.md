# GLDualQuadFix 0.1.1 维护源码

独立 Lilu 插件，原理、精确目标及尚未完成的验证见 [候选说明](../../docs/opengl-candidate.md)。本目录内的运行时 C++、汇编和生成页面数据与本机 0.1.1 相同。公开构建脚本仅调整依赖、工具链及输出位置的选择，不改变编译参数、插件逻辑和 IOKit 元数据。

## 文件

- `GLDualQuadFix.cpp`、`gl_page_guard.hpp`、`gl_page_validation.hpp`：插件与精确页面验证。
- `gl_blend_replacement.s`：142 字节候选载荷的汇编源。
- `gl_page_data.hpp`、`gl_blend_bytes.h`：指定 Apple 驱动的匹配数据与本地替换载荷；原生匹配数据不属于本地原创。
- `build_gl_probe.py`：严格核对本机 Apple 驱动 SHA-256 后再生成上述数据，系统版本不匹配即停止；只写本目录生成文件。
- `build_kext.py`：构建、静态分析、ad-hoc 签名及签名检查，仅写构建目录。
- `gl_inspect.c`：与仓库 `Tools/GLDualQuadFix/gl-inspect` 对应的只读检查工具。
- 三个 `test_*` 源码及 `Validation/`：CPU 检查与既有有效记录；未称作 GPU 测试。

## 构建

需要 Python 3、Apple Clang / macOS SDK、Lilu 与 MacKernelSDK。固定依赖已在相邻 `NootedRed-Init2-Source.zip` 的 `source/` 中提供：Lilu `0515f40b7f2a096adc85e832a4c6104fbd07f936`、MacKernelSDK `7af1933c27aefcbdf4809ee44478829aad30f9c1`。保留上游许可，参见根目录 LICENSES。

在仓库根目录解压依赖，再构建：

```sh
mkdir -p Sources/Dependencies
unzip Sources/NootedRed-Init2-Source.zip -d Sources/Dependencies
python3 Sources/GLDualQuadFix/build_kext.py
```

默认输出为 `Sources/GLDualQuadFix/build/package/`。可通过环境变量 `GLFIX_DEPS` 指向包含 `Lilu/`、`MacKernelSDK/` 的 `source` 目录，`GLFIX_CLANG` 指定 clang 可执行文件，`GLFIX_SDK` 指定 SDK 目录，`GLFIX_BUILD` 指定独立输出目录。未指定工具链时使用 `xcrun` 当前选择。

本机原构建工具为 Apple Clang 17 与 SDK 15.5；本次通过公开脚本重新构建，插件二进制与已部署 0.1.1 SHA-256 相同：

```text
43964c3db7a6aff5eeb0212e3c264fd81feb36035d855bc5b278b88d36d125bd
```

使用不同工具链时不保证逐字节重现，必须重新审计。生成页面数据只需在持有精确匹配的 Apple GL 驱动时执行 `python3 Sources/GLDualQuadFix/build_gl_probe.py`，同样支持上述依赖与 clang 路径变量；不要解除版本与完整页面校验来适配未知系统。

检查工具可在本目录运行 `clang -arch x86_64 -O2 gl_inspect.c -o gl-inspect` 重建。它只检查实际加载到新进程中的函数，不自动触发 GPU 渲染。

## 上游

使用 [acidanthera/Lilu](https://github.com/acidanthera/Lilu) 的插件接口与 `plugin_start.cpp`，使用 [MacKernelSDK](https://github.com/acidanthera/MacKernelSDK) 编译。Lilu BSD 许可见 [LICENSES/acidanthera-Lilu-LICENSE.txt](../../LICENSES/acidanthera-Lilu-LICENSE.txt)；现有源码包内保留依赖各自的完整声明。本实验不代表上游认可或提供维护保证。
