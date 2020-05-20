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
 * @file Sema.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * 
 */

#include "Sema.h"

#include "esp_log.h"

namespace util {

static const char* TAG = "Semaphore";

Semaphore::Semaphore(const char* dbg_name)
    : m_handle(),
      m_dbg_name(dbg_name)
{
    ESP_LOGD(TAG, "new semaphore \"%s\"", m_dbg_name);
    m_handle = xSemaphoreCreateBinary();
    xSemaphoreGive(m_handle);
}

Semaphore::Semaphore(Semaphore&& other)
    : m_handle(other.m_handle),
      m_dbg_name(other.m_dbg_name)
{
    other.m_handle = 0;
}

Semaphore::~Semaphore()
{
    if (m_handle != 0) vSemaphoreDelete(m_handle);
}

void Semaphore::take()
{
    ESP_LOGD(TAG, "take \"%s\"", m_dbg_name);
    xSemaphoreTake(m_handle, portMAX_DELAY);
}

bool Semaphore::take(std::uint32_t timeout_ms)
{
    ESP_LOGD(TAG, "take: %s for %d ms", m_dbg_name, timeout_ms);

	TickType_t timeout_ticks = timeout_ms / portTICK_PERIOD_MS;
    bool taken = xSemaphoreTake(m_handle, timeout_ticks) == pdTRUE;

    if (taken) {
        ESP_LOGD(TAG, "Semaphore taken:  %s", m_dbg_name);
    } else {
        ESP_LOGE(TAG, "Semaphore NOT taken:  %s", m_dbg_name);
    }
    return taken;
}

void Semaphore::give()
{
    ESP_LOGD(TAG, "give \"%s\"", m_dbg_name);
    xSemaphoreGive(m_handle);
}

void Semaphore::wait()
{
    ESP_LOGD(TAG, "wait on \"%s\"", m_dbg_name);

    xSemaphoreTake(m_handle, portMAX_DELAY);
    xSemaphoreGive(m_handle);
}

} // namespace util
