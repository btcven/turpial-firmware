MODULE = radio_firmware_net

ifneq (,$(filter aodvv2,$(USEMODULE)))
  DIRS += aodvv2
endif
ifneq (,$(filter manet,$(USEMODULE)))
  DIRS += manet
endif
ifneq (,$(filter vaina,$(USEMODULE)))
  DIRS += vaina
endif

include $(RIOTBASE)/Makefile.base
