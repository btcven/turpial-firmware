#ifndef SINGLENVS_H
#define SINGLENVS_H

#include "Arduino.h"
#include <nvs.h>
#include <string>


class SingleNVS {
    public:
        // This is how clients can access the single instance
        static SingleNVS* getInstance();
        virtual ~SingleNVS();

        esp_err_t begin();
        bool open(const char *name, nvs_open_mode openMode);
        void close();
        bool clear();


        void commit();
        void erase();
        void erase(std::string key);
        int get(std::string key, std::string* result, bool isBlob = false);
        int get(std::string key, uint8_t* result, size_t& length);
        int get(std::string key, uint32_t& value);
        int get(const char* key, void* result,size_t& length);

        void set(std::string key, std::string data, bool isBlob = false);
        void set(std::string key, uint32_t value);
        void set(std::string key, uint8_t* data, size_t length); 
        void set(const char* key, char* data, size_t length);


        protected:
        int value_;
        nvs_handle _handle;
        bool _started = false;
        bool _readOnly;

    private:
        const char* name_;
        //std::string m_name;
	    

        static SingleNVS* nvs_;   // The one, single instance
        SingleNVS() : value_(0) {} // private constructor
        SingleNVS(const SingleNVS&); //to avoid instances
        SingleNVS& operator=(const SingleNVS&);
};
#endif