# STM32 Black Pill – Timer ADC DMA UART (Bare-Metal)

## Objective
Implement a 100 Hz timer-triggered ADC acquisition system using DMA and transmit results over UART on an STM32F411 (Black Pill), without HAL, CMSIS, or physical hardware.


## 🎥 Demo Video

This short demo video explains the firmware architecture, build process, and execution flow of the project.

👉 [Watch Demo Video](./STM_32%20-%20Made%20with%20Clipchamp.mp4)


---

### 🧠 What the Video Demonstrates

- STM32CubeIDE project structure  
- Successful clean build (no errors)  
- Bare-metal firmware design (no HAL / CMSIS)  
- Timer → ADC → DMA → UART data flow explanation  
- Hardware-ready firmware (board not required for build validation)

## System Flow
1. TIM2 generates a 100 Hz trigger
2. TIM2 TRGO triggers ADC1
3. ADC1 samples channel 0 (PA0)
4. DMA2 Stream0 transfers ADC data to memory
5. DMA interrupt fires on buffer completion
6. ADC values are transmitted via USART2

## Clock Configuration
- Internal HSI 16 MHz
- No PLL
- No external crystal

## Tools Used
- STM32CubeIDE
- Bare-metal C
- GNU ARM toolchain

## Hardware
- No physical board used
- Firmware is hardware-ready and can be flashed directly to STM32F411

## Status
- Builds successfully
- Bare-metal implementation
- Interrupt-driven
- DMA-based
