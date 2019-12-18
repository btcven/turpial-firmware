/**
 * @file test_util_sema.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-12-02
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include "Sema.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <unity.h>

TEST_CASE("Semaphore take, give and wait", "[util]")
{
    util::Semaphore sema("TestSema");

    sema.take();
    sema.give();

    // Should not block
    sema.wait();
}
