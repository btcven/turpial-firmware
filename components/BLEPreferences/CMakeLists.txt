file(GLOB_RECURSE SOURCES src/*.cpp)
idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS src
                       REQUIRES BLE Network)
