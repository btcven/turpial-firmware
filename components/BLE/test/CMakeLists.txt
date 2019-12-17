file(GLOB_RECURSE SOURCES *.cpp)
idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS "."
                       REQUIRES BLE unity)
