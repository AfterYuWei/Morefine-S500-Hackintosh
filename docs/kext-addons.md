# 附加驱动

当前实际 SSD 使用 NVMeFix 1.1.3、官方 AMDRyzenCPUPowerManagement 0.7.2 和 SMCAMDProcessor 1（0.7.2f1 发布）。此前重启已确认加载、服务与初步读数；NVMe APST 和电源代理存在。AMD 驱动会接管 PM dispatch、初始化 P-state 并写入 C-state/HWCR，默认行为已在本机测试前接受。没有加手动调频或强制 ASPM 参数。

这些原始驱动本次保持不变。新清理核心组合仍需重启验证，组件加载结果不代表功耗改善、精度校准或睡眠稳定。需要停用 AMD 部分时应将两个 AMD 条目一起处理，保留 NVMeFix 可独立使用。维护来源见 Sources/Kext-Addons/upstream-assets.json。
