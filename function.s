
@REFERENCE: SEH500, Lab 7 "GPIO and Interrupt" was used as a reference to setup functions

.syntax unified
.cpu cortex-m4
.thumb

.text

.global led_setup               @ declare as a global variable
.type led_setup, %function      @ set to function type

led_setup:
    ldr r1, =0x40048038     @ System Clock Gate Control Register 5 (SIM_SCGC5)
    ldr r0, [r1]            @ read current register value
    orr r0, r0, #(1<<11)  @ For K66, red LED is at port C (bit 11)
    str r0, [r1]            @ apply the new settings

	@GREEN LED
    ldr r0, [r1]            @ read current register value
    orr r0, r0, #(1<<13)  @ Green = Port E, bit 13
    str r0, [r1]            @ apply the new settings

	@BLUE LED
	ldr r0, [r1]            @ read current register value
    orr r0, r0, #(1<<13)  @ Blue = Port A, bit 9
    str r0, [r1]            @ apply the new settings

    @-------------
    @RED LED
    ldr r1, =0x4004B024    @ For K66, PTC9 Pin Control Register (PORTC_PCR9)
    mov r0, #0x00000100     @ set pin to GPIO mode
    str r0, [r1]            @ apply the new settings

    @GREEN LED
    ldr r1, =0x4004D018     @(PORTE_PCR6)
    mov r0, #0x00000100     @ set pin to GPIO mode
    str r0, [r1]            @ apply the new settings

    @BLUE LED
    ldr r1, =0x4004902C     @(PORTA_PCR11)
    mov r0, #0x00000100     @ set pin to GPIO mode
    str r0, [r1]            @ apply the new settings

	@---------------
	@RED LED
    ldr r1, =0x400FF094    @ For K66, GPIOC Port Data Direction Register (GPIOC_PDDR)
    mov r0, #(1<<9)        @ For K66, set pin 9 to output mode
    str r0, [r1]            @ apply the new settings

    @GREEN LED
    ldr r1, =0x400FF114    @ For K66, GPIOC Port Data Direction Register (GPIOC_PDDR)
    mov r0, #(1<<6)        @ For K66, set pin 9 to output mode
    str r0, [r1]            @ apply the new settings

    @BLUE LED
    ldr r1, =0x400FF014    @ For K66, GPIOC Port Data Direction Register (GPIOC_PDDR)
    mov r0, #(1<<11)        @ For K66, set pin 9 to output mode
    str r0, [r1]            @ apply the new settings

    bx  lr

.global red_led_on
.type red_led_on, %function
.global red_led_off
.type red_led_off, %function


red_led_on:
    ldr r1, =0x400FF080    @ For K66, GPIOC Port Data Output Register (GPIOC_PDOR)
    mov r0, #0              @ set output to LOW, LED on
    str r0, [r1]            @ apply settings
    bx  lr

red_led_off:
    ldr r1, =0x400FF080    @ For K66, GPIOC Port Data Output Register (GPIOC_PDOR)
    mov r0, #(1<<9)        @ For K66, set pin 9 output to HIGH, LED off
    str r0, [r1]            @ apply settings
    bx  lr


.global green_led_on
.type green_led_on, %function
.global green_led_off
.type green_led_off, %function

green_led_on:
    ldr r1, =0x400FF100    @ For K66, GPIOC Port Data Output Register (GPIOC_PDOR)
    mov r0, #0              @ set output to LOW, LED on
    str r0, [r1]
    bx  lr            @ apply settings

green_led_off:
    ldr r1, =0x400FF100    @ For K66, GPIOC Port Data Output Register (GPIOC_PDOR)
    mov r0, #(1<<6)        @ For K66, set pin 9 output to HIGH, LED off
    str r0, [r1]
    bx  lr            @ apply settings

.global blue_led_on
.type blue_led_on, %function
.global blue_led_off
.type blue_led_off, %function


blue_led_on:
    ldr r1, =0x400FF000   @ For K66, GPIOC Port Data Output Register (GPIOC_PDOR)
    mov r0, #0              @ set output to LOW, LED on
    str r0, [r1]            @ apply settings
    bx  lr

blue_led_off:
    ldr r1, =0x400FF000    @ For K66, GPIOC Port Data Output Register (GPIOC_PDOR)
    mov r0, #(1<<11)        @ For K66, set pin 9 output to HIGH, LED off
    str r0, [r1]            @ apply settings
    bx  lr
