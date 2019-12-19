/**
 * @file Battery.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief
 * @version 0.1
 * @date 2019-11-22
 *
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 *
 */

#include "Battery.h"

#include <ErrUtil.h>

#include "FuelGauge.h"

namespace esc {

static const char* TAG = "ESC";

static xQueueHandle g_bat_evt_queue = nullptr;

static void IRAM_ATTR interruptIsrHandler(void* arg)
{
    xQueueSendFromISR(g_bat_evt_queue, arg, NULL);
}

Battery::Battery()
{
}

esp_err_t Battery::init(gpio_num_t gpout_io_num,
                        std::uint8_t soc_interval,
                        std::uint16_t max_capacity)
{
    static Interrupt g_int_task;
    static gpio_num_t g_gpout_num = gpout_io_num;

    FuelGauge& fuel_gauge = FuelGauge::getInstance();

    ESP_ERR_TRY(fuel_gauge.enterConfig(true));

    ESP_ERR_TRY(fuel_gauge.setCapacity(max_capacity));

    ESP_ERR_TRY(fuel_gauge.setGPOUTPolarity(true));
    ESP_ERR_TRY(fuel_gauge.setGPOUTFunction(SOC_INT));
    ESP_ERR_TRY(fuel_gauge.setSOCIDelta(soc_interval));

    ESP_ERR_TRY(fuel_gauge.exitConfig(true));

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_HIGH_LEVEL;
    io_conf.pin_bit_mask = (1 << static_cast<std::uint32_t>(gpout_io_num));
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;

    ESP_ERR_TRY(gpio_config(&io_conf));
    ESP_ERR_TRY(gpio_set_intr_type(gpout_io_num, GPIO_INTR_HIGH_LEVEL));
    ESP_ERR_TRY(gpio_install_isr_service(0));
    ESP_ERR_TRY(gpio_isr_handler_add(gpout_io_num, interruptIsrHandler, &g_gpout_num));

    g_bat_evt_queue = xQueueCreate(10, sizeof(gpio_num_t));

    g_int_task.start();

    return ESP_OK;
}

Interrupt::Interrupt()
    : Task("Interrupt", 2000, 5)
{
}

void Interrupt::run(void* task_data)
{
    esp_err_t err = ESP_FAIL;
 
    gpio_num_t io_pin = GPIO_NUM_MAX;
    FuelGauge& fuel_gauge = FuelGauge::getInstance();

    std::uint16_t voltage = 0;
    std::int16_t avg_current = 0;
    std::int16_t avg_power = 0;
    std::uint16_t soc = 0;

    while (true) {
        if (xQueueReceive(g_bat_evt_queue, &io_pin, portMAX_DELAY)) {
            ESP_LOGI(TAG, "GPOUT intr, val: %d", gpio_get_level(io_pin));

            err = fuel_gauge.voltage(&voltage);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Can't get voltage, err = %s",
                         esp_err_to_name(err));
                continue;
            }

            err = fuel_gauge.avgCurrent(&avg_current);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Can't get avg. current, err = %s",
                         esp_err_to_name(err));
                continue;
            }

            err = fuel_gauge.avgPower(&avg_power);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Can't get avg. power, err = %s",
                         esp_err_to_name(err));
                continue;
            }

            err = fuel_gauge.soc(SocMeasure::Unfiltered, &soc);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Can't get SoC, err = %s",
                         esp_err_to_name(err));
                continue;
            }

            ESP_LOGI(TAG, "%d mV | %d mA | %d mA | %d %%",
                     voltage,
                     avg_current,
                     avg_power,
                     soc);
        }
    }
}

} // namespace esc
