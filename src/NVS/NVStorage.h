/**
 * @file NVStorage.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-09-12
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef NVSTORAGE_H
#define NVSTORAGE_H

#include "Arduino.h"

#define NVS_BOOL_KEY "bool"
#define NVS_INT_KEY "int"
#define NVS_STR_KEY "str"

typedef enum
{
    NVS_BOOL,
    NVS_INT,
    NVS_STR
} nvs_param_t;

class NVStorage
{
protected:
    uint32_t _handle;
    bool _started;
    bool _readOnly;

public:
    /**
     * @brief Construct a new NVStorage object
     * 
     */
    NVStorage();

    /**
     * @brief Destroy the NVStorage object
     * 
     */
    ~NVStorage();

    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    bool begin();

    /**
     * @brief 
     * 
     * @param name 
     * @param readOnly 
     * @return true 
     * @return false 
     */
    bool open(const char *name, bool readOnly = false);

    /**
     * @brief 
     * 
     */
    void close();

    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    bool clear();

    /**
     * @brief 
     * 
     * @param key 
     * @return true 
     * @return false 
     */
    bool remove(const char *key);

    // !SET values
    /**
     * @brief Set the Param object
     * 
     * @param key 
     * @param type 
     * @param value 
     */
    void setParam(const char *key, nvs_param_t type, void *value);

    /**
     * @brief Set the String object
     * 
     * @param key 
     * @param value 
     */
    void setString(const char *key, const char *value);

    /**
     * @brief Set the Int object
     * 
     * @param key 
     * @param value 
     */
    void setInt(const char *key, int32_t value);

    /**
     * @brief Set the Bool object
     * 
     * @param key 
     * @param value 
     */
    void setBool(const char *key, uint8_t value);

    // !GET values
    void *getParam(const char *key, nvs_param_t type, void *defaultValue);

    /**
     * @brief Get the String object
     * 
     * @param key 
     * @param defaultValue 
     * @return char* 
     */
    char *getString(const char *key, char *defaultValue);

    /**
     * @brief Get the Int object
     * 
     * @param key 
     * @param defaultValue 
     * @return int32_t 
     */
    int32_t getInt(const char *key, int32_t defaultValue);

    /**
     * @brief Get the Bool object
     * 
     * @param key 
     * @param defaultValue 
     * @return uint8_t 
     */
    uint8_t getBool(const char *key, uint8_t defaultValue);
};

#endif // NVSTORAGE_H