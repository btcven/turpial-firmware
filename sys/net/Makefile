MODULE = turpial_firmware_net

ifneq (,$(filter vaina,$(USEMODULE)))
  DIRS += vaina
endif

ifneq (,$(filter turpial_firmware_coap,$(USEMODULE)))
  DIRS += coap
endif

include $(RIOTBASE)/Makefile.base
