/**
 * @file Socket.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2020-03-25
 * 
 * @copyright Copyright (c) 2020 Locha Mesh Developers
 * 
 */

#include <stdint.h>
#include <vector>


struct client_data_t {
    uint8_t shaUID;
    int timestamp;
    bool is_alive;
};


class Websocket
{

public:
    Websocket();
private:
    std::vector<client_data_t> m_client;
};
