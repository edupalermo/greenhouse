#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <stdio.h>

// Callback to handle each scan result
int scan_result_callback(void *env, const cyw43_ev_scan_result_t *result) {
    if (result->ssid[0]) {
        printf("SSID: %s, RSSI: %d dBm, Security: %d\n",
               result->ssid, result->rssi, result->auth_mode);
    } else {
        printf("SSID: <hidden>, RSSI: %d dBm, Security: %d\n",
               result->rssi, result->auth_mode);
    }
    return 0;
}

int main() {
    stdio_init_all();

    if (cyw43_arch_init_with_country(CYW43_COUNTRY_GERMANY)) {
        printf("Wi-Fi init failed\n");
        return 1;
    }

    printf("Wi-Fi initialized\n");

    cyw43_arch_enable_sta_mode();

    while(1) {

        printf("Scanning for Wi-Fi networks...\n");

        int scan_result = cyw43_wifi_scan(
            &cyw43_state,                      // cyw43_t*
            NULL,                              // use default scan options
            NULL,                              // no custom env/context needed
            scan_result_callback               // callback to print scan results
        );

        if (scan_result < 0) {
            printf("Scan failed with error code: %d\n", scan_result);
            return 1;
        }
        printf("Scan complete.\n");

        // Give time for scan to complete and callbacks to be called
        sleep_ms(10000);
    }

    return 0;
}
