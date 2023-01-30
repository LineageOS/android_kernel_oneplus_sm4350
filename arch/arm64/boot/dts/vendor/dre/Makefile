ifeq ($(CONFIG_BUILD_ARM64_DT_OVERLAY),y)
dtbo-$(CONFIG_ARCH_HOLI) += \
	dre-20818-mtp-overlay.dtbo \

dre-20818-mtp-overlay.dtbo-base := holi.dtb

else
dtb-$(CONFIG_ARCH_HOLI) += holi-rumi.dtb \
	holi-mtp.dtb \

endif


always		:= $(dtb-y)
subdir-y	:= $(dts-dirs)
clean-files	:= *.dtb *.dtbo
