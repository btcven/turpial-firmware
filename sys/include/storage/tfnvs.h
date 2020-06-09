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

#ifndef STORAGE_TFNVS_H
#define STORAGE_TFNVS_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize and open NVS
 *
 * @return 0 on succeess
 * @return 0< on failure
 */

int tfnvs_init(void);

/**
 * @brief   Commit (write) changes to flash
 *
 * @return 0 on succeess
 * @return 0< on failure
 */
int tfnvs_commit(void);

/**
 * @brief   Set the String object
 *
 * @param key[in]            key value
 * @param string_value[in]   string data
 *
 * @return    0: succeed
 * @return   -1: fail
 */
int tfnvs_set_u8(char *key, uint8_t v);

/**
 * @brief   Get the String object
 *
 * @param key[in]      key value
 * @param buffer[out]  return value
 * @param length[in]
 *
 * @return   0: succeed
 * @return  -1: fail
 */
int tfnvs_get_u8(char *key,  uint8_t *u8);

/**
 * @brief   function to save the blob in the memory
 *
 * @return 0 on succeess
 * @return 0< on failure
 */
int tfnvs_set_blob(char *key, uint8_t *buffer, size_t length);

/**
 * @brief   function to save the blob in the memory
 *
 * @return 0 on succeess
 * @return 0< on failure
 */
int tfnvs_get_blob(char *key, uint8_t *buffer, size_t *length);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* STORAGE_TFNVS_H */
