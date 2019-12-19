/**
 * @file TxBuffer.cpp
 * @author Locha Mesh project developers (locha.io)
 * @brief
 * @version 0.1
 * @date 2019-12-08
 *
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include "TxBuffer.h"

#include <istream>

#include <esp_log.h>

static const char* TAG = "TxBuffer";

TxBuffer::TxBuffer()
    : m_handle(nullptr)
{
    // 25 KiB buffer size
    m_handle = xRingbufferCreate(25600, RINGBUF_TYPE_BYTEBUF);
    if (m_handle == nullptr) {
        ESP_LOGE(TAG, "Can't create ringbuffer");
    }
}


void TxBuffer::onMessage(WebSocketInputStreambuf* input, WebSocket* ws)
{
    std::istream in(input);

    char buf[TXBUFFER_MTU] = {0};
    while (in.read(buf, sizeof(buf))) {
        send(reinterpret_cast<std::uint8_t*>(buf), TXBUFFER_MTU);
    }
}

std::uint8_t* TxBuffer::receive(std::size_t* size)
{
    void* ptr = xRingbufferReceiveUpTo(m_handle, size, portMAX_DELAY, TXBUFFER_MTU);
    return reinterpret_cast<std::uint8_t*>(ptr);
}

bool TxBuffer::send(const std::uint8_t* item, std::size_t size)
{
    return xRingbufferSend(m_handle, item, size, portMAX_DELAY) == pdTRUE;
}
