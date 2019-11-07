

#include <stdio.h>
#include <iostream>

#include "WiFiDTO.h"
#include "SerializablePOD.h"
#include <iostream>
#include <stdlib.h> 

#include "defaults.h"
/**
 * @brief Construct a new Wi Fi D T O:: Wi Fi D T O object
 * 
 * 
 * @param settings 
 */
WiFiDTO::WiFiDTO(wifi_dto_config_t& settings) {
 /*  ptrSettings_ = (wifi_dto_config_t*)malloc(sizeof(ptrSettings_) + 
                                                sizeof(char) * strlen(settings.apSSID) +  
                                                sizeof(char) * strlen(settings.apPassword) );   */

  ptrSettings_ = new wifi_dto_config_t;    
  ptrSettings_->apSSID = new char[sizeof(char) *  strlen(settings.apSSID)];  
  ptrSettings_->apPassword = new char[sizeof(char) * strlen(settings.apPassword)];                         
  ptrSettings_ = &settings;
} 

//implementation of all virtual methods from serializable interface

size_t WiFiDTO::serialize_size() const {
  size_t size;  
  size =  SerializablePOD<int>::serialize_size(ptrSettings_->apChannel)     + 
          SerializablePOD<int>::serialize_size(ptrSettings_->apMaxConn)     + 
          SerializablePOD<int>::serialize_size(ptrSettings_->WAP_enabled)   +
          SerializablePOD<int>::serialize_size(ptrSettings_->WST_enabled)   +
          SerializablePOD<int>::serialize_size(ptrSettings_->isOpen)        +
          SerializablePOD<char*>::serialize_size(ptrSettings_->apSSID)      + 
          SerializablePOD<char*>::serialize_size(ptrSettings_->apPassword);  
          
                    
  return size; 
}

void WiFiDTO::serialize(char* dataOut) const {
  std::cout <<"--------------------SERIALIZATION ---------->"<<std::endl;

  std::cout <<"apChannel: (int)------------->"<<"------>"<<&ptrSettings_->apChannel<<"--->Value: "<< ptrSettings_->apChannel<<std::endl;
  dataOut = SerializablePOD<int>::serialize(dataOut, ptrSettings_->apChannel);

  std::cout <<"apMaxConn: (int)------------->"<<"------->"<<&ptrSettings_->apMaxConn<<"--->Value: "<< ptrSettings_->apMaxConn<<std::endl;
  dataOut = SerializablePOD<int>::serialize(dataOut, ptrSettings_->apMaxConn);

  std::cout <<"Wap Enabled: bool------------->"<<"------->"<<&ptrSettings_->WAP_enabled<<"--->Value: "<< ptrSettings_->WAP_enabled<<std::endl;
  dataOut = SerializablePOD<int>::serialize(dataOut, ptrSettings_->WAP_enabled);
  
  std::cout <<"WST_enabled(bool): ---------->"<<"------->"<<&ptrSettings_->WST_enabled<<"--->Value: "<< ptrSettings_->WST_enabled<<std::endl;
  dataOut = SerializablePOD<int>::serialize(dataOut, ptrSettings_->WST_enabled);

  std::cout <<"isOpen: (bool)------------->"<<"------->"<<&ptrSettings_->isOpen<<"--->Value: "<< ptrSettings_->isOpen<<std::endl;
  dataOut = SerializablePOD<int>::serialize(dataOut, ptrSettings_->isOpen);

  //print the address stored in ptrSettings_ to know the address of field with data
  std::cout <<"apSSID: (char*) --------->Dir char*------>"<< static_cast<const void*>(&ptrSettings_->apSSID)<<std::endl;
  std::cout <<"apSSID: (char*) --------->Dir block memory with data char*------>"<< static_cast<const void*>(ptrSettings_->apSSID)<<std::endl;
  dataOut = SerializablePOD<char*>::serialize(dataOut, ptrSettings_->apSSID);

  std::cout <<"apPassword: (char*) -------->"<< static_cast<const void*>(ptrSettings_->apPassword)<<std::endl;
  dataOut  = SerializablePOD<char*>::serialize(dataOut, ptrSettings_->apPassword);   
} 

void WiFiDTO::deserialize(const char* dataIn) { 

  std::cout <<"--------------------DESERIALIZATION ---------->"<<std::endl;

  std::cout<<"dato deserializado en el metodo------------>"<<*(int*)dataIn<<"------->"<< static_cast<const void*>(&ptrSettings_->apChannel)<<std::endl;
  dataIn = SerializablePOD<int>::deserialize(dataIn, ptrSettings_->apChannel); 
  
  std::cout<<"dato deserializado en el metodo------------>"<<*(int*)dataIn<<"------->"<<static_cast<const void*>(&ptrSettings_->apMaxConn)<<std::endl;
  dataIn = SerializablePOD<int>::deserialize(dataIn, ptrSettings_->apMaxConn); 
  
  std::cout<<"dato deserializado en el metodo------------>"<<*(int*)dataIn<<"-------->"<<static_cast<const void*>(&ptrSettings_->WAP_enabled)<<std::endl;
  dataIn = SerializablePOD<int>::deserialize(dataIn, ptrSettings_->WAP_enabled);
  
  std::cout<<"dato deserializado en el metodo------------>"<<*(int*)dataIn<<"-------->"<<static_cast<const void*>(&ptrSettings_->WST_enabled)<<std::endl;
  dataIn = SerializablePOD<int>::deserialize(dataIn, ptrSettings_->WST_enabled);
  
  std::cout<<"dato deserializado en el metodo------------>"<<*(int*)dataIn<<"-------->"<<static_cast<const void*>(&ptrSettings_->isOpen)<<std::endl;
  dataIn = SerializablePOD<int>::deserialize(dataIn, ptrSettings_->isOpen); 

  dataIn = SerializablePOD<char*>::deserialize(dataIn, ptrSettings_->apSSID); 
  dataIn = SerializablePOD<char*>::deserialize(dataIn, ptrSettings_->apPassword);  
} 

void WiFiDTO::setData(wifi_dto_config_t data) {
  ptrSettings_= &data;
}

void WiFiDTO::printData(void) {
  std::cout<<std::endl;
  std::cout<<"**************ADDRESSES STRUCT***********************************"<<std::endl;
  std::cout <<"-----------------------------------------------"<<std::endl;
  wifi_dto_config_t* ps = *initPtrSettings_; //get the initial structure address 
  //this pointer is used to point to address structure but is able to increment addresses by 4 bytes with ptr_to_int++
  std::cout << (*ps).apChannel << std::endl;
  std::cout << (*ps).apMaxConn << std::endl;
  std::cout << (*ps).WAP_enabled << std::endl;
  std::cout << (*ps).WST_enabled << std::endl;
  std::cout << (*ps).isOpen << std::endl;
  std::cout << (*ps).apSSID << std::endl;
  std::cout << (*ps).apPassword << std::endl;
  
}
