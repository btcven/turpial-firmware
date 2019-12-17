#include <stdio.h>
#include <string.h>

#include <unity.h>

#include "Storage.h"

static void print_banner(const char* text);

extern "C" void app_main(void)
{
    storage::init();

    print_banner("Running all the registered tests");
    UNITY_BEGIN();
    unity_run_all_tests();
    UNITY_END();

    print_banner("Starting interactive test menu");
    unity_run_menu();
}

static void print_banner(const char* text)
{
    printf("\n#### %s #####\n\n", text);
}
