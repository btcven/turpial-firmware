#include "HttpServerHandler.h"
#include <iostream>
static const char* TAG = "HTTP-SERVER-HANDLER";

HttpServerHandler::HttpServerHandler() {
    
}

void HttpServerHandler::helloWorld(void) {
    std::cout << "Hello World !!!\n"std::endl;
}
