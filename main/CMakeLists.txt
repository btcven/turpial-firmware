idf_component_register(SRCS "main.cpp"
                       INCLUDE_DIRS .
                       REQUIRES
                           ESC
                           Network
                           NVS
                           Radio
                           Task
                           Util
                           HTTP)
