# 05 - Peripherals I/O

## Overview

This folder focuses on the fundamentals of **Input/Output (I/O) Peripherals** programming in the KL25Z microcontroller, part of the ARM Cortex-M0 family.

### Contents

- Main presentation: [`05_peripherals-io.pdf`](./05_peripherals-io.pdf)  
- Detailed theory document: [`theory.md`](./theory.md)  
- Example programs illustrating key GPIO concepts and simple interfacing with LEDs, switches, and keyboards in C.  

### Topics Covered

- GPIO basics: ports, pins, and memory mapping in KL25Z  
- Pin multiplexing and alternate functions  
- Configuring GPIO pins for input and output  
- Use of control registers (PCR, PDDR, PDOR, PSOR, PCOR, PTOR)  
- Practical examples for toggling LEDs and reading switches  
- Debounce techniques for mechanical switches  
- Interfacing parallel devices: LCDs and keypads  
- Key detection and scanning in matrix keypads  

### Suggested Workflow

1. Start by reading the theory in [`theory.md`](./theory.md) for foundational knowledge of GPIO programming and hardware registers.  
2. Study the presentation file `05_peripherals-io.pdf` for in-depth explanation with illustrations.  
3. Explore the example C programs to see practical implementation of GPIO control on the FRDM-KL25Z board.  
4. Experiment with code by compiling and running on your board for hands-on understanding.

### References

- [KL25 Sub-Family Reference Manual (pdf)](../kl25-sub-family-reference-manual.pdf)  
- ARM Cortex-M0 Reference Manual: https://developer.arm.com/documentation/ddi0419/latest  
- Freescale/NXP KL25Z and FRDM Board User Guides and Datasheets  

---
