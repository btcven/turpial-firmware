#include <cstdint>
#include <sstream>

#include <Arduino.h>
#include <unity.h>
#include "WiFiDTO.h"
#include "Namespace.h"

/**
 * @brief 
 * Test WiFIDTO Class Serialization 
 * This test in in order to show and proof how to use serialized and deserialized 
 * methods inside DTO class.
 *  to run test inside platform Core-cli the next command
 * platformio test -e featheresp32 -f test_wifi_dto
 * 
 */

void test_serialized_and_deserialized_wifi_dto(void) {
     wifi::DTOConfig wifi_params;  //to serialized object  declare and initialized this one to be serialized
     wifi_params.apChannel = 4;
     wifi_params.apMaxConn = 3;
     wifi_params.apPassword = tinystring::String("passwordTest");
     wifi_params.apSSID =     tinystring::String("ssidTest");
     wifi_params.isOpen = false;
     wifi_params.WAP_enabled = true;
     wifi_params.WST_enabled = false;
    
    wifi::DTOConfig wifi_params2; //to deserialized and  retrieve the information 
    //std::ostringstream blob;  //ok 1
    std::stringstream blob;
    wifi_params.serialize(blob);
    //auto stream_in = std::istringstream(blob.str()); //ok 2
    //wifi_params2.deserialize(stream_in); //ok   3
    wifi_params2.deserialize(blob);

    TEST_ASSERT_EQUAL_STRING(wifi_params.apPassword.c_str(), wifi_params2.apPassword.c_str());
    TEST_ASSERT_EQUAL_STRING(wifi_params.apSSID.c_str(),     wifi_params2.apSSID.c_str());
    TEST_ASSERT_EQUAL_INT8(wifi_params.apChannel,   wifi_params2.apChannel);
    TEST_ASSERT_EQUAL_INT8(wifi_params.apMaxConn,   wifi_params2.apMaxConn);
    TEST_ASSERT_EQUAL_INT8(wifi_params.isOpen,      wifi_params2.isOpen);
    TEST_ASSERT_EQUAL_INT8(wifi_params.WAP_enabled, wifi_params2.WAP_enabled);
    TEST_ASSERT_EQUAL_INT8(wifi_params.WST_enabled, wifi_params2.WST_enabled);

}



extern "C" void app_main() {
    delay(2000);
    UNITY_BEGIN();
        RUN_TEST(test_serialized_and_deserialized_wifi_dto);
    UNITY_END();
}