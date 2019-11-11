#include <Arduino.h>
#include <unity.h>

#include "SerializablePOD.h"

/**
 * @brief Serialize and deserialize a char* with SerializablePOD class.
 * 
 */
void test_serialize_deserialize_char(void) {
    const char* test_string = "WIFI_AP_SSID_EXAMPLE";

    size_t size = SerializablePOD::serialize_size<char*>(test_string);
    char* dataOut = std::malloc(size);
    dataOut = SerializablePOD::serialize<char*>(dataOut, test_string);

    char* dataIn = nullptr;
    SerializablePOD::deserialize<char*>(dataOut, &dataIn);

    TEST_ASSERT_EQUAL_STRING(test_string, dataIn);

    std::free(dataIn);
}

void setup() {
    delay(2000);
    UNITY_BEGIN();
}

void loop(){
    RUN_TEST(test_serialize_deserialize_char);
    UNITY_END();
}