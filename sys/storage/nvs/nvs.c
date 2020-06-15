
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
 * @defgroup     storage_tfnvs
 * @ingroup      storage_tfnvs
 * @{
 *
 * @brief       Non-Volatile Storage
 *
 * @author      Locha Mesh Developers(conctact@locha.io)
 * @}
 */

#include "storage/tfnvs.h"

#include "nvs_flash/include/nvs_flash.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

uint32_t _handle = 0;

int tfnvs_init(void)
{
    if (nvs_flash_init() != ESP_OK) {
        DEBUG_PUTS("tfnvs: open error, erasing flash and retrying");

        nvs_flash_erase();

        if (nvs_flash_init() != ESP_OK) {
            DEBUG_PUTS("tfnvs: fatal error, retry failed");
            return -1;
        }
    }

    if (nvs_open("storage", NVS_READWRITE, &_handle) != ESP_OK) {
        DEBUG_PUTS("tfnvs: couldn't open tfnvs handle");
        return -1;
    }

    return 0;
}

int tfnvs_commit(void)
{
    return nvs_commit(_handle) != ESP_OK ? -1 : 0;
}

int tfnvs_set_u8(char *key, uint8_t v)
{
    return nvs_set_u8(_handle, key, v) != ESP_OK ? -1 : 0;
}

int tfnvs_get_u8(char *key,  uint8_t *v)
{
    return nvs_get_u8(_handle, key, v) != ESP_OK ? -1 : 0;
}

int tfnvs_set_blob(char *key, uint8_t *buffer, size_t length)
{
    return nvs_set_blob(_handle, key, (void*)buffer, length) != ESP_OK ? -1 : 0;
}

int tfnvs_get_blob(char *key, uint8_t *buffer, size_t *length)
{
   return nvs_get_blob(_handle, key, buffer, length) != ESP_OK ? -1 : 0;
}
