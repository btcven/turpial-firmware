MODULE = oonf_api

ifneq (,$(filter oonf_common,$(USEMODULE)))
  DIRS += common
endif
ifneq (,$(filter oonf_rfc5444,$(USEMODULE)))
  DIRS += rfc5444
endif

include $(RIOTBASE)/Makefile.base
