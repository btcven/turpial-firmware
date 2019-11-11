#include <Arduino.h>
#include <unity.h>
#include <cstdint>

#include "SerializablePOD.h"

/**
 * @brief serialize and deserialize integer rountrip.
 * 
 */
template<typename T>
T serialize_deserialize_integer_roundtrip(T value) {
    size_t size = SerializablePOD<T>::serialize_size(value);
    char* dataOut = static_cast<char*>(std::malloc(size));
    dataOut = SerializablePOD<T>::serialize(dataOut, value);

    T dataIn;
    SerializablePOD<T>::deserialize(dataOut, dataIn);

    std::free(dataOut);

    return dataIn;
}

/**
 * @brief Serialize and deserialize a char* with SerializablePOD class.
 * 
 */
void test_serialize_deserialize_char(void) {
    // TODO(jeandudey): fix cast const char* cast to char*, this needs to be done because
    // SerializablePOD doesn't has specialization for const char*
    char* test_string = "WIFI_AP_SSID_EXAMPLE";

    std::size_t size = SerializablePOD<char*>::serialize_size(test_string);
    char* dataOut = static_cast<char*>(std::malloc(size));
    dataOut = SerializablePOD<char*>::serialize(dataOut, test_string);

    char* dataIn = nullptr;
    SerializablePOD<char*>::deserialize(dataOut, dataIn);

    TEST_ASSERT_EQUAL_STRING(test_string, dataIn);

    std::free(dataIn);
    std::free(dataOut);
}

/**
 * @brief test integer serialization with SerializablePOD
 * 
 */
void test_serialize_deserialize_integers(void) {
    std::uint8_t v0 = serialize_deserialize_integer_roundtrip<std::uint8_t>(0xff);
    TEST_ASSERT_EQUAL_UINT8(0xff, v0);
    std::uint16_t v1 = serialize_deserialize_integer_roundtrip<std::uint16_t>(0xffff);
    TEST_ASSERT_EQUAL_UINT8(0xffff, v1);
    std::uint32_t v2 = serialize_deserialize_integer_roundtrip<std::uint32_t>(0xffffffff);
    TEST_ASSERT_EQUAL_UINT8(0xffffffff, v2);
    
    std::int8_t v3 = serialize_deserialize_integer_roundtrip<std::uint8_t>(-125);
    TEST_ASSERT_EQUAL_UINT8(-125, v3);
    std::int16_t v4 = serialize_deserialize_integer_roundtrip<std::uint16_t>(3000);
    TEST_ASSERT_EQUAL_UINT8(3000, v4);
    std::int32_t v5 = serialize_deserialize_integer_roundtrip<std::uint32_t>(-65535);
    TEST_ASSERT_EQUAL_UINT8(-65535, v5);
}

void setup() {
    delay(2000);
    UNITY_BEGIN();
}

void loop(){
    RUN_TEST(test_serialize_deserialize_char);
    RUN_TEST(test_serialize_deserialize_integers);
    UNITY_END();
}