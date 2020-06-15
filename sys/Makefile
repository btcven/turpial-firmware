MODULE = tfsys

ifneq (,$(filter tfnet,$(USEMODULE)))
  DIRS += net
endif

ifneq (,$(filter tfstorage,$(USEMODULE)))
  DIRS += storage
endif

ifneq (,$(filter cjson,$(USEMODULE)))
  DIRS += cjson
endif

include $(RIOTBASE)/Makefile.base
