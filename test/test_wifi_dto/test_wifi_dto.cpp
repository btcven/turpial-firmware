#include <cstdint>
#include <sstream>

#include <Arduino.h>
#include <unity.h>

/* #include "SerializablePOD.h"
#include "TinyString.h" */
#include "WiFiDTO.h"

/**
 * @brief Test the String class constructors
 * 
 */
void test_serialized_wifi_dto(void) {
     wifi::DTOConfig wifi_params;  //to serialized object 
     wifi_params.apChannel = 4;
     wifi_params.apMaxConn = 3;
     wifi_params.apPassword = tinystring::String("passwordTest");
     wifi_params.apSSID =     tinystring::String("ssidTest");
     wifi_params.isOpen = false;
     wifi_params.WAP_enabled = true;
     wifi_params.WST_enabled = false;

    
    wifi::DTOConfig wifi_params2; //to deserialized and  retrieve the information 
    std::ostringstream blob;

    std::cout << "------------------------going to Serialized------------------------------" << std::endl;
    std::cout << "------------------------WIFI_PARMS WORKING AS A SERILIZER------------------------\n\n\n\n" << std::endl;
    wifi_params.serialize(blob);
    auto stream_in = std::istringstream(blob.str());

    std::cout << "------------------------going to Deserialized------------------------------" << std::endl;
    std::cout << "------------------------WIFI_PARMS-2 WORKING AS A DESERILIZER------------------------------" << std::endl;

    wifi_params2.deserialize(stream_in);

    std::cout << "--------------PARAMETRO1:  " << wifi_params2.apSSID.c_str() << std::endl;
    std::cout << "--------------PARAMETRO1:  " << wifi_params2.apPassword.c_str() << std::endl;


    TEST_ASSERT_EQUAL_STRING(wifi_params.apPassword.c_str(), wifi_params2.apPassword.c_str());
    TEST_ASSERT_EQUAL_STRING(wifi_params.apSSID.c_str(), wifi_params2.apSSID.c_str());

   /*  auto test_string_2 = tinystring::String();
    TEST_ASSERT_EQUAL_PTR(nullptr, test_string_2.c_str());  */
}



extern "C" void app_main() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_serialized_wifi_dto);
    /*RUN_TEST(test_length_string);
    RUN_TEST(test_serialize_deserialize_string);
    RUN_TEST(test_serialize_deserialize_string_with_operators); */
    UNITY_END();
}