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
 * @}
 */

#include <Vaina/Message.h>

#include <cstring>

namespace vaina {

static std::uint8_t typeNumber(MessageType type)
{
    return static_cast<std::uint8_t>(type);
}

int Message::serialize(MsgBuf& buf)
{
    int i = 0;

    buf[i + 0] = typeNumber(m_type);
    buf[i + 1] = m_seqno;
    i += 2;

    switch (m_type) {
        // ACKs don't have a payload
        case MessageType::Ack:
        case MessageType::Nack:
            break;

        case MessageType::RcsAdd:
            std::memcpy(&buf[i], &m_payload.rcs_add.ip, sizeof(esp_ip6_addr_t));
            i += sizeof(esp_ip6_addr_t);
            break;

        case MessageType::RcsDel:
            std::memcpy(&buf[i], &m_payload.rcs_del.ip, sizeof(esp_ip6_addr_t));
            i += sizeof(esp_ip6_addr_t);
            break;

        case MessageType::NibAdd:
            buf[i] = m_payload.nib_add.prefix;
            i += 1;
            std::memcpy(&buf[i], &m_payload.nib_add.ip, sizeof(esp_ip6_addr_t));
            i += sizeof(esp_ip6_addr_t);
            break;

        case MessageType::NibDel:
            buf[i] = m_payload.nib_add.prefix;
            i += 1;
            std::memcpy(&buf[i], &m_payload.nib_del.ip, sizeof(esp_ip6_addr_t));
            i += sizeof(esp_ip6_addr_t);
            break;
    }

    return i;
}

} // namespace vaina
