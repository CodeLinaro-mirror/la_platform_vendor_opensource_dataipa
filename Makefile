# SPDX-License-Identifier: GPL-2.0-only

all:
	cd drivers/platform/msm/ && $(MAKE)

obj-$(CONFIG_IPA3) += drivers/platform/msm/
