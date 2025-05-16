#ifndef DHT11_SENSOR_H
#define DHT11_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double temperature;
    double humidity;
} DHT11Data;

int check_temperature(DHT11Data *response);

#ifdef __cplusplus
}
#endif

#endif // DHT11_SENSOR_H