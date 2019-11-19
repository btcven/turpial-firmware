#include <cstdint>
#include <sstream>

#include <unity.h>

#include "SerializablePOD.h"
#include "TinyString.h"

/**
 * @brief Test the String class constructors
 * 
 */
void test_construct_string(void)
{
    auto test_string_1 = tinystring::String("LOCHA.IO");
    TEST_ASSERT_EQUAL_STRING("LOCHA.IO", test_string_1.c_str());
}

/**
 * @brief Serialize and deserialize a String
 * 
 */
void test_serialize_deserialize_string(void)
{
    auto test_string = tinystring::String("WIFI_AP_SSID_EXAMPLE");

    std::ostringstream stream_out;
    test_string.serialize(stream_out);


    auto test_string_deserialized = tinystring::String();
    auto stream_in = std::istringstream(stream_out.str());
    test_string_deserialized.deserialize(stream_in);

    TEST_ASSERT_EQUAL_STRING(test_string.c_str(), test_string_deserialized.c_str());
}

/**
 * @brief Test string copying
 * 
 */
void test_copy_to()
{
    char dest[7];
    auto test_string = tinystring::TinyString("SSID_0");
    TEST_ASSERT_EQUAL_INT(6, test_string.length());
    test_string.copy_to(dest);
    TEST_ASSERT_EQUAL_STRING(test_string.c_str(), dest);
}

extern "C" void app_main()
{
    UNITY_BEGIN();
    RUN_TEST(test_construct_string);
    RUN_TEST(test_serialize_deserialize_string);
    RUN_TEST(test_copy_to);
    UNITY_END();
}