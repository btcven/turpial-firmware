MODULE = turpial_firmware_sys

ifneq (,$(filter turpial_firmware_net,$(USEMODULE)))
  DIRS += net
endif

ifneq (,$(filter turpial_firmware_storage,$(USEMODULE)))
  DIRS += storage
endif

<<<<<<< HEAD
=======
ifneq (,$(filter cjson,$(USEMODULE)))
  DIRS += cjson
endif
>>>>>>> 2020_06_03_add_cjson

include $(RIOTBASE)/Makefile.base
