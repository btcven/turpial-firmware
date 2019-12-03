
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <unity.h>

#include <cstring>

#include "NVS.h"
#include "Preferences.h"
#include "defaults.h"

void test_start()
{
    esp_err_t err;
    storage::NVS::start();

    util::DefaultPreferences default_prefs;
    default_prefs.setApSSID(WAP_SSID);
    default_prefs.setApPassword(WAP_PASS);
    default_prefs.setApMaxConn(WAP_MAXCONN);
    default_prefs.setApChannel(WAP_CHANNEL);
    default_prefs.setStaSsid(WST_SSID);
    default_prefs.setStaPassword(WST_PASS);
    default_prefs.setWiFiMode(WIFI_MODE);

    util::Preferences& prefs = util::Preferences::getInstance();

    err = prefs.start(default_prefs);
    if (err != ESP_OK) TEST_FAIL();
}

extern "C" void app_main()
{
    vTaskDelay(2000);

    esp_err_t err = storage::NVS::start();
    if (err != ESP_OK) return;

    UNITY_BEGIN();
    RUN_TEST(test_start);
    UNITY_END();
}