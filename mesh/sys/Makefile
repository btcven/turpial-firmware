MODULE = radio_firmware_sys

ifneq (,$(filter shell_extended,$(USEMODULE)))
  DIRS += shell_extended
endif
ifneq (,$(filter radio_firmware_net,$(USEMODULE)))
  DIRS += net
endif
ifneq (,$(filter oonf_api,$(USEMODULE)))
  DIRS += oonf_api
endif
ifneq (,$(filter radio_firmware_drivers,$(USEMODULE)))
  DIRS += drivers
endif

include $(RIOTBASE)/Makefile.base
