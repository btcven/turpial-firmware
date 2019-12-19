/**
 * @file TxBuffer.h
 * @author Locha Mesh project developers (locha.io)
 * @brief
 * @version 0.1
 * @date 2019-12-08
 *
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#ifndef TXBUFFER_H
#define TXBUFFER_H

#include <cstdint>

#include <freertos/FreeRTOS.h>
#include <freertos/ringbuf.h>

#include <WebSocket.h>

#define TXBUFFER_MTU 512

/**
 * @brief TxBuffer communication
 *
 */
class TxBuffer : public WebSocketHandler
{
public:
    TxBuffer(TxBuffer const&) = delete;
    TxBuffer(TxBuffer&&) = delete;

    TxBuffer& operator=(TxBuffer const&) = delete;
    TxBuffer& operator=(TxBuffer&&) = delete;

    /**
     * @brief Get the TxBuffer unique instance
     *
     * @return TxBuffer& a reference to the TxBuffer
     */
    static TxBuffer& getInstance()
    {
        // Instatiated only once, won't be created again
        static TxBuffer g_instance;
        return g_instance;
    }

    /**
     * @brief Receive a pointer to buffer of MTU size in bytes.
     *
     * @return NULL on timeout, or a pointer to the data.
     */
    std::uint8_t* receive(std::size_t* size);

    virtual void onMessage(WebSocketInputStreambuf* input, WebSocket* ws);

private:
    /**
     * @brief Construct a new TxBuffer
     *
     */
    TxBuffer();

    bool send(const std::uint8_t* item, std::size_t size);

    RingbufHandle_t m_handle;
};

#endif // TXBUFFER_H
