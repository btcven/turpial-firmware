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
 * @ingroup      storage_tfsettings
 * @{
 *
 * @file
 * @brief       Turpial firmware persistent settings.
 *
 * @author      Locha Mesh Developers(conctact@locha.io)
 * @}
 */

#include "storage/tfsettings.h"
#include "storage/tfnvs.h"

#include <errno.h>

int tfsettings_set_ap(tfsettings_ap_t *settings)
{
    if (settings == NULL) {
        return -EINVAL;
    }

    if (tfnvs_set_blob("ap.ssid", settings->ssid, WIFI_SSID_LENGTH) < 0){
        return -EIO;
    }

    if (tfnvs_set_blob("ap.password", settings->password, WIFI_PASSWORD_LENGTH) < 0) {
        return -EIO;
    }

    if (tfnvs_set_blob("ap.glb", settings->glb.u8, sizeof(ipv6_addr_t)) < 0) {
        return -EIO;
    }

    if (tfnvs_set_u8("ap.glb_pfx_len", settings->glb_pfx_len) < 0) {
        return -EIO;
    }

    if (tfnvs_commit() < 0) {
        return -EIO;
    }

    return 0;
}

int tfsettings_get_ap(tfsettings_ap_t *settings)
{
    size_t len;

    if (settings == NULL) {
        return -EINVAL;
    }

    len = 0;
    if (tfnvs_get_blob("ap.ssid", NULL, &len) < 0) {
        return -EIO;
    }

    if (tfnvs_get_blob("ap.ssid", settings->ssid, &len) < 0) {
        return -EIO;
    }

    len = 0;
    if (tfnvs_get_blob("ap.password", NULL, &len) < 0) {
        return -EIO;
    }

    if (tfnvs_get_blob("ap.password", settings->password, &len) < 0) {
        return -EIO;
    }

    len = 0;
    if (tfnvs_get_blob("ap.glb", NULL, &len) < 0) {
        return -EIO;
    }

    if (tfnvs_get_blob("ap.glb", settings->glb.u8, &len) < 0) {
        return -EIO;
    }

    if (tfnvs_get_u8("ap.glb_pfx_len", &settings->glb_pfx_len) < 0){
        return -EIO;
    }

    return 0;
}
