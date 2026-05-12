set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

    set(SDK_TARGET_SYSROOT "/opt/fsl-imx-x11/4.1.15-2.1.0/sysroots/cortexa7hf-neon-poky-linux-gnueabi")
    set(CMAKE_SYSROOT ${SDK_TARGET_SYSROOT})
    set(IMX6ULL_ARCH_FLAGS "-mfloat-abi=hard -mfpu=neon-vfpv4 -mcpu=cortex-a7")

    set(CMAKE_C_FLAGS "${IMX6ULL_ARCH_FLAGS} ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${IMX6ULL_ARCH_FLAGS} ${CMAKE_CXX_FLAGS}")
        
    set(ENV{CCACHE_PATH} "/opt/fsl-imx-x11/4.1.15-2.1.0/sysroots/x86_64-pokysdk-linux/usr/bin")
    set(CMAKE_FIND_ROOT_PATH ${SDK_TARGET_SYSROOT})

    set(QT_HOST_PATH "/opt/fsl-imx-x11/4.1.15-2.1.0/sysroots/x86_64-pokysdk-linux/usr/bin")
    set(OE_QMAKE_PATH_EXTERNAL_HOST_BINS "${QT_HOST_PATH}")

    set(QT_MOC_EXECUTABLE "${QT_HOST_PATH}/moc")
    set(QT_UIC_EXECUTABLE "${QT_HOST_PATH}/uic")
    set(QT_RCC_EXECUTABLE "${QT_HOST_PATH}/rcc")
    
    set(Qt5_DIR "${CMAKE_SYSROOT}/usr/lib/cmake/Qt5")
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SYSROOT}/usr/lib/cmake/Qt5")
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SYSROOT}/usr/lib/cmake")