#include <Arduino.h>
#include <cstdint>

#include "unity.h"

#include "SerializablePOD.h"
#include "TinyString.h"

/**
 * @brief Test the String class constructors
 * 
 */
void test_construct_string(void) {
    auto test_string_1 = tinystring::String("LOCHA.IO");
    TEST_ASSERT_EQUAL_STRING("LOCHA.IO", test_string_1.c_str());

    auto test_string_2 = tinystring::String();
    TEST_ASSERT_EQUAL_PTR(nullptr, test_string_2.c_str());
}

/**
 * @brief Test the String class length() function
 * 
 */
void test_length_string(void) {
    auto test_string_1 = tinystring::String("LOCHA.IO");
    TEST_ASSERT_EQUAL_INT(8, test_string_1.length());

    auto test_string_2 = tinystring::String();
    TEST_ASSERT_EQUAL_PTR(nullptr, test_string_2.c_str());
}

/**
 * @brief Serialize and deserialize a String
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
    RUN_TEST(test_construct_string);
    RUN_TEST(test_length_string);
    RUN_TEST(test_serialize_deserialize_string);
    UNITY_END();
}