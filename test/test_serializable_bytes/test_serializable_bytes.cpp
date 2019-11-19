#include <cstdint>
#include <sstream>

#include <unity.h>

#include "SerializableBytes.h"
#include "SerializablePOD.h"

/**
 * @brief Serialize and deserialize a String
 * 
 */
void test_serialize_deserialize_bytes(void)
{
    SerializableBytes<32> test_ssid;
    test_ssid = "WIFI_AP_SSID_EXAMPLE";

    std::stringstream stream;
    test_ssid.serialize(stream);


    SerializableBytes<32> test_ssid_deserialized;
    test_ssid_deserialized.deserialize(stream);

    TEST_ASSERT_EQUAL_STRING(test_ssid.c_str(), test_ssid_deserialized.c_str());
}

extern "C" void app_main()
{
    UNITY_BEGIN();
    RUN_TEST(test_serialize_deserialize_bytes);
    UNITY_END();
}