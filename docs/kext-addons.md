# NVMe 与 CPU 电源管理

| 组件 | 版本 | 作用 |
| --- | --- | --- |
| NVMeFix | 1.1.3 | 第三方 NVMe 电源管理 |
| AMDRyzenCPUPowerManagement | 0.7.2 | CPU 电源管理与数据采集 |
| SMCAMDProcessor | 1.0.1 | 向 VirtualSMC 提供 CPU 传感器 |

两个 AMD 组件来自官方 `0.7.2f1` 发布。AMDRyzenCPUPowerManagement 包含默认电源控制行为，会接管 PM dispatch、初始化 P-state 并设置 C-state/HWCR；本配置使用默认行为，未添加手动调频或强制 ASPM 参数。

组件级检查已确认驱动加载、服务与初步读数，NVMe APST 和 NVMePMProxy 存在。读数精度、实际节能效果、睡眠唤醒及长期稳定性尚未完成验证。详细结果见 [附加驱动验证摘要](kext-addons-validation.json)。

停用 AMD 部分时需同时处理两个 AMD 条目；NVMeFix 可独立使用。下载与固定来源见 [组件来源](../Sources/Kext-Addons/upstream-assets.json)。
