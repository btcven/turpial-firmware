#include <WiFiDTO.h>
#include "hal/hardware.h"
#include <stdio.h>

//typedef struct wifi_dto_config_t

//constructor 
WiFiDTO::WiFiDTO(const wifi_dto_config_t& settings) {

}
//implementation of all virtual methods from serializable interface

void WiFiDTO::serialize_size() const {

}

void WiFiDTO::serialize(char* dataOut) const {

} 

void WiFiDTO::deserialize(const char* dataIn) { 

} 


