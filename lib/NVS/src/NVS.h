/**
 * @file NVS.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-20
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef NVS_H
#define NVS_H

#include "esp_err.h"
#include "nvs_flash.h"
#include "Task.h"
namespace nvs {

/**
 * @brief Initialize NVS
 * 
 * @return
 *      - ESP_OK: succeed
 *      - (others): failed
 */
esp_err_t init();

/**
 * @brief Representation of a NVS namespace.
 * 
 */
class Namespace  {
public:
    /**
     * @brief Construct a new Namespace
     * 
     */
    Namespace();

    /**
     * @brief Close the Namespace
     * 
     */
    ~Namespace();

    /**
     * @brief Open namespace
     * 
     * @attention 1. If a namespace is already opened it's going to be clossed.
     * 
     * @param name: namespace name 
     * @param open_mode: opening mode
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed 
     */
    esp_err_t open(const char* name, nvs_open_mode open_mode);

    /**
     * @brief Close the namespace
     * 
     */
    void close();

    /**
     * @brief Set a boolean
     * 
     * @param key: key in the namespace 
     * @param value: boolean value
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t set_bool(const char* key, bool value);

    /**
     * @brief Get a boolean
     * 
     * @param      key: key in the namespace
     * @param[out] value: boolean return value
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t get_bool(const char* key, bool& value);

    /**
     * @brief Commit changes to flash
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t commit();



private:
    bool m_is_opened;
    nvs_handle m_handle;
};

} // namespace nvs

#endif //