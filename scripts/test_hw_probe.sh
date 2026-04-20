#!/bin/bash
# 模拟系统驱动给探针发送实时的数据波动
# 由于 IMX6ULL 的内核基本都带有 /dev/udp 支持，我们可以直接利用 bash 发送 UDP 脉冲帧

echo "开始模拟外置硬件通过UDP发送数据到逻辑分析仪端口 (9090)..."
echo "请在板子上切换到 [硬件监控中心 -> 逻辑分析仪(探针)] 界面查看波形"

while true; do
    # 模拟 GPIO1_IO3 的闪烁 (周期2秒)
    val1=$((RANDOM % 2))
    # 兼容支持：尝试使用 bash 自带的 /dev/udp 设备投送
    echo -n "GPIO1_IO3:$val1" > /dev/udp/127.0.0.1/9090 2>/dev/null
    
    # 或者如果你没有 /dev/udp 但有 nc 命令可以使用:
    # echo -n "GPIO1_IO3:$val1" | nc -u -q0 127.0.0.1 9090 2>/dev/null
    
    sleep 0.5
    
    # 模拟另一路 SPI_CLK 的更快跳变 (周期0.2秒左右)
    val2=$((RANDOM % 2))
    echo -n "SPI_CLK:$val2" > /dev/udp/127.0.0.1/9090 2>/dev/null
    sleep 0.1
done
