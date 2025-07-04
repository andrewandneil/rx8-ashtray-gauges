/* 
 * This is the main header file for the RX-8 Ashtray Gauges project.
 * Original Author: Stephane Gilbert
 * Modified by: Andrew Wilson
 * BSD tree clause licence (SPDX: BSD-3-Clause)
*/

/* 
 * There are values in here you will need to change.
 * You will need to mesaure the resistance of resistors R3, R4, R8, R9, R10 and R11 and enter the values below.
*/

// Set this to zero if you'd prefer not to have the buzzer
// #define USE_BUZZER_ALERT 1

// Set this to zero if you don't want LEDs in a warning state
#define ENABLE_WARNING_LEDS 1

#define DEBUG_VALUES 0

// These values you can change to control when the warning triangle and warning LED is displayed to indicate a fault.
// Temperatures:
// The coolant temperature warning threshold, in Celsius
#define COOLANT_TEMP_WARNING_CELSIUS 110
// The oil temperature warning threshold, in Celsius
#define OIL_TEMP_WARNING_CELSIUS 120
// Pressures:
// The coolant pressure warning threshold, in PSI
#define COOLANT_PSI_WARNING 15
// The oil pressure warning threshold, in PSI
#define OIL_PSI_WARNING_LOW 13
#define OIL_PSI_WARNING_HIGH 150
// Voltage:
// Display a warning sign when the battery voltage is below or above these values
#define BATTERY_VOLTAGE_LOW_WARNING 11.5
#define BATTERY_VOLTAGE_HIGH_WARNING 15.0

// The speed (in Hz) at which the displays refresh the displayed values
#define DISPLAY_REFRESH_RATE_HZ 5 //4

// Position of the displayed value relative to the display half. Do not change this.
#define TEXT_POS_X 30
#define TEXT_POS_Y 4

// Distance from top to second half of display in pixels. Do not change this.
#define DISPLAY_HALF_TWO 32

// Values to tell functions which half of the screen we're printing on. Do not change this.
#define TOP_HALF 1
#define BOTTOM_HALF 0

/*
 * Teensy Pin Definitions
 * Correct as of PCB rev 2.2
 */
// Digital pins definitions
#define TEMPERATURE_UNIT_SELECTOR_INPUT_PIN 2 // C or F Jumper
#define OIL_THERMISTOR_REFERENCE_SELECT_OUTPUT_PIN 3
#define COOLANT_THERMISTOR_REFERENCE_SELECT_OUTPUT_PIN 4
#define HALL_EFFECT_SENSOR_INPUT_PIN 5
#define ALERT_BUZZER_OUTPUT_PIN 6
#define WARNING_LED_OUTPUT_PIN 7
#define SPARE_2_OUPUT_PIN 8
#define PRESSURE_UNIT_SELECTOR_INPUT_PIN 10
#define SCL1_PIN 16
#define SDA1_PIN 17
#define SDA0_PIN 18
#define SCL0_PIN 19

// Analogue pin definition
#define OIL_ANALOG_INPUT_PIN A0             // Digital out 14
#define COOLANT_ANALOG_INPUT_PIN A1         // Digital out 15
// A2, A3, A4, A5 are SCL & SDA pins
#define OIL_PSI_ANALOG_INPUT_PIN A6         // Digital out 20
#define ILLUMINATION_ANALOG_INPUT_PIN A7    // Digital out 21
#define VOLTAGE_ANALOG_INPUT_PIN A8         // Digital out 22
#define SPARE_1_INPUT_PIN A9                // Digital out 23

// Digital pins definitions
// If it's commentend out, it's in use above
#define UNUSED_PIN_0 0          // RX1
#define UNUSED_PIN_1 1          // TX1
//#define UNUSED_PIN_2 2
//#define UNUSED_PIN_3 3
//#define UNUSED_PIN_4 4
//#define UNUSED_PIN_5 5    
//#define UNUSED_PIN_6 6
//#define UNUSED_PIN_7 7      
//#define UNUSED_PIN_8 8   
#define UNUSED_PIN_9 9         
//#define UNUSED_PIN_10 10  
#define UNUSED_PIN_11 11        // MOSI
#define UNUSED_PIN_12 12        // MISO
//#define UNUSED_PIN_13 13      // LED
//#define UNUSED_PIN_14 14      // Analogue A0
//#define UNUSED_PIN_15 15      // Analogue A1
//#define UNUSED_PIN_16 16      // Analogue A2 / SCL1
//#define UNUSED_PIN_17 17      // Analogue A3 / SDA1
//#define UNUSED_PIN_18 18      // Analogue A4 / SDA0
//#define UNUSED_PIN_19 19      // Analogue A5 / SCL0
//#define UNUSED_PIN_20 20      // Analogue A6
//#define UNUSED_PIN_21 21      // Analogue A7
//#define UNUSED_PIN_22 22      // Analogue A8
//#define UNUSED_PIN_23 23      // Analogue A9

// We don't use pins 24-39 as they're not required and dificult to access.



/* Thermistor value and treshold
 * The Delphi 12160855 or AEM 30-2012 sensor, like all thermistors don't have a linear curve
 * So to have a good precision on the full temperature scale, we use two different resistor values
 * to calculate the temperature. One for the low end to mid, and one for the high end.
 * Since we mesure engine coolant temperature, the high end reading must be accurate.
 * The reference resistors and the threshold to switch between them are determined by the value below.
 * A hysteresis is used to avoid changing the reference too often.
 */
// Values for coolant thermistor's resistors (R11 - High, R10 - Low)
// For the best results, measure the actual value on your specific board and use high precision %1 resistor or better.
// Should be between 14800.0 - 15200.0
// Important: Enter the value as float
#define COOL_THERMISTOR_RESISTOR_REFERENCE_HIGH 14960.0
// This is the PARALLEL VALUE of both the above reference resistor and a second one
// !!IMPORTANT!! Measure the resistance of R11 and R10 and put the two values into this calulator:
// https://www.allaboutcircuits.com/tools/parallel-resistance-calculator/
// Then paste the equivalent resistance below.
// Should be between 890.0 - 990.0
// Important: Enter the value as float
#define COOL_THERMISTOR_RESISTOR_REFERENCE_LOW 935.59

// Values for oil thermistor's resistors (R9 - High, R8 - Low)
// For the best results, measure the actual value on your specific board and use high precision %1 resistor or better.
// Should be between 14800.0 - 15200.0
// Important: Enter the value as float
#define OIL_THERMISTOR_RESISTOR_REFERENCE_HIGH 14960.0 //15410.0
// This is the PARALLEL VALUE of both the above reference resistor and a second one
// !!IMPORTANT!! Measure the resistance of R9 and R8 and put the two values into this calulator:
// https://www.allaboutcircuits.com/tools/parallel-resistance-calculator/
// Then paste the equivalent resistance below.
// Should be between 890.0 - 990.0
// Important: Enter the value as float
#define OIL_THERMISTOR_RESISTOR_REFERENCE_LOW 935.59

// The threshold values to toggle between high resistor and low resistor
#define THERMISTOR_RESISTOR_REFERENCE_LOW_THRESHOLD 55
#define THERMISTOR_RESISTOR_REFERENCE_HIGH_THRESHOLD 50

// There is an onboard tension divider that allow the Teensy to read the supply voltage (~12V).
// The raw voltage is too high for the Teensy, so the voltage is divided with resistors.
// For the best results, measure the actual values on your specific board and use high precision %1 resistor or better.
// Important: Enter the value as float
// These values below allow a voltage range from 0 to 18V
#define VOLTAGE_DIVIDER_R1 6800.0 // R4
#define VOLTAGE_DIVIDER_R2 1500.0 // R3

// Adjust this in the range 0-255 to change screen brightness when car lights are turned on
// Should be greater than 0, otherwise displays are off
#define MINIMUM_BRIGHTNESS 2

// Buzzer Configuration
#define BUZZER_HZ 1500

// Type of sensor used for pressure. Do not change this.
// With the 10K and 20K resistor based voltage divider:
// AEM-30-2131-15G:
// Max: 4.8V becomes Max: 3.2V
// AEM-30-2131-100, AEM-30-2131-150, 200 PSI or 300 PSI:
// Max: 4.5V becomes Max: 3.0V
//#define PRESSURE_SENSOR_2131_15G 0
#define PRESSURE_SENSOR_2131_100 0
#define PRESSURE_SENSOR_2131_150 1
#define PRESSURE_SENSOR_200_PSI  2
#define PRESSURE_SENSOR_300_PSI  3

// According to this datasheet, PSI = (3.7529*(Voltage)) - 1.8765
// https://www.aemelectronics.com/files/instructions/30-2131-15G%20Sensor%20Data.pdf

// The following is for analogue read.
// Number of sample to read for each analogue acquisition
#define ANALOG_SAMPLES_COUNT 5
// The number of time to wait between analogue acquisitions.
#define ANALOG_DELAY_BETWEEN_ACQUISITIONS 5
// The highest tolerable voltage by the ADC
#define MAX_ANALOGUE_VOLTAGE 3.3

// ERROR Codes, DO NOT CHANGE
#define ENOERR 0
#define ERANGE 1
#define EDIVZERO 2
#define EINVALID 3

/* Custom icons definitions.
 * Made with Gimp and converted with image2cpp
 * https://javl.github.io/image2cpp
 * The original file are located in the 'bitmaps' folder
 * Credit for the following icons to Stephane Gilbert:
 *  coolant_icon_c, coolant_icon_f, degree_sign, fault_message, psi_sign, 
 *  rad_pressure_icon, rx8_logo, voltage_icon, voltage_sign, wanring_icon
 * Credit for the following icons to Andrew Wilson:
 *  bar_sign, oil_icon_c, oil_icon_f, oil_pressure_icon
 */
// 'degree_sign', 8x10px
const unsigned char epd_bitmap_degree_sign [] PROGMEM = {
    0x3C, 0x66, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0x66, 0x3C, 0xDE, 0x71, 0xB0, 0x78, 0x3C, 0x1E,
    0x0F, 0x07, 0x83, 0xE3, 0x6F, 0x30, 0x18, 0x0C, 0x00, 0x3B, 0x67, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3,
    0xC3, 0x67, 0x3B, 0x03, 0x03, 0x03, 0xDF, 0x31, 0x8C, 0x63, 0x18, 0xC6, 0x00, 0x3E, 0xE3, 0xC0,
    0xC0, 0xE0, 0x3C, 0x07, 0xC3, 0xE3, 0x7E, 0x66, 0xF6, 0x66, 0x66, 0x66, 0x67, 0xC3, 0xC3, 0xC3,
    0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC7, 0x7B, 0xC1, 0xA0, 0x98, 0xCC, 0x42, 0x21, 0xB0, 0xD0, 0x28
};

// 'coolant_icon_c', 24x25px
const unsigned char epd_bitmap_coolant_icon_c [] PROGMEM = {
    0x00, 0x18, 0x00, 0x00, 0x3c, 0x06, 0x00, 0x3c, 0x08, 0x00, 0x3f, 0x88, 0x00, 0x3f, 0x88, 0x00, 
    0x3c, 0x06, 0x00, 0x3c, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x3f, 0x80, 0x00, 0x3c, 0x00, 0x00, 0x3c, 
    0x00, 0x00, 0x3f, 0x80, 0x00, 0x3f, 0x80, 0x00, 0x3c, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x3c, 0x00, 
    0x00, 0x7e, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0x7e, 0x00, 0x33, 0x3c, 0xcc, 0x0c, 
    0x18, 0x30, 0xc0, 0xc3, 0x03, 0x33, 0x3c, 0xcc, 0x0c, 0x00, 0x30
};

// 'coolant_icon_f', 24x25px
const unsigned char epd_bitmap_coolant_icon_f [] PROGMEM = {
    0x00, 0x18, 0x00, 0x00, 0x3c, 0x0e, 0x00, 0x3c, 0x08, 0x00, 0x3f, 0x8c, 0x00, 0x3f, 0x88, 0x00, 
    0x3c, 0x08, 0x00, 0x3c, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x3f, 0x80, 0x00, 0x3c, 0x00, 0x00, 0x3c, 
    0x00, 0x00, 0x3f, 0x80, 0x00, 0x3f, 0x80, 0x00, 0x3c, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x3c, 0x00, 
    0x00, 0x7e, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0x7e, 0x00, 0x33, 0x3c, 0xcc, 0x0c, 
    0x18, 0x30, 0xc0, 0xc3, 0x03, 0x33, 0x3c, 0xcc, 0x0c, 0x00, 0x30
};

// 'oil_icon_c', 24x25px
const unsigned char epd_bitmap_oil_icon_c [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x60, 0x06, 0x00, 0x60, 0x08, 0x00, 0x7c, 0x08, 0x00, 0x7c, 0x08, 0x00, 
	0x60, 0x06, 0x00, 0x60, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x60, 0x00, 0x00, 0x60, 
	0x00, 0x00, 0x7c, 0x00, 0x00, 0x7c, 0x00, 0x70, 0x60, 0x04, 0x88, 0x60, 0x1e, 0x8c, 0xf0, 0xf2, 
	0x79, 0xfb, 0x20, 0x09, 0xf8, 0x42, 0x08, 0xf0, 0x47, 0x08, 0x60, 0x85, 0x08, 0x01, 0x05, 0x0f, 
	0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'oil_icon_f', 24x25px
const unsigned char epd_bitmap_oil_icon_f [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x60, 0x0e, 0x00, 0x60, 0x08, 0x00, 0x7c, 0x0c, 0x00, 0x7c, 0x08, 0x00, 
	0x60, 0x08, 0x00, 0x60, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x60, 0x00, 0x00, 0x60, 
	0x00, 0x00, 0x7c, 0x00, 0x00, 0x7c, 0x00, 0x70, 0x60, 0x04, 0x88, 0x60, 0x1e, 0x8c, 0xf0, 0xf2, 
	0x79, 0xfb, 0x20, 0x09, 0xf8, 0x42, 0x08, 0xf0, 0x47, 0x08, 0x60, 0x85, 0x08, 0x01, 0x05, 0x0f, 
	0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'warning_icon', 31x31px
const unsigned char epd_bitmap_warning_icon [] PROGMEM = {
    0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x07, 0xc0, 0x00,
    0x00, 0x07, 0xc0, 0x00, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x0e, 0xe0, 0x00, 0x00, 0x1c, 0x70, 0x00,
    0x00, 0x1c, 0x70, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x70, 0x1c, 0x00,
    0x00, 0x70, 0x1c, 0x00, 0x00, 0xe3, 0x8e, 0x00, 0x00, 0xe3, 0x8e, 0x00, 0x01, 0xc3, 0x87, 0x00,
    0x01, 0xc3, 0x87, 0x00, 0x03, 0x83, 0x83, 0x80, 0x03, 0x83, 0x83, 0x80, 0x07, 0x03, 0x81, 0xc0,
    0x07, 0x03, 0x81, 0xc0, 0x0e, 0x03, 0x80, 0xe0, 0x0e, 0x03, 0x80, 0xe0, 0x1c, 0x00, 0x00, 0x70,
    0x1c, 0x00, 0x00, 0x70, 0x38, 0x03, 0x80, 0x38, 0x38, 0x03, 0x80, 0x38, 0x70, 0x03, 0x80, 0x1c,
    0x70, 0x00, 0x00, 0x1c, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xfe
};

// 'psi_sign', 15x7px
const unsigned char epd_bitmap_psi_sign [] PROGMEM = {
    0xf3, 0xce, 0x92, 0x04, 0x92, 0x04, 0xf3, 0xc4, 0x80, 0x44, 0x80, 0x44, 0x83, 0xce
};

// 'bar_sign', 15x7px
const unsigned char epd_bitmap_bar_sign [] PROGMEM = {
    0xe3, 0x38, 0x94, 0xa4, 0x94, 0xa4, 0xe7, 0xb8, 0x94, 0xa4, 0x94, 0xa4, 0xe4, 0xa4
};

// 'rad_pressure_icon', 20x28px
const unsigned char epd_bitmap_rad_psi3_icon [] PROGMEM = {
    0x00, 0x07, 0xc0, 0x00, 0x02, 0x80, 0x00, 0x02, 0x80, 0x7f, 0xfe, 0xf0, 0x40, 0x00, 0x10, 0x40,
    0x00, 0x10, 0xc0, 0x00, 0x10, 0x00, 0x00, 0x10, 0xc0, 0x00, 0x10, 0x40, 0x00, 0x10, 0x40, 0x00,
    0x10, 0x48, 0x20, 0x90, 0x5c, 0x71, 0xd0, 0x7e, 0xfb, 0xf0, 0x7f, 0xff, 0xf0, 0x7f, 0xff, 0xf0,
    0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x20, 0x00, 0x00, 0x20, 0x00, 0x00, 0x20, 0x00, 0x00,
    0xf8, 0x00, 0x18, 0x70, 0xc0, 0x06, 0x23, 0x00, 0x01, 0x04, 0x00, 0x38, 0xf8, 0xe0, 0x04, 0x01,
    0x00, 0x03, 0xfe, 0x00
};

// 'oil_pressure_icon', 24x18px
const unsigned char epd_bitmap_oil_psi_icon [] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x71, 0xe0, 0x04, 0x88, 0xc0, 0x1e, 0x8f, 0xf0, 0xf2, 0x78, 
	0x0f, 0x20, 0x08, 0x00, 0x42, 0x08, 0x00, 0x47, 0x08, 0x00, 0x85, 0x08, 0x01, 0x05, 0x0f, 0xff, 
	0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc1, 0x81, 0x83, 0x26, 0x66, 0x64, 
	0x18, 0x18, 0x18, 0x00, 0x00, 0x00
};

// 'rx8_logo', 128x19px
const unsigned char epd_bitmap_rx8_logo [] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x1f, 0xff, 0xff, 0xc0, 
    0x00, 0x3f, 0xff, 0xff, 0xfe, 0x0f, 0x80, 0x00, 0x03, 0xff, 0x00, 0x01, 0xff, 0xe0, 0x00, 0x40, 
    0x01, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xf0, 0x00, 0x1f, 0xf8, 0x00, 0x07, 0xff, 0x00, 0x00, 0x00, 
    0x07, 0xff, 0x00, 0x1f, 0xff, 0xc1, 0xfc, 0x00, 0xff, 0xc0, 0x00, 0x07, 0xfe, 0x00, 0x00, 0x03, 
    0x00, 0x00, 0x00, 0x01, 0xff, 0xc0, 0xff, 0x03, 0xff, 0x00, 0x00, 0x07, 0xfe, 0x00, 0x1f, 0xfe, 
    0x00, 0x03, 0xc0, 0x01, 0xff, 0x00, 0x1f, 0xdf, 0xf8, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0x80, 
    0x00, 0x0f, 0x80, 0x0f, 0xfc, 0x00, 0x0f, 0xff, 0xe0, 0x00, 0x7f, 0x80, 0xff, 0xff, 0xe0, 0x00, 
    0x00, 0x3f, 0x00, 0x7f, 0xf0, 0x00, 0x07, 0xff, 0x80, 0x7f, 0xfe, 0x00, 0x7f, 0xff, 0x00, 0x00, 
    0x00, 0xfc, 0x0f, 0xfe, 0x00, 0x00, 0x0f, 0xfe, 0x03, 0xff, 0xf8, 0x07, 0xff, 0xff, 0xe0, 0x00, 
    0x01, 0xf8, 0xff, 0xc0, 0x00, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x7f, 0xf8, 0x00, 
    0x03, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x7f, 0xff, 0x80, 0x00, 0x01, 0xff, 0xc0, 0x1f, 0xfe, 0x00, 
    0x07, 0xff, 0xff, 0x80, 0x00, 0x01, 0xff, 0x9f, 0xc0, 0x00, 0x07, 0xfe, 0x00, 0x0f, 0xff, 0x00, 
    0x0f, 0xe0, 0xff, 0xf8, 0x00, 0x07, 0xfc, 0x0f, 0xf0, 0x00, 0x1f, 0xf8, 0x00, 0x07, 0xff, 0x00, 
    0x1f, 0xc0, 0x1f, 0xff, 0x80, 0x1f, 0xf8, 0x03, 0xf8, 0x00, 0x3f, 0xf0, 0x00, 0x0f, 0xfe, 0x00, 
    0x3f, 0x80, 0x07, 0xff, 0xf8, 0x7f, 0xf0, 0x01, 0xfe, 0x00, 0x3f, 0xf0, 0x00, 0x3f, 0xfc, 0x00, 
    0x3f, 0x80, 0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0xff, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0x00, 
    0x7f, 0x00, 0x00, 0x0f, 0xff, 0xfe, 0x00, 0x00, 0x3f, 0xc0, 0x07, 0xff, 0xff, 0xfc, 0x00, 0x00, 
    0xff, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x00
};

// 'fault_message', 102x26px
const unsigned char epd_bitmap_fault [] PROGMEM = {
    0x0f, 0xff, 0xe0, 0x0f, 0xc0, 0x0f, 0x80, 0x1f, 0x0f, 0x00, 0x1f, 0xff, 0xfc, 0x0f, 0xff, 0xe0, 
    0x0f, 0xc0, 0x0f, 0x80, 0x1e, 0x0f, 0x00, 0x1f, 0xff, 0xfc, 0x1f, 0xff, 0xe0, 0x1f, 0xc0, 0x0f, 
    0x80, 0x1e, 0x1f, 0x00, 0x3f, 0xff, 0xf8, 0x1f, 0xff, 0xc0, 0x1f, 0xc0, 0x0f, 0x80, 0x1e, 0x1f, 
    0x00, 0x3f, 0xff, 0xf8, 0x1f, 0xff, 0xc0, 0x1f, 0xe0, 0x0f, 0x00, 0x3e, 0x1f, 0x00, 0x3f, 0xff, 
    0xf0, 0x1f, 0x00, 0x00, 0x3d, 0xe0, 0x0f, 0x00, 0x3e, 0x1f, 0x00, 0x00, 0x7c, 0x00, 0x1e, 0x00, 
    0x00, 0x3d, 0xe0, 0x1f, 0x00, 0x3e, 0x1e, 0x00, 0x00, 0x7c, 0x00, 0x1e, 0x00, 0x00, 0x79, 0xe0, 
    0x1f, 0x00, 0x3c, 0x1e, 0x00, 0x00, 0x78, 0x00, 0x3e, 0x00, 0x00, 0xf9, 0xe0, 0x1f, 0x00, 0x3c, 
    0x3e, 0x00, 0x00, 0x78, 0x00, 0x3e, 0x00, 0x00, 0xf1, 0xf0, 0x1f, 0x00, 0x7c, 0x3e, 0x00, 0x00, 
    0xf8, 0x00, 0x3e, 0x00, 0x00, 0xf1, 0xf0, 0x1f, 0x00, 0x7c, 0x3e, 0x00, 0x00, 0xf8, 0x00, 0x3f, 
    0xff, 0x81, 0xf1, 0xf0, 0x1e, 0x00, 0x7c, 0x3e, 0x00, 0x00, 0xf8, 0x00, 0x3f, 0xff, 0x81, 0xe0, 
    0xf0, 0x3e, 0x00, 0x7c, 0x3c, 0x00, 0x00, 0xf8, 0x00, 0x3f, 0xff, 0x03, 0xe0, 0xf0, 0x3e, 0x00, 
    0x78, 0x3c, 0x00, 0x00, 0xf0, 0x00, 0x7f, 0xff, 0x03, 0xc0, 0xf0, 0x3e, 0x00, 0x78, 0x7c, 0x00, 
    0x00, 0xf0, 0x00, 0x7c, 0x00, 0x07, 0xff, 0xf0, 0x3e, 0x00, 0xf8, 0x7c, 0x00, 0x01, 0xf0, 0x00, 
    0x7c, 0x00, 0x07, 0xff, 0xf0, 0x3c, 0x00, 0xf8, 0x7c, 0x00, 0x01, 0xf0, 0x00, 0x78, 0x00, 0x07, 
    0xff, 0xf8, 0x3c, 0x00, 0xf8, 0x7c, 0x00, 0x01, 0xf0, 0x00, 0x78, 0x00, 0x0f, 0xff, 0xf8, 0x3c, 
    0x00, 0xf8, 0x78, 0x00, 0x01, 0xf0, 0x00, 0x78, 0x00, 0x1f, 0x00, 0xf8, 0x3e, 0x01, 0xf0, 0x78, 
    0x00, 0x01, 0xe0, 0x00, 0xf8, 0x00, 0x1e, 0x00, 0x78, 0x3f, 0x03, 0xf0, 0xf8, 0x00, 0x01, 0xe0, 
    0x00, 0xf8, 0x00, 0x3e, 0x00, 0x78, 0x1f, 0xff, 0xe0, 0xff, 0xff, 0x03, 0xe0, 0x00, 0xf8, 0x00, 
    0x3c, 0x00, 0x78, 0x1f, 0xff, 0xc0, 0xff, 0xff, 0x03, 0xe0, 0x00, 0xf8, 0x00, 0x3c, 0x00, 0x78, 
    0x1f, 0xff, 0xc0, 0xff, 0xff, 0x03, 0xe0, 0x00, 0xf0, 0x00, 0x7c, 0x00, 0x7c, 0x07, 0xff, 0x80, 
    0xff, 0xff, 0x03, 0xe0, 0x00, 0xf0, 0x00, 0x78, 0x00, 0x7c, 0x01, 0xfc, 0x00, 0xff, 0xff, 0x03, 
    0xc0, 0x00
};

// 'voltage_icon', 12x28px
const unsigned char epd_bitmap_voltage_icon [] PROGMEM = {
    0x07, 0xc0, 0x0f, 0x80, 0x0f, 0x80, 0x0f, 0x80, 0x0f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1e, 0x00, 
    0x3e, 0x00, 0x3e, 0x00, 0x3e, 0x00, 0x3c, 0x00, 0x7c, 0x00, 0x7f, 0xf0, 0x7f, 0xe0, 0x7f, 0xe0, 
    0xff, 0xc0, 0x03, 0xc0, 0x03, 0x80, 0x03, 0x80, 0x03, 0x80, 0x03, 0x00, 0x07, 0x00, 0x06, 0x00, 
    0x06, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00
};

// 'voltage_sign', 7x8px
const unsigned char epd_bitmap_voltage_sign [] PROGMEM = {
    0x82, 0x82, 0x44, 0x44, 0x28, 0x28, 0x10, 0x10
};

// Icons size object
typedef struct {
    uint8_t width;       // Icon width
    uint8_t height;      // Icon height
} IconSize;

// Icon size table
const IconSize iconSize[] PROGMEM = {
    {8, 10},    // Degree sign
    {24, 25},   // Coolant icon Celsius
    {24, 25},   // Coolant icon Fahrenheit
    {24, 25},   // Oil icon Celsius
    {24, 25},   // Oil icon Fahrenheit
    {31, 31},   // Warning Icon
    {15, 7},    // PSI sign
    {15, 7},    // BAR sign
    {20, 28},   // Radiator pressure icon
    {24, 18},   // Oil pressure icon
    {128, 19},  // RX-8 stylized logo
    {102, 26},  // Fault message
    {12, 28},   // Voltage icon
    {7, 8}      // Voltage sign
};    

enum class Icon: uint8_t {
    degree_sign = 0,
    coolant_icon_c,
    coolant_icon_f,
    oil_icon_c,
    oil_icon_f,
    warning_icon,
    psi_sign,
    bar_sign,
    radiator_pressure_icon,
    oil_pressure_icon,
    rx8_logo,
    fault_message,
    voltage_icon,
    voltage_sign
};

const unsigned char* epd_bitmap_allArray[sizeof(iconSize) / sizeof(IconSize)] = {
    epd_bitmap_degree_sign,
    epd_bitmap_coolant_icon_c,
    epd_bitmap_coolant_icon_f,
    epd_bitmap_oil_icon_c,
    epd_bitmap_oil_icon_f,
    epd_bitmap_warning_icon,
    epd_bitmap_psi_sign,
    epd_bitmap_bar_sign,
    epd_bitmap_rad_psi3_icon,
    epd_bitmap_oil_psi_icon,
    epd_bitmap_rx8_logo,
    epd_bitmap_fault,
    epd_bitmap_voltage_icon,
    epd_bitmap_voltage_sign
};
