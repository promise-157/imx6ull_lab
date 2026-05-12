#!/bin/bash

echo "========================================================"
echo "    i.MX6ULL 硬件资源一键全扫探雷器 - 究极进化版 (By Copilot)   "
echo "========================================================"

echo -e "\n[🌟 多媒体与视觉类]"
if [ -d /sys/class/graphics ]; then
    for fb in /sys/class/graphics/fb*; do
        [ -d "$fb" ] || continue
        name=$(cat $fb/name 2>/dev/null)
        echo "  👉 屏幕控制器 (FrameBuffer): [$name]"
        echo "     [C++ 路径]: 操作 /dev/$(basename $fb) 进行 mmap() 显存映射或 Qt QPA 绑定"
    done
fi
if [ -d /sys/class/video4linux ]; then
    for cam in /sys/class/video4linux/video*; do
        [ -d "$cam" ] || continue
        name=$(cat $cam/name 2>/dev/null)
        echo "  👉 摄像头输入 (V4L2): [$name]"
        echo "     [C++ 路径]: open(\"/dev/$(basename $cam)\")，使用 ioctl(VIDIOC_*) 操作"
    done
fi
if [ -d /sys/class/sound ]; then
    for snd in /sys/class/sound/card*; do
        [ -d "$snd" ] || continue
        id=$(cat $snd/id 2>/dev/null)
        echo "  👉 声卡音频设备 (ALSA): [声卡名称 $id]"
        echo "     [C++ 路径]: 见 /dev/snd/，编程建议使用 ALSA 库 (libasound)"
    done
fi

echo -e "\n[🌟 标准总线与高速通信类]"
for tty in /dev/ttymxc*; do
    [ -e "$tty" ] || continue
    echo "  👉 硬件串口 (UART): [$tty] - SoC直出终端或外挂模块"
done
if [ -d /sys/class/net ]; then
    for net in /sys/class/net/*; do
        [ -d "$net" ] || continue
        name=$(basename $net)
        [ "$name" = "lo" ] && continue
        echo "  👉 网络接口 (ETH/CAN/WIFI): [$name]"
    done
fi
if [ -d /sys/bus/spi/devices ]; then
    for spi in /sys/bus/spi/devices/*; do
        [ -d "$spi" ] || continue
        dir_name=$(basename $spi)
        if echo "$dir_name" | grep -q "spi"; then
            echo "  👉 SPI 片选设备: [$dir_name] (如 SPI-Flash/高速ADC)"
            echo "     [C++ 路径]: /dev/spidev$dir_name (若开启了 spidev 驱动)"
        fi
    done
fi
if [ -d /sys/bus/i2c/devices ]; then
    for dev in /sys/bus/i2c/devices/*; do
        [ -d "$dev" ] || continue
        dir_name=$(basename $dev)
        if echo "$dir_name" | grep -q "-"; then
            chip_name=$(cat $dev/name 2>/dev/null || echo "Unknown")
            echo "  👉 I2C 从机实体: [$chip_name] (总线地址 $dir_name)"
            if [ -e "/dev/$chip_name" ]; then
                echo "     [🔥 发现专属节点]: 厂商写了专属闭源/自定义驱动！直接 open(\"/dev/$chip_name\") 读写！"
            elif ls /dev/*$chip_name* 1> /dev/null 2>&1; then
                 f=$(ls /dev/*$chip_name* | head -n 1)
                 echo "     [🔥 发现模糊匹配节点]: 可能是 $f，可以直接使用 open() 操作。"
            else
                echo "     [提示]: 未发现专属设备节点，需打开 /dev/i2c-$(echo $dir_name | cut -d'-' -f1) 自写 I2C 报文收发。"
            fi
        fi
    done
fi

echo -e "\n[🌟 基础 IO 引脚类 (GPIO)]"
if [ -d /sys/class/gpio ]; then
    echo "  👉 GPIO 控制器 (芯片基座):"
    for chip in /sys/class/gpio/gpiochip*; do
        [ -d "$chip" ] || continue
        label=$(cat $chip/label 2>/dev/null)
        base=$(cat $chip/base 2>/dev/null)
        ngpio=$(cat $chip/ngpio 2>/dev/null)
        echo "     - [${label}] (基址: $base, 引脚数: $ngpio) -> 对应新版接口 /dev/$(basename $chip)"
    done
    
    exported=$(ls -d /sys/class/gpio/gpio[0-9]* 2>/dev/null)
    if [ -n "$exported" ]; then
        echo "  👉 已导出的野生 GPIO 引脚:"
        for g in $exported; do
            dir=$(basename $g)
            dir_in_out=$(cat $g/direction 2>/dev/null)
            echo "     - [$dir] (方向: $dir_in_out) -> [C++ 路径]: 读写 $g/value"
        done
    else
        echo "     [提示]: 当前没有被 sysfs 激活导出的散装 GPIO 引脚 (需 echo 编号到 export 导出)。"
    fi
fi

echo -e "\n[🌟 交互与外设控制类]"
if [ -d /sys/class/input ]; then
    for event in /sys/class/input/event*; do
        [ -e "$event" ] || continue
        dev_name=$(cat ${event}/device/name 2>/dev/null || echo "Unknown")
        echo "  👉 触摸屏/鼠标键盘: [$dev_name]"
        echo "     [C++ 路径]: 操作 /dev/input/$(basename $event) 监听各类 input_event"
    done
fi
if [ -d /sys/class/pwm ]; then
    for pwm in /sys/class/pwm/pwmchip*; do
        [ -d "$pwm" ] || continue
        echo "  👉 脉宽调节器 (PWM): [$(basename $pwm)] (常用于电机、呼吸灯及LCD背光调光)"
        echo "     [C++ 路径]: /sys/class/pwm/$(basename $pwm)/ (通过 echo 写入周期与占空比)"
    done
fi
if [ -d /sys/class/leds ]; then
    for led in /sys/class/leds/*; do
        [ -d "$led" ] || continue
        echo "  👉 LED 指示状态灯: [$(basename $led)]"
        echo "     [C++ 路径]: /sys/class/leds/$(basename $led)/brightness"
    done
fi

echo -e "\n[🌟 工业监测与杂项类]"
if [ -d /sys/class/iio ]; then
    for iio in /sys/class/iio/iio:device*; do
        [ -d "$iio" ] || continue
        name=$(cat $iio/name 2>/dev/null)
        echo "  👉 IIO 规范传感器 (陀螺仪/光感/磁力计): [$name]"
    done
fi
if [ -d /sys/class/hwmon ]; then
    for hw in /sys/class/hwmon/hwmon*; do
        [ -d "$hw" ] || continue
        name=$(cat $hw/name 2>/dev/null)
        echo "  👉 硬件环境监测 (温度/电压): [$name] ($hw)"
    done
fi
if [ -d /sys/class/rtc ]; then
    for rtc in /sys/class/rtc/rtc*; do
        [ -d "$rtc" ] || continue
        name=$(cat $rtc/name 2>/dev/null)
        echo "  👉 实时硬件时钟 (RTC): [$(basename $rtc) $name] - $(cat $rtc/date) $(cat $rtc/time)"
    done
fi
if [ -d /sys/class/watchdog ]; then
    for wdg in /sys/class/watchdog/watchdog*; do
        [ -d "$wdg" ] || continue
        echo "  👉 硬件看门狗 (WatchDog): [$(basename $wdg)] -> 对应 /dev/$(basename $wdg)"
    done
fi

if [ -d /sys/class/misc ]; then
    echo -n "  👉 其他离散字符设备 (Misc): "
    misc_devs=""
    for m in /sys/class/misc/*; do
        [ -d "$m" ] || continue
        misc_devs="$misc_devs [$(basename $m)]"
    done
    echo "$misc_devs"
fi

echo -e "\n========================================================"
echo "🎯 全盘扫描完毕！"
echo "========================================================"
