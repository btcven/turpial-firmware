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
#include "net/vaina.h"
#include "net/tfcoap.h"
#include "net/gnrc/ipv6/nib.h"

#include "storage/tfnvs.h"
#include "storage/tfsettings.h"

#include "ipv6_autogen.h"

static int wifi_init(void);
static int vaina_init(void);
static void shell_init(void);

static int board_config_cmd(int argc, char **argv);
static int rcs_add_cmd(int argc, char **argv);

/**
 * @brief   Network interfaces PID.
 *
 * These are the available network interfaces on the Turpial device, please
 * note that these values _aren't_ fixed, they _might_ and _will_ change in
 * the future, so don't worry if on the console you get various errors, just
 * update accordingly, this happens because there's still an ongoing process
 * on RIOT to add identifiers to network interfaces so this doesn't has to be
 * done manually in the future.
 *
 * If something has it's own thread and is created before these network
 * interfaces you might want to update them with a fast `ifconfig` and replacing
 * the values yourself here ;).
 *
 * @{
 */
#define ESP_WIFI_AP_IF (9)
#define ESP_SLIPDEV_IF (10)
/** @} */

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static const shell_command_t shell_commands[] = {
    { "board_config", "print board configuration", board_config_cmd },
    { "rcs_add", "Add client to RCS", rcs_add_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("Welcome to Turpial ESP32 MCU!");

    tf_coat_init();

    if (tfnvs_init() < 0){
       printf("Error: Couldn't initialize NVS\n");
    }

    if (vaina_init() < 0) {
        printf("Error: Couldn't initialize VAINA\n");
    }

    if (wifi_init() < 0) {
        printf("Error: Couldn't initialize WiFi\n");
    }

    shell_init();

    /* Should be never reached */
    return 0;
}

static int vaina_init(void)
{
    /* Initialize VAINA client */
    gnrc_netif_t *slipdev_iface = gnrc_netif_get_by_pid(ESP_SLIPDEV_IF);
    if (slipdev_iface == NULL) {
        printf("Error: ESP32 SLIP network interface doesn't exists.\n");
        return -1;
    }

    if (vaina_client_init(slipdev_iface) < 0) {
        printf("Error: failed VAINA initialization.\n");
        return -1;
    }

    return 0;
}

static int wifi_init(void)
{
    if (!IS_ACTIVE(MODULE_ESP_WIFI_AP)) {
        printf("Error: dear user, you need a network interface to access the mesh :=)\n");
        return -1;
    }

    gnrc_netif_t *wifi_iface = gnrc_netif_get_by_pid(ESP_WIFI_AP_IF);
    if (wifi_iface == NULL) {
        printf("Error: ESP32 WiFi SoftAP interface doesn't exists.\n");
        return -1;
    }

    /* TODO: set SSID/PASSWORD */
    tfsettings_ap_t ap;
#if IS_ACTIVE(CONFIG_TURPIAL_STATIC_ADDRESS)
    /* Parse static global address */
    if (ipv6_addr_from_str(&ap.glb, CONFIG_TURPIAL_STATIC_GLOBAL_ADDR) == NULL) {
        printf("Error: Invalid IPv6 static global address\n");
        return -1;
    }
    ap.glb_pfx_len = CONFIG_TURPIAL_GLOBAL_PREFIX;
#else /* IS_ACTIVE(TURPIAL_STATIC_ADDRESS) */
    if (tfsettings_get_ap(&ap) < 0) {
        printf("Error: configuration not saved, saving defaults\n");

        turpial_autogen_ipv6(&ap.glb);
        ap.glb_pfx_len = CONFIG_TURPIAL_GLOBAL_PREFIX;

        if (tfsettings_set_ap(&ap) < 0) {
            printf("Error: could not save default confiugration\n");
        }
    }
#endif /* IS_ACTIVE(TURPIAL_STATIC_ADDRESS) */
    char ipstr[IPV6_ADDR_MAX_STR_LEN];
    printf("Using address %s/%d\n",
           ipv6_addr_to_str(ipstr, &ap.glb, sizeof(ipstr)), ap.glb_pfx_len);


    /* Add node IPv6 global address */
    if (gnrc_netif_ipv6_addr_add(wifi_iface, &ap.glb, ap.glb_pfx_len,
                                 GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID) < 0) {
        printf("Error: Couldn't add IPv6 global address\n");
        return -1;
    }

    /* Set the RTR_ADV flag */
    gnrc_ipv6_nib_change_rtr_adv_iface(wifi_iface, true);

    return 0;
}

static void shell_init(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    /* Start shell */
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
}

static int board_config_cmd(int argc, char **argv)
{
    print_board_config();
    return 0;
}

static int rcs_add_cmd(int argc, char **argv)
{
    ipv6_addr_t addr;

    if (argc < 2) {
        printf("Usage: rcs_add <address>\n");
        return -1;
    }

    if (ipv6_addr_from_str(&addr, argv[1]) == NULL) {
        printf("Invalid IPv6 address!\n");
        return -1;
    }

    vaina_msg_t msg = {
        .msg = VAINA_MSG_RCS_ADD,
        .seqno = vaina_seqno(),
    };
    msg.payload.rcs_add.ip = addr;

    if (vaina_client_send(&msg) < 0) {
        printf("Error: couldn't send VAINA message!\n");
        return -1;
    }

    printf("Success: sent rcs_add with addr %s\n", argv[1]);

    return 0;
}
