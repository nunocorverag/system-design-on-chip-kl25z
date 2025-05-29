# Theory: Peripherals I/O Programming

## Introduction

This document covers the key concepts and programming fundamentals for General Purpose Input/Output (GPIO) peripherals on the KL25Z microcontroller (ARM Cortex-M0). GPIO pins serve as the primary interface between the microcontroller and external devices such as LEDs, switches, LCDs, and keypads.

The KL25Z MCU provides five GPIO ports (A to E), each with up to 32 pins, though not all pins are physically accessible. Effective peripheral programming requires understanding GPIO memory mapping, pin multiplexing, control registers, and electrical characteristics.

---

## 1. GPIO Ports and Memory Mapping

- **GPIO Ports**: Named from Port A through Port E.
- **Memory Address Ranges** (example from APB bus):
  - Port A: `0x400FF000` – `0x400FF017`
  - Port B: `0x400FF040` – `0x400FF057`
  - Port C: `0x400FF080` – `0x400FF097`
  - Port D: `0x400FF0C0` – `0x400FF0D7`
  - Port E: `0x400FF100` – `0x400FF117`

The CPU accesses GPIO control and data registers through these addresses.

---

## 2. GPIO Pin Control and Multiplexing

- Each pin is controlled by a **Pin Control Register (PORTx_PCRn)**.
- The **MUX field (bits 8-10)** selects the pin's function:
  - `001` = GPIO function.
  - Other values select alternate peripheral functions (ADC, UART, SPI, etc.).
- Other configurable fields include:
  - Pull-up/pull-down resistors enable and select.
  - Drive strength.
  - Slew rate.
  - Interrupt configuration.

Pin multiplexing enables multiple functions on the same physical pin, optimizing pin usage.

---

## 3. Key GPIO Registers

- **PDOR (Port Data Output Register)**: Output data for pins set as outputs.
- **PSOR (Port Set Output Register)**: Writing 1 sets the corresponding output bit.
- **PCOR (Port Clear Output Register)**: Writing 1 clears the corresponding output bit.
- **PTOR (Port Toggle Output Register)**: Writing 1 toggles the output bit.
- **PDDR (Port Data Direction Register)**: Configures pins as inputs (`0`) or outputs (`1`).
- **PDIR (Port Data Input Register)**: Reads current logic level of pins.

Using PSOR, PCOR, and PTOR allows atomic pin manipulation without disturbing other pins.

---

## 4. Programming Steps for GPIO

Typical steps to toggle an LED or read an input:

1. Enable clock gating to the target port via the **SIM_SCGC5** register.
2. Configure the pin’s **PORTx_PCRn** for GPIO function (`MUX=1`) and enable internal pull-up or pull-down if needed.
3. Set the pin as output or input in **PDDR**.
4. Control the pin state using **PSOR/PCOR/PTOR** or read input state from **PDIR**.
5. Use software delay routines as needed for visual effects or debouncing.

---

## 5. Internal Pull-Up and Pull-Down Resistors

- The pull enable bit (`PE`) activates internal resistors.
- The pull select bit (`PS`) determines resistor type:
  - `0` = pull-down resistor.
  - `1` = pull-up resistor.
- Internal pull resistors are useful for stable default input states, preventing floating inputs.

---

## 6. Using External Resistors

- External pull-up or pull-down resistors can be used by disabling internal resistors (`PE=0`).
- External resistor choice affects input voltage levels and noise immunity.
- When a switch is connected:
  - Pull-up configuration keeps input high when switch is open; input goes low when switch is closed (connected to ground).
  - Pull-down configuration keeps input low when switch is open; input goes high when switch is closed (connected to Vcc).

---

## 7. References to Code Examples

- LED toggle example: [`examples/1_led_toggle.c`](./examples/1_led_toggle.c)
- Switch input reading example: [`examples/2_switch_read.c`](./examples/2_switch_read.c)
- Basic GPIO input/output operations: [`examples/3_gpio_basic.c`](./examples/3_gpio_basic.c)

---

## References

- KL25 Sub-Family Reference Manual: [`../kl25-sub-family-reference-manual.pdf`](../kl25-sub-family-reference-manual.pdf)  
- ARM Cortex-M0 Technical Reference Manual: https://developer.arm.com/documentation/ddi0419/latest  
- Freescale FRDM-KL25Z Board User Guide

---

This theory document complements the slide deck `05_peripherals-io.pdf` which contains detailed diagrams and further explanations.
