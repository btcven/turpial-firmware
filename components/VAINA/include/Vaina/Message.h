/**
 * Copyright 2020 btcven and Locha Mesh developers
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @ingroup     vaina
 * @{
 *
 * @file
 * @author      Locha Mesh Developers (contact@locha.io)
 * @brief       VAINA Messages
 */

#ifndef VAINA_MESSAGE_H
#define VAINA_MESSAGE_H

#include <cstdint>
#include <array>

#include <esp_netif_ip_addr.h>

namespace vaina {

/**
 * @brief   VAINA message types
 */
enum class MessageType : std::uint8_t {
    Ack = 0,    /**< Message acknowledged */
    Nack = 1,   /**< Message not-acknowledged */
    RcsAdd = 2, /**< Add entry to Router Client Set */
    RcsDel = 3, /**< Delete entry from Router Client Set */
    NibAdd = 4, /**< Add entry to NIB */
    NibDel = 5, /**< Delete entry from NIB */
};

/**
 * @brief   RCS add
 */
struct RcsAdd {
    esp_ip6_addr_t ip; /**< IPv6 address to add */
};

/**
 * @brief   RCS delete
 */
struct RcsDel {
    esp_ip6_addr_t ip; /**< IPv6 address to delete */
};

/**
 * @brief   NIB add
 */
struct NibAdd {
    std::uint8_t prefix; /**< IPv6 address prefix in bits */
    esp_ip6_addr_t ip; /**< IPv6 address to add */
};

/**
 * @brief   NIB delete
 */
struct NibDel {
    std::uint8_t prefix; /** IPv6 address prefix in bits */
    esp_ip6_addr_t ip; /**< IPv6 address to delete */
};

/**
 * @brief   Message payload
 *
 * This is the payload of a message, only ACK and NACK have no payload
 * (only a sequence number). We use an union and not inherithance, classes, or
 * anything more complex as the goal is to not have any allocations. This way
 * the size for each message is known at compile time.
 */
union MessagePayload {
    RcsAdd rcs_add; /**< RCS add message */
    RcsDel rcs_del; /**< RCS delete message */
    NibAdd nib_add; /**< NIB add message */
    NibDel nib_del; /**< NIB delete message */
};

/**
 * @brief   VAINA max message size in bytes.
 */
static constexpr std::size_t MAX_MESSAGE_SIZE = 2 + sizeof(MessagePayload);

typedef std::array<std::uint8_t, MAX_MESSAGE_SIZE> MsgBuf;

/**
 * @brief   Generic message type
 */
class Message {
public:
    /**
     * @brief   Construct an RCS add Message
     */
    Message(std::uint8_t seqno, RcsAdd& payload)
        : m_type(MessageType::RcsAdd)
        , m_seqno(seqno)
        , m_payload{}
    {
        m_payload.rcs_add = payload;
    }

    /**
     * @brief   Construct an RCS delete Message
     */
    Message(std::uint8_t seqno, RcsDel& payload)
        : m_type(MessageType::RcsDel)
        , m_seqno(seqno)
        , m_payload{}
    {
        m_payload.rcs_del = payload;
    }

    /**
     * @brief   Get message type
     */
    MessageType type() { return m_type; }

    /**
     * @brief   Get message sequence number
     */
    std::uint8_t seqno() { return m_seqno; }

    /**
     * @brief   Serialize this message
     *
     * @param[out] buf The output message buffer.
     *
     * @return The number of bytes written to `buf`.
     */
    int serialize(MsgBuf& buf);

private:
    MessageType m_type;       /**< Message type */
    std::uint8_t m_seqno;     /**< The sequence number for this messsage */
    MessagePayload m_payload; /**< Message payload, it's an union */
};

} // namespace vaina

#endif // VAINA_MESSAGE_H
