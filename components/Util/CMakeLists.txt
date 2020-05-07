file(GLOB_RECURSE SOURCES src/*.cpp)
idf_component_register(SRCS ${SOURCES}
                       REQUIRES cbor
                       INCLUDE_DIRS src)
