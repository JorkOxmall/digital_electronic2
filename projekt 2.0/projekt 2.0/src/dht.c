#include "dht.h"
#include <util/delay.h>

#define DHT_TIMEOUT 200

static uint8_t dht_read_byte(uint8_t pin) {
    uint8_t result = 0;
    for (uint8_t i = 0; i < 8; i++) {
        while (!(PIND & (1 << pin))); // wait for pin to go high
        _delay_us(30);
        if (PIND & (1 << pin)) {
            result |= (1 << (7 - i));
        }
        while (PIND & (1 << pin)); // wait for pin to go low
    }
    return result;
}

int8_t dht_read_data(uint8_t type, uint8_t pin, int8_t *humidity, int8_t *temperature) {
    uint8_t data[5] = {0};

    // Send start signal
    DDRD |= (1 << pin); // set pin as output
    PORTD &= ~(1 << pin); // pull pin low
    _delay_ms(20);
    PORTD |= (1 << pin); // pull pin high
    _delay_us(40);
    DDRD &= ~(1 << pin); // set pin as input

    // Wait for response
    _delay_us(80);
    if (PIND & (1 << pin)) return -1;
    _delay_us(80);
    if (!(PIND & (1 << pin))) return -1;

    // Read data
    for (uint8_t i = 0; i < 5; i++) {
        data[i] = dht_read_byte(pin);
    }

    // Check checksum
    if (data[4] != (data[0] + data[1] + data[2] + data[3])) return -1;

    // Convert data
    *humidity = data[0];
    *temperature = data[2];

    return 0;
}