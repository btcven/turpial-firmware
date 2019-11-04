
#include "hal/hardware.h"
#include <stdio.h>
#include <iostream>

#include "WiFiDTO.h"
#include "SerializablePOD.h"
#include <iostream>
#include<stdlib.h> 

//constructor 
WiFiDTO::WiFiDTO(wifi_dto_config_t& settings) {
  ptrSettings_ = (wifi_dto_config_t*)malloc(sizeof(ptrSettings_) + 
                                                sizeof(char) * strlen(settings.apSSID) +  
                                                sizeof(char) * strlen(settings.apPassword) );  

  //ptrSettings_ = new wifi_dto_config_t;    
  /* ptrSettings_->apSSID = new char[sizeof(char) *  strlen(settings.apSSID)];  
  ptrSettings_->apPassword = new char[sizeof(char) * strlen(settings.apPassword)]; */                            
  ptrSettings_ = &settings;
} 

//implementation of all virtual methods from serializable interface

size_t WiFiDTO::serialize_size() const {
  size_t size;  
  size =  SerializablePOD<int>::serialize_size(ptrSettings_->apChannel)   + 
          SerializablePOD<int>::serialize_size(ptrSettings_->apMaxConn)   + 
          SerializablePOD<int>::serialize_size(ptrSettings_->WAP_enabled) +
          SerializablePOD<int>::serialize_size(ptrSettings_->WST_enabled) +
          SerializablePOD<int>::serialize_size(ptrSettings_->isOpen)      +
          SerializablePOD<char*>::serialize_size(ptrSettings_->apSSID)    + 
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
  /* std::cout <<"--------------------SERIALIZATION ---------->"<<std::endl;

  std::cout <<"apChannel: (int)------------->"<< static_cast<const void*>(dataOut)<<"------>"<<ptrSettings_->apChannel<<std::endl;
  dataOut = SerializablePOD<int>::serialize(dataOut, ptrSettings_->apChannel);

  std::cout <<"apMaxConn: (int)------------->"<< static_cast<const void*>(dataOut)<<"------->"<<ptrSettings_->apMaxConn<<std::endl;
  dataOut = SerializablePOD<int>::serialize(dataOut, ptrSettings_->apMaxConn);

   std::cout <<"Wap Enabled: bool------------->"<< static_cast<const void*>(dataOut)<<"------->"<<ptrSettings_->WAP_enabled<<std::endl;
  dataOut = SerializablePOD<int>::serialize(dataOut, ptrSettings_->WAP_enabled);
  
  std::cout <<"WST_enabled(bool): ---------->"<< static_cast<const void*>(dataOut)<<"------->"<<ptrSettings_->WST_enabled<<std::endl;
  dataOut = SerializablePOD<int>::serialize(dataOut, ptrSettings_->WST_enabled);

  std::cout <<"isOpen: (bool)------------->"<< static_cast<const void*>(dataOut)<<"------->"<<ptrSettings_->isOpen<<std::endl;
  dataOut = SerializablePOD<int>::serialize(dataOut, ptrSettings_->isOpen);

  std::cout <<"apSSID: (char*) --------->"<< static_cast<const void*>(dataOut)<<std::endl;
  dataOut = SerializablePOD<char*>::serialize(dataOut, ptrSettings_->apSSID);

  std::cout <<"apPassword: (char*) -------->"<< static_cast<const void*>(dataOut)<<std::endl;
  dataOut  = SerializablePOD<char*>::serialize(dataOut, ptrSettings_->apPassword);  */
  
} 

void WiFiDTO::deserialize(const char* dataIn) { 
  std::cout <<"--------------------DESERIALIZATION ---------->"<<std::endl;

  SerializablePOD<int>::deserialize(dataIn, ptrSettings_->apChannel); 
  std::cout<<"dato deserializado en el metodo------------>"<<*(int*)dataIn<<"------->"<< static_cast<const void*>(&ptrSettings_->apChannel)<<std::endl;

  dataIn = SerializablePOD<int>::deserialize(dataIn, ptrSettings_->apMaxConn); 
  std::cout<<"dato deserializado en el metodo------------>"<<*(int*)dataIn<<"------->"<<static_cast<const void*>(&ptrSettings_->apMaxConn)<<std::endl;

  dataIn = SerializablePOD<int>::deserialize(dataIn, ptrSettings_->WAP_enabled);
  std::cout<<"dato deserializado en el metodo------------>"<<*(int*)dataIn<<"-------->"<<static_cast<const void*>(&ptrSettings_->WAP_enabled)<<std::endl;

  dataIn = SerializablePOD<int>::deserialize(dataIn, ptrSettings_->WST_enabled);
  std::cout<<"dato deserializado en el metodo------------>"<<*(int*)dataIn<<"-------->"<<static_cast<const void*>(&ptrSettings_->WST_enabled)<<std::endl;

  dataIn = SerializablePOD<int>::deserialize(dataIn, ptrSettings_->isOpen); 
  std::cout<<"dato deserializado en el metodo------------>"<<*(int*)dataIn<<"-------->"<<static_cast<const void*>(&ptrSettings_->isOpen)<<std::endl;

  dataIn = dataIn + 4;

  
  dataIn = SerializablePOD<char*>::deserialize(dataIn, ptrSettings_->apSSID); 
  dataIn = SerializablePOD<char*>::deserialize(dataIn, ptrSettings_->apPassword);  
 /*  std::cout <<"--------------------DESERIALIZATION ---------->"<<std::endl;

  //std::cout <<": ---------->"<< static_cast<const void*>(dataIn)<<std::endl;
  SerializablePOD<int>::deserialize(dataIn, ptrSettings_->apChannel); 
  std::cout<<"dato deserializado en el metodo------------>"<<*(int*)dataIn<<"------->"<< static_cast<const void*>(dataIn)<<std::endl;

  //std::cout <<": ---------->"<< static_cast<const void*>(dataIn)<<std::endl;
   dataIn = SerializablePOD<int>::deserialize(dataIn, ptrSettings_->apMaxConn); 
  std::cout<<"dato deserializado en el metodo------------>"<<*(int*)dataIn<<"------->"<<static_cast<const void*>(dataIn)<<std::endl;
  
  //std::cout <<": ---------->"<< static_cast<const void*>(dataIn)<<std::endl;
  dataIn = SerializablePOD<int>::deserialize(dataIn, ptrSettings_->WAP_enabled);
  std::cout<<"dato deserializado en el metodo------------>"<<*(int*)dataIn<<"-------->"<<static_cast<const void*>(dataIn)<<std::endl;

  //std::cout <<": ---------->"<< static_cast<const void*>(dataIn)<<std::endl;
  dataIn = SerializablePOD<int>::deserialize(dataIn, ptrSettings_->WST_enabled);
  std::cout<<"dato deserializado en el metodo------------>"<<*(int*)dataIn<<"-------->"<<static_cast<const void*>(dataIn)<<std::endl;

  //std::cout <<": ---------->"<< static_cast<const void*>(dataIn)<<std::endl;
  dataIn = SerializablePOD<int>::deserialize(dataIn, ptrSettings_->isOpen); 
  std::cout<<"dato deserializado en el metodo------------>"<<*(int*)dataIn<<"-------->"<<static_cast<const void*>(dataIn)<<std::endl;
  dataIn = dataIn + 4;
  //std::cout <<": ---------->"<< static_cast<const void*>(dataIn)<<std::endl;
  dataIn = SerializablePOD<char*>::deserialize(dataIn, ptrSettings_->apSSID); 
  std::cout<<"dato deserializado en el metodo------------>"<<static_cast<const void*>(dataIn)<<std::endl;
  std::cout <<": Last---------->"<< static_cast<const void*>(dataIn)<<std::endl;
  //std::cout <<": ---------->"<< static_cast<const void*>(dataIn)<<std::endl;
  dataIn = SerializablePOD<char*>::deserialize(dataIn, ptrSettings_->apPassword); 
  std::cout<<"dato deserializado en el metodo------------>"<<*(char*)dataIn<<std::endl;  */
 

} 

void WiFiDTO::setData(wifi_dto_config_t data) {
  ptrSettings_= &data;
}

void WiFiDTO::printData(void) {
  std::cout<<"el dato de test"<<ptrSettings_->apChannel <<std::endl;
  std::cout<<"el dato de test"<<ptrSettings_->apMaxConn<<std::endl;
  std::cout<<"el dato de test"<<ptrSettings_->isOpen <<std::endl;
  std::cout<<"el dato de test"<<ptrSettings_->WAP_enabled <<std::endl;
  std::cout<<"el dato de test"<<ptrSettings_->WST_enabled <<std::endl;

  std::cout<<"*************************************************"<<std::endl;

  
}
