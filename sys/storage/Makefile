MODULE = tfstorage

ifneq (,$(filter tfnvs,$(USEMODULE)))
  DIRS += nvs
endif

ifneq (,$(filter tfsettings,$(USEMODULE)))
  DIRS += settings
endif

include $(RIOTBASE)/Makefile.base
