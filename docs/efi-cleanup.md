# 当前 EFI 清理

以真实SSD106文件为基线，不再以仓库旧混合EFI拼接。删除15个停用模块源码/头文件、对应入口、Xcode引用与旧实验工具；NootedRed从28个C++单元减为25个。保留默认VCN关闭时的引擎分配/能力行为。

配置删除一条MaxKernel=25.3.99的旧TSC补丁，当前Darwin25.6有效补丁集合未变；修正旧NootedRed版本、OpenCanopy RELEASE和蓝牙用途注释，将旧USB测试Shell名称改为UEFI Shell。已启用18个kext、三个SSDT、三个UEFI驱动与图形资源保留。

清理后仍106文件，GLDualQuadFix0.2.0逐字节不变。公开EFI与EFI.zip只有身份字段被脱敏；私人完整ZIP和最新组件回退留在本机。旧公开EFI ZIP、旧Init2源码ZIP和过时GL0.1.1说明从当前树移除，Git历史保留。

清理版核心的重新构建、CPU回归、初始化审计、静态分析、签名、ocvalidate与发布脱敏检查需要全部记录；重启/应用长期验证单独记录，不能由编译或历史反馈代替。
