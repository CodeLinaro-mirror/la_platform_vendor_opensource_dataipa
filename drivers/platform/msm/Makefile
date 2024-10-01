ifeq ($(KP_MODULE_ROOT),)
KP_MODULE_ROOT=$(KERNEL_SRC)/$(M)
endif

#Enabling BAZEL
LOCAL_MODULE_DDK_BUILD := true
LOCAL_MODULE_KO_DIRS := gsi/gsim.ko
LOCAL_MODULE_KO_DIRS += ipa/ipam.ko
LOCAL_MODULE_KO_DIRS += ipa/ipanetm.ko
LOCAL_MODULE_KO_DIRS += ipa/ipatestm.ko

KBUILD_OPTIONS+=KBUILD_DTC_INCLUDE=$(KP_MODULE_ROOT)
KBUILD_OPTIONS += CONFIG_IPA3=m

all: modules # dtbs

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) clean

%:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) $@ $(KBUILD_OPTIONS)
