#ifndef MCP3008_H
#define MCP3008_H

#include <stdint.h>  // for uint8_t, uint16_t

uint16_t read_adc(uint8_t channel);

#endif // MCP3008_H