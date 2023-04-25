# SPDX-License-Identifier: GPL-2.0-only
obj-$(CONFIG_POWER_AVS)		+= avs/
obj-$(CONFIG_POWER_RESET)	+= reset/
obj-$(CONFIG_POWER_SUPPLY)	+= supply/
ifeq ($(CONFIG_OPLUS_SM6375R_CHARGER),y)
obj-$(CONFIG_OPLUS_SM6375R_CHARGER) += oplus_chg/
else
obj-$(CONFIG_OPLUS_SM4350R_CHARGER)	+= oplus/
endif
