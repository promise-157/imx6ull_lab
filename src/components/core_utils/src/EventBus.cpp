#include "EventBus.h"

// 为了能在跨线程信号槽中作为参数传递特殊的 QVariant类型（如果未来有的话）
// 或者确保 moc 编译器能正确解析 EventBus.h 而占位
// 目前逻辑都实现在了 EventBus.h 头文件的模板中
