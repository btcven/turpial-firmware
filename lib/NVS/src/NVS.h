/**
 * @file NVS.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-20
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#ifndef NVS_H
#define NVS_H

#include <cstddef>
#include <cstdint>
#include <vector>

#include "nvs_flash.h"

namespace storage {

class NVS
{
public:
    /**
     * @brief Construct a new NVS object
     * 
     */
    NVS();

    NVS(NVS const&) = delete;
    NVS& operator=(NVS const&) = delete;

    /**
     * @brief Destroy the NVS object
     * 
     */
    ~NVS();

    /**
     * @brief Initialize NVS
     * 
     * @attention 1. Before using any NVS API this must be called.
     * 
     * @return esp_err_t 
     */
    static esp_err_t start();

    /**
     * @brief Open a namespace
     * 
     * @param name: namespace
     * @param mode: open mode
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t open(const char* name, nvs_open_mode mode);

    /**
     * @brief Set the a blob
     * 
     * @param key: key name
     * @param data: blob data
     * @param len: blob len
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t setBlob(const char* key, const std::uint8_t* data, std::size_t len);

    /**
     * @brief Get a blob
     * 
     * @param      key: key name
     * @param[out] data: blob data
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t getBlob(const char* key, std::vector<uint8_t>& data);

    /**
     * @brief Set an std::uint8_t
     * 
     * @param key: key name
     * @param value: value
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed 
     */
    esp_err_t setU8(const char* key, std::uint8_t value);

    /**
     * @brief Get an std::uint8_t
     * 
     * @param      key: key name
     * @param[out] value: return value
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t getU8(const char* key, std::uint8_t& value);

    /**
     * @brief Commit changes to flash storage
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t commit();

private:
    nvs_handle m_handle;
    bool m_is_opened;
};

} // namespace storage

#endif // NVS_H