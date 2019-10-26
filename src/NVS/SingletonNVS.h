#ifndef SINGLETONNVS_H
#define SINGLETONNVS_H

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

class SingletonNVS {
    public:
        // This is how clients can access the single instance
        static SingletonNVS* getInstance();

        esp_err_t begin();
        bool open(const char *name, bool readOnly = false);
        void close();
        bool clear();
        bool remove(const char *key);
        void setParam(const char *key, nvs_param_t type, void *value);
        void setString(const char *key, const char *value);
        void setInt(const char *key, int32_t value);
        void setBool(const char *key, uint8_t value);           
        
        void *getParam(const char *key, nvs_param_t type, void *defaultValue);
        char *getString(const char *key, char *defaultValue);
        int32_t getInt(const char *key, int32_t defaultValue);
        uint8_t getBool(const char *key, uint8_t defaultValue);
        
        void setValue(int val)  {value_ = val;}
        int  getValue()         {return(value_);}

        protected:
        int value_;
        uint32_t _handle;
        bool _started = false;
        bool _readOnly;

    private:
    static SingletonNVS* nvs_;   // The one, single instance
    SingletonNVS() : value_(0) {} // private constructor
    SingletonNVS(const SingletonNVS&); //to avoid instances
    SingletonNVS& operator=(const SingletonNVS&);
};
#endif