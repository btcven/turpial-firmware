file(GLOB_RECURSE SOURCES src/*.cpp)
idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS
                           src
                           ../../main
                       REQUIRES esp_https_server json ESC Network NVS
                       EMBED_TXTFILES "certs/cacert.pem"
                                   "certs/prvtkey.pem")
                  
