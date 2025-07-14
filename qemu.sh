qemu-system-aarch64 \
	-M raspi4 \
	-cpu cortex-a57 \
	-nographic \
	-m 1024 \
	-kernel u-boot-dtb.bin \
	-s -S
