/**
 * @file UIRBcore_Pins.h
 * @brief Pin configuration definitions for the Universal IR Blaster (%UIRB) system.
 * 
 * This header file provides the pin assignments and configurations for the Universal IR Blaster (%UIRB) 
 * system, designed around the ATMega328P microcontroller in the TQFP package. It defines macros for 
 * each pin used in the system, including those for the CP2104 USB-to-UART bridge, buttons, LEDs, 
 * IR transmitter/receiver, and other hardware components.
 *
 * @note
 * - If the board version is not defined or recognized, a compilation error will occur.
 * - Configurable pull-up resistors and other options can be enabled or disabled through macros.
 * - This file is compatible with Doxygen for generating documentation.
 * 
 * @author 
 * Djordje Mandic (https://linktr.ee/djordjemandic)
 * 
 * @version 0.2.0.0
 * @date 2024-12-12
 * 
 * @copyright Copyright (c) 2024 Djordje Mandic (https://linktr.ee/djordjemandic)
 * 
 * MIT License
 * 
 * Copyright (c) 2024 Djordje Mandic (https://linktr.ee/djordjemandic)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef UIRBcore_Pins_h
#define UIRBcore_Pins_h

/**
 * @def UIRB_BOARD_V02
 * @brief Macro indicating the hardware version of the %UIRB board.
 * 
 * This macro is defined externally (e.g., through the build system or compiler flags) 
 * to specify that the %UIRB board corresponds to version 0.2. When this macro is defined, 
 * various constants and configurations within the firmware are adjusted to align with the 
 * specific capabilities and requirements of the hardware revision.
 * 
 * @note This macro must be defined externally (e.g., using `-DUIRB_BOARD_V02` during compilation) 
 *       and is not included or modified within the source files.
 * 
 * @see @ref uirbcore::eeprom::UIRB_HW_VER for the hardware version constant influenced by this macro.
 */
#if defined(__DOXYGEN__)
    #define UIRB_BOARD_V02
    #undef UIRB_BOARD_V02
#endif

#if defined(UIRB_BOARD_V02) || defined(__DOXYGEN__)
    /**
     * @name CP2104 USB-to-UART Pins
     * 
     * @details
     * The `DTR` pin of the CP2104 USB-to-UART bridge is connected to the `RST` pin of the ATMega328P via a 100nF capacitor. 
     * This connection enables the CP2104 to automatically reset the ATMega328P when a serial connection is established from the host.
     * When the host asserts `DTR`, a short pulse is sent to the `RST` pin through the capacitor, resetting the MCU. 
     * This setup is essential for programming the ATMega328P via the CP2104 without manual intervention.
     * This connection is standard in Arduino-compatible designs to enable seamless programming and debugging.
     * 
     * The `RX` and `TX` pins of the CP2104 are connected to the corresponding `TX` and `RX` pins of the ATMega328P 
     * through series resistors to prevent phantom powering and unwanted current flows between the devices.
     * - **Resistor Values:**
     *   - Typical resistor values range from 1kΩ to 10kΩ.
     *   - These resistors prevent damage and undesired behavior when either device is powered off.
     * - **Alternate Use of TX Pin:**
     *   - The `TX` pin of the ATMega328P can also function as a **slave select (SS)** line for SPI devices. (See @ref PIN_PROG and @ref PIN_TX)
     *   - Using the `TX` pin as a SS line is safe, provided it is not transmitting serial data simultaneously.
     * 
     * @warning
     * - Ensure proper resistor sizing for current flow limitation and signal integrity.
     * - Avoid simultaneous use of the `TX` pin for SPI and serial communication.
     * 
     * @{
     */
    #if defined(PIN_USB_IO3)
        #if PIN_USB_IO3 != 4
            #undef PIN_USB_IO3
            #warning "PIN_USB_IO3 is fixed to 4 (PD4)"
        #endif  // PIN_USB_IO3 != 4
    #endif  // defined(PIN_USB_IO3)

    /**
     * @brief MCU pin connected to GPIO.3 of the CP2104 USB-to-UART bridge.
     * 
     * This pin connects the PD4 pin of the ATMega328P to the GPIO.3 pin of the CP2104 USB-to-UART bridge. The GPIO.3 
     * pin is configured as an input on the CP2104 by default and can function as an open-drain output, capable of sinking current.
     * 
     * @details
     * - **Arduino Pin Number:** 4 (Digital pin 4 or PD4 on ATMega328P).
     * - **PCINT Interrupt:** `PD4` corresponds to `PCINT20`, which is part of `PCINT2` group on ATMega328P. 
     *   This allows the use of pin change interrupts for wakeup or other interrupt-driven functionality.
     * - **Hardware Connection:**
     *   - `PD4` on ATMega328P is directly connected to GPIO.3 on CP2104.
     *   - There is no series resistor between `PD4` and GPIO.3 because the internal pull-up resistor on `PD4` is sufficient 
     *     to prevent phantom powering. The internal pull-up can also be disabled if not required.
     * - **Usage:**
     *   - GPIO.3 on the CP2104 is an open-drain output and can sink current. Therefore:
     *     - `PD4` must not be configured as a logical HIGH output to prevent damage.
     *     - It can be safely configured as:
     *       - An input with an internal pull-up enabled (to allow GPIO.3 to sink current or read a high state).
     *       - A logical LOW output (to allow GPIO.3 to read a low state).
     *   - This line can be used for bit-banging or slow communication with the USB host independently of the USART interface.
     * - **Notes:**
     *   - The open-drain behavior of GPIO.3 allows for signaling or basic communication but requires care in configuring `PD4` 
     *     to avoid conflicts or potential damage.
     *   - The lack of a series resistor simplifies the design but necessitates careful pin management.
     * 
     * @warning
     * - Do not configure `PD4` as a logical HIGH output when connected to `GPIO.3`, as it could cause excessive current and damage both devices.
     * - Ensure proper pin configuration when using this pin for communication or signaling purposes.
     */
    #define PIN_USB_IO3 (4)
    /** @} */ // End of CP2104 USB-to-UART GPIO Pins

    /**
     * @name General-Purpose Pins
     * @{
     */
    #if defined(PIN_PULLDOWN_RESISTOR)
        #if PIN_PULLDOWN_RESISTOR != 5
            #undef PIN_PULLDOWN_RESISTOR
            #warning "PIN_PULLDOWN_RESISTOR is fixed to 5 (PD5)"
        #endif  // PIN_PULLDOWN_RESISTOR != 5
    #endif  // defined(PIN_PULLDOWN_RESISTOR)
    /** 
     * @brief MCU pin connected to a general-purpose pulldown resistor for debugging or hardware configuration.
     * 
     * This pin is connected to a pulldown resistor, serving as a general-purpose signal input or output 
     * for expansion, debugging, or as a hardware configuration option. It is not tied to any specific circuit 
     * in the default configuration but provides flexibility for custom implementations.
     * 
     * @details
     * - **Arduino Pin Number:** 5 (PD5 on ATMega328P).
     * - **Primary Use:** Placeholder for debugging, signal measurement, or hardware configuration.
     * - **Potential Use Cases:**
     *   - Measuring signals routed to this pin during testing or runtime.
     *   - Providing a fixed logic level (low) for hardware configuration options.
     *   - Debugging GPIO behavior or detecting external hardware states.
     * - **Hardware Notes:**
     *   - The resistor ensures the pin is pulled low when not driven by an external signal.
     *   - The pin can be repurposed for various experimental or custom features.
     * 
     * @note This pin is available for user-defined purposes and does not have a predefined role in the default design.
     */
    #define PIN_PULLDOWN_RESISTOR (5)

    /**
     * @name General-Purpose Pins (Serial Peripheral Interface and In Circuit Programming)
     * @{
     */
    #if defined(PIN_SPI_SCK)
        #if PIN_SPI_SCK != 13
            #undef PIN_SPI_SCK
            #warning "PIN_SPI_SCK is fixed to 13 (PB5)"
        #endif  // PIN_SPI_SCK != 13
    #endif  // defined(PIN_SPI_SCK)
    /**
     * @brief MCU pin connected to the SPI SCK (Serial Clock) line.
     * 
     * This pin provides the clock signal for synchronizing data transmission between the microcontroller 
     * (master) and peripheral devices (slaves) on the SPI (Serial Peripheral Interface) bus. It is typically 
     * configured as an output when the microcontroller is in master mode.
     * 
     * @details
     * - **Arduino Pin Number:** 13 (PB5 on ATMega328P).
     * - **Functionality:**
     *   - Outputs the clock signal to synchronize SPI data transmission.
     *   - Works in coordination with @ref PIN_SPI_MOSI, @ref PIN_SPI_MISO, and the slave select (SS) pin. 
     *   - On %UIRB board V0.2 (@ref UIRB_BOARD_V02), the slave select pin can be @ref PIN_PROG or @ref PIN_TX.
     *   - Shared with the onboard status LED (PIN_STAT_LED), which may cause unintended toggling of the LED during SPI communication.
     * - **Hardware Notes:**
     *   - Ensure the SPI clock frequency is compatible with the slave devices to prevent data errors.
     *   - Sharing this pin with the status LED may require careful software handling to avoid conflicts.
     * 
     * @warning Be cautious of potential interference caused by the shared LED connection.
     */
    #define PIN_SPI_SCK (13)

    #if defined(PIN_SPI_MISO)
        #if PIN_SPI_MISO != 12
            #undef PIN_SPI_MISO
            #warning "PIN_SPI_MISO is fixed to 12 (PB4)"
        #endif  // PIN_SPI_MISO != 12
    #endif  // defined(PIN_SPI_MISO)
    /**
     * @brief MCU pin connected to the SPI MISO (Master In Slave Out) line.
     * 
     * This pin is used for receiving data from peripheral devices (slaves) to the microcontroller (master) 
     * on the SPI (Serial Peripheral Interface) bus. It is typically configured as an input when the 
     * microcontroller is in master mode.
     * 
     * @details
     * - **Arduino Pin Number:** 12 (PB4 on ATMega328P).
     * - **Functionality:**
     *   - Receives data from slave devices during SPI communication.
     *   - Works in coordination with @ref PIN_SPI_MOSI, @ref PIN_SPI_SCK, and the slave select (SS) pin. 
     *   - On %UIRB board V0.2 (@ref UIRB_BOARD_V02), the slave select pin can be @ref PIN_PROG or @ref PIN_TX.
     *   - Shared with the IR signal receiving functionality (@ref PIN_IR_RECEIVE) on the %UIRB board V0.2 (@ref UIRB_BOARD_V02).
     * - **Hardware Notes:**
     *   - This pin can be used to carry the signal for @ref PIN_IR_CAPTURE with a specific hardware modification that routes the signal 
     *     from the IR receiver to this pin and then to @ref PIN_IR_CAPTURE for input capture functionality.
     * 
     * @warning Misconfigured SPI settings or improper hardware modifications can lead to data transmission errors or hardware damage.
     */
    #define PIN_SPI_MISO (12)

    #if defined(PIN_SPI_MOSI)
        #if PIN_SPI_MOSI != 11
            #undef PIN_SPI_MOSI
            #warning "PIN_SPI_MOSI is fixed to 11 (PB3)"
        #endif  // PIN_SPI_MOSI != 11
    #endif  // defined(PIN_SPI_MOSI)
    /**
     * @brief MCU pin connected to the SPI MOSI (Master Out Slave In) line.
     * 
     * This pin is used for transmitting data from the microcontroller (master) to peripheral devices (slaves) 
     * on the SPI (Serial Peripheral Interface) bus. It is typically configured as an output when the 
     * microcontroller is in master mode.
     * 
     * @details
     * - **Arduino Pin Number:** 11 (PB3 on ATMega328P).
     * - **Functionality:**
     *   - Transmits data from the master device to one or more slave devices.
     *   - Works in coordination with @ref PIN_SPI_MISO (for receiving data), @ref PIN_SPI_SCK (clock signal), 
     *     and the slave select (SS) pin.
     *   - On %UIRB board V0.2 (@ref UIRB_BOARD_V02), the slave select pin can be @ref PIN_PROG or @ref PIN_TX.
     * 
     * @warning Incorrect configuration or misconnection of the MOSI line can lead to data transmission errors or hardware damage.
     * 
     * @note This pin is shared with other functionalities of expansion modules, such as the In Circuit Programming (ICP) module.
     */
    #define PIN_SPI_MOSI (11)
    /** @} */ // End of SPI and ICP Pins

    /**
     * @name General-Purpose Pins (Universal Asynchronous Receiver-Transmitter)
     * @{
     */
    #if defined(PIN_TX)
        #if PIN_TX != 1
            #undef PIN_TX
            #warning "PIN_TX is fixed to 1 (PD1)"
        #endif  // PIN_TX != 1
    #endif  // defined(PIN_TX)
    /**
     * @brief MCU pin connected to the UART TX (Transmit) line.
     * 
     * This pin is used for transmitting data from the microcontroller to external devices (e.g., serial terminals, sensors, or other microcontrollers) 
     * via the UART (Universal Asynchronous Receiver-Transmitter) interface. It is typically configured as an output and works 
     * in coordination with the @ref PIN_RX pin.
     * 
     * @details
     * - **Arduino Pin Number:** 1 (PD1 on ATMega328P).
     * - **Functionality:**
     *   - Transmits serial data from the microcontroller to external devices.
     *   - Operates as part of the UART communication interface alongside @ref PIN_RX.
     * - **Hardware Notes:**
     *   - The TX line should be connected to the RX line of the external device.
     * - **Alternate Usage:**
     *   - This pin can also function as a **slave select (SS)** line for SPI devices when UART is not in use:
     *     - Using @ref PIN_TX as a SS line is safe if no serial data is being transmitted simultaneously.
     *     - This makes it a viable option for multiplexing functionalities in space-constrained designs.
     * 
     * @warning Ensure proper configuration when sharing this pin between UART and SPI functionalities.
     * 
     * @note Use caution when repurposing this pin for SPI functionalities, and validate its behavior in your application.
     */
    #define PIN_TX (1)

    #if defined(PIN_RX)
        #if PIN_RX != 0
            #undef PIN_RX
            #warning "PIN_RX is fixed to 0 (PD0)"
        #endif  // PIN_RX != 0
    #endif  // defined(PIN_RX)
    /**
     * @brief MCU pin connected to the UART RX (Receive) line.
     * 
     * This pin is used for receiving data from external devices (e.g., serial terminals, sensors, or other microcontrollers) 
     * via the UART (Universal Asynchronous Receiver-Transmitter) interface. It is typically configured as an input and 
     * works in coordination with the @ref PIN_TX pin.
     * 
     * @details
     * - **Arduino Pin Number:** 0 (PD0 on ATMega328P).
     * - **Functionality:**
     *   - Receives serial data from external devices.
     *   - Operates as part of the UART communication interface alongside @ref PIN_TX.
     *   - Supports standard baud rates and communication protocols as configured in the UART driver.
     * - **Hardware Notes:**
     *   - The RX line should be connected to the TX line of the external device.
     *   - Ensure proper voltage levels for compatibility with the external device to avoid hardware damage.
     * 
     * @warning
     * - Avoid configuring this pin as an output while it is connected to another device's TX line to prevent electrical conflicts.
     * - Ensure the pin is not driven by multiple sources to avoid potential damage or communication errors.
     * 
     * @note Use caution when sharing this pin with other peripherals or functionalities to avoid communication disruptions.
     */
    #define PIN_RX (0)
    /** @} */ // End of UART Pins

    /**
     * @name General-Purpose Pins (TP4057 Lithium-ion Battery Charger)
     * @{
     */
    #if defined(PIN_PROG)
        #if PIN_PROG != 15
            #undef PIN_PROG
            #warning "PIN_PROG is fixed to 15 (PC1)"
        #endif  // PIN_PROG != 15
    #endif  // defined(PIN_PROG)
    /**
     * @brief MCU pin connected to the `PROG` pin of the TP4057 lithium-ion battery charger IC via an RC low-pass filter.
     * 
     * This pin provides control and monitoring capabilities for the TP4057 charger IC. The `PROG` pin of the TP4057 
     * is responsible for limiting and monitoring the charging current based on the voltage at the pin, which is determined 
     * by the external resistor network. The MCU pin connected to `PROG` is utilized for reading the voltage and dynamically 
     * adjusting the charging current.
     * 
     * @details
     * - **Arduino Pin Number:** 15 (Analog pin 1 or PC1 on ATMega328P).
     * - **Hardware Connection:**
     *   - This pin is connected to the `PROG` pin of the TP4057 via a 10kΩ resistor and a 10nF capacitor (forming an RC low-pass filter).
     *   - The filter ensures smooth voltage readings and prevents rapid fluctuations from affecting the charging current reading.
     * - **Functionality:**
     *   - **Voltage to Current Mapping:**
     *     - The voltage at the `PROG` pin determines the charging current:
     *       - Below 100mV: The charger is in "floating" mode, waiting for a recharge.
     *       - Between 100mV and 1000mV: The charger is in constant voltage (CV) mode.
     *       - Around 1000mV: The charger is in constant current (CC) mode.
     *   - **Dynamic Current Control:**
     *     - Setting this MCU pin to LOW adds a 10kΩ resistor in parallel with the existing `PROG` pin resistor (5kΩ).
     *       This effectively reduces the resistance to 3.33kΩ, increasing the charging current to approximately 300mA.
     *     - Setting this MCU pin to HIGH drives the voltage at the `PROG` pin above 1100mV, reducing the charging current 
     *       to near zero, effectively turning off charging.
     * - **Use Cases:**
     *   - Monitoring the charging current and state by reading the voltage at the `PROG` pin.
     *   - Dynamically adjusting the charging current based on system requirements or battery state.
     *   - This pin can also be used as a **slave select (SS)** pin for SPI devices:
     *     - While the charger is actively charging, toggling this pin for SPI communication **does interfere** with 
     *       the charger operation due to the RC low-pass filter directly connected to this pin. Avoid using 
     *       this pin for SPI communication during charging if charging is required to not be interrupted.
     *     - When the charger is powered off (USB cable disconnected), using this pin as a slave select has no effect 
     *       on the charger and does not cause any harm.
     * - **Charger Behavior:**
     *   - The TP4057 adjusts its PROG pin voltage with respect to the charging current automatically.
     *   - The charging current is calculated using the formula:
     *     \f[
     *     I_{\text{chg}} \, \text{(mA)} = \frac{V_{\text{prog}} \, \text{(mV)} \cdot 1000}{R_{\text{prog}} \, \text{(Ω)}}
     *     \f]
     *   - In this configuration:
     *     - A 5kΩ resistor sets the current to approximately 200mA.
     *     - A 10kΩ resistor in parallel (this pin set to LOW) reduces the resistance to 3.33kΩ, increasing the charging current to approximately 300mA.
     *     - When this pin is set to HIGH, the voltage at the `PROG` pin is above 1100mV, reducing the charging current to near zero, effectively turning off charging.
     * 
     * @warning
     * - Ensure proper hardware configuration to avoid inaccurate voltage or current measurements.
     * - Calibrate the internal bandgap reference voltage using @ref uirbcore::UIRB::setInternalBandgapReferenceVoltageMilivolts().
     * - The MCU pin must be handled carefully to avoid unintended behavior:
     *   - Ensure proper mode configuration (`INPUT`, `OUTPUT`, or `INPUT_PULLUP`) when accessing the pin.
     * - Avoid excessive charging currents by ensuring resistor values are accurate and appropriately selected.
     * 
     * @note
     * - This pin's functionality depends on the TP4057 charger IC and must align with its specifications.
     * - Use the provided library methods such as @ref uirbcore::PowerInfoData::getChargerState() or @ref uirbcore::PowerInfoData::prog_milivolts_to_charging_current_miliamps() to 
     *   estimate the charger state or calculate the charging current based on the `PROG` pin voltage.
     * - Analog readings on this pin provide voltage information that can be used to infer the current charging state 
     *   (e.g., floating, constant current, constant voltage). Ensure the internal bandgap reference voltage is properly 
     *   calibrated using @ref setAnalogReference and @ref uirbcore::UIRB::setInternalBandgapReferenceVoltageMilivolts().
     */
    #define PIN_PROG (15)

    #if defined(__DOXYGEN__)
        /** 
         * @brief Alias for @ref PIN_PROG.
         */
        #define PIN_A1 PIN_PROG
    #endif  // defined(__DOXYGEN__)
    /** @} */ // End of Charger Pins
    /** @} */ // End of General-Purpose Pins

    /**
     * @name IR Transmitter and Receiver Pins
     * @{
     */
    #if defined(PIN_IR_LED)
        #if PIN_IR_LED != 3
            #undef PIN_IR_LED
            #warning "PIN_IR_LED is fixed to 3 (PD3)"
        #endif  // PIN_IR_LED != 3
    #endif  // defined(PIN_IR_LED)
    /** 
     * @brief MCU pin connected to the IR LED driver for transmitting IR signals.
     * 
     * This pin controls the IR LED driver circuit, enabling the %UIRB device to transmit infrared signals 
     * for remote control or communication purposes. The IR LED driver amplifies the signal output from 
     * this pin to achieve sufficient intensity for effective transmission.
     * 
     * @details
     * - **Arduino Pin Number:** 3 (PD3 on ATMega328P).
     * - **Functionality:**
     *   - The pin generates a modulated signal (e.g., 38 kHz carrier frequency) for transmitting IR codes.
     *   - Signal modulation is software-controlled, ensuring compatibility with various IR protocols.
     * - **Hardware Notes:**
     *   - The pin is connected to an IR LED driver circuit.
     *   - The driver circuit protects the MCU from the high current demands of the IR LED.
     * - **Timer Used:** 
     *   - Hardware Timer 2 is used to generate the modulated signal.
     * - **Use Cases:**
     *   - Transmitting IR commands to control TVs, air conditioners, or other consumer electronics.
     *   - Acting as a universal IR blaster for custom applications.
     *   - Capturing transmitted IR signals supplied from expansion boards.
     * - **Considerations:**
     *   - Overheating or overdriving the IR LED should be avoided to prevent damage.
     * 
     * @warning
     * - The signal modulated on this pin **must not exceed a 50% duty cycle**, with **33% duty cycle recommended**, 
     *   to prevent overheating and damage to the IR LEDs. 
     * - IR LEDs are configured to operate in a pulsed high-power mode and are not designed for continuous operation. 
     *   Leaving the LEDs ON continuously will lead to thermal damage and permanent failure.
     * - Proper timing and modulation must be implemented in software to ensure safe and reliable operation.
     * - **When using avr8-stub:** The AVR8 software debugger stub utilizes the INT1 interrupt (on @ref PIN_IR_LED)
     *   for generating software interrupts. During debugging with avr8-stub:
     *   - The interrupt configuration forces the pin to output a low state.
     *   - Timer 2 and IR LED output functionality are unavailable.
     *   - Attempting to transmit IR signals while using avr8-stub will lead to conflicts and undefined behavior.
     * 
     * @note The software must configure Timer 2 and generate a properly modulated signal on this pin to ensure 
     *       compatibility with target devices. When debugging, plan for alternative functionality to avoid conflicts.
     */
    #define PIN_IR_LED (3)

    #if defined(PIN_IR_CAPTURE)
        #if PIN_IR_CAPTURE != 8
            #undef PIN_IR_CAPTURE
            #warning "PIN_IR_CAPTURE is fixed to 8 (PB0)"
        #endif  // PIN_IR_CAPTURE != 8
    #endif  // defined(PIN_IR_CAPTURE)
    /** 
     * @brief MCU pin with input capture functionality, used for precise IR signal timing with a non-demodulating receiver.
     * 
     * This pin is connected to a non-demodulating IR receiver, such as the TSMP58000, to capture raw IR signal timings 
     * accurately. The input capture functionality of the microcontroller ensures precise measurement of signal edges 
     * for decoding IR protocols.
     * 
     * @details
     * - **Arduino Pin Number:** 8 (PB0 on ATMega328P).
     * - **Functionality:**
     *   - The pin leverages the input capture feature of Timer 1 to detect and timestamp signal transitions 
     *     (rising or falling edges) from the IR receiver.
     *   - This raw timing data is essential for decoding IR signals directly without relying on a demodulating receiver.
     * - **Receiver Type:** 
     *   - Non-demodulating IR receivers like the TSMP58000 output the raw modulated signal, including the carrier frequency (e.g., 38 kHz).
     *   - These receivers do not filter or process the signal, making them ideal for custom protocols or advanced IR analysis.
     * - **Use Cases:**
     *   - Precise measurement of carrier signals and pulse timings for advanced IR protocols.
     *   - Capturing and analyzing raw IR signals for learning and debugging purposes.
     *   - Supporting raw signal capture where demodulated data is insufficient or unavailable.
     * - **Hardware Notes:**
     *   - Capture with a non-demodulating sensor (e.g., TSMP58000) is recommended if you're unsure of the signal's nature. 
     *   - Raw capturing is done via PB0 (ICP1) pin of the ATMega328P, and this configuration is hardware-fixed—it cannot 
     *     be changed.
     *   - To ensure proper operation on %UIRB board V0.2, please follow the following steps:
     *     - Route the signal to pin 12 of the TQFP package (PB0) via a 1 kΩ - 10 kΩ resistor. This protects the internal GPIO 
     *       drivers of the ATMega328P from potential damage.
     *     - Pull the signal up to VCC using a pull-up resistor before it enters the resistor.
     *     - Ensure the internal pull-up for PB0 (ICP1) is disabled to prevent signal conflicts.
     * 
     * @warning
     * - Ensure proper signal conditioning from the non-demodulating receiver to prevent signal distortion or noise.
     * - Ambient light and other IR sources may interfere with raw signals. Consider shielding or filtering techniques 
     *   for improved accuracy.
     * - Misconfigured connections or pull-ups can lead to inaccurate signal capture or damage to the MCU's GPIO.
     * 
     * @note
     * - The input capture feature on this pin is configured via Timer 1. Ensure proper initialization of the timer 
     *   and capture settings in the firmware.
     * - For applications requiring only the demodulated signal (e.g., standard IR protocols like NEC or RC5), consider using 
     *   @ref PIN_IR_RECEIVE instead, which connects to a demodulating receiver.
     * 
     * @see @ref PIN_IR_CAPTURE_PULLUP for the configuration flag that indicates whether the pull-up resistor should be enabled on the IR capture pin.
     */
    #define PIN_IR_CAPTURE (8)

    /**
     * @brief Configuration flag for enabling the pull-up resistor on the IR capture pin.
     * 
     * This macro is a user-defined flag, typically set during compilation using the `-D` option, to signal 
     * whether the pull-up resistor on the IR capture pin should be enabled. It does not directly modify the 
     * code but serves as a configuration indicator for developers to adjust their logic accordingly.
     * 
     * @details
     * - **Possible Values:**
     *   - `1` (enabled): Use `-DIR_CAPTURE_PIN_PULLUP=1`.
     *   - `0` (disabled): Use `-DIR_CAPTURE_PIN_PULLUP=0`.
     *   - If not defined, it defaults to `0` (disabled).
     * 
     * - **Behavior:**
     *   - This macro is a static configuration flag and does not directly interact with the codebase.
     *   - Users must manually check its value to decide whether to enable the pull-up resistor during runtime.
     * 
     * Example Code Usage:
     * @code
     * #if PIN_IR_CAPTURE_PULLUP
     *     pinMode(PIN_IR_CAPTURE, INPUT_PULLUP);
     * #else
     *     pinMode(PIN_IR_CAPTURE, INPUT);
     * #endif
     * @endcode
     * 
     * @warning
     * - This macro does not alter code behavior automatically. Users must explicitly check and handle it.
     * - Ensure consistent use of the flag in the codebase for predictable behavior.
     * 
     * @note The flag only supports numeric values (`1` or `0`). Providing invalid values will result in a default of `0`.
     * 
     * @see @ref PIN_IR_CAPTURE for the IR capture pin configuration.
     */
    #if !defined(PIN_IR_CAPTURE_PULLUP) || defined(__DOXYGEN__)
        #define PIN_IR_CAPTURE_PULLUP (0)
    #elif (PIN_IR_CAPTURE_PULLUP != 0 && PIN_IR_CAPTURE_PULLUP != 1)
        #undef PIN_IR_CAPTURE_PULLUP
        #define PIN_IR_CAPTURE_PULLUP (0)
    #endif

    /**
     * @brief MCU pin connected to the demodulating IR receiver for receiving IR signals.
     * 
     * This pin is dedicated to receiving signals from a demodulating IR receiver, such as the TSOP4838, which processes 
     * modulated IR signals and outputs a clean, demodulated digital signal. It is designed for applications that involve 
     * standard IR communication protocols, such as remote controls for consumer electronics.
     * 
     * @details
     * - **Arduino Pin Number:** 12 (same as @ref PIN_SPI_MISO, PB4 on ATMega328P).
     * - **Receiver Type:** 
     *   - Demodulating IR receivers like the TSOP4838 decode the modulated carrier signal (e.g., 38 kHz) and output a 
     *     digital signal representing the logic states of the encoded data.
     *   - These receivers filter out noise and ambient IR interference, ensuring reliable communication in typical IR environments.
     * - **Functionality:**
     *   - The pin reads the digital output from the demodulating receiver.
     *   - Used to decode standard IR protocols such as NEC, RC5, and Sony, where the modulated carrier is removed, leaving only 
     *     the encoded data.
     * - **Use Cases:**
     *   - Applications where protocol decoding requires only the demodulated signal without the carrier frequency.
     *   - Standard remote-control communication for TVs, air conditioners, and other consumer electronics.
     *   - Supporting IR-based sensors or devices using off-the-shelf IR remotes.
     * - **Hardware Notes:**
     *   - Ensure the demodulating receiver is configured to match the carrier frequency of the transmitted IR signals 
     *     (e.g., 38 kHz for TSOP4838).
     *   - Connect the output of the demodulating receiver directly to PB4 with appropriate decoupling capacitors on the power 
     *     supply pins of the receiver for stable operation.
     *   - The pin can be configured as a digital input with an optional pull-up resistor if required by the receiver circuit.
     * 
     * @warning
     * - Ambient IR interference, such as sunlight or fluorescent lighting, can impact performance. Use shielding or filtering 
     *   to improve reliability in challenging environments.
     * - Ensure the receiver's output voltage levels are compatible with the MCU to prevent damage or misreads.
     * 
     * @note 
     * - Demodulating receivers simplify IR protocol handling by removing the need for carrier detection in software, making them 
     *   ideal for standard applications where signal timing is already encoded.
     * - For applications requiring raw signal capture, consider using @ref PIN_IR_CAPTURE instead, which supports non-demodulating 
     *   IR receivers.
     * 
     * @see @ref PIN_IR_RECEIVE_PULLUP for the configuration flag that indicates whether the pull-up resistor should be enabled on the IR receive pin.
     * @see @ref PIN_SPI_MISO for the SPI MISO (Master In Slave Out) pin configuration.
     */
    #if !defined(PIN_IR_RECEIVE) || defined(__DOXYGEN__)
        #define PIN_IR_RECEIVE PIN_SPI_MISO
    #endif  // !defined(PIN_IR_RECEIVE) || defined(__DOXYGEN__)

    /**
     * @brief Configuration flag for enabling the pull-up resistor on the IR receive pin.
     * 
     * This macro is a user-defined flag, typically set during compilation using the `-D` option, to signal 
     * whether the pull-up resistor on the IR receive pin should be enabled. It does not directly modify the 
     * code but serves as a configuration indicator for developers to adjust their logic accordingly.
     * 
     * @details
     * - **Possible Values:**
     *   - `1` (enabled): Use `-DPIN_IR_RECEIVE_PULLUP=1`.
     *   - `0` (disabled): Use `-DPIN_IR_RECEIVE_PULLUP=0`.
     *   - If not defined, it defaults to `1` (enabled).
     * 
     * - **Behavior:**
     *   - This macro is a static configuration flag and does not directly interact with the codebase.
     *   - Users must manually check its value to decide whether to enable the pull-up resistor during runtime.
     * 
     * Example Code Usage:
     * @code
     * #if PIN_IR_RECEIVE_PULLUP
     *     pinMode(PIN_IR_RECEIVE, INPUT_PULLUP);
     * #else
     *     pinMode(PIN_IR_RECEIVE, INPUT);
     * #endif
     * @endcode
     * 
     * @warning
     * - This macro does not alter code behavior automatically. Users must explicitly check and handle it.
     * - Ensure consistent use of the flag in the codebase for predictable behavior.
     * 
     * @note The flag only supports numeric values (`1` or `0`). Providing invalid values will result in a default of `1`.
     * 
     * @see @ref PIN_IR_RECEIVE for the IR receive pin configuration.
     */
    #if !defined(PIN_IR_RECEIVE_PULLUP) || defined(__DOXYGEN__)
        #define PIN_IR_RECEIVE_PULLUP (1)
    #elif (PIN_IR_RECEIVE_PULLUP != 0 && PIN_IR_RECEIVE_PULLUP != 1)
        #undef PIN_IR_RECEIVE_PULLUP
        #define PIN_IR_RECEIVE_PULLUP (1)
    #endif
    /** @} */ // End of IR Transmitter and Receiver Pins

    /**
     * @name Button and Configuration Pins
     * @{
     */
    #if defined(PIN_BUTTON_WAKEUP)
        #if PIN_BUTTON_WAKEUP != 2
            #undef PIN_BUTTON_WAKEUP
            #warning "PIN_BUTTON_WAKEUP is fixed to 2 (PD2)"
        #endif  // PIN_BUTTON_WAKEUP != 2
    #endif  // defined(PIN_BUTTON_WAKEUP)
    /** 
     * @brief MCU pin connected to the wake-up button for waking the device from sleep mode.
     * 
     * This pin is dedicated to the wake-up button, which allows the user to wake the %UIRB device 
     * from a low-power sleep mode. Pressing the button generates a signal that triggers the microcontroller 
     * to resume active operation.
     * 
     * @details
     * - **Arduino Pin Number:** 2 (PD2 on ATMega328P).
     * - **Button Behavior:**
     *   - The button is connected between the pin and ground.
     *   - When the button is pressed, the pin reads a LOW signal (active state).
     *   - When the button is released, the pin reads a HIGH signal due to the internal pull-up resistor.
     * - **Wake-up Functionality:**
     *   - The pin is configured as an external interrupt pin (INT0) to wake the device from sleep mode.
     *   - The interrupt should be configured to trigger on a falling edge (button press).
     * - **Use Cases:**
     *   - Waking the device from sleep mode to conserve power when idle.
     *   - Providing a dedicated control for user interaction without affecting other functionalities.
     * - **Hardware Notes:**
     *   - Internal pull-up resistor should be enabled to ensure the pin reads HIGH when the button is not pressed.
     *   - External pull-up resistors are not required since the library manages the internal pull-up.
     *   - Software debouncing is recommended for stable operation.
     * 
     * @note The active state of the button corresponds to LOW, as defined by @ref BUTTON_PIN_ACTIVE_STATE.
     */
    #define PIN_BUTTON_WAKEUP (2)

    #if defined(PIN_BUTTON_OPTION_1)
        #if PIN_BUTTON_OPTION_1 != 16
            #undef PIN_BUTTON_OPTION_1
            #warning "PIN_BUTTON_OPTION_1 is fixed to 16 (PC2)"
        #endif  // PIN_BUTTON_OPTION_1 != 16
    #endif  // defined(PIN_BUTTON_OPTION_1)
    /**  
     * @brief MCU pin connected to the first option button.
     * 
     * This pin is dedicated to the first option button on the %UIRB board, typically used for 
     * user input to trigger specific actions, toggle modes, or provide additional controls.
     * 
     * @details
     * - **Arduino Pin Number:** 16 (PC2 on ATMega328P).
     * - **Button Behavior:**
     *   - The button is connected between the pin and ground.
     *   - When the button is pressed, the pin reads a LOW signal (active state).
     *   - When the button is released, the pin reads a HIGH signal due to the internal pull-up resistor.
     * - **Use Cases:**
     *   - Navigation through settings or options.
     *   - Activation of auxiliary features.
     *   - Controlling advanced functions or modes in firmware.
     * - **Hardware Notes:**
     *   - Internal pull-up resistor should be enabled for this pin to ensure a stable HIGH signal when the button is not pressed.
     *   - External pull-up is not required as the library handles enabling the internal pull-up.
     *   - Software debouncing is recommended for clean signal transitions.
     * 
     * @note The active state of the button is configured as LOW, as specified by @ref BUTTON_PIN_ACTIVE_STATE.
     */
    #define PIN_BUTTON_OPTION_1 (16)

    #if defined(PIN_BUTTON_OPTION_2)
        #if PIN_BUTTON_OPTION_2 != 17
            #undef PIN_BUTTON_OPTION_2
            #warning "PIN_BUTTON_OPTION_2 is fixed to 17 (PC3)"
        #endif  // PIN_BUTTON_OPTION_2 != 17
    #endif  // defined(PIN_BUTTON_OPTION_2)
    /**  
     * @brief MCU pin connected to the second option button.
     * 
     * This pin is dedicated to the second option button on the %UIRB board, typically used for 
     * user input to trigger specific actions, toggle modes, or provide additional controls.
     * 
     * @details
     * - **Arduino Pin Number:** 17 (PC3 on ATMega328P).
     * - **Button Behavior:**
     *   - The button is connected between the pin and ground.
     *   - When the button is pressed, the pin reads a LOW signal (active state).
     *   - When the button is released, the pin reads a HIGH signal due to the internal pull-up resistor.
     * - **Use Cases:**
     *   - Navigation through settings or options.
     *   - Activation of auxiliary features.
     *   - Controlling advanced functions or modes in firmware.
     * - **Hardware Notes:**
     *   - Internal pull-up resistor should be enabled for this pin to ensure a stable HIGH signal when the button is not pressed.
     *   - External pull-up is not required as the library handles enabling the internal pull-up.
     *   - Software debouncing is recommended for clean signal transitions.
     * 
     * @note The active state of the button is configured as LOW, as specified by @ref BUTTON_PIN_ACTIVE_STATE.
     */
    #define PIN_BUTTON_OPTION_2 (17)

    #if defined(PIN_BUTTON_OPTION_3)
        #if PIN_BUTTON_OPTION_3 != 18
            #undef PIN_BUTTON_OPTION_3
            #warning "PIN_BUTTON_OPTION_3 is fixed to 18 (PC4)"
        #endif  // PIN_BUTTON_OPTION_3 != 18
    #endif  // defined(PIN_BUTTON_OPTION_3)
    /**  
     * @brief MCU pin connected to the third option button.
     * 
     * This pin is dedicated to the third option button on the %UIRB board, typically used for 
     * user input to trigger specific actions, toggle modes, or provide additional controls.
     * 
     * @details
     * - **Arduino Pin Number:** 18 (PC4 on ATMega328P).
     * - **Button Behavior:**
     *   - The button is connected between the pin and ground.
     *   - When the button is pressed, the pin reads a LOW signal (active state).
     *   - When the button is released, the pin reads a HIGH signal due to the internal pull-up resistor.
     * - **Use Cases:**
     *   - Navigation through settings or options.
     *   - Activation of auxiliary features.
     *   - Controlling advanced functions or modes in firmware.
     * - **Hardware Notes:**
     *   - Internal pull-up resistor should be enabled for this pin to ensure a stable HIGH signal when the button is not pressed.
     *   - External pull-up is not required as the library handles enabling the internal pull-up.
     *   - Software debouncing is recommended for clean signal transitions.
     * 
     * @note The active state of the button is configured as LOW, as specified by @ref BUTTON_PIN_ACTIVE_STATE.
     */
    #define PIN_BUTTON_OPTION_3 (18)

    #if defined(BUTTON_PIN_ACTIVE_STATE)
        #if BUTTON_PIN_ACTIVE_STATE != 0
            #undef BUTTON_PIN_ACTIVE_STATE
            #warning "BUTTON_PIN_ACTIVE_STATE is fixed to 0 (LOW)"
        #endif  // BUTTON_PIN_ACTIVE_STATE != 0
    #endif  // defined(BUTTON_PIN_ACTIVE_STATE)
    /** 
     * @brief Logical active state of the button pins.
     * 
     * This macro defines the logical active state of the button pins. When set to `0` (LOW), 
     * the buttons are active when the pin reads a LOW signal. This is typically used for buttons 
     * connected between the pin and ground with an internal pull-up resistor enabled.
     * 
     * @details
     * - **Possible Values:**
     *   - `0` (LOW): Buttons are active when the pin reads a LOW signal.
     *   - `1` (HIGH): Buttons are active when the pin reads a HIGH signal.
     * - **Default:** `0` (LOW)
     * 
     * Example Code Usage:
     * @code
     * // Example: Checking button state based on BUTTON_PIN_ACTIVE_STATE
     * pinMode(PIN_BUTTON_WAKEUP, INPUT_PULLUP);  // Configure button pin with pull-up enabled
     * 
     * if (digitalRead(PIN_BUTTON_WAKEUP) == BUTTON_PIN_ACTIVE_STATE)
     * {
     *     // Button is pressed
     *     Serial.println("Button Pressed");
     * }
     * else
     * {
     *     // Button is not pressed
     *     Serial.println("Button Released");
     * }
     * @endcode
     * 
     * @warning
     * - Ensure that the button pin is properly configured (e.g., with a pull-up resistor) to avoid floating states.
     * - Leave this macro as `0` (LOW) unless you have done specifing modifications to the buttons on the %UIRB board.
     */
    #define BUTTON_PIN_ACTIVE_STATE (0)
    /** @} */ // End of Button Pins

    /**
     * @name LED Pins
     * @{
     */
    #if defined(PIN_STAT_LED)
        #if PIN_STAT_LED != 13
            #undef PIN_STAT_LED
            #warning "PIN_STAT_LED is fixed to 13 (PB5)"
        #endif  // PIN_STAT_LED != 13
    #endif  // defined(PIN_STAT_LED)
    /** 
     * @brief MCU pin connected to the onboard status LED.
     * 
     * This pin is connected to the built-in status LED on the %UIRB board, 
     * typically used to indicate the device's operational state, errors, 
     * or other status information. The LED is controlled by toggling the 
     * digital output state of the associated pin.
     * 
     * @details
     * - **Arduino Pin Number:** 13 (same as @ref PIN_SPI_SCK, PB5 on ATMega328P).
     * - **LED Behavior:** 
     *   - A HIGH output turns the LED on (sourcing current to the LED).
     *   - A LOW output turns the LED off.
     * - **Use Cases:** 
     *   - Power-on indicator.
     *   - Visual feedback during programming.
     *   - Status indication for errors or operational modes.
     * - **Driver Circuit:** 
     *   - The LED is connected to the microcontroller pin through a current-limiting resistor.
     * - **Shared Functionality:**
     *   - This pin is shared with @ref PIN_SPI_SCK for SPI communication.
     *   - While SPI and LED functionalities can coexist in some scenarios, ensure proper pin state management 
     *     to avoid unintended interference.
     * 
     * @note This pin is also aliased as @ref LED_BUILTIN for compatibility with Arduino frameworks.
     */
    #define PIN_STAT_LED PIN_SPI_SCK

    #if defined(LED_BUILTIN)
        #undef LED_BUILTIN
    #endif  // defined(LED_BUILTIN)
    /** 
     * @brief Alias for @ref PIN_STAT_LED.
     */
    #define LED_BUILTIN PIN_STAT_LED
    /** @} */ // End of LED Pins

#else  // defined(UIRB_BOARD_V02) || defined(__DOXYGEN__)

    #error "Unknown UIRB board version."

#endif  // defined(UIRB_BOARD_V02) || defined(__DOXYGEN__)

#endif  // UIRBcore_Pins_h
