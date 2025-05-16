#include "pico/stdlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dht11.h"


#define TIMEOUT_US 1000  // 1 millisecond
#define MAX_ATTEMPTS 30  
#define GPIO 1

uint64_t measure_hold_time_us(int target_value);
void customSleep(unsigned int microseconds);
int getByte(uint8_t *data, int index);
int internal_check_temperature(DHT11Data *response);


int check_temperature(DHT11Data *response) {
    int count = 0;

    int status;
    do {
        status = internal_check_temperature(response);
        if (status == 0) {
            return status;
        }
        switch(status) {
            case -1:
                printf("Attempt %d: Timeout while waiting for DHT11 line state.\n", count);
                break;
            case -2:
                printf("Attempt %d: Failed to access GPIO line.\n", count);
                break;
            case -3:
                printf("Attempt %d: Checksum mismatch — data integrity error.\n", count);
                break;

        }
        sleep_ms(1000);
        count = count + 1;
    } while (count < MAX_ATTEMPTS);

    return status;
}


int internal_check_temperature(DHT11Data *response) {
    gpio_init(GPIO); //TODO I think this should be called only once

    // printf("Setting GPIO to write and sending request to dth11\n");
    gpio_set_dir(GPIO, true); // Set GPIO to write mode
    gpio_put(GPIO, 1);
    customSleep(18000);

    gpio_put(GPIO, 0);
    customSleep(18000); // 18 ms

    // printf("Setting GPIO to read and trying to read the response from dth11\n");
    gpio_set_dir(GPIO, false); // Set GPIO to read mode


    uint64_t initial_response_zero = measure_hold_time_us(0);
    if (initial_response_zero == UINT64_MAX) {
        return -1;
    }
    //printf("First 0 response was hold for %d", initial_response_zero);

    uint64_t initial_response_one = measure_hold_time_us(1);
    if (initial_response_one == UINT64_MAX) {
        return -1;
    }
    //printf("Second 1 response was hold for %d", initial_response_one);


    int i;
    uint8_t data[40];

    for (i = 0; i < 40; i++) {
        uint64_t zeroTime = measure_hold_time_us(0);
        if (zeroTime == UINT64_MAX) {
            return -1;
        }
    
        uint64_t oneTime = measure_hold_time_us(1);
        if (oneTime == UINT64_MAX) {
            return -1;
        }

        if (abs(oneTime - 27) < abs(oneTime - 70)) {
            data[i] = 0;
        }
        else {
            data[i] = 1;
        }
    }

    if (getByte(data, 32) != (getByte(data, 0) + getByte(data, 8) + getByte(data, 16) + getByte(data, 24))) {
        return -3;
    }

    response->temperature = getByte(data, 16) + (getByte(data, 24) / 100);
    response->humidity = getByte(data, 0) + (getByte(data, 8) / 100);

    // printf("Temperature %d,%d\n", getByte(data, 16), getByte(data, 24));
    // printf("Humidity %d,%d\n", getByte(data, 0), getByte(data, 8));
    // printf("Checksum %d\n", getByte(data, 32));

    return 0;
}

int getByte(uint8_t *data, int index) {
    int total = 0;
    for (int i = 0; i < 8; i++) {
        total <<= 1;                // shift bits left by 1 (same as multiply by 2)
        total |= data[index + i];  // add the current bit (0 or 1)
    }
    return total;    
}

void customSleep(unsigned int microseconds) {
    sleep_us(microseconds);    
}

uint64_t measure_hold_time_us(int target_value) {
    uint64_t start, now, end;

    // Wait until the line equals the target value
    start = time_us_64();
    while (gpio_get(GPIO) != target_value) {
        now = time_us_64();
        if (now - start > TIMEOUT_US) {
            return UINT64_MAX;
        }
    }

    // Start timing
    start = time_us_64();

    // Wait while the pin remains at the target value or timeout
    while (gpio_get(GPIO) == target_value) {
        now = time_us_64();
        if (now - start > TIMEOUT_US) {
            return UINT64_MAX;
        }
    }

    end = time_us_64();

    return end - start;  // Return the duration the pin stayed at target_value
}
