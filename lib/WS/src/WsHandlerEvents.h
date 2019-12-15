
#ifndef WSHANDLEREVENTS_H
#define WSHANDLEREVENTS_H

#include "WebSocket.h"
#include <esp_log.h>
#include <iostream>
#include <sstream>
using namespace std;

extern ws_list_t wsClients;

class WsHandlerEvents : public WebSocketHandler
{
public:
    virtual void onClose(WebSocket* pWebSocket)
    {
        int fd = pWebSocket->getSocket().getFD();
        wsClients.erase(fd);
        ESP_LOGI("EVENTS", "Clients availables:------>>> %d", wsClients.size());
        pWebSocket->close();
    }

    virtual void onMessage(WebSocketInputStreambuf* pWebSocketInputStreambuf, WebSocket* pWebSocket) override
    {
        stringstream s2;
        s2 << pWebSocketInputStreambuf;
        std::cout << "Message from client " << pWebSocket->getSocket().getFD() << "<-->" << s2.rdbuf()->str() << std::endl;
        pWebSocket->send("hello world from websocket handler line 28", 0x02); //0x01 BIN  0x02 TEXT

        ESP_LOGI("TESTING CUSTOM CALLBACK ON SOCKET EVENT************", "WE CAN PUT CUSTOM CODE HERE :)");
    }

    virtual void onError(std::string error) override
    {
        ESP_LOGI("EVENTS--->", "THERE ARE ERRROS HERE-----:(");
    }
};

#endif