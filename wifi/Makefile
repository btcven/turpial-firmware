APPLICATION = wifi-subsys

EXTERNAL_BOARD_DIRS += $(CURDIR)/../boards
EXTERNAL_MODULE_DIRS += $(CURDIR)/../core
EXTERNAL_MODULE_DIRS += sys

BOARD ?= turpial-esp32

RIOTBASE ?= $(CURDIR)/../RIOT
APPBASE ?= $(CURDIR)

DEVELHELP ?= 1
QUIET ?= 1

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
USEMODULE += turpial_firmware_coap
USEMODULE += vaina
USEMODULE += cjson
USEMODULE += nvs
USEMODULE += wifi_settings
USEMODULE += slipdev

CFLAGS += "-DESP_WIFI_CHANNEL=1"

# Use UART_DEV(1) for slipdev as a default, baudrate by default is 115200
SLIP_UART ?= 1
CFLAGS += "-DSLIPDEV_PARAM_UART=UART_DEV($(SLIP_UART))"

include $(RIOTBASE)/Makefile.include

# esp32 cpu doesn't support a netopt for WiFi SSID (yet) nor Kconfig.
# TODO: fix this on RIOT.
ifneq ("Turpial_",$(CONFIG_TURPIAL_AP_SSID))
  CFLAGS += -DESP_WIFI_AP_SSID=\"$(CONFIG_TURPIAL_AP_SSID)\"
else
  CFLAGS += -DESP_WIFI_AP_PREFIX=\"Turpial_\"
endif

ifneq ("mypassword",$(CONFIG_TURPIAL_AP_PASSWORD))
  CFLAGS += -DESP_WIFI_PASS=\"$(CONFIG_TURPIAL_AP_PASSWORD)\"
else
  CFLAGS += -DESP_WIFI_PASS=\"mypassword\"
endif
