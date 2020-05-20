/**
 * Copyright 2020 btcven and Locha Mesh developers
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** 
 * @file Storage.h
 * @author Locha Mesh Developers (contact@locha.io)
 *  
 */

#ifndef NVS_H
#define NVS_H

#include <esp_err.h>
#include <nvs_flash.h>
#include <string>

namespace storage
{
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
    class NVS
    {
    public:
        /**
     * @brief Construct a new Namespace
     * 
     */
        NVS();

        /**
     * @brief Close the Namespace
     * 
     */
        ~NVS();

        /**
     * @brief Open namespace
     * 
     * @attention 1. If a namespace is already opened it's going to be closed.
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
     * @brief Checks whether the Turpial device is configured or not
     * 
     * @param is_configured: return value
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
        esp_err_t getIsConfigured(bool& is_configured);

        /**
     * @brief Set the String object
     * 
     * @param key 
     * @param username 
     * @return esp_err_t 
     */
        esp_err_t setString(std::string key, std::string data);

        /**
     * @brief Get the String object
     * 
     * @param key 
     * @param data 
     * @return esp_err_t 
     */
        esp_err_t getString(std::string key, char* buffer, size_t* length);

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

} // namespace storage

#endif //
