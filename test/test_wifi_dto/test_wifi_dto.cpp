#include <cstdint>
#include <sstream>

#include "Namespace.h"
#include "WiFiDTO.h"
#include <Arduino.h>
#include <unity.h>

/**
 * @brief 
 * Test WiFIDTO Class Serialization 
 * This test in in order to show and proof how to use serialized and deserialized 
 * methods inside DTO class.
 *  to run test inside platform Core-cli the next command
 * platformio test -e featheresp32 -f test_wifi_dto
 * 
 */

void test_serialized_and_deserialized_wifi_dto(void)
{
    wifi::DTOConfig wifi_params; //to serialized object  declare and initialized this one to be serialized
    wifi_params.ap_channel = 4;
    wifi_params.ap_max_conn = 3;
    wifi_params.ap_password = tinystring::String("passwordTest");
    wifi_params.ap_ssid = tinystring::String("ssidTest");
    wifi_params.is_open = false;
    wifi_params.wap_enabled = true;
    wifi_params.wst_enabled = false;

    wifi::DTOConfig wifi_params2; //to deserialized and  retrieve the information
    //std::ostringstream blob;  //ok 1
    std::stringstream blob;
    wifi_params.serialize(blob);
    //auto stream_in = std::istringstream(blob.str()); //ok 2
    //wifi_params2.deserialize(stream_in); //ok   3
    wifi_params2.deserialize(blob);

    TEST_ASSERT_EQUAL_STRING(wifi_params.ap_password.c_str(), wifi_params2.ap_password.c_str());
    TEST_ASSERT_EQUAL_STRING(wifi_params.ap_ssid.c_str(), wifi_params2.ap_ssid.c_str());
    TEST_ASSERT_EQUAL_INT8(wifi_params.ap_channel, wifi_params2.ap_channel);
    TEST_ASSERT_EQUAL_INT8(wifi_params.ap_max_conn, wifi_params2.ap_max_conn);
    TEST_ASSERT_EQUAL_INT8(wifi_params.is_open, wifi_params2.is_open);
    TEST_ASSERT_EQUAL_INT8(wifi_params.wap_enabled, wifi_params2.wap_enabled);
    TEST_ASSERT_EQUAL_INT8(wifi_params.wst_enabled, wifi_params2.wst_enabled);
}


extern "C" void app_main()
{
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_serialized_and_deserialized_wifi_dto);
    UNITY_END();
}