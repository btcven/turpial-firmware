/**
 * @file ErrUtil.h
 * @author Locha Mesh project developers (locha.io)
 * @brief
 * @version 0.1
 * @date 2019-12-17
 *
 * @copyright Copyright (c) 2019 Locha Mesh project
 *
 */

#ifndef UTIL_ERRUTIL_H
#define UTIL_ERRUTIL_H

#include <esp_err.h>
#include <esp_log.h>

#define ESP_ERR_TRY(expr)     \
    do {                      \
        esp_err_t err = expr; \
        if (err != ESP_OK) {  \
            return err;       \
        }                     \
    } while (false)

#endif // UTIL_ERRUTIL_H
