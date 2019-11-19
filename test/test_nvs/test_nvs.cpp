#include <cstdint>
#include <sstream>

#include "WiFiDTO.h"
#include <unity.h>

#include "Namespace.h"

/**
 * @brief 
 * Test NVS
 * it try to show how to used class to store and retrieve a blob object
 * to run test inside platform Core-cli the next command
 * platformio test -e featheresp32 -f test_nvs
 *  
 */


void test_open_namespace(void)
{
    auto nvs_err = nvs::begin();
    nvs::Namespace wifi_nvs;
    auto err = wifi_nvs.open("TEST", NVS_READONLY);
    if (err != ESP_OK) {
        TEST_FAIL();
    }
    wifi_nvs.close();
}


void test_create_and_save_blob(void)
{
    wifi::DTOConfig wifi_params; //to serialized object  declare and initialized this one to be serialized
    wifi_params.ap_channel = 4;
    wifi_params.ap_max_conn = 3;
    wifi_params.ap_password = "passwordTest";
    wifi_params.ap_ssid = "ssidTest";
    wifi_params.is_open = false;
    wifi_params.wifi_mode = WIFI_MODE_AP;

    std::stringstream blob;
    wifi_params.serialize(blob); //get serialized struct inside blob stream

    nvs::Namespace wifi_nvs;
    auto nvs_err = nvs::begin();
    auto err = wifi_nvs.open("TEST", NVS_READWRITE);
    if (err != ESP_OK) {
        TEST_FAIL();
    }
    nvs_err = wifi_nvs.set_blob("test_BLOB", blob.str().c_str(), wifi_params.serialize_size()); //save blob into NVS
    if (nvs_err != ESP_OK) {
        TEST_FAIL();
    }
    wifi_nvs.close();
}

void test_read_blob_and_deserialize(void)
{
    std::stringstream blob_from_nvs;
    wifi::DTOConfig wifi_params; //object to deserialized stream is empty
    nvs::Namespace wifi_nvs;     //object to recovery the blob from nvs
    auto nvs_err = nvs::begin();
    nvs_err = wifi_nvs.open("TEST", NVS_READONLY);
    nvs_err = wifi_nvs.get_blob("test_BLOB", blob_from_nvs);
    wifi_params.deserialize(blob_from_nvs);

    TEST_ASSERT_EQUAL_STRING("passwordTest", wifi_params.ap_password.c_str());
    TEST_ASSERT_EQUAL_STRING("ssidTest", wifi_params.ap_ssid.c_str());
    TEST_ASSERT_EQUAL_INT8(4, wifi_params.ap_channel);
    TEST_ASSERT_EQUAL_INT8(3, wifi_params.ap_max_conn);
    TEST_ASSERT_EQUAL_INT8(false, wifi_params.is_open);
    TEST_ASSERT_EQUAL_INT8(WIFI_MODE_AP, wifi_params.wifi_mode);
}

void test_remove_blob(void)
{
    nvs::Namespace wifi_nvs; //object to recovery the blob from nvs
    auto nvs_err = nvs::begin();
    nvs_err = nvs::begin();
    nvs_err = wifi_nvs.open("TEST", NVS_READWRITE);
    nvs_err = wifi_nvs.erase_key("test_BLOB");
    if (nvs_err != ESP_OK) {
        TEST_FAIL();
    }
    wifi_nvs.close();
}

extern "C" void app_main()
{
    UNITY_BEGIN();
    RUN_TEST(test_open_namespace);
    RUN_TEST(test_create_and_save_blob);
    RUN_TEST(test_read_blob_and_deserialize);
    RUN_TEST(test_remove_blob);
    UNITY_END();
}