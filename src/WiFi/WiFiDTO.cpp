#include <WiFiDTO.h>
#include "hal/hardware.h"
#include <stdio.h>

//typedef struct wifi_dto_config_t

//constructor 
WiFiDTO::WiFiDTO(const wifi_dto_config_t& settings) {
    std::cout<<"esto es una prueba mas"<<std::endl;
    std::cout<<"constructor working fine"<<std::endl;
}
//default constructor
WiFiDTO::WiFiDTO() { 

}
//implementation of all virtual methods from serializable interface

void WiFiDTO::serialize_size() const {

}

void WiFiDTO::serialize(char* dataOut) const {

} 

void WiFiDTO::deserialize(const char* dataIn) { 

} 


