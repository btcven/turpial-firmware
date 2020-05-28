MODULE = turpial_firmware_storage

ifneq (,$(filter vaina,$(USEMODULE)))
  DIRS += nvs
endif


ifneq (,$(filter wifi_settings,$(USEMODULE)))
  DIRS += wifiSettings
endif

include $(RIOTBASE)/Makefile.base