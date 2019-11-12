#include <Arduino.h>
#include <cstdint>

#include "unity.h"

#include "SerializablePOD.h"

/**
 * @brief serialize and deserialize integer rountrip.
 * 
 */
template<typename T>
T serialize_deserialize_integer_roundtrip(T value) {
    
    std::cout << "TEST---serialize_deserialize_integer_roundtrip" << std::endl;
    size_t size = pod::serialize_size<T>(value);
    char* dataOut = new char[size];
    std::cout << ": dir Buffer before serialized-1---------->" << static_cast<const void*> (dataOut) << std::endl;
    pod::serialize<T>(dataOut, value);
    std::cout << ": dir Buffer after serialized-1---------->" << static_cast<const void*> (dataOut) << std::endl;
    T dataIn;
    pod::deserialize<T>(dataOut, dataIn);
    
    delete[] dataOut;
    return dataIn;
}

/**
 * @brief test integer serialization with SerializablePOD
 * 
 */
void test_serialize_deserialize_integers(void) {
    std::cout << "TEST---serialize_deserialize_integer" << std::endl;
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

extern "C" void app_main() {
    // 2 s delay before starting
    delay(2000);

    UNITY_BEGIN();
    RUN_TEST(test_serialize_deserialize_integers);
    UNITY_END();
}