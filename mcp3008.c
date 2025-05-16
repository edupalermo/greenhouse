#include "mcp3008.h"

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <stdio.h>

// Define SPI1 pins
#define SPI_PORT spi0
#define MCP3008_SCK  18 //CLOCK
#define MCP3008_MISO 16 // Data from MCP3008 to Pico
#define MCP3008_MOSI 19 // Data from Pico to MCP3008
#define MCP3008_CS   17  // Chip select

#define SPI_TIMEOUT_US 1000 

uint16_t read_adc(uint8_t channel) {

    static bool gpio_initialized = false;

    if (!gpio_initialized) {
        // spi_init(SPI_PORT, 1000 * 1000);  // 1 MHz (your current value)
        spi_init(SPI_PORT, 100 * 1000); // 100 kHz

        gpio_set_function(MCP3008_MISO, GPIO_FUNC_SPI);
        gpio_set_function(MCP3008_CS, GPIO_FUNC_SIO);  // manual CS
        gpio_set_function(MCP3008_SCK, GPIO_FUNC_SPI); // Clock
        gpio_set_function(MCP3008_MOSI, GPIO_FUNC_SPI);

        gpio_init(MCP3008_CS);
        gpio_set_dir(MCP3008_CS, GPIO_OUT);
        gpio_put(MCP3008_CS, 1);  // deselect initially        

        gpio_initialized = true;
    }

    uint8_t tx[] = {
        0b00000001,
        (uint8_t)(0b10000000 | (channel << 4)),
        0x00
    };
    uint8_t rx[3] = {0, 0, 0};

    gpio_put(MCP3008_CS, 0);
    spi_write_read_blocking(SPI_PORT, tx, rx, 3);
    gpio_put(MCP3008_CS, 1);

    uint16_t result = ((rx[1] & 0x03) << 8) | rx[2];
    float voltage = (result * 3.3f) / 1023.0f;

    return result;
}