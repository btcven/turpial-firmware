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
    // GET values
    const char *getString(const char *key, const char *defaultValue);
};

#endif // PERSISTENCE_H