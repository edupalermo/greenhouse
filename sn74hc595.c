#include "sn74hc595.h"

#include "pico/stdlib.h"

#define SN74HC595_DS     15 // DATA
#define SN74HC595_SHCP   14 // CLOCK
#define SN74HC595_STCP   13 // LATCH

/**
 * Set an output on 
 */
void shift_out(uint8_t value) {

    static bool gpio_initilized = false;

    if (!gpio_initilized) {
        gpio_init(SN74HC595_DS);
        gpio_set_dir(SN74HC595_DS, GPIO_OUT);

        gpio_init(SN74HC595_SHCP);
        gpio_set_dir(SN74HC595_SHCP, GPIO_OUT);

        gpio_init(SN74HC595_STCP);
        gpio_set_dir(SN74HC595_STCP, GPIO_OUT);


        gpio_initilized = true;
    }


    gpio_put(SN74HC595_STCP, 0);  // Begin latch low

    for (int i = 7; i >= 0; i--) {
        bool bit = (value >> i) & 1;
        gpio_put(SN74HC595_DS, bit);

        gpio_put(SN74HC595_SHCP, 1);
        sleep_us(1); // Small delay to allow clock edge to settle

        gpio_put(SN74HC595_SHCP, 0);
        sleep_us(1);  // Optional: delay for falling edge
    }

    gpio_put(SN74HC595_STCP, 1);  // Pulse latch high to update outputs
}