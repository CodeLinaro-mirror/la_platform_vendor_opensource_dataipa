# SPDX-License-Identifier: GPL-2.0-only

obj-m += ipa_stats.o
ipa_stats-y := src/ipa_stats_genl.o

ccflags-y := -I$(src)/inc -I$(src)/../drivers/platform/msm/include -I$(src)/../exports

KBUILD_EXTRA_SYMBOLS += $(abspath $(M)/../Module.symvers)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean
