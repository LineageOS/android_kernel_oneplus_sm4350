ifeq ($(CONFIG_BUILD_ARM64_DT_OVERLAY),y)
dtbo-$(CONFIG_ARCH_HOLI) += blair-mulan-21095-overlay.dtbo
dtbo-$(CONFIG_ARCH_HOLI) += blair-mulan-21295-overlay.dtbo
dtbo-$(CONFIG_ARCH_HOLI) += blair-mulan-21296-overlay.dtbo
dtbo-$(CONFIG_ARCH_HOLI) += blair-gunnar-20826-overlay.dtbo
dtbo-$(CONFIG_ARCH_HOLI) += blair-gunnar-20825-overlay.dtbo

blair-mulan-21095-overlay.dtbo-base := blair-mulan.dtb
blair-mulan-21295-overlay.dtbo-base := blair-mulan.dtb
blair-mulan-21296-overlay.dtbo-base := blair-mulan.dtb
blair-gunnar-20826-overlay.dtbo-base := blair-mulan.dtb
blair-gunnar-20825-overlay.dtbo-base := blair-mulan.dtb
endif

always := $(dtb-y)
subdir-y := $(dts-dirs)
clean-files := *.dtb *.dtbo
