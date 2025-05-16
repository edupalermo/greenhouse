#ifndef SN74HC595_H
#define SN74HC595_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>  // for uint8_t, uint16_t

void shift_out(uint8_t value);

#ifdef __cplusplus
}
#endif

#endif // SN74HC595_H

