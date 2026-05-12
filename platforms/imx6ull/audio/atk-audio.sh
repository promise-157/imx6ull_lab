#!/bin/bash

RECORD_FILE="./record.wav"
PLAY_FILE="./short.mp3"
   
# 检查命令是否存在
check_command() {
    command -v "$1" > /dev/null 2>&1
}

# 初始化音频设备
init_audio_device() {
    amixer cset name='PCM Volume' 182 > /dev/null 2>&1
    amixer cset name='Mono Mux' 'Stereo' > /dev/null 2>&1
    amixer cset name='Playback De-emphasis' 2 > /dev/null 2>&1
    amixer cset name='Capture Digital Volume' 182 > /dev/null 2>&1
    amixer cset name='Capture Mute' 'on' > /dev/null 2>&1
    amixer cset name='Capture Polarity' 'Normal' > /dev/null 2>&1
    amixer cset name='3D Mode' 'No 3D' > /dev/null 2>&1
    amixer cset name='ALC Capture Attack Time' 5 > /dev/null 2>&1
    amixer cset name='ALC Capture Decay Time' 2 > /dev/null 2>&1
    amixer cset name='ALC Capture Function' 'Stereo' > /dev/null 2>&1
    amixer cset name='ALC Capture Hold Time' 2 > /dev/null 2>&1
    amixer cset name='ALC Capture Max PGA' 3 > /dev/null 2>&1
    amixer cset name='ALC Capture Min PGA' 6 > /dev/null 2>&1
    amixer cset name='ALC Capture NG Switch' 'on' > /dev/null 2>&1
    amixer cset name='ALC Capture NG Threshold' 9 > /dev/null 2>&1
    amixer cset name='ALC Capture NG Type' 'Mute ADC Output' > /dev/null 2>&1
    amixer cset name='ALC Capture Target Volume' 12 > /dev/null 2>&1
    amixer cset name='ALC Capture ZC Switch' 'on' > /dev/null 2>&1
    amixer cset name='Left Channel Capture Volume' 100% > /dev/null 2>&1
    amixer cset name='Right Channel Capture Volume' 100% > /dev/null 2>&1
    amixer cset name='Left Mixer Left Bypass Volume' 71% > /dev/null 2>&1
    amixer cset name='Right Mixer Right Bypass Volume' 71% > /dev/null 2>&1
    amixer cset name='Output 1 Playback Volume' 91% > /dev/null 2>&1
    amixer cset name='Output 2 Playback Volume' 91% > /dev/null 2>&1
    amixer cset name='ZC Timeout Switch' 'on' > /dev/null 2>&1
    amixer cset name='Left PGA Mux' 'DifferentialL' > /dev/null 2>&1
    amixer cset name='Right PGA Mux' 'DifferentialR' > /dev/null 2>&1
    amixer cset name='Left Mixer Left Bypass Switch' 'on' > /dev/null 2>&1
    amixer cset name='Right Mixer Right Bypass Switch' 'on' > /dev/null 2>&1
    amixer cset name='Left Mixer Left Playback Switch' 'on' > /dev/null 2>&1
    amixer cset name='Right Mixer Right Playback Switch' 'on' > /dev/null 2>&1
}

function init_board_mic() {
    # 初始化板载麦克风
    check_command amixer && {
        amixer cset name='Differential Mux' 'Line 2' > /dev/null 2>&1
        amixer cset name='Left Line Mux' 'Line 2L' > /dev/null 2>&1
        amixer cset name='Right Line Mux' 'Line 2R' > /dev/null 2>&1
    }
}

function init_line_in() {
    # 初始化Line IN
    check_command amixer && {
        amixer cset name='Differential Mux' 'Line 1' > /dev/null 2>&1
        amixer cset name='Left Line Mux' 'Line 1L' > /dev/null 2>&1
        amixer cset name='Right Line Mux' 'Line 1R' > /dev/null 2>&1
    }
}

function cleanup() {
    # 清理并退出
    printf "\n清理并退出...\n"
    stty sane  # 还原终端状态
    exit 0
}

function apply_config() {
    printf "\n可选麦克风测试项目:\n"
    printf "1. Line IN\n"
    printf "2. 板载麦克风\n"

    while true; do
        read -r -p "请输入您的选择: " choice

        if [[ "$choice" == "1" || "$choice" == "2" ]]; then
            break
        else
            printf "无效输入。请输入1或2。\n"
        fi
    done

    printf "\n应用麦克风配置项 %s\n" "$choice"
    case $choice in
        1)
            init_line_in
            ;;
        2)
            init_board_mic
            ;;
        *)
            printf "无效选项\n"
            ;;
    esac
}

# 捕获Ctrl+C信号，并调用cleanup函数
trap cleanup SIGINT

init_audio_device

while true; do
    printf "\n可选ES8388测试项目:\n"
    printf "1. 录音测试\n"
    printf "2. 播音测试\n"

    while true; do
        read -r -p "输入您需要测试的项目编号（1或2）: " command
        case $command in
            1)
                apply_config
                printf "\n开始录音...\n"
                check_command arecord && arecord -f dat -d 5 "$RECORD_FILE" > /dev/null 2>&1
                printf "\n播放录音...\n"
                check_command aplay && aplay "$RECORD_FILE" > /dev/null 2>&1
                break
                ;;
            2)
                printf "\n开始播音，按 Ctrl+C 可退出播音\n"
                gst-play-1.0 --audiosink="alsasink" "$PLAY_FILE" > /dev/null 2>&1
                break
                ;;
            *)
                cleanup
                ;;
        esac
    done

    break
done


