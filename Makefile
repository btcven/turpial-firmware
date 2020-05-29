# name of your application
APPLICATION = radio-firmware

# If no BOARD is found in the environment, use this default:
BOARD ?= esp32-wrover-kit
EXTERNAL_BOARD_DIRS ?= $(CURDIR)/boards

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/RIOT

# Application absolute path
APPBASE ?= $(CURDIR)

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

EXTERNAL_MODULE_DIRS += sys

# Initialize GNRC netif
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif

USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_udp
USEMODULE += gnrc_pktdump
USEMODULE += gnrc_icmpv6_echo

USEMODULE += netstats_l2
USEMODULE += netstats_ipv6

USEMODULE += shell
USEMODULE += shell_commands

USEMODULE += esp_wifi_ap

USEMODULE += vaina

USEMODULE += slipdev

UART1_TXD ?= "GPIO21"
UART1_RXD ?= "GPIO22"

# UART_DEV(1) pin definitions, can't be changed through Kconfig
# TODO: open issue in RIOT to see if this is possible.
CFLAGS += "-DUART1_TXD=$(UART1_TXD)"
CFLAGS += "-DUART1_RXD=$(UART1_RXD)"

CFLAGS += "-DESP_WIFI_CHANNEL=1"

# Use UART_DEV(1) for slipdev as a default, baudrate by default is 115200
SLIP_UART ?= 1
CFLAGS += "-DSLIPDEV_PARAM_UART=UART_DEV($(SLIP_UART))"

include $(RIOTBASE)/Makefile.include

