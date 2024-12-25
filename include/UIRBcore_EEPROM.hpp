/**
 * @file UIRBcore_EEPROM.hpp
 * @brief Header for managing configuration and metadata stored in EEPROM for the %UIRB system.
 * 
 * This header provides the necessary definitions, structures, constants, and class declarations
 * required to store, retrieve, and manipulate configuration data in the EEPROM memory
 * of the %UIRB system. It includes compact and efficient representations of hardware and 
 * software configurations, manufacturing information, runtime statistics, and more.
 * 
 * @details
 * - The @ref UIRB_EEPROM_BYPASS_DEBUG macro allows all EEPROM operations to be redirected to RAM for debugging purposes, 
 *   ensuring data persistence is emulated without requiring physical EEPROM.
 * - The @ref UIRB_EEPROM_RPROG_DEBUG macro enables debugging of the charger programming resistor value (`Rprog`) by 
 *   setting it to a specific value during runtime. This is especially useful in simulations or for testing different configurations.
 * 
 * @note Both macros are intended for debugging and testing scenarios. Refer to their documentation for detailed usage guidelines.
 * 
 * @note This file is part of the %UIRB firmware library and is tailored for the %UIRB system.
 *       Adjustments may be required for use in other applications.
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
#ifndef UIRBcore_EEPROM_hpp
#define UIRBcore_EEPROM_hpp

#include <Arduino.h>
#include <UIRBcore_Defs.h>

#if defined(UIRB_EEPROM_DATA_ADDR_START)
    #error "UIRB_EEPROM_DATA_ADDR_START is fixed to 0x00"
#endif  // defined(UIRB_EEPROM_DATA_ADDR_START)

/**
 * @brief The starting address in EEPROM where @ref uirbcore::eeprom::EEPROMData is stored.
 * 
 * This constant defines the base memory address in the EEPROM used for storing 
 * the configuration and metadata of the %UIRB. All related data structures 
 * and variables will be stored sequentially starting from this address.
 * 
 * @note Ensure that the address does not conflict with other EEPROM allocations 
 * within the system.
 */
#define UIRB_EEPROM_DATA_ADDR_START (0x00U)

namespace uirbcore 
{
    /**
     * @brief Forward declaration of the @ref UIRB class.
     * 
     */
    class UIRB;

    /**
     * @brief Contains definitions and utilities for managing configuration data in EEPROM for the %UIRB system.
     * 
     * The `eeprom` namespace encapsulates all components related to the storage, retrieval, and management 
     * of configuration data in the EEPROM memory of the %UIRB system. It includes:
     * - Data structures representing hardware and software configurations.
     * - Helper functions and utilities for manipulating data stored in EEPROM.
     * - Classes and constants that provide a high-level interface for managing EEPROM data.
     * 
     * @details
     * The namespace is organized to ensure a clear separation of responsibilities, with each component 
     * focusing on specific aspects of EEPROM management. This modular approach simplifies the integration 
     * of new features and ensures consistency across different parts of the system.
     * 
     * **Core Components:**
     * - @ref EEPROMData : A packed structure that stores all hardware and software configurations, 
     *   metadata, and runtime statistics.
     * - @ref HardwareVersion : A compact union representing the hardware version using major and minor 
     *   version numbers.
     * - @ref HardwareManufactureDate : A compact union representing the manufacturing date with a year offset 
     *   from 2020 and the month.
     * - @ref SoftwareConfig : A bitfield union for storing various software configuration flags.
     * - @ref EEPROMDataManager : A high-level class for managing @ref EEPROMData in EEPROM and RAM.
     * - Constants such as @ref EEPROMDataManager::INVALID_CHARGER_PROG_RESISTANCE and @ref EEPROMDataManager::INVALID_MANUFACTURE_MONTH 
     *   for error handling and validation.
     * 
     * **Design Considerations:**
     * - The structures and classes in this namespace are designed to be memory-efficient, leveraging 
     *   packed and aligned data representations.
     * - Each component includes safeguards for data validation and error handling to ensure robust operation.
     * 
     * **Key Features:**
     * - Compact and efficient representations of configuration data.
     * - Flexible and extensible design to accommodate future hardware revisions or feature enhancements.
     * - High-level interfaces for managing EEPROM data with field-specific accessors and mutators.
     * 
     * @note The components in this namespace are tailored specifically for the %UIRB system. While they 
     *       can be adapted for other applications, modifications may be necessary to ensure compatibility.
     * 
     * @see @ref EEPROMDataManager for the main interface to manage EEPROM data.
     * @see @ref EEPROMData for the data structure representing the core configuration.
     */
    namespace eeprom
    {
        /**
         * @brief Represents the hardware version of the %UIRB system.
         * 
         * The @ref HardwareVersion union provides a compact and efficient representation of the hardware version.
         * It encodes both the major and minor version numbers into a single byte.
         * 
         * @details
         * - The @ref major field occupies the upper nibble and represents the major version of the hardware, 
         *   with a valid range of `[0-15]`.
         * - The @ref minor field occupies the lower nibble and represents the minor version of the hardware, 
         *   with a valid range of `[0-15]`.
         * - The @ref version_byte field combines the @ref major and @ref minor fields into a single `uint8_t` value, 
         *   where the higher nibble stores the @ref major version and the lower nibble stores the @ref minor version.
         * 
         * @note This union is packed and aligned to a single byte for efficient memory usage.
         * 
         * @see @ref EEPROMDataManager::get_hardware_version() to retrieve the stored hardware version.
         */
        union HardwareVersion
        {
            struct
            {
                uint8_t minor : 4; /**< @brief Minor version `[0-15]`. Part of @ref HardwareVersion. */
                uint8_t major : 4; /**< @brief Major version `[0-15]`. Part of @ref HardwareVersion. */
            };
            uint8_t version_byte; /**< @brief Combined @ref major and @ref minor version in a single byte. */
        } __attribute__((packed, aligned(1)));

        /**
         * @brief Represents the hardware version of the %UIRB board.
         * 
         * The @ref UIRB_HW_VER constant defines the specific hardware version of the %UIRB board. 
         * It is statically initialized based on predefined macros during compilation to ensure compatibility 
         * and version control in the firmware.
         * 
         * @details
         * - When @ref UIRB_EEPROM_BYPASS_DEBUG is defined, or during documentation generation 
         *   (e.g., with Doxygen), it defaults to `{ .minor = 0, .major = 0 }`. This ensures compatibility 
         *   in debugging scenarios or when the version macro is undefined.
         * - When @ref UIRB_BOARD_V02 is defined, this constant is set to `{ .minor = 2, .major = 0 }`, 
         *   representing hardware version 0.2.
         * - If no version macro is explicitly defined, it also defaults to `{ .minor = 0, .major = 0 }`.
         * 
         * @note
         * - This constant is defined as `static constexpr` to ensure it is evaluated at compile time, 
         *   minimizing runtime overhead.
         * - When @ref UIRB_EEPROM_BYPASS_DEBUG is enabled, this default version is used for emulation 
         *   and testing purposes.
         * 
         * @see @ref HardwareVersion for the structure used to represent the hardware version.
         * @see @ref EEPROMDataManager::get_hardware_version() for retrieving the hardware version stored in EEPROM.
         */
        static constexpr HardwareVersion UIRB_HW_VER =
        #if defined(__DOXYGEN__) || defined(UIRB_EEPROM_BYPASS_DEBUG)
            { { .minor = 0, .major = 0 } };
        #elif defined(UIRB_BOARD_V02)
            { { .minor = 2, .major = 0 } };
        #else
            { { .minor = 0, .major = 0 } };
        #endif

        /**
         * @brief Represents the hardware manufacture date in a compact format.
         * 
         * The @ref HardwareManufactureDate union provides a memory-efficient representation of the 
         * manufacturing date for %UIRB hardware. It encodes the year as an offset from 2020 and the month 
         * in a single byte, allowing easy storage and retrieval.
         * 
         * @details
         * - The @ref year_offset_from_2020 field occupies the upper nibble and represents the year offset 
         *   from 2020, with a valid range of `[0-15]`. This corresponds to the years `[2020-2035]`.
         * - The @ref month field occupies the lower nibble and represents the month of manufacture, 
         *   with a valid range of `[1-12]`.
         * - The @ref month_year_byte field combines the @ref year_offset_from_2020 and @ref month fields 
         *   into a single `uint8_t` value. The higher nibble stores the year offset, and the lower nibble stores the month.
         * 
         * @note This union is packed and aligned to a single byte for optimal memory usage.
         * 
         * @see @ref EEPROMDataManager::get_board_manufacture_year() to retrieve the year of manufacture.
         * @see @ref EEPROMDataManager::get_board_manufacture_month() to retrieve the %month of manufacture.
         */
        union HardwareManufactureDate
        {
            struct
            {
                uint8_t year_offset_from_2020 : 4; /**< @brief Year offset from 2020 `[0-15]`, corresponding to `[2020-2035]`. Part of @ref HardwareManufactureDate. */
                uint8_t month : 4; /**< @brief %Month of manufacture `[1-12]`. Part of @ref HardwareManufactureDate. */
            };
            uint8_t month_year_byte; /**< @brief Combined @ref year_offset_from_2020 and @ref month in a single byte. */
        } __attribute__((packed, aligned(1)));

        /**
         * @brief Represents the software configuration settings for the %UIRB system.
         * 
         * The @ref SoftwareConfig union provides a compact representation of software configuration flags.
         * Each flag occupies a single bit, allowing multiple configuration options to be stored within a single byte.
         * 
         * @details
         * - The `struct` inside the union defines individual boolean flags for specific configuration options:
         *   - @ref avr_serial_debugger_enabled : Indicates if the firmware is compiled with @ref AVR_DEBUG enabled, 
         *     allowing the AVR serial debugger (`avr8-stub`) to operate.
         *   - @ref sleep_mode_allowed : Indicates if sleep mode is permitted.
         *   - @ref sleep_mode_io3_wakeup_enabled : Indicates if the MCU can be woken up by the IO3 pin.
         *   - @ref boot_count_increment_enabled : Indicates if the boot count should be incremented upon system boot.
         *   - @ref reserved_config_1, @ref reserved_config_2, @ref reserved_config_3, @ref reserved_config_4 : Reserved 
         *     for future use.
         * - The @ref config_byte field combines all these flags into a single `uint8_t` value, where each bit 
         *   represents a specific configuration option.
         * 
         * @note This union is packed and aligned to a single byte for optimal memory efficiency.
         * 
         * @see @ref EEPROMDataManager::is_avr_serial_debugger_enabled() to check if the AVR debugger is enabled.
         * @see @ref EEPROMDataManager::allow_sleep_mode() to enable or disable sleep mode.
         * @see @ref EEPROMDataManager::allow_sleep_mode_io3_wakeup() to enable or disable wakeup from the IO3 pin.
         */
        union SoftwareConfig
        {
            struct
            {
                bool avr_serial_debugger_enabled : 1; /**< @brief Indicates if firmware is compiled with @ref AVR_DEBUG defined (AVR serial debugger is enabled). Part of @ref SoftwareConfig. */
                bool sleep_mode_allowed : 1; /**< @brief Indicates if sleep mode is allowed. Part of @ref SoftwareConfig. */
                bool sleep_mode_io3_wakeup_enabled : 1; /**< @brief Indicates if the MCU can be woken up by IO3 pin. Part of @ref SoftwareConfig. */
                bool boot_count_increment_enabled : 1; /**< @brief Indicates if the boot count should be incremented on boot. Part of @ref SoftwareConfig. */
                bool reserved_config_1 : 1; /**< @brief Reserved for future use. Part of @ref SoftwareConfig. */
                bool reserved_config_2 : 1; /**< @brief Reserved for future use. Part of @ref SoftwareConfig. */
                bool reserved_config_3 : 1; /**< @brief Reserved for future use. Part of @ref SoftwareConfig. */
                bool reserved_config_4 : 1; /**< @brief Reserved for future use. Part of @ref SoftwareConfig. */
            };
            uint8_t config_byte; /**< Combined flags as a single byte (`uint8_t`). */
        } __attribute__((packed, aligned(1)));

        /**
         * @brief Represents the serial number of the %UIRB system.
         * 
         * The @ref SerialNumber union provides a compact representation of the %UIRB's serial number, 
         * with additional reserved bits for internal use. It ensures efficient storage and access while 
         * allowing extensibility for future use cases.
         * 
         * @details
         * - The `struct` inside the union defines the following fields:
         *   - @ref number : The serial number of the %UIRB, stored in the lower 14 bits. This value is constrained 
         *     to the range `[0 - ` @ref EEPROMDataManager::UIRB_SERIAL_NUMBER_MAX `]`.
         *   - @ref reserved : Upper 2 bits reserved for internal use, providing flexibility for future extensions 
         *     or internal flags without altering the serial number representation.
         *   - Reserved bits are broken down into:
         *     - @ref reserved_bit_0 : A general-purpose reserved bit for potential future functionality.
         *     - @ref reserved_bit_1 : A debug flag that indicates an unknown or invalid serial number. When this bit 
         *       is set, the @ref number field should be treated as invalid and the serial number should be interpreted 
         *       as @ref EEPROMDataManager::INVALID_UIRB_SERIAL_NUMBER.
         * - The @ref serial_number_u16 field combines @ref number and @ref reserved into a single `uint16_t` value 
         *   for compact storage and efficient access.
         * 
         * @note
         * - This union is packed and aligned to ensure minimal memory usage and consistent access across architectures.
         * - Always validate the serial number using @ref reserved_bit_1 to ensure it is valid before using it.
         * 
         * @warning
         * - Misinterpreting the reserved bits may result in undefined behavior. Ensure proper handling based on their 
         *   documented roles.
         * 
         * @see @ref EEPROMDataManager::get_uirb_board_serial_number() to retrieve the serial number.
         * @see @ref EEPROMDataManager::INVALID_UIRB_SERIAL_NUMBER for the constant indicating an invalid serial number.
         */
        union SerialNumber
        {
            struct
            {
                uint16_t number : 14; /**< @brief Serial number of %UIRB `[0 - ` @ref EEPROMDataManager::UIRB_SERIAL_NUMBER_MAX `]`. Part of @ref SerialNumber. */
                union
                {
                    struct
                    {
                        uint8_t reserved_bit_0 : 1; /**< @brief First reserved bit. Part of @ref SerialNumber. */
                        uint8_t reserved_bit_1 : 1; /**< @brief Debug flag indicating an unknown serial number. 
                                                     *  When this bit is set, the number field is invalid 
                                                     *  and should be treated as unknown, regardless of its value.
                                                     *  Part of @ref SerialNumber.
                                                     */
                    };
                    uint8_t reserved : 2; /**< @brief Upper 2 bits reserved for internal use. Part of @ref SerialNumber. */
                };
            };
            uint16_t serial_number_u16; /**< @brief Combined serial @ref number and @ref reserved bits as a single `uint16_t`. */
        } __attribute__((packed, aligned(1)));

        /**
         * @brief Length of the factory CP2104 USB serial number (excluding null terminator).
         * 
         * This constant defines the fixed length of the factory-assigned CP2104 USB serial number, 
         * which is exactly 8 characters long. The serial number does not include a null terminator, 
         * making it suitable for storage in fixed-size buffers or EEPROM.
         * 
         * @details
         * - This length is specific to the CP2104 USB-to-UART bridge used in the %UIRB system.
         * - When retrieving or storing the serial number, ensure that the buffer accommodates 8 characters 
         *   without appending a null terminator unless explicitly required.
         * 
         * @see @ref EEPROMData for the overall structure of the EEPROM data.
         * @see @ref EEPROMDataManager::get_factory_cp2104_usb_serial_number_cstr(char*, const size_t) const for retrieving the serial number.
         */
        constexpr uint8_t DATA_FACTORY_CP2104_SERIAL_NUM_LEN = 8U;

        /**
         * @brief Represents the data structure stored in EEPROM for the %UIRB system.
         * 
         * The @ref EEPROMData struct encapsulates all configuration and state-related information 
         * required for the operation of the %UIRB board. It is designed to be compact and efficient 
         * for storage in the EEPROM memory of the microcontroller.
         * 
         * @details
         * This structure is packed and aligned to 1 byte to ensure minimal memory usage and compatibility 
         * with EEPROM storage. The fields include hardware and software configurations, manufacturing 
         * information, and runtime statistics.
         * 
         * **Fields:**
         * - @ref hardware_version : Represents the hardware version of the %UIRB board using major and minor versioning.
         * - @ref bandgap_1v1_reference_offset : Offset in millivolts from the 1100mV bandgap reference voltage.
         * - @ref stat_led_brightness : Brightness level of the status LED (range `[0-255]`).
         * - @ref charger_prog_resistor_ohms : Resistance of the `Rprog` resistor connected to the charger’s `PROG` pin.
         * - @ref software_config : Flags for various software configurations, including debugger and sleep mode settings.
         * - @ref hardware_manufacture_date : Encodes the manufacturing month and year offset from 2020.
         * - @ref boot_count : Tracks the number of times the board has been booted.
         * - @ref uirb_serial_number : Unique serial number of the %UIRB board (range `[0 - ` @ref EEPROMDataManager::UIRB_SERIAL_NUMBER_MAX `]`).
         * - @ref factory_cp2104_usb_serial_number : Fixed-length serial number (8 ASCII characters) for the CP2104 USB interface.
         * 
         * @see @ref operator==(const EEPROMData&, const EEPROMData&) for the equality comparison operator.
         * @see @ref EEPROMDataManager for methods to read, write, and manipulate this structure in EEPROM.
         * @see @ref HardwareVersion for the `union` representing the hardware version.
         * @see @ref SoftwareConfig for the `union` representing software configuration options.
         * @see @ref HardwareManufactureDate for the `union` representing the manufacturing date.
         * @see @ref SerialNumber for the `union` representing the serial %number of the %UIRB system.
         */
        struct EEPROMData
        {
            HardwareVersion hardware_version; /**< @brief Hardware version of %UIRB board. */
            int8_t bandgap_1v1_reference_offset; /**< @brief Offset from 1100mV bandgap reference voltage, in millivolts. */
            uint8_t stat_led_brightness; /**< @brief Status LED brightness (range `[0-255]`). */
            uint16_t charger_prog_resistor_ohms; /**< @brief Resistance of `Rprog` resistor connected to `PROG` pin of the charger. */
            SoftwareConfig software_config; /**< @brief Flags for software configuration options. */
            HardwareManufactureDate hardware_manufacture_date; /**< @brief Manufacturing date: month and year offset from 2020. */
            uint32_t boot_count; /**< @brief Total boot count of the board. */
            SerialNumber uirb_serial_number; /**< @brief Unique serial number of the %UIRB board (range `[0 - ` @ref EEPROMDataManager::UIRB_SERIAL_NUMBER_MAX `]`). */
            char factory_cp2104_usb_serial_number[DATA_FACTORY_CP2104_SERIAL_NUM_LEN]; /**< @brief CP2104 USB serial number (8 ASCII characters, not null-terminated). */
        } __attribute__((packed, aligned(1)));

        /**
         * @brief Compares two @ref EEPROMData structures for equality.
         * 
         * This operator checks if all fields in the left-hand side (LHS) and 
         * right-hand side (RHS) @ref EEPROMData structs are identical.
         * 
         * @param[in] lhs The left-hand side @ref EEPROMData struct.
         * @param[in] rhs The right-hand side @ref EEPROMData struct.
         * @return bool Returns `true` if all fields match, otherwise `false`.
         * @retval true All fields in the LHS and RHS @ref EEPROMData structs are identical.
         * @retval false At least one field in the LHS and RHS @ref EEPROMData structs differs.
         * 
         * @note
         * - If @ref UIRB_USE_MEMCMP_FOR_STRUCT_COMPARISON is defined, a direct memory comparison using 
         *   `memcmp` is performed for efficiency.
         * - If @ref UIRB_USE_MEMCMP_FOR_STRUCT_COMPARISON is not defined, each field in the @ref EEPROMData 
         *   struct is compared individually to account for potential differences in alignment or padding. 
         *   The @ref EEPROMData::factory_cp2104_usb_serial_number field is compared manually as it is a fixed-length 
         *   character array without a null terminator.
         * 
         * @warning When using `memcmp`, differences in padding or alignment may lead to incorrect results.
         * 
         * @see @ref EEPROMData for the overall structure of the EEPROM data.
         * @see @ref UIRB_USE_MEMCMP_FOR_STRUCT_COMPARISON for enabling direct memory comparison.
         */
        bool operator==(const EEPROMData& lhs, const EEPROMData& rhs);

        /**
         * @brief Compares two @ref EEPROMData structures for inequality.
         * 
         * This operator checks if any field in the left-hand side (LHS) and 
         * right-hand side (RHS) @ref EEPROMData structs differs. It returns 
         * `true` if at least one field does not match, and `false` if all 
         * fields are identical.
         * 
         * @param[in] lhs The left-hand side @ref EEPROMData struct.
         * @param[in] rhs The right-hand side @ref EEPROMData struct.
         * @return bool Returns `true` if any field differs, otherwise `false`.
         * @retval true At least one field in the LHS and RHS @ref EEPROMData structs differs.
         * @retval false All fields in the LHS and RHS @ref EEPROMData structs are identical.
         * 
         * @details
         * - This operator is implemented as the logical negation of the equality operator (`==`).
         * - The equality operator performs the actual field comparisons, and this operator simply 
         *   negates the result.
         * 
         * @see @ref operator== for the equality comparison logic.
         * @see @ref UIRB_USE_MEMCMP_FOR_STRUCT_COMPARISON for enabling direct memory comparison.
         */
        bool operator!=(const EEPROMData& lhs, const EEPROMData& rhs);

        /**
         * @brief A utility class for managing configuration data stored in EEPROM.
         * 
         * The @ref EEPROMDataManager class provides an interface to read, write, and manipulate 
         * the @ref EEPROMData structure stored in the EEPROM memory of the %UIRB system. This 
         * class ensures data consistency and allows easy access to hardware configurations, 
         * runtime statistics, and metadata.
         * 
         * @details
         * The class maintains an in-memory representation of the @ref EEPROMData structure, 
         * enabling efficient manipulation without frequent EEPROM access. Changes made 
         * to the in-memory data are not saved to EEPROM until explicitly committed. The class 
         * provides methods for:
         * - Loading data from EEPROM into RAM.
         * - Saving data from RAM back to EEPROM.
         * - Accessing and updating specific configuration fields, such as hardware version, 
         *   manufacturing date, boot count, and more.
         * - Performing validation checks on certain fields, such as ensuring serial numbers 
         *   and resistor values fall within valid ranges.
         * 
         * The @ref EEPROMDataManager class also includes static utility functions for directly 
         * reading from or writing to EEPROM, which can be used for low-level operations 
         * or testing.
         * 
         * **Key Features:**
         * - Verifies hardware version compatibility during initialization.
         * - Supports efficient in-memory manipulation of EEPROM data.
         * - Provides field-specific accessors and mutators for individual configuration settings.
         * - Implements safeguards against invalid data, ensuring robust operation.
         * 
         * @note This class is tightly coupled with the @ref EEPROMData structure, which encapsulates 
         *       all hardware and software configuration settings.
         * 
         * @see @ref EEPROMData for the structure this class manages.
         */
        class EEPROMDataManager 
        {
            public:
                /**
                 * @brief Constructs a new @ref EEPROMDataManager object and initializes it with data stored in EEPROM or RAM (in debug mode).
                 * 
                 * This constructor initializes the @ref EEPROMDataManager object by reading the @ref HardwareVersion from 
                 * either EEPROM or RAM, depending on whether @ref UIRB_EEPROM_BYPASS_DEBUG is defined. It then verifies that 
                 * the retrieved hardware version matches the predefined constant @ref UIRB_HW_VER. If the versions match, the 
                 * full @ref EEPROMData structure is loaded into the internal RAM representation for further operations.
                 * 
                 * @details
                 * - In normal operation, the @ref HardwareVersion is read from EEPROM starting at the address defined by 
                 *   @ref CORE_DATA_ADDR_START.
                 * - When @ref UIRB_EEPROM_BYPASS_DEBUG is defined, the @ref HardwareVersion is initialized with the default 
                 *   value defined in @ref DEBUG_EEPROM_DATA.
                 * - If the hardware versions match, the constructor automatically loads the full @ref EEPROMData structure 
                 *   into RAM by calling @ref load_from_eeprom().
                 * 
                 * @note
                 * - Use this constructor to ensure that the @ref EEPROMDataManager object is initialized with data directly 
                 *   from EEPROM, while validating that the stored hardware version matches the expected version.
                 * - In debug mode, the data is retrieved from RAM and is not persistent between reboots or power cycles.
                 * 
                 * @warning
                 * - If the hardware versions do not match, the full @ref EEPROMData structure will not be loaded, and the 
                 *   object will remain partially initialized. Ensure proper handling of this scenario in your application.
                 * - When using debug mode (@ref UIRB_EEPROM_BYPASS_DEBUG), ensure that the default values in 
                 *   @ref DEBUG_EEPROM_DATA are appropriate for your use case.
                 * 
                 * @see @ref hardware_version_matches() for hardware version validation.
                 * @see @ref load_from_eeprom() for loading the full @ref EEPROMData structure.
                 * @see @ref CORE_DATA_ADDR_START for the starting EEPROM address.
                 * @see @ref DEBUG_EEPROM_DATA for default debug mode values.
                 */
                EEPROMDataManager();

                /**
                 * @brief Constructs a new @ref EEPROMDataManager object with a specified @ref EEPROMData structure.
                 * 
                 * This constructor initializes the @ref EEPROMDataManager object with the provided @ref EEPROMData 
                 * structure, storing it in the internal RAM representation.
                 * 
                 * @param[in] data A reference to the @ref EEPROMData structure containing the initial configuration 
                 *                 and metadata for the %UIRB board.
                 * 
                 * @note Use this constructor when you want to initialize the @ref EEPROMDataManager object 
                 *       with a specific configuration instead of reading it from EEPROM.
                 */
                explicit EEPROMDataManager(const EEPROMData& data);

                /**
                 * @brief Checks if the @ref HardwareVersion stored in RAM matches the predefined @ref UIRB_HW_VER.
                 * 
                 * This method compares the @ref HardwareVersion value currently stored in RAM 
                 * with the predefined hardware version constant @ref UIRB_HW_VER to ensure compatibility 
                 * between the firmware and the hardware.
                 * 
                 * @return `bool` Indicates whether the @ref HardwareVersion in RAM matches @ref UIRB_HW_VER.
                 * @retval true The @ref HardwareVersion in RAM matches the predefined @ref UIRB_HW_VER.
                 * @retval false The @ref HardwareVersion in RAM does not match @ref UIRB_HW_VER.
                 * 
                 * @note This method is useful for verifying that the firmware is being used on the 
                 *       correct hardware revision.
                 */
                bool hardware_version_matches() const;

                /**
                 * @brief Retrieves the @ref HardwareVersion stored in RAM.
                 * 
                 * This method provides access to the @ref HardwareVersion stored in the @ref EEPROMData 
                 * structure in RAM, which represents the hardware revision of the %UIRB board.
                 * 
                 * @return @ref HardwareVersion The current hardware version stored in RAM as a union object.
                 * 
                 * @note The @ref HardwareVersion includes both the major and minor version fields, 
                 *       combined into a compact structure for efficient storage and retrieval.
                 */
                HardwareVersion get_hardware_version() const;

                /**
                 * @brief Retrieves the bandgap reference voltage in millivolts from RAM.
                 * 
                 * This method calculates and returns the current bandgap reference voltage based on 
                 * the offset stored in the @ref EEPROMData structure in RAM. The bandgap voltage is centered 
                 * around `1100mV`, with the offset applied to determine the final value.
                 * 
                 * @return `uint16_t` The bandgap reference voltage in millivolts.
                 * 
                 * @note The returned value is the sum of the base voltage (`1100mV`) and the offset 
                 *       stored in the @ref EEPROMData::bandgap_1v1_reference_offset field.
                 */
                uint16_t get_bandgap_reference_milivolts() const;

                /**
                 * @brief Sets the bandgap reference voltage offset in millivolts in RAM.
                 * 
                 * This method validates and updates the bandgap reference voltage offset in the @ref EEPROMData 
                 * structure stored in RAM. The offset is calculated as the difference between the provided 
                 * voltage and the base voltage of `1100mV`. The provided voltage must fall within the range 
                 * `[972mV - 1227mV]` to ensure valid operation.
                 * 
                 * @param[in] milivolts The desired bandgap reference voltage in millivolts.
                 * @return `bool` Indicates whether the offset was set successfully.
                 * @retval true The offset was calculated and stored successfully.
                 * @retval false The provided @p milivolts value is out of the valid range `[972mV - 1227mV]`.
                 * 
                 * @note The valid range of the @p milivolts parameter is derived from the base voltage 
                 *       (`1100mV`) and the minimum and maximum allowable offsets (`-128mV` to `+127mV`).
                 * 
                 * @warning Setting an out-of-range voltage will result in a `false` return value and no 
                 *          updates to the stored offset.
                 */
                bool set_bandgap_reference_milivolts(const uint16_t milivolts);

                /**
                 * @brief Retrieves the brightness level of the status LED stored in RAM.
                 * 
                 * This method returns the current brightness level of the status LED, which is stored in the 
                 * @ref EEPROMData structure in RAM. The brightness is represented as an 8-bit unsigned integer.
                 * 
                 * @return `uint8_t` The brightness level of the status LED, with a valid range of `[0 - 255]`.
                 * 
                 * @note The brightness value corresponds to the intensity of the LED, where `0` represents 
                 *       the minimum brightness (LED off) and `255` represents the maximum brightness.
                 */
                uint8_t get_stat_led_brightness() const;

                /**
                 * @brief Sets the brightness level of the status LED in RAM.
                 * 
                 * This method updates the brightness level of the status LED in the @ref EEPROMData structure 
                 * stored in RAM. The brightness is represented as an 8-bit unsigned integer.
                 * 
                 * @param[in] brightness The desired brightness level for the status LED, within the range `[0 - 255]`.
                 * 
                 * @note The brightness value corresponds to the intensity of the LED, where `0` represents 
                 *       the minimum brightness (LED off) and `255` represents the maximum brightness.
                 */
                void set_stat_led_brightness(const uint8_t brightness);

                /**
                 * @brief Retrieves the resistance of the `Rprog` resistor stored in RAM, in ohms.
                 * 
                 * This method fetches the resistance value of the `Rprog` resistor that controls the 
                 * charging current of the charger. The value is retrieved from the @ref EEPROMData structure in RAM.
                 * 
                 * @return `uint16_t` The resistance of the `Rprog` resistor in ohms.
                 * @retval #INVALID_CHARGER_PROG_RESISTANCE Indicates the resistance value is invalid.
                 */
                uint16_t get_charger_prog_resistor_ohms() const;

                /**
                 * @brief Sets the resistance of the `Rprog` resistor in RAM, in ohms.
                 * 
                 * This method updates the resistance value of the `Rprog` resistor in the @ref EEPROMData 
                 * structure stored in RAM. The `Rprog` resistor is connected to the charger's `PROG` pin 
                 * and determines the charging current.
                 * 
                 * @param[in] ohms The resistance value of the `Rprog` resistor in ohms.
                 * @return `bool` Indicates whether the resistance was successfully updated.
                 * @retval true The resistance was set successfully.
                 * @retval false The provided resistance value is less than the minimum valid threshold 
                 *         (#CHARGER_PROG_RESISTANCE_MIN).
                 * 
                 * @note The minimum valid resistance value is defined by @ref CHARGER_PROG_RESISTANCE_MIN. 
                 *       Values below this threshold are considered invalid and will not be accepted.
                 */
                bool set_charger_prog_resistor_ohms(const uint16_t ohms);

                /**
                 * @brief Checks if the AVR serial debugger (`avr8-stub`) is present in the firmware.
                 * 
                 * This method retrieves the flag stored in RAM indicating whether the firmware 
                 * was compiled with the @ref AVR_DEBUG macro defined.
                 * 
                 * @return `bool` Represents whether the AVR serial debugger (`avr8-stub`) is or was present in the firmware.
                 * @retval true The firmware is compiled with #AVR_DEBUG defined.
                 * @retval false The firmware is not compiled with #AVR_DEBUG defined.
                 */
                bool is_avr_serial_debugger_enabled() const;

                /**
                 * @brief Enables or disables the AVR serial debugger (`avr8-stub`) flag in RAM.
                 * 
                 * This method sets the flag in RAM indicating whether the firmware 
                 * is compiled with the @ref AVR_DEBUG macro defined.
                 * 
                 * @param[in] enabled `bool` Represents whether the AVR serial debugger is present in the firmware.
                 * @arg @c true AVR serial debugger is present in the firmware.
                 * @arg @c false AVR serial debugger is not present in the firmware.
                 */
                void set_avr_serial_debugger(const bool enabled);

                /**
                 * @brief Checks if sleep mode is allowed.
                 * 
                 * This method retrieves the flag stored in RAM indicating whether the system is permitted 
                 * to enter sleep mode.
                 * 
                 * @return bool Represents whether sleep mode is allowed. 
                 * @retval true Sleep mode is allowed.
                 * @retval false Sleep mode is not allowed.
                 */
                bool is_sleep_mode_allowed() const;

                /**
                 * @brief Enables or disables sleep mode in RAM.
                 * 
                 * This method sets the flag in RAM indicating whether the system is permitted 
                 * to enter sleep mode.
                 * 
                 * @param[in] allowed `bool` Represents whether sleep mode is allowed.
                 * @arg @p true Allows the system to enter sleep mode.
                 * @arg @p false Prevents the system from entering sleep mode.
                 */
                void allow_sleep_mode(const bool allowed);

                /**
                 * @brief Checks if the MCU can be woken up by the IO3 pin.
                 * 
                 * This method retrieves the flag stored in RAM indicating whether the system is configured 
                 * to wake up the MCU using the IO3 pin.
                 * 
                 * @return bool Represents whether the MCU can be woken up by the IO3 pin.
                 * @retval true The MCU can be woken up by the IO3 pin.
                 * @retval false The MCU cannot be woken up by the IO3 pin.
                 */
                bool is_sleep_mode_io3_wakeup_allowed() const;

                /**
                 * @brief Enables or disables IO3 pin wakeup in RAM.
                 * 
                 * This method sets the flag in RAM indicating whether the system is configured 
                 * to wake up the MCU using the IO3 pin.
                 * 
                 * @param[in] allowed `bool` Represents whether the MCU can be woken up by the IO3 pin.
                 * @arg @p true Allows the MCU to be woken up by the IO3 pin.
                 * @arg @p false Prevents the MCU from being woken up by the IO3 pin.
                 */
                void allow_sleep_mode_io3_wakeup(const bool allowed);

                /**
                 * @brief Checks if boot count incrementing is allowed.
                 * 
                 * This method retrieves the flag stored in RAM indicating whether the system is permitted 
                 * to increment the boot count upon each boot.
                 * 
                 * @return bool 
                 * @retval true Boot count incrementing is allowed.
                 * @retval false Boot count incrementing is not allowed.
                 */
                bool is_boot_count_increment_allowed() const;

                /**
                 * @brief Enables or disables boot count incrementing in RAM.
                 * 
                 * This method sets the flag in RAM indicating whether the system is permitted 
                 * to increment the boot count upon each boot.
                 * 
                 * @param[in] allowed `bool` Represents whether boot count incrementing is allowed.
                 * @arg @p true Allows boot count incrementing.
                 * @arg @p false Prevents boot count incrementing.
                 */
                void allow_boot_count_increment(const bool allowed);

                /**
                 * @brief Retrieves the board's manufacture year stored in RAM.
                 * 
                 * This method provides the year when the %UIRB hardware was manufactured, 
                 * calculated from the year offset stored in the @ref EEPROMData structure.
                 * 
                 * @return `uint16_t` The year of manufacture in the range `[2020-2035]`.
                 * 
                 * @note The year is derived from an offset of `[0-15]` added to 2020, as stored in EEPROM.
                 * 
                 * @see @ref HardwareManufactureDate for the `union` representing the manufacturing date.
                 */
                uint16_t get_board_manufacture_year() const;

                /**
                 * @brief Updates the board's manufacture year in RAM.
                 * 
                 * This method sets the manufacture year in the @ref EEPROMData structure stored in RAM. 
                 * The year is stored as an offset from 2020, and this method ensures the provided 
                 * year is within the valid range.
                 * 
                 * @param[in] year The year of manufacture, which must be in the range `[2020-2035]`.
                 * @return bool Indicates whether the year was successfully updated.
                 * @retval true The year was successfully set.
                 * @retval false The provided year is outside the valid range `[2020-2035]`.
                 * 
                 * @note The year is stored as an offset from 2020 to minimize memory usage.
                 * 
                 * @warning Ensure that the input year is within the valid range to avoid errors.
                 * 
                 * @see @ref HardwareManufactureDate for the `union` representing the manufacturing date.
                 */
                bool set_board_manufacture_year(const uint16_t year);

                /**
                 * @brief Retrieves the board's manufacture %month stored in RAM.
                 * 
                 * This method provides the %month when the %UIRB hardware was manufactured, 
                 * as stored in the @ref EEPROMData structure.
                 * 
                 * @return `uint8_t` The %month of manufacture in the range `[1-12]`.
                 * @retval #INVALID_MANUFACTURE_MONTH If the stored %month is outside the valid range.
                 * 
                 * @note Months are represented as integers `[1-12]`, where `1` corresponds to January and `12` to December.
                 */
                uint8_t get_board_manufacture_month() const;

                /**
                 * @brief Updates the board's manufacture %month in RAM.
                 * 
                 * This method sets the manufacture %month in the @ref EEPROMData structure stored in RAM. 
                 * It ensures the provided @p month is within the valid range.
                 * 
                 * @param[in] month The %month of manufacture, which must be in the range `[1-12]`.
                 * @return bool Indicates whether the %month was successfully updated.
                 * @retval true The %month was successfully set.
                 * @retval false The provided @p month is outside the valid range `[1-12]`.
                 * 
                 * @note Months are validated to ensure they represent a valid calendar %month.
                 * 
                 * @warning Ensure that the input @p month is within the valid range to avoid errors.
                 */
                bool set_board_manufacture_month(const uint8_t month);

                /**
                 * @brief Retrieves the boot count stored in RAM.
                 * 
                 * This method returns the total number of times the %UIRB system has booted, as stored in 
                 * the @ref EEPROMData struct. The boot count is a running tally maintained in RAM.
                 * 
                 * @return `uint32_t` The total boot count.
                 * 
                 * @note The boot count is updated only if incrementing is explicitly allowed and the 
                 *       `increment_boot_count()` method is called.
                 */
                uint32_t get_boot_count() const;

                /**
                 * @brief Updates the boot count stored in RAM.
                 * 
                 * This method sets a new boot count value in the @ref EEPROMData struct stored in RAM.
                 * It does not directly affect the EEPROM until explicitly saved.
                 * 
                 * @param[in] boot_count The new boot count value to store in RAM.
                 * 
                 * @note Ensure the provided @p boot_count value is meaningful and does not exceed 
                 *       the maximum allowable range for a `uint32_t`.
                 */
                void set_boot_count(const uint32_t boot_count);

                /**
                 * @brief Increments the boot count stored in RAM by 1.
                 * 
                 * This method increases the boot count in the @ref EEPROMData struct by 1, provided that 
                 * the incrementing operation is allowed and the boot count has not reached its maximum value.
                 * 
                 * @return bool Indicates whether the boot count was successfully incremented.
                 * @retval true The boot count was incremented by 1.
                 * @retval false The boot count could not be incremented, either because it has reached 
                 *                 the maximum value (`UINT32_MAX`) or incrementing is disabled.
                 * 
                 * @note The boot count is stored as a `uint32_t`, which has a maximum value of `UINT32_MAX`.
                 *       If the boot count exceeds this value, further increments are not possible.
                 * 
                 * @warning Ensure that incrementing is enabled before calling this method or the boot count 
                 *          will not be incremented.
                 */
                bool increment_boot_count();

                /**
                 * @brief Retrieves the %UIRB board serial number stored in RAM.
                 * 
                 * This method returns the serial number of the %UIRB board stored in the @ref eeprom_core_data_ member.
                 * The serial number is validated to ensure it falls within the valid range `[0 - ` @ref UIRB_SERIAL_NUMBER_MAX `]`. 
                 * If the serial number is invalid (e.g., reserved debug bit is set or the value exceeds 
                 * @ref UIRB_SERIAL_NUMBER_MAX), the method returns the constant @ref INVALID_UIRB_SERIAL_NUMBER.
                 * 
                 * @return uint16_t The %UIRB board serial number.
                 * @retval #INVALID_UIRB_SERIAL_NUMBER If the serial number is invalid or out of range.
                 * 
                 * @details
                 * - The method checks the @ref reserved_bit_1 field of the serial number. If this bit is set, the serial number 
                 *   is treated as invalid and @ref INVALID_UIRB_SERIAL_NUMBER is returned.
                 * - The method also verifies that the serial number is within the valid range `[0 - ` @ref UIRB_SERIAL_NUMBER_MAX `]`.
                 * - If both conditions are satisfied, the method returns the valid serial number stored in 
                 *   @ref eeprom_core_data_.
                 * 
                 * @note This method ensures robustness by handling scenarios where the serial number may be corrupted 
                 *       or marked invalid during debugging.
                 * 
                 * @warning If the serial number is invalid, it is up to the caller to handle the returned 
                 *          @ref INVALID_UIRB_SERIAL_NUMBER appropriately.
                 * 
                 * @see @ref UIRB_SERIAL_NUMBER_MAX for the maximum allowable serial number value.
                 * @see @ref INVALID_UIRB_SERIAL_NUMBER for the constant returned in case of invalid serial numbers.
                 */
                uint16_t get_uirb_board_serial_number() const;

                /**
                 * @brief Updates the %UIRB board serial number in the @ref EEPROMData struct stored in RAM.
                 * 
                 * This method validates and updates the serial number in the @ref eeprom_core_data_ member if it falls 
                 * within the valid range `[1 - ` @ref UIRB_SERIAL_NUMBER_MAX `]`. Serial numbers outside this range are rejected, and 
                 * the method returns `false`.
                 * 
                 * @param[in] serial_number The %UIRB board serial number to set. Must be in the range `[1 - ` @ref UIRB_SERIAL_NUMBER_MAX `]`.
                 * @return bool Indicates whether the serial number was successfully updated.
                 * @retval true The serial number was validated and successfully updated.
                 * @retval false The provided serial number is outside the valid range.
                 * 
                 * @note The serial number is expected to be a positive integer within the specified range. 
                 *       A value of `0` or a number exceeding @ref UIRB_SERIAL_NUMBER_MAX will result in failure.
                 * 
                 * @see @ref get_uirb_board_serial_number() for retrieving the stored serial %number.
                 */
                bool set_uirb_board_serial_number(const uint16_t serial_number);

                /**
                 * @brief Updates the factory CP2104 USB serial number in the @ref EEPROMData struct stored in RAM.
                 * 
                 * Copies the provided string into the @ref EEPROMData::factory_cp2104_usb_serial_number field of the 
                 * @ref eeprom_core_data_ member. The input string must be exactly 8 characters long, excluding 
                 * the null terminator.
                 * 
                 * @param[in] cstr Pointer to a null-terminated string to copy into the @ref eeprom_core_data_ member.
                 * @return bool Indicates whether the operation was successful.
                 * @retval true The input string is exactly 8 characters long and successfully copied.
                 * @retval false The input string is not exactly 8 characters long.
                 * 
                 * @note The input string must not include a null terminator within the first 8 characters, as 
                 *       the serial number is stored as a fixed-length field without a null terminator.
                 */
                bool set_factory_cp2104_usb_serial_number_cstr(const char* cstr);

                /**
                 * @brief Retrieves the factory CP2104 USB serial number from the @ref EEPROMData struct stored in RAM.
                 * 
                 * Copies the factory CP2104 USB serial number into the provided buffer and appends a null terminator. 
                 * The buffer must be large enough to hold 9 characters (8 for the serial number and 1 for the null terminator).
                 * 
                 * @param[out] buffer Pointer to the buffer where the serial number will be copied.
                 * @param[in] buffer_len Size of the provided buffer.
                 * @return char* Pointer to the start of the buffer containing the null-terminated serial number.
                 * @retval nullptr The provided buffer is too small to hold the serial number and null terminator.
                 * 
                 * @note Ensure the buffer is at least 9 bytes in size to prevent truncation or buffer overflow.
                 * @warning If the buffer is too small, no data is copied, and the function returns `nullptr`.
                 */
                char* get_factory_cp2104_usb_serial_number_cstr(char* buffer, const size_t buffer_len) const;

                /**
                 * @brief Retrieves the @ref EEPROMData struct stored in RAM and copies it into the provided reference.
                 * 
                 * This method copies the current in-memory representation of the @ref EEPROMData struct 
                 * from the @ref eeprom_core_data_ member to the provided reference.
                 * 
                 * @param[out] data Reference to an @ref EEPROMData struct where the retrieved data will be stored.
                 * 
                 * @note This method does not modify the internal state of the @ref EEPROMDataManager class.
                 * 
                 * @see @ref get() for retrieving the data as a returned instance.
                 */
                void get(EEPROMData& data) const;

                /**
                 * @brief Retrieves the @ref EEPROMData struct stored in RAM as a new instance.
                 * 
                 * This method returns a copy of the current in-memory representation of the 
                 * @ref EEPROMData struct stored in the @ref eeprom_core_data_ member.
                 * 
                 * @return An instance of the @ref EEPROMData struct containing the current data in RAM.
                 * 
                 * @note Use this method for scenarios where a new instance of the data is preferred 
                 *       over modifying an existing reference.
                 * 
                 * @see @ref get(EEPROMData & data) const for copying data into a provided reference.
                 */
                EEPROMData get() const;

                /**
                 * @brief Updates the @ref EEPROMData struct stored in RAM with the provided data.
                 * 
                 * This method copies the content of the provided @ref EEPROMData struct into 
                 * the in-memory representation stored in the @ref eeprom_core_data_ member.
                 * 
                 * @param[in] data Reference to an @ref EEPROMData struct containing the data to copy.
                 * 
                 * @note Changes made using this method affect only the in-memory data. To persist 
                 *       the changes to EEPROM, call the @ref save_to_eeprom() method.
                 */
                void set(const EEPROMData& data);

                /**
                 * @brief Updates the @ref EEPROMData struct stored in RAM using data from another @ref EEPROMDataManager.
                 * 
                 * This method copies the in-memory @ref EEPROMData struct from another @ref EEPROMDataManager 
                 * instance into the @ref eeprom_core_data_ member of the current object.
                 * 
                 * @param[in] dataManager Reference to an @ref EEPROMDataManager object to copy data from.
                 * 
                 * @note Changes made using this method affect only the in-memory data of the current object. 
                 *       To persist the changes to EEPROM, call the @ref save_to_eeprom() method.
                 */
                void set(const EEPROMDataManager& dataManager);

                /**
                 * @brief Loads the @ref EEPROMData struct from EEPROM into RAM.
                 * 
                 * This method reads the data stored in EEPROM at the location defined 
                 * by @ref CORE_DATA_ADDR_START and updates the in-memory representation 
                 * in the @ref eeprom_core_data_ structure.
                 * 
                 * @note This function overwrites the current in-memory data with the 
                 *       contents of the EEPROM.
                 * 
                 * @see @ref read_from_eeprom(EEPROMData&) for the underlying read implementation.
                 */
                void load_from_eeprom();

                /**
                 * @brief Reads the @ref EEPROMData structure from EEPROM or RAM (in debug mode) into the provided reference.
                 * 
                 * This static method retrieves the @ref EEPROMData structure either from EEPROM or from the RAM-based 
                 * @ref EEPROM_DATA (if @ref UIRB_EEPROM_BYPASS_DEBUG is defined) and copies it into the specified reference.
                 * In normal operation, the data is read from EEPROM starting at the address specified by @ref CORE_DATA_ADDR_START. 
                 * In debug mode, the data is directly retrieved from RAM.
                 * 
                 * @param[out] data Reference to the @ref EEPROMData structure where the retrieved data will be stored.
                 * 
                 * @details
                 * - When @ref UIRB_EEPROM_BYPASS_DEBUG is defined, data is read from RAM via the global @ref EEPROM_DATA 
                 *   instead of EEPROM.
                 * - In normal operation (EEPROM storage), data is retrieved from the EEPROM starting at the 
                 *   address defined by @ref CORE_DATA_ADDR_START.
                 * - This method does not modify any internal state of the @ref EEPROMDataManager class. It is a low-level 
                 *   utility function for retrieving stored data.
                 * 
                 * @note
                 * - In debug mode, data stored in RAM is volatile and will not persist between reboots or power cycles.
                 * - Ensure the structure and alignment of the @ref EEPROMData match the memory layout stored in EEPROM 
                 *   to avoid data corruption or invalid results.
                 * 
                 * @warning
                 * - Improper use of this function with mismatched structures may lead to undefined behavior or data corruption.
                 * - When using debug mode (@ref UIRB_EEPROM_BYPASS_DEBUG), ensure that the system is configured correctly 
                 *   and that GPIO pins are adequately protected.
                 * 
                 * @see @ref read_from_eeprom() for an alternative method returning a new instance.
                 * @see @ref CORE_DATA_ADDR_START for the starting EEPROM address.
                 * @see @ref UIRB_EEPROM_BYPASS_DEBUG for debug mode behavior.
                 */
                static void read_from_eeprom(EEPROMData& data);

                /**
                 * @brief Reads the @ref EEPROMData structure from EEPROM or RAM (in debug mode) and returns a new instance.
                 * 
                 * This static method retrieves the @ref EEPROMData structure from either EEPROM or RAM, depending on whether 
                 * @ref UIRB_EEPROM_BYPASS_DEBUG is defined, and returns it as a new instance. In normal operation, the data is 
                 * read from EEPROM starting at the address specified by @ref CORE_DATA_ADDR_START. In debug mode, the data is 
                 * retrieved directly from the RAM-based @ref EEPROM_DATA.
                 * 
                 * @return An instance of the @ref EEPROMData structure containing the retrieved data.
                 * 
                 * @details
                 * - When @ref UIRB_EEPROM_BYPASS_DEBUG is defined, the data is read from RAM using the global 
                 *   @ref EEPROM_DATA instead of EEPROM.
                 * - In normal operation (EEPROM storage), the data is read from the EEPROM starting at 
                 *   @ref CORE_DATA_ADDR_START.
                 * - This method provides a convenience wrapper for scenarios where a direct return of a new 
                 *   @ref EEPROMData instance is preferred over passing a reference.
                 * 
                 * @note
                 * - In debug mode, data stored in RAM is volatile and will not persist between reboots or power cycles.
                 * - Ensure the structure and alignment of @ref EEPROMData match the memory layout stored in EEPROM 
                 *   to avoid data corruption or invalid results.
                 * - This method internally calls @ref read_from_eeprom(EEPROMData&) to retrieve the data.
                 * 
                 * @warning
                 * - Improper use of this method with mismatched structures may lead to undefined behavior or data corruption.
                 * - When using debug mode (@ref UIRB_EEPROM_BYPASS_DEBUG), ensure that the system is configured correctly 
                 *   and that GPIO pins are adequately protected.
                 * 
                 * @see @ref read_from_eeprom(EEPROMData&) for a direct version using a reference.
                 * @see @ref CORE_DATA_ADDR_START for the starting EEPROM address.
                 * @see @ref UIRB_EEPROM_BYPASS_DEBUG for debug mode behavior.
                 */
                static EEPROMData read_from_eeprom();

                /**
                 * @brief Represents an invalid resistance value for the %UIRB charger's Rprog resistor.
                 * 
                 * This constant is used to indicate an error or uninitialized state for the Rprog resistor 
                 * resistance, particularly in cases where a valid resistance value is unavailable or below 
                 * the operational threshold.
                 * 
                 * @details
                 * - The value is set to `1` ohm, which is considered invalid for normal operation.
                 * - This constant ensures consistent error handling and validation in the firmware.
                 * 
                 * @note Ensure that the actual Rprog resistor value is greater than @ref CHARGER_PROG_RESISTANCE_MIN 
                 *       to maintain proper operation.
                 */
                static constexpr uint8_t INVALID_CHARGER_PROG_RESISTANCE = 1U;

                /**
                 * @brief Represents an invalid manufacturing month for the %UIRB hardware.
                 * 
                 * This constant is used to indicate an error or uninitialized state for the manufacturing month, 
                 * ensuring that the month value is validated before use in calculations or reporting.
                 * 
                 * @details
                 * - The value is set to `0`, which is outside the valid range `[1-12]`.
                 * - This constant aids in error detection and prevents undefined behavior due to invalid month values.
                 * 
                 * @see @ref EEPROMDataManager::get_board_manufacture_month() for retrieving the manufacturing month.
                 */
                static constexpr uint8_t INVALID_MANUFACTURE_MONTH = 0U;

                /**
                 * @brief Represents an invalid serial number for the %UIRB hardware.
                 * 
                 * This constant is used to signify an error or uninitialized state for the %UIRB serial number. 
                 * It helps identify cases where the serial number is either missing or exceeds the valid range.
                 * 
                 * @details
                 * - The value is set to `UINT16_MAX`, the maximum possible value for a `uint16_t`.
                 * - Serial numbers are considered valid if they fall within the range `[1, UIRB_SERIAL_NUMBER_MAX]`.
                 * 
                 * @see @ref EEPROMDataManager::get_uirb_board_serial_number() for retrieving the board's serial number.
                 * @see @ref UIRB_SERIAL_NUMBER_MAX for the maximum valid serial number.
                 */
                static constexpr uint16_t INVALID_UIRB_SERIAL_NUMBER = UINT16_MAX;
            private:
                /**
                 * @brief Internal instance of the @ref EEPROMData structure.
                 * 
                 * This member variable holds the core configuration and metadata for the %UIRB board in RAM. 
                 * It acts as the primary in-memory representation of the EEPROM data, allowing easy manipulation 
                 * and temporary storage before committing changes to EEPROM.
                 * 
                 * @details
                 * - The structure is initialized with default values or values read from EEPROM during 
                 *   the initialization of the @ref EEPROMDataManager class.
                 * - Modifications to this structure do not directly affect the EEPROM until explicitly 
                 *   written back using the @ref save_to_eeprom() method.
                 * 
                 * @note This structure is packed and aligned for compatibility with EEPROM storage.
                 */
                EEPROMData eeprom_core_data_ = { 0 };

                /**
                 * @brief Saves the @ref eeprom_core_data_ structure to EEPROM.
                 * 
                 * This function writes the current state of the @ref eeprom_core_data_ member variable 
                 * to the EEPROM starting at the address defined by @ref CORE_DATA_ADDR_START. 
                 * After writing, it verifies that the written data matches the in-memory data.
                 * 
                 * @return bool Indicates whether the data was successfully saved to EEPROM.
                 * @retval true The data stored in EEPROM matches the in-memory @ref eeprom_core_data_ structure.
                 * @retval false The data in EEPROM does not match the in-memory structure, or the operation failed.
                 * 
                 * @details
                 * - This method ensures data integrity by comparing the stored data against the written data.
                 * 
                 * @see @ref CORE_DATA_ADDR_START for the starting EEPROM address of the stored data.
                 * @see @ref EEPROMDataManager::store_to_eeprom(const EEPROMData&) for direct data saving.
                 */
                bool save_to_eeprom() const;

                /**
                 * @brief Writes a specified @ref EEPROMData structure to EEPROM or RAM (in debug mode).
                 * 
                 * This static method writes the given @ref EEPROMData structure to either EEPROM or RAM, depending 
                 * on whether @ref UIRB_EEPROM_BYPASS_DEBUG is defined. In normal operation, the data is written to EEPROM 
                 * starting at the address defined by @ref CORE_DATA_ADDR_START. In debug mode, the data is stored in the 
                 * RAM-based @ref EEPROM_DATA and does not persist after a reboot or power cycle.
                 * 
                 * @param[in] data The @ref EEPROMData structure to be stored.
                 * @return bool Indicates whether the specified data was successfully stored and verified.
                 * @retval true The written data matches the provided @ref EEPROMData structure.
                 * @retval false The stored data does not match the provided structure, or the operation failed.
                 * 
                 * @details
                 * - When @ref UIRB_EEPROM_BYPASS_DEBUG is defined, the data is written to RAM via the global @ref EEPROM_DATA 
                 *   instead of EEPROM. This is useful for debugging scenarios with simulators like `simavr`.
                 * - In normal operation, the data is written to EEPROM starting at @ref CORE_DATA_ADDR_START, and its 
                 *   integrity is verified by reading the data back and comparing it to the in-memory representation.
                 * - This method does not modify any member variables of the @ref EEPROMDataManager class.
                 * 
                 * @note
                 * - In debug mode, data stored in RAM is volatile and will not persist between reboots or power cycles.
                 * - Ensure that the structure and alignment of the @ref EEPROMData match the expected memory layout in EEPROM 
                 *   to prevent data corruption or invalid results.
                 * 
                 * @warning
                 * - Improper use of this method with mismatched or invalid @ref EEPROMData structures may lead to undefined 
                 *   behavior or data corruption.
                 * - When using debug mode (@ref UIRB_EEPROM_BYPASS_DEBUG), ensure proper configuration and GPIO protection.
                 * 
                 * @see @ref CORE_DATA_ADDR_START for the starting EEPROM address.
                 * @see @ref read_from_eeprom() for verifying stored data.
                 * @see @ref UIRB_EEPROM_BYPASS_DEBUG for debugging behavior and limitations.
                 */
                static bool store_to_eeprom(const EEPROMData& data);

                /**
                 * @brief Starting address for %UIRB core data in EEPROM.
                 * 
                 * This constant defines the beginning address in the EEPROM where the %UIRB core data 
                 * is stored. It is derived from the globally defined macro @ref UIRB_EEPROM_DATA_ADDR_START.
                 * 
                 * @details
                 * - The starting address is used as a reference for reading and writing core data to the EEPROM.
                 * - This address is typically aligned with the memory layout of the EEPROM to ensure proper storage.
                 * 
                 * @note Ensure that no other data overlaps with this address range in the EEPROM.
                 * 
                 * @see @ref EEPROMDataManager::DATA_ADDR_END for the end address of the %UIRB core data.
                 */
                static constexpr uint16_t CORE_DATA_ADDR_START = UIRB_EEPROM_DATA_ADDR_START;

                /**
                 * @brief Ending address for %UIRB core data in EEPROM.
                 * 
                 * This constant defines the last address (exclusive) in the EEPROM where the %UIRB core data 
                 * is stored. It is calculated as @ref CORE_DATA_ADDR_START + sizeof(EEPROMData), accounting 
                 * for the size of the @ref EEPROMData structure.
                 * 
                 * @details
                 * - The end address helps determine the range of memory occupied by %UIRB core data.
                 * - This value is critical for ensuring no data corruption occurs due to overlapping memory regions.
                 * 
                 * @note The address range `[CORE_DATA_ADDR_START, DATA_ADDR_END)` represents the allocated 
                 *       region for %UIRB core data in the EEPROM.
                 * 
                 * @see @ref EEPROMDataManager::CORE_DATA_ADDR_START for the starting address of the UIRB core data.
                 */
                static constexpr uint16_t DATA_ADDR_END = CORE_DATA_ADDR_START + sizeof(EEPROMData);

                /**
                 * @brief Maximum valid serial number for the %UIRB board.
                 * 
                 * This constant defines the upper limit for valid serial numbers assigned to %UIRB boards.
                 * Serial numbers exceeding this value are considered invalid.
                 * 
                 * @details
                 * - The maximum allowable value is `9999`.
                 * 
                 * @note Ensure that serial numbers are assigned within the valid range `[1, 9999]` to avoid 
                 *       errors or undefined behavior in the firmware.
                 */
                static constexpr uint16_t UIRB_SERIAL_NUMBER_MAX = 9999U;

                /**
                 * @brief Minimum valid resistance of the `Rprog` resistor in ohms.
                 * 
                 * This constant defines the lower threshold for the resistance of the `Rprog` resistor 
                 * connected to the charger's PROG pin. Resistance values below this threshold require 
                 * a different formula for calculating the charging current.
                 * 
                 * @details
                 * - Resistance values below `3333` ohms may lead to inaccurate current calculations 
                 *   or fall outside the recommended operating range of the hardware.
                 * - The threshold is set to ensure compatibility with the standard charging current 
                 *   calculation formula used by the firmware.
                 * 
                 * @note Ensure that the `Rprog` resistor's actual resistance matches or exceeds 
                 *       this value to guarantee proper operation and reliable current measurements.
                 */
                static constexpr uint16_t CHARGER_PROG_RESISTANCE_MIN = 3333U;

                /**
                 * @brief Grants access to all private and protected members of this class to @ref UIRB.
                 * 
                 * The @ref UIRB class is declared as a friend to allow it unrestricted access to the internal 
                 * implementation of the @ref EEPROMDataManager class.
                 * 
                 * @see UIRB for details about the UIRB core class.
                 */
                friend class uirbcore::UIRB;
        };

    #if defined(UIRB_EEPROM_BYPASS_DEBUG) || defined(__DOXYGEN__)
        /**
         * @brief Default debug EEPROM data used when @ref UIRB_EEPROM_BYPASS_DEBUG is enabled.
         * 
         * This static constant defines the default values for the @ref EEPROMData structure when the 
         * @ref UIRB_EEPROM_BYPASS_DEBUG macro is defined. Data operations are redirected to RAM 
         * instead of EEPROM, and this structure serves as the initial state of the emulated EEPROM data.
         * 
         * @details
         * - Provides predefined values for all fields in the @ref EEPROMData structure, ensuring that 
         *   all necessary fields are initialized correctly for debugging scenarios.
         * - If @ref UIRB_EEPROM_RPROG_DEBUG is defined, the `charger_prog_resistor_ohms` field is set 
         *   to its value. Otherwise, it is initialized to @ref EEPROMDataManager::INVALID_CHARGER_PROG_RESISTANCE.
         * - The `factory_cp2104_usb_serial_number` field is set to "EEDBG=1", indicating that the 
         *   EEPROM bypass mode is active.
         * 
         * @note
         * - This structure is used only in debug mode and does not persist between reboots or power cycles.
         * - Ensure appropriate values for debugging scenarios to prevent undefined behavior.
         * 
         * @warning
         * - Do not use this structure in production environments.
         * - GPIO pins may behave differently when debugging mode is enabled; ensure hardware safety.
         * 
         * @see @ref UIRB_EEPROM_BYPASS_DEBUG for more details about EEPROM bypass mode.
         * @see @ref UIRB_EEPROM_RPROG_DEBUG for debugging the charger programming resistor value.
         * @see @ref UIRB_HW_VER for the default hardware version.
         */
        static constexpr EEPROMData DEBUG_EEPROM_DATA = 
        {
            .hardware_version = uirbcore::eeprom::UIRB_HW_VER, /**< @brief Default hardware version set to 0.0. */
            .bandgap_1v1_reference_offset = 0, /**< @brief No offset for 1.1V bandgap reference voltage. */
            .stat_led_brightness = 0, /**< @brief LED brightness set to 0 (safe default for debugging). */
        #if defined(UIRB_EEPROM_RPROG_DEBUG)
            .charger_prog_resistor_ohms = UIRB_EEPROM_RPROG_DEBUG, /**< @brief Debug value for Rprog resistor set via @ref UIRB_EEPROM_RPROG_DEBUG. */
        #else
            .charger_prog_resistor_ohms = uirbcore::eeprom::EEPROMDataManager::INVALID_CHARGER_PROG_RESISTANCE, /**< @brief Invalid Rprog resistance. */
        #endif
            .software_config = { .config_byte = 0 }, /**< @brief All software configuration options disabled. */
            .hardware_manufacture_date = { .month_year_byte = 0xFFU }, /**< @brief Invalid manufacture date (year 2035, month 15). */
            .boot_count = UINT32_MAX, /**< @brief Maximum boot count to indicate invalid data. */
            .uirb_serial_number = { .serial_number_u16 = uirbcore::eeprom::EEPROMDataManager::INVALID_UIRB_SERIAL_NUMBER }, /**< @brief Invalid serial number. */
            .factory_cp2104_usb_serial_number = { 'E', 'E', 'P', 'D', 'B', 'G', '=', '1' } /**< @brief Indicates EEPROM bypass mode is active. */
        };
    #endif
    }  // namesapce eeprom
}  // namespace uirbcore
#endif  // UIRBcore_EEPROM_hpp