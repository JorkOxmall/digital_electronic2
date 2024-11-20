#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "dht.h"

#define DHTPIN 2
#define LDRPIN 0 // Analogový pin A0

void uart_init(unsigned int ubrr) {
    // Nastavení baud rate
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    // Povolení vysílače
    UCSR0B = (1 << TXEN0);
    // Nastavení formátu rámce: 8 datových bitů, 1 stop bit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(unsigned char data) {
    // Čekání na uvolnění vysílacího bufferu
    while (!(UCSR0A & (1 << UDRE0)));
    // Zapsání dat do vysílacího bufferu
    UDR0 = data;
}

void uart_print(const char *str) {
    while (*str) {
        uart_transmit(*str++);
    }
}

void setup() {
    // Inicializace UART
    uart_init(103); // 9600 baud rate při 16 MHz

    // Aktivace interního pull-up rezistoru na DHTPIN
    DDRD &= ~(1 << DHTPIN); // Nastavení pinu jako vstup
    PORTD |= (1 << DHTPIN); // Aktivace interního pull-up rezistoru

    // Inicializace ADC
    ADMUX = (1 << REFS0); // Referenční napětí AVcc
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Povolení ADC a nastavení prescaleru na 128
}

uint16_t read_adc(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F); // Vybrání kanálu
    ADCSRA |= (1 << ADSC); // Zahájení konverze
    while (ADCSRA & (1 << ADSC)); // Čekání na dokončení konverze
    return ADC;
}

void loop() {
    int8_t temperature = 0;
    int8_t humidity = 0;

    // Čtení vlhkosti a teploty
    if (dht_read_data(DHT_TYPE_DHT22, DHTPIN, &humidity, &temperature) == 0) {
        // Výpis dat na sériový monitor
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "Vlhkost: %d %%\n", humidity);
        uart_print(buffer);
        snprintf(buffer, sizeof(buffer), "Teplota: %d °C\n", temperature);
        uart_print(buffer);
    } else {
        uart_print("Chyba při čtení z DHT senzoru!\n");
    }

    // Čtení hodnoty z fotorezistoru
    uint16_t ldr_value = read_adc(LDRPIN);
    if (ldr_value < 512) {
        uart_print("Noc\n");
    } else {
        uart_print("Den\n");
    }

    _delay_ms(2000); // Zpoždění mezi měřeními
}

int main(void) {
    setup();
    while (1) {
        loop();
    }
    return 0;
}