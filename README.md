# STM_411CEU6_BatteryUnit

Firmware for a portable charge station built around the STM32F411CEU6 MCU. The project manages the station UI, JK/Jikong BMS communication, charger power control, inverter and USB outputs, cooling, and low-power modes.

The first hardware revision was assembled as a working charge station. The second hardware revision has the firmware side completed, but the station hardware has not been assembled yet.

## Features

- TouchGFX GUI for a 320x240 ILI9341 display.
- Main and debug screens with battery state, power, time remaining, inverter, USB, charger, fan, and error indicators.
- JK/Jikong BMS communication over UART.
- BMS frame generation and parsing for cell voltages, total voltage, current, SOC, temperatures, capacity, status, and warning flags.
- Charger power setting through a digital potentiometer.
- Charger protection flow based on BMS communication state and cell overvoltage checks.
- Inverter and USB output control with physical buttons and LED indication.
- Fan control from two NTC temperature sensors.
- Display brightness control and automatic GUI task suspend/resume when the screen is off.
- STM32 stop mode entry and wake-up flow for reduced power consumption.

## Hardware

Main target hardware:

- MCU: STM32F411CEU6.
- Display: ILI9341, 320x240, SPI-connected.
- BMS: JK/Jikong BMS connected through UART.
- Charger control: digital potentiometer based power adjustment.
- Temperature sensing: two NTC inputs through ADC.
- Outputs: inverter enable, USB enable, charger enable, fan power, status LEDs.
- Inputs: power/screen button, settings buttons, USB button, inverter button, charger/BMS status pins.

The pinout and peripheral configuration are defined in `STM_411CEU6_BatteryUnit.ioc`.

## Software Stack

- STM32CubeIDE project for STM32F411CEU6.
- STM32Cube FW_F4 HAL and CMSIS.
- FreeRTOS with CMSIS-RTOS v2 integration.
- TouchGFX 4.24.1 generated UI project.
- C and C++ application code, with the main application logic in `Application/`.

## UI Assets

The source UI media files are kept in `resources/`. TouchGFX-generated image sources are kept separately under `TouchGFX/generated/images/`.

These are source assets rather than full screen captures; the final screens are assembled in TouchGFX.

| 320x240 layout grid | Charger-connected icon | Battery warning fill |
| --- | --- | --- |
| ![320x240 UI grid](resources/grid.png) | ![Charger-connected plug icon](resources/icons/plug_big_green.png) | ![Red battery fill asset](resources/bars/battery_fill_red.png) |

## Project Layout

- `Application/` - station-specific C++ logic: BMS, charger, inverter, USB, fan, display, settings, power mode handling, and shared data.
- `Core/` - STM32Cube generated startup, peripheral initialization, FreeRTOS entry points, and interrupt handlers.
- `Drivers/` - STM32 HAL, CMSIS, ILI9341 driver, and NTC helper library.
- `Middlewares/` - FreeRTOS and TouchGFX middleware.
- `TouchGFX/` - TouchGFX UI project, generated assets, screens, presenters, and simulator files.
- `resources/` - source UI images and icons.
- `BMS-RS485Communication protocol.pdf` and `Jikong-BMS-RS485-Communication example.pdf` - protocol reference material for JK/Jikong BMS communication.

## Runtime Architecture

The firmware starts two FreeRTOS tasks:

- `defaultTask` runs `Program_Process()`, which creates and runs `IdleProcess`.
- `GUI_Task` initializes the ILI9341 display, starts the display PWM/timer services, and runs the TouchGFX task.

`IdleProcess` is the station control loop. It periodically updates:

- `BMSUpdater` for BMS power, UART requests, parsing, errors, and data freshness.
- `ChargerHandler` for DC input state, charger enable, charge power, and safety checks.
- `InverterHandler` and `USBHandler` for output control and LED feedback.
- `FanHandler` for NTC temperature reads and fan power.
- `Display320x240` for brightness and GUI task suspend/resume.
- `SettingsHandler` for charge power, screen mode, and brightness settings.

The GUI reads a read-only shared `ProcessData` snapshot through `SharedData` and updates the main/debug screens from that state.

## Build

Open the project in STM32CubeIDE and build the required configuration. The repository contains STM32CubeIDE project files, launch configurations, linker scripts, generated TouchGFX sources, and middleware needed by the firmware.

When regenerating code from STM32CubeMX or TouchGFX Designer, keep user-code sections and review generated changes before flashing hardware.

## Safety Notice

This firmware controls battery charging and high-current power outputs. Review the hardware design, BMS limits, charger behavior, thermal behavior, and fail-safe states before using it with real batteries or loads. Use current-limited supplies and appropriate protection during bring-up.

## License

This project's own source code is licensed under the Apache License 2.0. See `LICENSE`.

Bundled third-party components keep their original licenses. Check the license files under `Drivers/` and `Middlewares/` for STM32 HAL, CMSIS, FreeRTOS, TouchGFX, and related dependencies.
