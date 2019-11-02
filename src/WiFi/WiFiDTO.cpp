
#include "hal/hardware.h"
#include <stdio.h>
#include <iostream>

#include "WiFiDTO.h"
#include "SerializablePOD.h"


//constructor 
WiFiDTO::WiFiDTO(wifi_dto_config_t& settings) {
 /*  settings_.WAP_enabled = settings.WAP_enabled;
  settings_.apChannel = settings.apChannel;
  settings_.apMaxConn = settings.apMaxConn;
  settings_.apPassword = settings.apPassword;
  settings_.apSSID = settings.apSSID;
  settings_.WAP_enabled = settings.WAP_enabled;
  settings_.WST_enabled =settings.WST_enabled;  */
  settings_ = settings;
  // settings_ = settings;
  std::cout<<"esto es una prueba mas"<<std::endl;
  std::cout<<"constructor working fine"<<std::endl;

  std::cout << "Showing Data" << std::endl;
  std::cout << "Settings.apChannel " << settings_.apChannel << std::endl;
  std::cout << "Settings.apMaxConn " << settings_.apMaxConn << std::endl;
  std::cout << "Settings.apPassword " << settings_.apPassword << std::endl;
  std::cout << "Settings.SSID " << settings_.apSSID << std::endl;
  std::cout << "Settings.Wap Enabled " << settings_.WAP_enabled << std::endl;
  std::cout << "Settings.WST " << settings_.WST_enabled << std::endl;  
  std::cout << "Settings.isOpen " << settings_.isOpen << std::endl;
} 
//default constructor
WiFiDTO::WiFiDTO() {
  std::cout <<"contructor  void*" <<std::endl;
 }
//implementation of all virtual methods from serializable interface

size_t WiFiDTO::serialize_size() const {
  size_t size;  
  size =  SerializablePOD<int>::serialize_size(settings_.apChannel) + 
          SerializablePOD<int>::serialize_size(settings_.apMaxConn) + 
          SerializablePOD<int>::serialize_size(settings_.WAP_enabled) +
          SerializablePOD<int>::serialize_size(settings_.WST_enabled) +
          SerializablePOD<int>::serialize_size(settings_.isOpen) +
          SerializablePOD<char*>::serialize_size(settings_.apSSID)  + 
          SerializablePOD<char*>::serialize_size(settings_.apPassword); 
          
                    
  return size; 
}

void WiFiDTO::serialize(char* dataOut) const {
  std::cout <<"--------------------SERIALIZATION ---------->"<<std::endl;

  std::cout <<"apChannel: (int)------------->"<< static_cast<const void*>(dataOut)<<"------>"<<settings_.apChannel<<std::endl;
  dataOut = SerializablePOD<int>::serialize(dataOut, settings_.apChannel);

  std::cout <<"apMaxConn: (int)------------->"<< static_cast<const void*>(dataOut)<<"------->"<<settings_.apMaxConn<<std::endl;
  dataOut = SerializablePOD<int>::serialize(dataOut, settings_.apMaxConn);

   std::cout <<"Wap Enabled: bool------------->"<< static_cast<const void*>(dataOut)<<"------->"<<settings_.WAP_enabled<<std::endl;
  dataOut = SerializablePOD<int>::serialize(dataOut, settings_.WAP_enabled);
  
  std::cout <<"WST_enabled(bool): ---------->"<< static_cast<const void*>(dataOut)<<"------->"<<settings_.WST_enabled<<std::endl;
  dataOut = SerializablePOD<int>::serialize(dataOut, settings_.WST_enabled);

  std::cout <<"isOpen: (bool)------------->"<< static_cast<const void*>(dataOut)<<"------->"<<settings_.isOpen<<std::endl;
  dataOut = SerializablePOD<int>::serialize(dataOut, settings_.isOpen);

  std::cout <<"apSSID: (char*) --------->"<< static_cast<const void*>(dataOut)<<std::endl;
  dataOut = SerializablePOD<char*>::serialize(dataOut, settings_.apSSID);

  std::cout <<"apPassword: (char*) -------->"<< static_cast<const void*>(dataOut)<<std::endl;
  dataOut  = SerializablePOD<char*>::serialize(dataOut, settings_.apPassword); 
 
 
} 

void WiFiDTO::deserialize(const char* dataIn) { 
  std::cout <<"--------------------DESERIALIZATION ---------->"<<std::endl;

  //std::cout <<": ---------->"<< static_cast<const void*>(dataIn)<<std::endl;
  SerializablePOD<int>::deserialize(dataIn, settings_.apChannel); 
  std::cout<<"dato deserializado en el metodo------------>"<<*(int*)dataIn<<"------->"<< static_cast<const void*>(dataIn)<<std::endl;

  //std::cout <<": ---------->"<< static_cast<const void*>(dataIn)<<std::endl;
   dataIn = SerializablePOD<int>::deserialize(dataIn, settings_.apMaxConn); 
  std::cout<<"dato deserializado en el metodo------------>"<<*(int*)dataIn<<"------->"<<static_cast<const void*>(dataIn)<<std::endl;
  
  //std::cout <<": ---------->"<< static_cast<const void*>(dataIn)<<std::endl;
  dataIn = SerializablePOD<int>::deserialize(dataIn, settings_.WAP_enabled);
  std::cout<<"dato deserializado en el metodo------------>"<<*(int*)dataIn<<"-------->"<<static_cast<const void*>(dataIn)<<std::endl;

  //std::cout <<": ---------->"<< static_cast<const void*>(dataIn)<<std::endl;
  dataIn = SerializablePOD<int>::deserialize(dataIn, settings_.WST_enabled);
  std::cout<<"dato deserializado en el metodo------------>"<<*(int*)dataIn<<"-------->"<<static_cast<const void*>(dataIn)<<std::endl;

  //std::cout <<": ---------->"<< static_cast<const void*>(dataIn)<<std::endl;
  dataIn = SerializablePOD<int>::deserialize(dataIn, settings_.isOpen); 
  std::cout<<"dato deserializado en el metodo------------>"<<*(int*)dataIn<<"-------->"<<static_cast<const void*>(dataIn)<<std::endl;
  
  //std::cout <<": ---------->"<< static_cast<const void*>(dataIn)<<std::endl;
  dataIn = SerializablePOD<char*>::deserialize(dataIn, settings_.apSSID); 
  std::cout<<"dato deserializado en el metodo------------>"<<*(char*)dataIn<<"------->"<<static_cast<const void*>(dataIn)<<std::endl;
 
  //std::cout <<": ---------->"<< static_cast<const void*>(dataIn)<<std::endl;
  dataIn = SerializablePOD<char*>::deserialize(dataIn, settings_.apPassword); 
  std::cout<<"dato deserializado en el metodo------------>"<<*(char*)dataIn<<std::endl;  
 

} 

void WiFiDTO::setData(wifi_dto_config_t data) {
  settings_= data;
}

void WiFiDTO::printData(void) {
  std::cout<<"el dato de test"<<settings_.apChannel <<std::endl;
  std::cout<<"el dato de test"<<settings_.apMaxConn<<std::endl;
  std::cout<<"el dato de test"<<settings_.isOpen <<std::endl;
  std::cout<<"el dato de test"<<settings_.WAP_enabled <<std::endl;
  std::cout<<"el dato de test"<<settings_.WST_enabled <<std::endl;

  std::cout<<"*************************************************"<<std::endl;

  
}
