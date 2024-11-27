#ifndef DHT_H
#define DHT_H

#include <avr/io.h>

#define DHT_TYPE_DHT22 22

int8_t dht_read_data(uint8_t type, uint8_t pin, int8_t *humidity, int8_t *temperature);

#endif // DHT_H