/**
 * @file main.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-09-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */

/* #include <cstdio>
#include <memory>
#include <sstream>

#include "esp_log.h"
#include "sdkconfig.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "BLEPreferences.h"
#include "NVS.h"
#include "WiFi.h"

#include "defaults.h"

static const char* TAG = "app_main";

esp_err_t getIsConfigured(bool& is_configured)
{
    esp_err_t err;

    storage::NVS app_nvs;
    err = app_nvs.open(NVS_APP_NAMESPACE, NVS_READWRITE);
    if (err != ESP_OK) {
        const char* err_str = esp_err_to_name(err);
        ESP_LOGE(TAG,
            "Couldn't open namespace \"%s\" (%s)",
            NVS_APP_NAMESPACE,
            err_str);
        return err;
    }

    err = app_nvs.get_bool(NVS_IS_CONFIGURED_KEY, is_configured);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        // Set is_configured to true on flash so on next init the config is
        // readed directly by the ESP-IDF Wi-Fi library component.
        err = app_nvs.set_bool(NVS_IS_CONFIGURED_KEY, true);
        if (err != ESP_OK) return err;
        err = app_nvs.commit();
        if (err != ESP_OK) return err;
        // Set the return variable to "false" to forcibly set the default
        // configuration
        is_configured = false;
    } else {
        return err;
    }

    return ESP_OK;
}


extern "C" void app_main()
{
    esp_err_t err;

    err = storage::init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't initialize NVS, error (%s)", esp_err_to_name(err));
        return;
    }

    bool is_configured = false;
    err = getIsConfigured(is_configured);
    if (err != ESP_OK) {
        ESP_LOGE(TAG,
            "Couldn't get \"is_configured\" value (%s)",
            esp_err_to_name(err));
    }

    network::WiFi& wifi = network::WiFi::getInstance();
    err = wifi.init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't initalize Wi-Fi interface (%s)", esp_err_to_name(err));
        return;
    }

    if (!is_configured) {
        wifi.setMode(WIFI_MODE);

        network::APConfig ap_config = {
            .ssid = WAP_SSID,
            .password = WAP_PASS,
            .authmode = WAP_AUTHMODE,
            .max_conn = WAP_MAXCONN,
            .channel = WAP_CHANNEL,
        };
        wifi.setApConfig(ap_config);

        network::STAConfig sta_config = {
            .ssid = WST_SSID,
            .password = WST_PASS,
        };
        wifi.setStaConfig(sta_config);
    }

    err = wifi.start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't start Wi-Fi, err = %s", esp_err_to_name(err));
        return;
    }

    ble::ServerParams server_params;
    server_params.device_name = "Turpial-1234";
    server_params.static_passkey = 123456;
    server_params.app_id = 0;
    ble_preferences::start(server_params);
} */





#include "driver/uart.h"
#include "esp_console.h"
#include "esp_log.h"
#include "esp_vfs_dev.h"
#include "esp_vfs_fat.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"



int runShow(int argc, char *argv[]) {
  printf("Found show!\n");
  return 0;
}

int runGreet(int argc, char *argv[]) {
  printf("Found Greet!\n");
  return 0;
}

static void initialize_console() {
  // Disable buffering on stdin
  setvbuf(stdin, NULL, _IONBF, 0);

  // Minicom, screen, idf_monitor send CR when ENTER key is pressed
  esp_vfs_dev_uart_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
  // Move the caret to the beginning of the next line on '\n'
  esp_vfs_dev_uart_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

  // Configure UART. Note that REF_TICK is used so that the baud rate remains
  //  correct while APB frequency is changing in light sleep mode.

/*   uart_config_t uart_config;// = {
    uart_config.baud_rate = CONFIG_CONSOLE_UART_BAUDRATE;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    
    //uart_config.use_ref_tick = true;
  ESP_ERROR_CHECK(uart_param_config(static_cast<uart_port_t>(0), &uart_config));

  // Install UART driver for interrupt-driven reads and writes 
  ESP_ERROR_CHECK(uart_driver_install(static_cast<uart_port_t>(0), 256, 0, 0, NULL, 0));

  // Tell VFS to use UART driver
  esp_vfs_dev_uart_use_driver(CONFIG_CONSOLE_UART_NUM); */

  uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));
    //uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, 1024 * 2, 0, 0, NULL, 0));

  // Initialize the console
  esp_console_config_t console_config; //= {
    console_config.max_cmdline_args = 8,
    console_config.max_cmdline_length = 256,
    console_config.hint_color = atoi(LOG_COLOR_CYAN);

  ESP_ERROR_CHECK(esp_console_init(&console_config));

  linenoiseSetMultiLine(1);

  //esp_console_register_help_command();

  esp_console_cmd_t consoleCmd;
  consoleCmd.command = "show";
  consoleCmd.func = runShow;
  consoleCmd.help = "Show something";
  // consoleCmd.argtable = show_argtable;
  esp_console_cmd_register(&consoleCmd);

  consoleCmd.command = "greet";
  consoleCmd.func = runGreet;
  consoleCmd.help = "Greet someone";
  // consoleCmd.argtable = greet_argtable;
  esp_console_cmd_register(&consoleCmd);

linenoiseClearScreen();
  linenoiseSetMultiLine(0);
}

extern "C" void app_main() {

  initialize_console();

  // Register commands
  esp_console_register_help_command();
  //register_system();

  const char *prompt = LOG_COLOR_I "esp32> " LOG_RESET_COLOR;

  // Main loop
  while (true) {
  
    // Get a line using linenoise.
    // The line is returned when ENTER is pressed.
    char *line = linenoise(prompt);
    if (line == NULL) { // Ignore empty lines
      continue;
    }
    // Try to run the command
    int ret;
    esp_err_t err = esp_console_run(line, &ret);
    if (err == ESP_ERR_NOT_FOUND) {
      printf("Unrecognized command\n");
    } else if (err == ESP_ERR_INVALID_ARG) {
      // command was empty
    } else if (err == ESP_OK && ret != ESP_OK) {
      printf("Command returned non-zero error code: 0x%x (%s)\n", ret,
             esp_err_to_name(err));
    } else if (err != ESP_OK) {
      printf("Internal error: %s\n", esp_err_to_name(err));
    }
    // linenoise allocates line buffer on the heap, so need to free it
    linenoiseFree(line);
  }
}
