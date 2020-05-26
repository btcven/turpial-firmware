/**
 * @defgroup    main
 * @ingroup     main
 *
 * @{
 *
 * @file
 * @brief       Main firmware file
 * @author      Locha Mesh Developers (contact@locha.io)
 * @}
 */

#include <stdio.h>

#include "net/gnrc/netif.h"
#include "net/netif.h"

#include "shell.h"
#include "msg.h"

#define ESP_WIFI_AP_NAME "8"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static int board_config_cmd(int argc, char **argv)
{
    print_board_config();
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "board_config", "print board configuration", board_config_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("Welcome to Turpial ESP32 MCU!");

    if (!IS_ACTIVE(MODULE_ESP_WIFI_AP)) {
        printf("Error: dear user, you need a network interface to access the mesh :=)\n");
    }

    netif_t *wifi_iface = netif_get_by_name(ESP_WIFI_AP_NAME);
    if (wifi_iface == NULL) {
        printf("Error: ESP32 WiFi SoftAP interface doesn't exists.\n");
    }
    else {
        ipv6_addr_t addr;
        if (ipv6_addr_from_str(&addr, CONFIG_TURPIAL_GLOBAL_ADDR) == NULL) {
            printf("Error: Invalid address");
        }

        uint16_t flags = GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID |
                         (CONFIG_TURPIAL_GLOBAL_PREFIX << 8);
        if (netif_set_opt(wifi_iface, NETOPT_IPV6_ADDR, flags, &addr,
                          sizeof(addr)) < 0) {
            printf("Error: Couldn't add global IPv6 address");
        }

        /* Set the RTR_ADV flag */
        netopt_enable_t set = NETOPT_ENABLE;
        if (netif_set_opt(wifi_iface, NETOPT_IPV6_SND_RTR_ADV, 0, &set,
                          sizeof(netopt_enable_t)) < 0) {
            printf("Error: Couldn't set RTR_ADV flag on ESP32 SoftAP\n");
        }
    }

    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    /* Start shell */
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* Should be never reached */
    return 0;
}
