APPLICATION = mesh-subsys
BOARD ?= turpial-cc1312

EXTERNAL_BOARD_DIRS ?= $(CURDIR)/../boards
EXTERNAL_MODULE_DIRS += $(CURDIR)/../core
EXTERNAL_MODULE_DIRS += sys

RIOTBASE ?= $(CURDIR)/../RIOT
APPBASE ?= $(CURDIR)
RADIOBASE ?= $(CURDIR)

DEVELHELP ?= 1
QUIET ?= 1

# Initialize GNRC netif
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif

USEMODULE += gnrc_ipv6_router
USEMODULE += gnrc_icmpv6
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_udp
USEMODULE += gnrc_pktdump

USEMODULE += manet
USEMODULE += aodvv2
USEMODULE += shell_extended
USEMODULE += vaina

USEMODULE += ps
USEMODULE += netstats_l2
USEMODULE += netstats_ipv6

USEMODULE += slipdev

USE_SLIPTTY ?= 0
ifeq (1,$(USE_SLIPTTY))
  USEMODULE += slipdev_stdio

  SLIP_UART     ?= "0"
  SLIP_BAUDRATE ?= 115200
else
  SLIP_UART     ?= "1"
  SLIP_BAUDRATE ?= 115200
endif

# export slip parameters
CFLAGS += -DSLIPDEV_PARAM_UART="UART_DEV($(SLIP_UART))"
CFLAGS += -DSLIPDEV_PARAM_BAUDRATE=$(SLIP_BAUDRATE)

ifeq (1,$(USE_SLIPTTY))
.PHONY: host-tools

HOST_TOOLS=sliptty

host-tools:
	$(Q)env -u CC -u CFLAGS make -C $(RIOTTOOLS)

sliptty:
	$(Q)env -u CC -u CFLAGS make -C $(RIOTTOOLS)/sliptty

  IPV6_PREFIX = "fc00::/16"

# Configure terminal parameters
  TERMDEPS += host-tools
  TERMPROG ?= sudo sh $(CURDIR)/dist/tools/vaina/start_network.sh
  TERMFLAGS ?= $(FLAGS_EXTRAS) $(IPV6_PREFIX) $(PORT) $(SLIP_BAUDRATE)
endif

include $(RIOTBASE)/Makefile.include
