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
#include "nvs_flash.h"
#include "nvs.h"

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
        /* data */
        uint32_t _handle;
        bool _started;
        bool _readOnly;

    public:
        NVStorage();
        ~NVStorage();
        bool begin();
        bool open(const char *name, bool readOnly = false);
        void close();
        bool clear();
        bool remove(const char *key);
        // SET values
        void setParam(const char *key, nvs_param_t type, void *value);
        void setString(const char *key, const char *value);
        void setInt(const char *key, int32_t value);
        void setBool(const char *key, uint8_t value);
        // GET values
        void *getParam(const char *key, nvs_param_t type, void *defaultValue);
        char *getString(const char *key, char *defaultValue);
        int32_t getInt(const char *key, int32_t defaultValue);
        uint8_t getBool(const char *key, uint8_t defaultValue);
};

#endif // NVSTORAGE_H