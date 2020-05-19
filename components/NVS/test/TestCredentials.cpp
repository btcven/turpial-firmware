
#include "Credentials.h"
#include <cstring>
#include <iostream>
#include <unity.h>


TEST_CASE("set init credentials", "[credentials]")
{
    esp_err_t err = credentials::setInitialCredentials();
    if (err != ESP_OK) TEST_FAIL();
}


TEST_CASE("get credetials", "[credentials]")
{
    store_credentials_t value;

    esp_err_t err = credentials::getCredentials(&value);
    if (err != ESP_OK) TEST_FAIL();
}


TEST_CASE("credentials compare", "[credentials]")
{
    const char* value1 = "test";
    const char* value2 = "test";

    bool res = credentials::credentialCompare(value1, value2);

    TEST_ASSERT_TRUE(res);
}


TEST_CASE("save new credentials", "[credentials]")
{
    store_credentials_t initialCretendials;
    esp_err_t err = credentials::getCredentials(&initialCretendials);
    if (err != ESP_OK) TEST_FAIL();


    store_credentials_t newCredentials;
    const char*  value = "test";
    memcpy(newCredentials.nvs_username, value, strlen(value) + 1);
    memcpy(newCredentials.nvs_password, value, strlen(value) + 1);

    err = credentials::saveNewCredentials(newCredentials);
    if (err != ESP_OK) TEST_FAIL();

    err = credentials::saveNewCredentials(initialCretendials);
    if (err != ESP_OK) TEST_FAIL();
}
