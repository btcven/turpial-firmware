#include <cstdint>
#include <sstream>

#include <Arduino.h>
#include <unity.h>
#include "WiFiDTO.h"

#include "Namespace.h"

/**
 * @brief 
 * Test NVS
 * it try to show how to used class to store and retrieve a blob object
 * to run test inside platform Core-cli the next command
 * platformio test -e featheresp32 -f test_nvs
 *  
 */


void test_open_namespace(void) {
    auto nvs_err = nvs::begin();
    nvs::Namespace wifi_nvs;
    auto err = wifi_nvs.open("TEST", NVS_READONLY);
    if (err != ESP_OK) {
        TEST_FAIL();  
    }
}

void test_open_and_reopen_namespace(void) {
    test_open_namespace();

}

void test_create_and_save_blob(void) {
    wifi::DTOConfig wifi_params;  //to serialized object  declare and initialized this one to be serialized
    wifi_params.apChannel = 4;
    wifi_params.apMaxConn = 3;
    wifi_params.apPassword = tinystring::String("passwordTest");
    wifi_params.apSSID =     tinystring::String("ssidTest");
    wifi_params.isOpen = false;
    wifi_params.WAP_enabled = true;
    wifi_params.WST_enabled = false; 
    
    //std::ostringstream blob;
    std::stringstream blob;
    wifi_params.serialize(blob);  //get serialized struct inside blob stream

    nvs::Namespace wifi_nvs;
    auto nvs_err = nvs::begin();
    auto err = wifi_nvs.open("TEST", NVS_READWRITE);
    if (err != ESP_OK) {
        TEST_FAIL();   
    }
    nvs_err =  wifi_nvs.set_blob("test_BLOB", blob.str().c_str(), wifi_params.serialize_size()); //save blob into NVS
    if(nvs_err != ESP_OK) {
        TEST_FAIL();
    }


}

void test_read_blob_and_deserialize_it(void) {
    std::stringstream blob_from_nvs;
    wifi::DTOConfig wifi_params;  //object to deserialized stream
    nvs::Namespace wifi_nvs;  //object to recovery the blob from nvs
    auto nvs_err = nvs::begin();
    auto err = wifi_nvs.open("TEST", NVS_READONLY);
    err = wifi_nvs.get_blob("test_BLOB", blob_from_nvs);
    wifi_params.deserialize(blob_from_nvs);

    std::cout << "SSID: " << wifi_params.apSSID.c_str() << std::endl;
    std::cout << "PASSWORD: " << wifi_params.apPassword.c_str() << std::endl;
    std::cout << "APCHANNEL " << (int)wifi_params.apChannel << std::endl;
    std::cout << "apMACCONNECT " << (int)wifi_params.apMaxConn<< std::endl;
    std::cout << "ISOPEN " << wifi_params.isOpen << std::endl;
    std::cout << "WAPENABLED " << wifi_params.WAP_enabled << std::endl;
    std::cout << "WST ENABLED " << wifi_params.WST_enabled << std::endl; 
}



extern "C" void app_main() {
    delay(2000);
    UNITY_BEGIN();
    //
    RUN_TEST(test_open_namespace);
    RUN_TEST(test_create_and_save_blob);
    RUN_TEST(test_read_blob_and_deserialize_it);
    UNITY_END();
}