
# Embedded Systems Code Guide

## Introduction

This document serves as a reference for common code examples used in embedded systems programming. It includes code for GPIO initialization, interrupt handling, timers, and the SysTick timer, primarily using the Freescale KL25Z microcontroller.

---

## 1. GPIO Programming

### 1.1 GPIO Initialization (for LCD)
This code configures GPIO pins for interfacing with an LCD on the Freescale KL25Z.

```c
void LCD_init(void) {
    SIM->SCGC5 |= 0x1000;  // enable clock to Port D
    PORTD->PCR[0] = 0x100; // make PTD0 pin as GPIO
    PORTD->PCR[1] = 0x100; // make PTD1 pin as GPIO
    PORTD->PCR[6] = 0x100; // make PTD6 pin as GPIO
    PORTD->PCR[7] = 0x100; // make PTD7 pin as GPIO
    PTD->PDDR = 0xFF; // make PTD7-0 as output pins
    SIM->SCGC5 |= 0x0200; // enable clock to Port A
    PORTA->PCR[2] = 0x100; // make PTA2 pin as GPIO
    PORTA->PCR[4] = 0x100; // make PTA4 pin as GPIO
    PORTA->PCR[5] = 0x100; // make PTA5 pin as GPIO
    PTA->PDDR |= 0x34; // make PTA5, 4, 2 as output pins
}
```

**Explanation**: This function initializes an LCD by configuring the appropriate GPIO pins as outputs. 

---

## 2. Interrupts

### 2.1 Interrupt Configuration (GPIO)
This section shows how to configure an interrupt for a GPIO pin.

```c
PORTA->PCR[1] |= 0x00100;  // make it GPIO
PORTA->PCR[1] |= 0x00003;  // enable pull-up
PTA->PDDR &= ~0x0002;       // make pin input
PORTA->PCR[1] &= ~0xF0000;  // clear interrupt selection
PORTA->PCR[1] |= 0xA0000;   // enable falling edge INT
NVIC->ISER[0] |= 0x40000000; // enable INT30 (bit 30 of ISER[0])
__enable_irq();             // global enable IRQs
```

**Explanation**: Configures PTA1 pin to trigger interrupts on a falling edge. Interrupts are enabled globally.

---

## 3. Timers

### 3.1 Timer Interrupt Handler
Here’s how to handle a timer interrupt:

```c
void TPM0_IRQHandler(void) {
    PTB->PTOR = 0x40000; // toggle red LED
    TPM0->SC |= 0x80;     // clear TOF
}
```

**Explanation**: This ISR toggles the red LED when a timer overflow occurs.

---

## 4. SysTick Timer

### 4.1 SysTick Delay Function
This function uses the SysTick timer to create a delay.

```c
void delayMs(int n) {
    int i;
    SysTick->LOAD = 41940 - 1;     // Load value for 1ms delay
    SysTick->CTRL = 0x5;           // Enable SysTick, use system clock
    for (i = 0; i < n; i++) {
        while ((SysTick->CTRL & 0x10000) == 0) {} // wait until COUNT flag is set
    }
    SysTick->CTRL = 0;             // Stop the timer
}
```

**Explanation**: This function creates a delay of `n` milliseconds using SysTick.

---
