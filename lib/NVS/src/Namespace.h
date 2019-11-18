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
     * @param name namespace name
     * @param mode open mode
     * @return esp_err_t NVS open error
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
     * @return esp_err_t 
     */
    esp_err_t erase_all();

    /**
     * @brief Erase the given key
     * 
     * @param key the key to erase
     * @return esp_err_t 
     */
    esp_err_t erase_key(const char* key);

    /**
     * @brief Get a blob from
     * 
     * @param key the blob key 
     * @param result the pointer where the blob is stored.
     * @param length 
     * @return esp_err_t 
     */
    esp_err_t get_blob(const char* key, std::ostream& data);

    /**
     * @brief 
     * 
     * @param key 
     * @param data 
     * @param length 
     * @return esp_err_t 
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