idf_component_register(SRCS "main.cpp"
                       INCLUDE_DIRS .
                       REQUIRES
                           BLE
                           BLEPreferences
                           Network
                           NVS
                           Radio
                           Task
                           Util
                           WS)
