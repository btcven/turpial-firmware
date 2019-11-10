

#include <stdio.h>
#include <iostream>

#include "WiFiDTO.h"
#include "SerializablePOD.h"
#include <iostream>
#include <stdlib.h> 

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

WiFiDTO::WiFiDTO() {
   ptrSettings_ = new wifi_dto_config_t;    
  ptrSettings_->apSSID = new char[sizeof(char) * 1];  
  ptrSettings_->apPassword = new char[sizeof(char) * 1];                         
  
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

void WiFiDTO::setData(wifi_dto_config_t& data) {
  //if data to replace is greather than data inside structure
  //we need to realocate memory to the field inside the structure
   //ptrSettings_ = &data;
  if( (size_t)strlen(data.apSSID) > (size_t)strlen(ptrSettings_->apSSID) ) {   
    ptrSettings_->apSSID = (char*)calloc(strlen(data.apSSID),1); 
  } 
  
  if( (size_t)strlen(data.apPassword) > (size_t)strlen(ptrSettings_->apPassword) ) { 
    ptrSettings_->apPassword = (char*)calloc(strlen(data.apPassword),1);
  }
  
     memcpy(ptrSettings_->apSSID,data.apSSID,strlen(data.apSSID)+1);
     memcpy(ptrSettings_->apPassword,data.apPassword,strlen(data.apPassword)+1);
}


//just for testing
void WiFiDTO::setData(void) {
        char temp[] = "hel=jhjsd,msndckjshdukshdkjsnc,mbkjhjkxhckjxhcj____";
        char temp2[] = "Camttttttttttttttttttrrrrrrrrrrrrrrrgggggggg____";
        ptrSettings_->apChannel = 9;
        ptrSettings_->apMaxConn = 10;
        ptrSettings_->WAP_enabled = 0;
        ptrSettings_->WST_enabled = 0;
        
        if( (size_t)strlen(temp) > (size_t)strlen(ptrSettings_->apSSID) ) { 
          ptrSettings_->apSSID = (char*)calloc(strlen(temp),1);
        }

        if( (size_t)strlen(temp2) > (size_t)strlen(ptrSettings_->apPassword) ) { 
          ptrSettings_->apSSID = (char*)calloc(strlen(temp),1);
        }
    
        memcpy(ptrSettings_->apSSID,temp,strlen(temp)+1);
        memcpy(ptrSettings_->apPassword,temp2,strlen(temp2)+1);
     
    }


 //just for testing   
void WiFiDTO::printData(void) {
  std::cout<<std::endl;
  std::cout<<"**************DATA STRUCT***********************************"<<std::endl;
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

 /* std::cout <<"Salida: " << ptrSettings_->apChannel << std::endl;
   std::cout << "Salida: " << ptrSettings_->apMaxConn << std::endl;
   std::cout << "Salida: " << ptrSettings_->WAP_enabled << std::endl;
   std::cout << "Salida: " << ptrSettings_->apSSID << std::endl;
   std::cout << "Salida: " << ptrSettings_->apPassword << std::endl; */
}
