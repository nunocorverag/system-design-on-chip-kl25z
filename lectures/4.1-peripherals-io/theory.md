# Theory: Peripherals I/O Programming

## Introduction

This document introduces the key concepts and programming details for General Purpose Input/Output (GPIO) peripherals on the KL25Z microcontroller (ARM Cortex-M0). GPIO pins provide the basic interface between the microcontroller and external devices such as LEDs, switches, LCDs, and keypads.

The KL25Z MCU has five GPIO ports (A to E), each with up to 32 pins, though not all pins are physically available. Understanding GPIO memory mapping, pin multiplexing, and control registers is essential for effective peripheral programming.

---

## 1. GPIO Ports and Memory Mapping

- **GPIO Ports**: Ports A through E, each mapped to specific memory address ranges.
- **Memory Addresses** for GPIO ports (APB bus example):
  - Port A: `0x400FF000` to `0x400FF017`
  - Port B: `0x400FF040` to `0x400FF057`
  - Port C: `0x400FF080` to `0x400FF097`
  - Port D: `0x400FF0C0` to `0x400FF0D7`
  - Port E: `0x400FF100` to `0x400FF117`

The CPU accesses GPIO registers via these addresses to configure pin behavior and read/write data.

---

## 2. GPIO Pin Control and Multiplexing

Each pin's behavior is controlled by a **Pin Control Register (PORTx_PCRn)** that selects its function:

- **MUX field (bits 8-10)** selects the pin function, where `001` corresponds to GPIO.
- Additional bits configure drive strength, pull-up/pull-down resistors, slew rate, and interrupt configuration.

Pin multiplexing allows a single physical pin to serve multiple functions (GPIO, ADC input, UART, I2C, etc.), reducing the total pins required.

---

## 3. GPIO Registers Overview

Key registers for each GPIO port:

- **PDOR (Port Data Output Register)**: Holds output data for pins configured as output.
- **PSOR (Port Set Output Register)**: Writing 1 sets the corresponding output bit.
- **PCOR (Port Clear Output Register)**: Writing 1 clears the corresponding output bit.
- **PTOR (Port Toggle Output Register)**: Writing 1 toggles the corresponding output bit.
- **PDDR (Port Data Direction Register)**: Configures pins as input (0) or output (1).
- **PDIR (Port Data Input Register)**: Reads the current state of pins.

Using PSOR/PCOR/PTOR allows atomic manipulation of individual pins without affecting others.

---

## 4. Programming GPIO: Example Steps to Toggle an LED

1. Enable the clock to the port using the **SIM_SCGC5** register.
2. Configure the pin’s **PORTx_PCRn** register for GPIO function and desired electrical characteristics.
3. Set the pin as output in the port's **PDDR** register.
4. Use **PSOR/PCOR/PTOR** or **PDOR** to set, clear, or toggle the pin state.
5. Implement delay routines to observe LED blinking.

---

## 5. Reading Switches and Handling Input

- Configure the input pin with internal pull-up or pull-down resistors via **PORTx_PCRn**.
- Set the pin as input in **PDDR**.
- Read the pin state via **PDIR**.
- Debounce input by software delay or hardware filtering to prevent false triggering caused by mechanical switch bouncing.

---

## 6. Advanced: Interfacing with LCD and Keypads

- LCDs typically require multiple GPIO pins for data and control lines.
- Keypads use matrix scanning: rows are driven outputs and columns are inputs with pull-ups.
- The microcontroller sequentially drives rows low and reads columns to detect pressed keys.
- Debounce and key identification logic are crucial for reliable input.

---

## References

- KL25 Sub-Family Reference Manual: [`../kl25-sub-family-reference-manual.pdf`](../kl25-sub-family-reference-manual.pdf)  
- KL25Z User Manual and Datasheet  
- ARM Cortex-M0 Technical Reference Manual: https://developer.arm.com/documentation/ddi0419/latest  
- Freescale FRDM-KL25Z Board User Guide

---

This document complements the slide deck `05_peripherals-io.pdf` which covers these topics with detailed diagrams and code snippets.

---