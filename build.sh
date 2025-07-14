bear make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- clean
bear make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- mrproper
bear make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- pi4_defconfig
#bear make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- menuconfig
bear make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- -j$(nproc) V=1
