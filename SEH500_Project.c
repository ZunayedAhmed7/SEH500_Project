
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "fsl_i2c.h"

#define SENSOR_ADDRRESS 0x1D
#define WRITE_REG_ADDRRESS 0x01

// Threshold: if axis value > 1000, that direction is tilted
#define TILT_THRESHOLD 1000

// Global variable to track if system is running
volatile bool is_running = false;  // Start in STOPPED state

//Assembly Function Declarations
void led_setup();
void red_led_on();
void red_led_off();
void blue_led_on();
void blue_led_off();
void green_led_on();
void green_led_off();

void turn_all_led_off(){
	green_led_off();
	red_led_off();
	blue_led_off();

};

void turn_white_led_on(){ //R+G+B = White
	green_led_on();
	red_led_on();
	blue_led_on();
}
/* PORTA_IRQn interrupt handler */
void GPIOA_IRQHANDLER(void) {
	//Clear intterupt
	GPIO_PortClearInterruptFlags(GPIOA, 1U << 10U);

    // Toggle the running state
    is_running = !is_running;

    if (is_running) {
        PRINTF("\n\r*** STARTED ***\n\r");
    } else {
        PRINTF("\n\r*** STOPPED ***\n\r");
        turn_all_led_off();  // Turn off all LEDs when stopped
    }
}


int main(void) {
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    led_setup();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    BOARD_InitDebugConsole();
#endif
    uint8_t data[6];  // Will hold X, Y, Z data (2 bytes each)
    int16_t x, y, z;
    uint8_t sensorSetup[2];

    // Setup I2C transfer structure
    i2c_master_transfer_t ic2_transaction;
    ic2_transaction.slaveAddress = SENSOR_ADDRRESS;

    // ACTIVATE THE SENSOR (write 0x01 to register 0x2A)
    sensorSetup[0] = 0x2A;  // CTRL_REG1 register
    sensorSetup[1] = 0x01;  // Set ACTIVE bit

    //Setup Data packagee
    ic2_transaction.direction = kI2C_Write;
    ic2_transaction.data = sensorSetup;
    ic2_transaction.dataSize = 2;
    ic2_transaction.flags = kI2C_TransferDefaultFlag;
    I2C_MasterTransferBlocking(I2C0, &ic2_transaction);

    // Small delay for sensor to start
    for(volatile int i=0; i<100000; i++);
    PRINTF("\n\r*** Stand By Mode ***\n\r");
    while(1)
     {
         // Only run if button has been pressed (is_running = true)
         if (is_running) {
             // Step 1: Tell sensor which register to read (WRITE_REG_ADDRRESS = 0x01)
             uint8_t reg = WRITE_REG_ADDRRESS;
             ic2_transaction.direction = kI2C_Write;
             ic2_transaction.data = &reg;
             ic2_transaction.dataSize = 1;
             ic2_transaction.flags = kI2C_TransferNoStopFlag;
             I2C_MasterTransferBlocking(I2C0, &ic2_transaction);

             // Step 2: Read 6 bytes (X, Y, Z - 2 bytes each)
             ic2_transaction.direction = kI2C_Read;
             ic2_transaction.data = data;
             ic2_transaction.dataSize = 6;
             ic2_transaction.flags = kI2C_TransferRepeatedStartFlag;
             I2C_MasterTransferBlocking(I2C0, &ic2_transaction);

             // Step 3: Combine bytes into X, Y, Z values, Bit shift by 2 as sensor output is 14 bits combined
             x = ((int16_t)(data[0] << 8 | data[1])) >> 2;
             y = ((int16_t)(data[2] << 8 | data[3])) >> 2;
             z = ((int16_t)(data[4] << 8 | data[5])) >> 2;

             // Step 4: Detect tilt direction
             if (x > TILT_THRESHOLD) {
                 PRINTF("RIGHT\n\r");
                 turn_all_led_off();
                 red_led_on();
             }
             else if (x < -TILT_THRESHOLD) {
                 PRINTF("LEFT\n\r");
                 turn_all_led_off();
                 blue_led_on();
             }
             else if (y > TILT_THRESHOLD) {
                 PRINTF("FORWARD\n\r");
                 turn_all_led_off();
                 green_led_on();
             }
             else if (y < -TILT_THRESHOLD) {
                 PRINTF("BACKWARD\n\r");
                 turn_all_led_off();
                 turn_white_led_on();
             }
             else {
                 PRINTF("FLAT\n\r");
                 turn_all_led_off();
             }

             // Delay
             for(volatile int i=0; i<1000000; i++);
         }
         else {
        	 PRINTF("\n\r*** Stand By Mode ***\n\r");
             //delay
             for(volatile int i=0; i<1000000; i++);
         }
     }
}
