#include <Arduino.h>
#include <cstdint>

#include "unity.h"

#include "SerializablePOD.h"
#include "TinyString.h"

/**
 * @brief Serialize and deserialize a char* with SerializablePOD class.
 * 
 */
void test_serialize_deserialize_string(void) {
    auto test_string = tinystring::String("WIFI_AP_SSID_EXAMPLE");

    auto size = test_string.serialize_size();
    char* dataOut = new char[size];
    test_string.serialize(dataOut);

    auto test_string_deserialized = tinystring::String();
    test_string_deserialized.deserialize(dataOut);

    TEST_ASSERT_EQUAL_STRING(test_string.c_str(), test_string_deserialized.c_str());

    delete[] dataOut;
}

extern "C" void app_main() {
    // 2 s delay before starting
    delay(2000);

    UNITY_BEGIN();
    RUN_TEST(test_serialize_deserialize_string);
    UNITY_END();
}