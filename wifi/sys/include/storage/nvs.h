
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
 * @defgroup     storage_nvs
 * @ingroup      storage_nvs
 * @{
 *
 * @file        nvs.h
 * @brief       this component uses the nvs_flash library 
 *              for esp32 memory management
 *
 * @author      Locha Mesh Developers(conctact@locha.io)
 * @}
 */


#ifndef STORAGE_NVS_H
#define STORAGE_NVS_H

/**
 * @brief   Initialize and open NVS
 * 
 * @return   ESP_OK: succeed
 * @return   ESP_FAIL: fail    
 */

int nvs_init(void);

/**
 * @brief   Set the String object
 * 
 * @param key[in]            key value
 * @param string_value[in]   string data
 * 
 * @return    0: succeed
 * @return   -1: fail 
 */
int nvs_set_string(char* key, char* string_value);

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
int nvs_get_string(char* key,  char* buffer, size_t* length);

/**
 * @brief   Set a boolean
 * 
 * @param key[in]:    key in the namespace 
 * @param value[in]:  boolean value
 * 
 * @return   0: succeed
 * @return  -1: fail 
 *      
 */
int set_bool(char* key, bool value);

/**
 * @brief   Get a boolean
 * 
 * @param[in]    key: key in the namespace
 * @param[out]   value: boolean return value
 * 
 * @return   0: succeed
 * @return  -1: fail 
 */
int get_bool(char* key, bool* value);

/**
 * @brief   Commit changes to flash
 * 
 * @return   0: succeed
 * @return  -1: fail 
 */
int _nvs_commit(void);

/**
 * @brief   Close the namespace
 * 
 * @return   0: succeed
 * @return  -1: fail 
 */ 
int _nvs_close(void);

/**
 * @brief   function to save the blob in the memory
 * 
 * @return   0: succeed
 * @return  -1: fail 
 */ 
int  set_blob(char* key, uint8_t* buffer, size_t length); 

/**
 * @brief   function to save the blob in the memory
 * 
 * @return   0: succeed
 * @return  -1: fail 
 */ 
int  get_blob(char* key, uint8_t* buffer, size_t length); 

#endif /* NET_VAINA_H */
