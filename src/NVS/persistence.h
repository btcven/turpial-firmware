/**
 * @file persistence.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-09-12
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef PERSISTENCE_H
#define PERSISTENCE_H
#include "Arduino.h"

#define NVS_BOOL_KEY "bool"
#define NVS_INT_KEY "int"
#define NVS_STR_KEY "str"

class Persistence
{
protected:
    /* data */
    uint32_t _handle;
    bool _started;
    bool _readOnly;

public:
    Persistence();
    ~Persistence();
    bool begin(const char *name, bool readOnly = false);
    void end();
    bool clear();
    bool remove(const char *key);
    // SET values
    size_t setString(const char *key, const char *value);
    void setInt(const char *key, int32_t value);
    void setBool(const char *key, uint8_t value);
    // GET values
    char *getString(const char *key, char *defaultValue);
    int32_t getInt(const char *key, int32_t defaultValue);
    uint8_t getBool(const char *key, uint8_t defaultValue);
};

#endif // PERSISTENCE_H