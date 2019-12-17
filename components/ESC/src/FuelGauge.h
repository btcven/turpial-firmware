#ifndef FUEL_GAUGE_H
#define FUEL_GAUGE_H
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"


#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */

#define I2C_MASTER_TX_BUF_ENABLE 1 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_ENABLE 1 /*!< I2C master doesn't need buffer */

#define I2C_MASTER_ACK_EN true   /*!< Enable ack check for master */
#define I2C_MASTER_ACK_DIS false /*!< Disable ack check for master */


namespace esc {


uint8_t fuelgauge_addr = 0x55;
i2c_mode_t esc_i2c_mode = I2C_MODE_MASTER;
i2c_port_t esc_i2c_port = I2C_NUM_0;

gpio_num_t esc_sda_pin = GPIO_NUM_23;
gpio_num_t esc_scl_pin = GPIO_NUM_22;

class FuelGauge
{
private:
    i2c_config_t m_conf;
    i2c_port_t m_i2c_master_port = esc_i2c_port;
    uint32_t esc_i2c_frequency = 100000;
    uint32_t m_ticksToWait = 10;
    int32_t m_timeout = 2000;

public:
    /**
     * @brief Construct a new Fuel Gauge object
     * 
     */
    FuelGauge();
    /**
     * @brief Init i2c iface
     * 
     * @return esp_err_t 
     */
    esp_err_t i2cInit();
    /**
     * @brief delete i2c iface
     * 
     * @return esp_err_t 
     */
    esp_err_t i2cDelete();
    /**
     * @brief read Word
     * 
     * @param command 
     * @return uint16_t 
     */
    uint16_t readWord(uint8_t command);
    /**
     * @brief Get the Voltage object
     * 
     * @return int16_t 
     */
    int16_t getVoltage();
    /**
     * @brief Get the Avg Current object
     * 
     * @return int16_t 
     */
    int16_t getAvgCurrent();
    /**
     * @brief Get the Avg Power object
     * 
     * @return int16_t 
     */
    int16_t getAvgPower();
    /**
     * @brief Destroy the Fuel Gauge object
     * 
     */
    ~FuelGauge();
};


} // namespace esc


#endif //FUEL_GAUGE_H
