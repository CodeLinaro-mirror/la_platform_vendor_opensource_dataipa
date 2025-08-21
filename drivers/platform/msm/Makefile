KP_MODULE_ROOT = $(shell pwd)
#Enabling BAZEL
LOCAL_MODULE_DDK_BUILD := true
LOCAL_MODULE_KO_DIRS := gsi/gsim.ko
LOCAL_MODULE_KO_DIRS += ipa/ipam.ko
LOCAL_MODULE_KO_DIRS += ipa/ipanetm.ko
LOCAL_MODULE_KO_DIRS += ipa/ipatestm.ko

KBUILD_OPTIONS+=KBUILD_DTC_INCLUDE=$(KP_MODULE_ROOT)
KBUILD_OPTIONS += CONFIG_IPA3=m
KBUILD_OPTIONS+=KERNEL_ROOT=$(ROOT_DIR)/$(KERNEL_DIR)

.PHONY: clean all modules install

SRC := $(shell pwd)

all: modules modules_install # dtbs

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) clean

modules:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules $(KBUILD_OPTIONS)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install $(KBUILD_OPTIONS) INSTALL_MOD_PATH=$(INSTALL_MOD_PATH)