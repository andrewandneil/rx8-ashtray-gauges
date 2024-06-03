# The Firmware

Open this folder using PlatformIO in Visual Studio Code (or VSCodium). Ensure you have the Adafruit GFX Library and Adafruit SSD1306 libraries installed (these should install automatically from the platformio.ini file on first opening).

IMPORTANT: To ensure proper temperature, pressure and battery voltage calculations, you need to measure the exact resistance of `ADD RESISTOR NUMBERS HERE` and add these values to the marked sections in the `coolant_monitor.h` file.

For example, if you measured a resistance of 13.9K on the coolant high side resistor, change the line:
- `#define COOL_THERMISTOR_RESISTOR_REFERENCE_HIGH 15020.0` to `#define COOL_THERMISTOR_RESISTOR_REFERENCE_HIGH 13900.0`

You must add the `.0` at the end as the value must be reported as a float.


Build this and then upload it to the Teensy using the onboard micro-USB port.

(Better tutorial to follow :) )