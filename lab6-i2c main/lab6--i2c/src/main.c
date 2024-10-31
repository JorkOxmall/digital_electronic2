#include "timer.h"
#include "twi.h"
#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

/* Global variables --------------------------------------------------*/
struct DHT_values_structure {
    uint8_t hum_int;
    uint8_t hum_dec;
    uint8_t temp_int;
    uint8_t temp_dec;
    uint8_t checksum;
} dht12;

volatile uint8_t new_sensor_data = 0;

#define SENSOR_ADR 0x5c
#define SENSOR_HUM_MEM 0
#define SENSOR_TEMP_MEM 2
#define SENSOR_CHECKSUM 4

/* Function definitions ----------------------------------------------*/
/**********************************************************************
* Function: Main function where the program execution begins
* Purpose:  Wait for new data from the sensor and send them to UART.
* Returns:  none
**********************************************************************/
int main(void)
{
    char string[10];  // Increased size for larger numbers

    // TWI initialization
    twi_init();

    // UART initialization
    uart_init(UART_BAUD_SELECT(115200, F_CPU));

    sei();  // Enable global interrupts

    // Test if sensor is ready
    if (twi_test_address(SENSOR_ADR) == 0)
        uart_puts("I2C sensor detected\r\n");
    else {
        uart_puts("[ERROR] I2C device not detected\r\n");
        while (1);
    }

    // Timer1 configuration
    TIM1_ovf_1sec();
    TIM1_ovf_enable();

    // Infinite loop
    while (1) {
        if (new_sensor_data == 1) {
            // Display humidity
            itoa(dht12.hum_int, string, 10);
            uart_puts("Humidity: ");
            uart_puts(string);
            uart_puts(".");
            itoa(dht12.hum_dec, string, 10);
            uart_puts(string);
            uart_puts(" %RH\r\n");

            // Display temperature
            itoa(dht12.temp_int, string, 10);
            uart_puts("Temperature: ");
            uart_puts(string);
            uart_puts(".");
            itoa(dht12.temp_dec, string, 10);
            uart_puts(string);
            uart_puts(" °C\r\n");

            // Display checksum
            itoa(dht12.checksum, string, 10);
            uart_puts("Checksum: ");
            uart_puts(string);
            uart_puts("\r\n");

            // Reset the new data flag
            new_sensor_data = 0;
        }
    }

    return 0;  // Will never reach this
}

/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
* Function: Timer/Counter1 overflow interrupt
* Purpose:  Read temperature and humidity from DHT12, SLA = 0x5c.
**********************************************************************/
ISR(TIMER1_OVF_vect)
{
    // Read values from Temp/Humid sensor
    twi_start();
    if (twi_write((SENSOR_ADR << 1) | TWI_WRITE) == 0) {
        // Set internal memory location for humidity
        twi_write(SENSOR_HUM_MEM);
        twi_stop();

        // Read data from internal memory
        twi_start();
        twi_write((SENSOR_ADR << 1) | TWI_READ);
        dht12.hum_int = twi_read(TWI_ACK);
        dht12.hum_dec = twi_read(TWI_ACK);
        dht12.temp_int = twi_read(TWI_ACK);
        dht12.temp_dec = twi_read(TWI_ACK);
        dht12.checksum = twi_read(TWI_NACK);
        twi_stop();

        // Checksum validation
        uint8_t calculated_checksum = dht12.hum_int + dht12.hum_dec + 
                                       dht12.temp_int + dht12.temp_dec;
        if (calculated_checksum == dht12.checksum) {
            new_sensor_data = 1; // Indicate that new data is available
        } else {
            uart_puts("[ERROR] Checksum mismatch\r\n");
        }
    } else {
        twi_stop();
    }
}
