#include <Serializable.h>
#include <iostream>
#include <hal/hardware.h>
#include <SerializablePOD.h>

/*
This class represent the data that need to travel throught: 
1. System storage
2. Sockets
3. Serial communication
*/
typedef struct wifi_dto_config_t {
    bool WAP_enabled = WAP_ENABLED; // Default value
    bool WST_enabled = WST_ENABLED; // Default value
    bool isOpen;
    char *apSSID = WAP_SSID;
    char *apPassword = WAP_PASS;
    int32_t apChannel = WAP_CHANNEL;
    int32_t apMaxConn = WAP_MAXCONN;
} wifi_dto_config_t;

class WiFiDTO : public Serializable {

public:

    WiFiDTO(const wifi_dto_config_t& settings);
    WiFiDTO();

    virtual void serialize_size() const;
    virtual void serialize(char* dataOut) const;
    virtual void deserialize(const char* dataIn);

private:
    wifi_dto_config_t *settings_;
};