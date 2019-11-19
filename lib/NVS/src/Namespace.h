#ifndef NVS_NAMESPACE_H
#define NVS_NAMESPACE_H

#include "Arduino.h"
#include "nvs_flash.h"
#include <iostream>
#include <nvs.h>

namespace nvs {

/**
 * @brief Represents a RAII version of a NVS namespace.
 * 
 */
class Namespace
{
public:
    /**
     * @brief Construct a new Namespace object ready to be opened.
     * 
     */
    Namespace();

    /**
     * @brief Destroy the Namespace object. Closes the namespace.
     * 
     */
    ~Namespace();

    /**
     * @brief Open the namespace
     * 
     * Warning: you MUST call nvs::begin before opening any namespace,
     * otherwise the function will error out.
     * 
     * @param[in] name: namespace name
     * @param[in] mode: open mode
     * 
     * @return
     *      - ESP_OK: on success
     *      - ESP_FAIL: if a "Namespace" is already opened
     */
    esp_err_t open(const char* name, nvs_open_mode mode);

    /**
     * @brief Closes this namespace, if opened
     * 
     */
    void close();

    /**
     * @brief Delete all keys in this namespace
     * 
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t erase_all();

    /**
     * @brief Erase the given key
     * 
     * @param[in] key: the key to erase
     * 
     * @return
     *      - ESP_OK: on success 
     */
    esp_err_t erase_key(const char* key);

    /**
     * @brief Get a blob from
     * 
     * @param[in]  key: the blob key 
     * @param[out] result: the pointer where the blob is stored
     * 
     * @return
     *      - ESP_OK: on success 
     */
    esp_err_t get_blob(const char* key, std::ostream& data);

    /**
     * @brief Set a blob to that key
     * 
     * @param[in] key: the blob key
     * @param[in] data: the blob
     * @param[in] length: the blob length
     * 
     * @return
     *      - ESP_OK: on success 
     */
    // TODO(jeandudey): make this work with streams.
    esp_err_t set_blob(const char* key, const void* data, const std::size_t length);

    esp_err_t commit();

private:
    nvs_handle m_handle;
    bool m_is_opened;
};

/**
 * @brief Initialize the NVS
 * 
 */
esp_err_t begin();

} // namespace nvs

#endif // NVS_NAMESPACE