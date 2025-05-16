# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.13)

set(PROGRAM_NAME pico_w_webserver)
set(PICO_BOARD pico_w)

include(pico_sdk_import.cmake)

project(pico_w_webserver)

pico_sdk_init()

add_executable(${PROGRAM_NAME}
    main.c
    dht11.c
    sn74hc595.c
    mcp3008.c
)

target_sources(${PROGRAM_NAME} PRIVATE
    ${PICO_SDK_PATH}/lib/lwip/src/apps/sntp/sntp.c
)

target_include_directories(${PROGRAM_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_definitions(${PROGRAM_NAME} PRIVATE
    LWIP_HTTPD=1
    LWIP_HTTPD_CUSTOM_FILES=1
    LWIP_HTTPD_DYNAMIC_HEADERS=1
    LWIP_HTTPD_SSI=0
    LWIP_HTTPD_CGI=0
    SNTP_SUPPORT=1
    SNTP_SERVER_DNS=1
    MEMP_NUM_SYS_TIMEOUT=10
)

target_link_libraries(${PROGRAM_NAME}
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_http
    pico_stdlib
    hardware_adc
    hardware_rtc
    hardware_spi
)

pico_enable_stdio_usb(${PROGRAM_NAME} TRUE)
pico_enable_stdio_uart(${PROGRAM_NAME} FALSE)

pico_add_extra_outputs(${PROGRAM_NAME})