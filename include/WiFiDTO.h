#include <Serializable.h>
#include <iostream>
#include <hal/hardware.h>


/*
This class represent the data that need to travel throught: 
1. System storage
2. Sockets
3. Serial communication
*/
typedef struct wifi_dto_config_t {
    int apChannel;
    int apMaxConn;
    int WAP_enabled; // Default value
    int WST_enabled; // Default value
    int isOpen;
    char* apSSID;
    char* apPassword;
    
   
} wifi_dto_config_t;

class WiFiDTO : public Serializable {

public:

    WiFiDTO(wifi_dto_config_t& settings);
    WiFiDTO();

    virtual size_t serialize_size() const;
    virtual void serialize(char* dataOut) const;
    virtual void deserialize(const char* dataIn);

private:
    wifi_dto_config_t settings_;
};