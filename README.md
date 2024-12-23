# UIRBcore Library

UIRBcore is an Arduino library tailored for managing the Universal Infrared Blaster (UIRB) board. This library provides core functionalities like power management and system initialization, while relying on external IR libraries for signal transmission and reception.

**Note:** This library is a proof of concept and is not optimized for performance or low-power applications. It is intended as an example and should be avoided in performance-critical scenarios.

This library is designed with PlatformIO in mind and requires the **uirb-v02-atmega328p** custom board definition or the `-DUIRB_BOARD_V02` flag supplied during compilation.

## Features

- **Power Management**: Built-in support for monitoring battery voltage and charging states.
- **Fixed Pin Assignments**: Predefined pin assignments managed directly within the library.
- **EEPROM Management**: Efficient storage and retrieval of configuration data.
- **PlatformIO Support**: Optimized for PlatformIO projects, ensuring robust build and dependency management.

## Prerequisites

To use UIRBcore, ensure you have the following:

1. [PlatformIO](https://platformio.org/) installed.
2. The custom board definition for **uirb-v02-atmega328p** installed. You will need to provide a path to the definition during project setup. (A repository link with more information will be added here in the future.)

## Installation

### Using PlatformIO Library Manager

Add UIRBcore to your `platformio.ini` file using one of the following approaches:

```ini
lib_deps =
    DjordjeMandic/UIRBcorelib @ ^1.0.0  ; Depend on the latest 1.x stable version
```

> **Tip:** Learn more about semantic versioning at [devhints.io/semver](https://devhints.io/semver) and [semver.org](https://semver.org/).

```ini
lib_deps =
    https://github.com/DjordjeMandic/UIRBcorelib.git#dev  ; Depend on the development Git branch
```

> **Tip:** Discover more about specifying packages in PlatformIO by visiting the [official documentation on package specifications](https://docs.platformio.org/en/latest/core/userguide/pkg/cmd_install.html#package-specifications).

### Manually

1. Download the latest release from the [UIRBcore GitHub repository](https://github.com/DjordjeMandic/UIRBcorelib).
2. Place the library folder in your project’s `lib` directory.

## Usage

Here’s a quick example of how to initialize the UIRBcore library:

```cpp
#include <Arduino.h>
#include <UIRBcore.hpp>

using namespace uirbcore;

UIRB& uirb = UIRB::getInstance();

void setup()
{
    Serial.begin(1000000);

    if (!uirb.begin())
    {
        Serial.println(F("UIRBcore Initialization Failed!"));
        // Ensure uirb.begin() succeeds before proceeding to avoid hardware damage.
        while (1);
    }

    Serial.println("UIRBcore initialized successfully.");

    // Additional setup code here
}

void loop()
{
    // Main application logic
}
```

> **Important:** The ATMEGA328P EEPROM must store a valid hardware version for the UIRB constructor to initialize correctly. If the EEPROM does not contain valid data, the constructor will cause the system to hang and reboot.

> **Note:** For IR signal transmission and reception, use compatible external libraries.

## Examples

Explore the provided example sketches:

- [`Basic`](./examples/Basic): A basic implementation of the UIRBcore library.
- [`Calibration`](./examples/Calibration): Demonstrates how to calibrate and configure the system.

These examples are located within the [`examples`](./examples) folder of the library.

## Configuration

The library uses fixed pin assignments predefined for the UIRB V0.2 board. Refer to the source code for exact pin mappings.

## Doxygen Documentation and Scripts

This project uses Doxygen for documentation. Use the script [`doxygen_generate.ps1`](./scripts/doxygen_generate.ps1) to generate the documentation:

```powershell
pwsh ./scripts/doxygen_generate.ps1 Doxyfile
```

The script [`update_version.py`](./scripts/update_version.py) updates [`UIRBcore_Version.h`](./include/UIRBcore_Version.h) with the latest Git tag/commit information. To run the script, use the following command:

```bash
python ./scripts/update_version.py
```

> **Note:** All scripts should be executed from the root of the repository.

> **Limitation:** Simulation using simavr in PlatformIO is not supported because there is currently no method to pre-load EEPROM data before the simulation.

## Contribution

Contributions are welcome! If you have ideas for improvements or encounter issues, please open an issue or submit a pull request on the [GitHub repository](https://github.com/DjordjeMandic/UIRBcorelib).

## License

This project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for details.

## Acknowledgments

- **Author**: Djordje Mandic ([linktr.ee/djordjemandic](https://linktr.ee/djordjemandic))
- **Supported Hardware**: UIRB V0.2 board.

---

_For more information about the custom board definition and the hardware design, a dedicated repository link will be added here soon._
