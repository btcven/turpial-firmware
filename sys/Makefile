MODULE = turpial_firmware_sys

ifneq (,$(filter turpial_firmware_net,$(USEMODULE)))
  DIRS += net
  DIRS += storage
endif

include $(RIOTBASE)/Makefile.base
