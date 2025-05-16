// main.c

#include <stdio.h>
#include "lwip/apps/httpd.h"
#include "lwip/apps/sntp.h"
#include "lwip/apps/fs.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwipopts.h"
#include "hardware/rtc.h"

#include "mcp3008.h"
#include "sn74hc595.h"
#include "dht11.h"
#include "ntp_time.h"

const char ssid[] = "FRITZ!Box 7590 CI";
const char pass[] = "06168719993669220503";


void print_current_datetime();
void reset_wifi();
void initialize_sensors();

int main()
{
    stdio_init_all(); // Needed to view output via USB

    rtc_init(); // Initiate the clock

    if (cyw43_arch_init_with_country(CYW43_COUNTRY_GERMANY))
    {
        printf("failed to initialise\n");
        return 1;
    }
    printf("initialised\n");

    cyw43_arch_enable_sta_mode();

    // 🔋 Set to no power saving (maximum performance)
    if (cyw43_wifi_pm(&cyw43_state, 0) != 0) {
        printf("Failed to set power mode\n");
    } else {
        printf("Power mode set to NONE\n");
    }


    int retries = 50;
    while (retries--) {
        printf("Trying to connect to SSID [%s] \n", ssid);
        printf("Trying to connect to PASSWORD [%s] \n", pass);
    
        printf("Attempting to connect to Wi-Fi... (%d retries left)\n", retries);
        //const uint8_t my_bssid[6] = { 0x08, 0xB6, 0x57, 0x5A, 0xA5, 0x17 };

        // if (cyw43_arch_wifi_connect_bssid_timeout_ms(ssid, my_bssid, pass, CYW43_AUTH_WPA2_MIXED_PSK, 10000) == 0) {

        // if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
            printf("connected\n");
            break;
        } else {
            printf("failed to connect, retrying...\n");
            reset_wifi();
            sleep_ms(3000);  // Wait before retry
        }
    }

    if (retries <= 0) {
        printf("All connection attempts failed\n");
        return 1;
    }

    // Use NTP to set the current datetime
    printf("Trying to use NTP to set current datetime\n");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    ip_addr_t addr;
    IP4_ADDR(&addr, 129, 6, 15, 28); // time.nist.gov
    sntp_setserver(0, &addr);
    sntp_init();    

    // initialize_sensors();


    // Initialise web server
    httpd_init();
    printf("Http server initialised\n");
    

    int value = 0x02;

    // Infinite loop
    while (true) {
        print_current_datetime();

        // printf("CLICK!");
        // shift_out(0x02);
        // sleep_ms(2000);
        // shift_out(0x00);

        int c;
        for (c=0; c<=7; c++) {
            printf("Channel %d - %d \n", c, read_adc(c));
        }

        sleep_ms(5000);
    }
}

void print_current_datetime() {
    datetime_t now;
    rtc_get_datetime(&now);
    printf("Current time: %04d-%02d-%02d %02d:%02d:%02d\n",
           now.year, now.month, now.day,
           now.hour, now.min, now.sec);
}

void reset_wifi() {
    cyw43_arch_deinit();     // Deinitialize Wi-Fi
    sleep_ms(100);           // Short delay to stabilize
    if (cyw43_arch_init()) { // Reinitialize Wi-Fi
        printf("Wi-Fi re-init failed!\n");
        return;
    }
    cyw43_arch_enable_sta_mode();
}

// Called by SNTP after time is received
void sntp_set_system_time(uint32_t sec) {
    datetime_t t;
    time_t rawtime = sec;
    struct tm *ptm = gmtime(&rawtime);  // convert to UTC

    t.year = ptm->tm_year + 1900;
    t.month = ptm->tm_mon + 1;
    t.day = ptm->tm_mday;
    t.dotw = ptm->tm_wday;
    t.hour = ptm->tm_hour;
    t.min = ptm->tm_min;
    t.sec = ptm->tm_sec;

    rtc_set_datetime(&t);
    printf("Time updated via SNTP: %04d-%02d-%02d %02d:%02d:%02d\n",
        t.year, t.month, t.day, t.hour, t.min, t.sec);
}

int fs_open_custom(struct fs_file *file, const char *name) {
    printf("fs_open_custom on file [%s]\n", name);
    if (strcmp(name, "/metric") == 0) {
        printf("Trying to generate a json\n", name);
        static char json_buf[128];
        uint32_t seconds = to_ms_since_boot(get_absolute_time()) / 1000;
        snprintf(json_buf, sizeof(json_buf),
            "HTTP/1.0 200 OK\r\nContent-Type: application/json\r\n\r\n"
            "{ \"uptime\": %lu }", seconds);

        file->data = json_buf;
        file->len = strlen(json_buf);
        file->index = strlen(json_buf);
        file->flags = FS_FILE_FLAGS_HEADER_INCLUDED;

        printf("Returning %d\n", ERR_OK);

        return 1;
    }

    static char not_found[] =
    "HTTP/1.0 404 Not Found\r\n"
    "Content-Type: text/html\r\n\r\n"
    "<h1>404 Not Found</h1><p>This path is not served.</p>";

    file->data = not_found;
    file->len = strlen(not_found);
    file->index = strlen(not_found);
    file->flags = FS_FILE_FLAGS_HEADER_INCLUDED;
    return 1;
}

void fs_close_custom(struct fs_file *file) {
    printf("fs_close_custom\n");
}

void initialize_sensors() {
    DHT11Data dht11data;
 
    check_temperature(&dht11data);

    printf("Temperature %.3f \n", dht11data.temperature);
    printf("Humidity %.3f \n", dht11data.humidity);
}