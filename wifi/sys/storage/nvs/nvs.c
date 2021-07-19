
/**
 * Copyright 2021 btcven and Locha Mesh developers
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
 * @ingroup     storage_nvs
 * @{
 *
 * @file        nvs.c
 * @brief       this component uses the nvs_flash library 
 *              for esp32 memory management
 *
 * @author      Locha Mesh Developers(conctact@locha.io)
 * @}
 */


#include "nvs_flash/include/nvs_flash.h"
#define ENABLE_DEBUG (1)
#include "debug.h"
#include "storage/nvs.h"

uint32_t _handle  /** NVS handle */;
bool is_open = false;


int nvs_init(void)
{

    esp_err_t err = nvs_flash_init();
    
    if (err != ESP_OK) {
        nvs_flash_erase();
        err = nvs_flash_init();
    }
    err = nvs_open("storage", NVS_READWRITE, &_handle);
    if (err != ESP_OK) {
        printf("Error (%d) opening NVS handle!\n", err);
    } 

   is_open = true;
  return err;
}

int nvs_set_string(char* key, char* string_value)
{
    if(!is_open){
        printf("Error: NVS is close \n");
        return ESP_FAIL;
    }

    return nvs_set_str(_handle, key, string_value);
}


int nvs_get_string(char* key,  char* buffer, size_t* length)
{
    if(!is_open){
        printf("Error: NVS is close \n");
        return ESP_FAIL;
    } 

    return nvs_get_str(_handle, key, buffer, length);
}


int set_bool(char* key, bool value)
{   
     if(!is_open){
         printf("Error: NVS is close \n");
        return ESP_FAIL;
    } 
    return nvs_set_u8(_handle, key, value ? 1 : 0);
}


int get_bool(char* key, bool* value )
{    

    if(!is_open){
       printf("Error: NVS is close \n");
       return ESP_FAIL; 
    }
    
    uint8_t v = 0;
    esp_err_t err = nvs_get_u8(_handle, key, &v);
    if (err != ESP_OK) {
        return err;
    }

    if (v == 1) {
        *value = true;
    } else {
         *value = false;
    }

    return 0;
}


int _nvs_commit(void)
{
    
    if(!is_open){
       printf("Error: NVS is close \n");
       return ESP_FAIL; 
    }
    return nvs_commit(_handle);   
}


int _nvs_close(void)
{
    printf("Closing nvs \n");

    if (!is_open) {
     nvs_close(_handle);
    }
    is_open = false;

    return ESP_OK;
}

int set_blob(char* key, uint8_t* buffer, size_t length)
{
   if(!is_open){
       printf("Error: NVS is close \n");
       return ESP_FAIL; 
    }

    esp_err_t err = nvs_set_blob(_handle, key, (void*)buffer, length);
    
    return err;
} 

int get_blob(char* key, uint8_t* buffer, size_t length)
{
    if(!is_open){
       printf("Error: NVS is close \n");
       return ESP_FAIL; 
    }

   return nvs_get_blob(_handle, key, (void*)buffer, (size_t*)length);
} 
