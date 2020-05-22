/**
 * @{
 * @file
 * @author      Locha Mesh Developers (contact@locha.io)
 * @brief       Main firmware file
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "msg.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

int main(void)
{
    puts("Welcome to Turpial ESP32 MCU!");

    if (!IS_ACTIVE(MODULE_ESP_WIFI_AP)) {
        printf("Error: dear user, you need a network interface to access the mesh :=)\n");
    }

    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    /* Start shell */
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* Should be never reached */
    return 0;
}
