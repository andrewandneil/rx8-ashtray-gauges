/* 
 * This is the main source code for the RX-8 Ashtray Gauges project.
 * It targets a Teensy 4.0
 * Original Author: Stephane Gilbert
 * Modified by: Andrew Wilson
 * BSD tree clause licence (SPDX: BSD-3-Clause)
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "coolant_monitor.h"
#include "FreeSans18pt7bNum.h"

#define OLED_RESET 4 // Reset for Adafruit SSD1306

// Using this constructor to have the maximum I2C communication speed.
Adafruit_SSD1306 display_1(128, 64, &Wire, OLED_RESET);
Adafruit_SSD1306 display_2(128, 64, &Wire1, OLED_RESET);

// This timer is used to run the buzzer for a user defined number of seconds.
// Called on entering fault or warning state.
IntervalTimer buzzerTimer;

// Values to cache the current reading in memory
float current_oil_temp;
float current_oil_psi;
bool oil_thermistor_reference_mode_high = true;
float current_coolant_temp;
//float current_coolant_psi;
float current_supply_voltage;
bool cool_thermistor_reference_mode_high = true;

// General booleans we can check to see what's going on
bool temperatureUnitIsFahrenheit = false;
bool pressureUnitIsBar = false;
bool currentDaylight = true;
bool lidClosed = false;

bool in_alert = false; 
bool buzzer_currently_playing = false;

bool coolant_temp_warn_happened = false;
//bool coolant_psi_warn_happened = false;
bool oil_temp_warn_happened = false;
bool oil_psi_warn_happened = false;
bool voltage_warn_happened = false;

// Read the specified analogue input pin many times and return the mean
// pin: The pin on which the analogue read will occur
// Return: A float between 0 and 1023 representing the analogue value on the specified pin
float readAnalogInputRaw(uint8_t pin)
{
    uint16_t cumulative_value = 0;
    int single_value;

    // The first read is a dummy read only to warm up the ADC, so we Loop from zero to
    // ANALOG_SAMPLES_COUNT inclusively, in fact ANALOG_SAMPLES_COUNT + 1 times.
    for (size_t i = 0; i <= ANALOG_SAMPLES_COUNT; i++)
    {
        single_value = analogRead(pin);

        // If it's the first read, force the value to zero, it it will not be taken into account
        cumulative_value += i > 0 ? single_value : 0;
        delay(ANALOG_DELAY_BETWEEN_ACQUISITIONS);
    }

    // Return the arithmetic mean
    return (float)cumulative_value / (float)ANALOG_SAMPLES_COUNT;
}

// Returns the voltage on the specified pin
// pin: The pin on which the analogue read will occur
// Return: A float representing the voltage read at the specified pin
float readVoltage(uint8_t pin)
{
    float value = readAnalogInputRaw(pin);

    return (MAX_ANALOGUE_VOLTAGE / 1023) * value;
}

// Invalidate the display value so the next reading will force the display to be redrawn
void forceDisplayRefresh()
{
    current_oil_psi = __FLT_MIN__;
    current_oil_temp = __FLT_MIN__;
    current_coolant_temp = __FLT_MIN__;
    current_supply_voltage = __FLT_MIN__;
}

// Draw an icon using specified display object
// display: An instance reference of the Adafruit_SSD1306 class
// icon: The specific icon to draw, emum value
// xpos, ypos: The top left corner to start the drawing 
void drawIcon(Adafruit_SSD1306 &display, const Icon icon, const uint8_t xpos, const uint8_t ypos)
{
    display.drawBitmap(
        (uint16_t)xpos,
        (uint16_t)ypos,
        epd_bitmap_allArray[(uint8_t)icon],
        pgm_read_byte(&iconSize[(uint8_t)icon].width),
        pgm_read_byte(&iconSize[(uint8_t)icon].height),
        1);
}

// Draw the warning icon using the the specified display object
// We also turn on the warning LED to notify the user of warning state
// display: An instance reference of the Adafruit_SSD1306 class, the display to write to
// half: True to print on the top half of display, False to print on the bottom half of display
void drawWarning(Adafruit_SSD1306 &display, bool half)
{
    if (half) {
        drawIcon(display, Icon::warning_icon, 95, 0);
    } else {
        drawIcon(display, Icon::warning_icon, 95, 0 + DISPLAY_HALF_TWO);
    }
    #if ENABLE_WARNING_LEDS
    digitalWrite(WARNING_LED_OUTPUT_PIN, HIGH);
    in_alert = true;
    #endif
}

// Display a fault message using the the specified display object
// We also turn on the warning LED to notify the user of fault state
// display: An instance reference of the Adafruit_SSD1306 class, the display to write to
// half: True to print on the top half of display, False to print on the bottom half of display
void displayFault(Adafruit_SSD1306 &display, bool half)
{
    if (half) {
        drawIcon(display, Icon::fault_message, 11, 3);
    } else {
        drawIcon(display, Icon::fault_message, 11, 3 + DISPLAY_HALF_TWO);
    }

    #if ENABLE_WARNING_LEDS
    digitalWrite(WARNING_LED_OUTPUT_PIN, HIGH);
    in_alert = true;
    #endif

    // Ensure if we go out of fault, the display will refresh the actual value
    forceDisplayRefresh();
}

// Sets the reference resistor (pull down) for the oil thermistor
// value: True to set the high resistor value, False to select the low resistor value.
void setThermistorHighReferenceOil(const bool value)
{
    // The logic is inverted here by the FDN337N on the PCB
    digitalWrite(OIL_THERMISTOR_REFERENCE_SELECT_OUTPUT_PIN, value ? LOW : HIGH);
    oil_thermistor_reference_mode_high = value;
}

// Sets the reference resistor (pull down) for the coolant thermistor
// value: True to set the high resistor value, False to select the low resistor value.
void setThermistorHighReferenceCoolant(const bool value)
{
    // The logic is inverted here by the FDN337N on the PCB
    digitalWrite(COOLANT_THERMISTOR_REFERENCE_SELECT_OUTPUT_PIN, value ? LOW : HIGH);
    cool_thermistor_reference_mode_high = value;
}

// Convert the provided temperature from Celsius to Fahrenheit
// temperature: The temperature to convert from Celsius
// Return: A float representing the converted temperature, in Fahrenheit
float convertToFahrenheit(float temperature)
{
    return temperature * 9.0 / 5.0 + 32.0;
}

// Convert the provided pressue from PSI to Bar.
// pressure: The pressure to convert from PSI
// Return: A float representing the converted pressure, in Bar.
float convertToBar(float pressure) 
{
    return (pressure / 14.5038);
}

// Read the coolant/oil thermistor resistor value and convert it to temperature in Celsius
// using the Steinhart-Hart equation.
// TC: The variable that will hold the returned temperature value
// pinRead: The pin we are reading our analogue voltage from
// Return: ENOERR if the conversion succeeded and the value has been placed in the TC parameter, otherwise the error code
// Note: The function also manages the pull-down resistor set related to the sensor
int getFluidTempCelsius(float &TC, uint8_t pinRead)
{
    float tResValue;
    float logTResValue;
    float TK;
    float analogueValue;
    float t_res_ref;

    // The constant values for the AEM-30-2012 have been calculated with this online calculator:
    // https://www.thinksrs.com/downloads/programs/therm%20calc/ntccalibrator/ntccalculator.html
    // The reference resistor and temperature used for the calculator was extracted from the
    // following datasheet from AEM:
    // https://documents.aemelectronics.com/techlibrary_30-2012_water_temp_sensor_kit.pdf
    float c1 = 1.144169514e-3;   // -40C, 402392 OHMs4
    float c2 = 2.302830665e-4;   // 50C, 3911 OHMs
    float c3 = 0.8052469400e-7;  // 150C, 189.3 OHMs

    // Get the analogue value on the input pin
    analogueValue = readAnalogInputRaw(pinRead);

    // (2.878/5)*1023 = 588.8
    // LOW: 981 HIGH: 15090
    // 981 * (1023/588.8-1) = 721
    // log(721) = 2.8579
    // (1.0 / (0.001144169514 + (0.0002302830665 * 2.8579) + (0.00000008052469400 * 2.8579 * 2.8579 * 2.8579))) = 554.26

    // Avoid dividing by zero
    if (analogueValue == 0) {
        return EDIVZERO;
    }

    // Discard any values that do not make sense
    if (analogueValue < 0) {
        return ERANGE;
    }

    // Use the correct pull down resistor reference value according to the actual configured value
    if (pinRead == OIL_ANALOG_INPUT_PIN) {
        t_res_ref = oil_thermistor_reference_mode_high ? OIL_THERMISTOR_RESISTOR_REFERENCE_HIGH : OIL_THERMISTOR_RESISTOR_REFERENCE_LOW;
    } else {
        t_res_ref = cool_thermistor_reference_mode_high ? COOL_THERMISTOR_RESISTOR_REFERENCE_HIGH : COOL_THERMISTOR_RESISTOR_REFERENCE_LOW;
    }

    // Compute the thermistor resistor value, using the equation R2 = R1 * (Vin / Vout - 1)
    tResValue = (t_res_ref * (1023.0 / ((float)analogueValue) - 1.0));

    // Convert the thermistor resistor value to temperature in Kelvin using the Steinhart-Hart equation
    logTResValue = log(tResValue);
    TK = (1.0 / (c1 + c2 * logTResValue + c3 * logTResValue * logTResValue * logTResValue));

    // Ensure the temperature is between the sensor range: 233.15K to 425.15K (-40C to 150C)
    if (TK < 233.15 || TK > 423.15) {
        return ERANGE;
    }

    // Convert the temperature from Kelvin to Celsius
    TC = TK - 273.15;
    
    if (pinRead == OIL_ANALOG_INPUT_PIN) {
        // Manage the oil pull-down resistor reference for the next run
        if (oil_thermistor_reference_mode_high && TC > THERMISTOR_RESISTOR_REFERENCE_LOW_THRESHOLD)
            setThermistorHighReferenceOil(false);

        if (! oil_thermistor_reference_mode_high && TC < THERMISTOR_RESISTOR_REFERENCE_HIGH_THRESHOLD)
            setThermistorHighReferenceOil(true);
    } else {
        // Manage the coolant pull-down resistor reference for the next run
        if (cool_thermistor_reference_mode_high && TC > THERMISTOR_RESISTOR_REFERENCE_LOW_THRESHOLD)
            setThermistorHighReferenceCoolant(false);

        if (! cool_thermistor_reference_mode_high && TC < THERMISTOR_RESISTOR_REFERENCE_HIGH_THRESHOLD)
            setThermistorHighReferenceCoolant(true);
    }
    
    return ENOERR;
}

// Get the Coolant/Oil pressure in PSIG.
// PSIG is PSI above ambient pressure.
// psi: The variable that will hold the returned PSI value
// sensorType: The type of pressure sensor used for the fluid
// pinRead: The pin we are reading our analogue voltage from
// Return: ENOERR if the conversion succeeded and the value has been placed in the psi
//         parameter, otherwise the error code
int getFluidPsi(float &psi, bool sensorType, uint8_t pinRead)
{
    float volts_32bit, volts;
    volts_32bit = readVoltage(pinRead);

    // Because the Teensy 4.0's ADC has a max of 3.3V we need to convert the 0-3.3V range back to 0-5V
    volts = (volts_32bit / MAX_ANALOGUE_VOLTAGE) * 5;

    if (sensorType) {
        // AEM30-2131-100 or AEM30-2130-100
        // Ensure the voltage is between the sensor range, otherwise return an error
        if (volts < 0.5 || volts > 4.5) {
            return ERANGE;
        }

        // Convert voltage to PSI
        // According to this datasheet, PSI = (25*(Voltage)) - 12.5
        // https://documents.aemelectronics.com/techlibrary_30-2131-100_sensor_data.pdf
        // https://documents.aemelectronics.com/techlibrary_30-2130-100_sensor_data.pdf
        psi = 25 * volts - 12.5;
    } else {
        // AEM30-2131-15G
        // Ensure the voltage is between the sensor range, otherwise return an error
        if (volts < 0.2 || volts > 4.8) {
            return ERANGE;
        }

        // Convert voltage to PSI
        // According to this datasheet, PSI = (3.7529*(Voltage)) - 1.8765
        // https://www.aemelectronics.com/files/instructions/30-2131-15G%20Sensor%20Data.pdf
        psi = 3.7529 * volts - 1.8765;
    }

    if (psi < 0)
        psi = 0;

    return ENOERR;
}

// Read the supply voltage before the DC-DC converter
// It should be between 11.5 and 14.5
// voltage: The variable that will hold the returned voltage value
// Return: ENOERR if the conversion succeeded and the value has been placed in the voltage
//         parameter, otherwise the error code
int getSupplyVoltage(float &voltage)
{
    float volts, supply_voltage;
    volts = readVoltage(VOLTAGE_ANALOG_INPUT_PIN);

    // Convert pin voltage to actual voltage based on the onboard tension divider
    supply_voltage = volts / (VOLTAGE_DIVIDER_R2 / (VOLTAGE_DIVIDER_R1 + VOLTAGE_DIVIDER_R2));

    // We can't be alive and have a supply voltage below 7V at the same time
    if (supply_voltage < 7)
        return ERANGE;
    
    voltage = supply_voltage;
    return ENOERR;
}

// Return true if the illumination (parking lights) are turned off, otherwise false
// Valid voltage are between 0V and 15V. Anything below 1.7v is considered day lignt
// Note: There a voltage divisor on the board that divides by roughly 4.830
// 18K and 4.7K = 22.7K / 4.7K = 4.830
// So 1.7V input would read .352V and 16V would read 3.31V at the analogue pin
// Note: There is no needs to use high precision function here. A simple analogue read
// is good enaugh.
bool isDayLight()
{
    float v;
    v = (float)analogRead(ILLUMINATION_ANALOG_INPUT_PIN) * (MAX_ANALOGUE_VOLTAGE / 1023.0);
    return (v < 0.35) ? true : false;
}

// If dayLight is true, set all displays to their maximum luminosity, dim them otherwise
void setDayLight(bool dayLight)
{
    currentDaylight = dayLight;
    display_1.ssd1306_command(SSD1306_SETCONTRAST);
    display_1.ssd1306_command(dayLight ? 0xFF : MINIMUM_BRIGHTNESS);
    display_2.ssd1306_command(SSD1306_SETCONTRAST);
    display_2.ssd1306_command(dayLight ? 0xFF : MINIMUM_BRIGHTNESS);
}

// Ensure the display intensity is set according to the current daylight status
void processDayLight()
{
    // Don't need to bother dimming the displays if the lid is closed.
    if (lidClosed)
        return;
    bool dayLight = isDayLight();
    if (dayLight != currentDaylight) {
        setDayLight(dayLight);
    }
}

// Return true if the hall effect sensor detects a magnet (ashtray lid is closed), otherwise false
// We're using the Teensy's internal pullup resistor, so no need for any fancy analogue checks.
// We can just read if the pin is high or low depending on the presence of the magnet.
bool isLidClosed() 
{
    return digitalRead(HALL_EFFECT_SENSOR_INPUT_PIN);
}

// If lidStatus is true, turn off all displays, otherwise turn them back on
void toggleDisplays(bool lidStatus) 
{
    lidClosed = lidStatus;
    if (lidStatus) {
        display_1.ssd1306_command(SSD1306_DISPLAYOFF);
        display_2.ssd1306_command(SSD1306_DISPLAYOFF);
    } else {
        display_1.ssd1306_command(SSD1306_DISPLAYON);
        display_2.ssd1306_command(SSD1306_DISPLAYON);
    }
}

// Ensure the state of the displays is set according to the current lid status
void processLidStatus() 
{
    bool lidStatus = isLidClosed();
    if (lidStatus != lidClosed) {
        toggleDisplays(lidStatus);
    }
}

// Here a fault or warning state has been entered, so we activate and then deactivate the buzzer.
void handleBuzzer() 
{
    digitalWrite(ALERT_BUZZER_OUTPUT_PIN, buzzer_currently_playing);
    buzzer_currently_playing = false;
}

// Update the oil temperature on the specified display with the specified temperature.
// If the Fahrenheit selector jumper has been present during boot time, display the
// temperature in Fahrenheit, display in Celsius otherwise.
// display: An instance of the Adafruit_SSD1306 class representing the display
//          on which the value will be displayed
// temperature: The temperature to display, in Celsius
// On display's first half
void updateOilTemp(Adafruit_SSD1306 &display, float temperature)
{
    // To prevent LED flickering we only want to clear our alert if we're updating a display
    in_alert = false;
    current_oil_temp = temperature;

    // Print the coolant value and the icon according to the desired units
    display.setCursor(TEXT_POS_X, TEXT_POS_Y + 24);
    if (!temperatureUnitIsFahrenheit) {
        // Jumper not present, Display in Celsius
        drawIcon(display, Icon::oil_icon_c, 0, 5);
        display.print(round(temperature), 0);
    } else {
        // Jumper present. Convert to Fahrenheit
        drawIcon(display, Icon::oil_icon_f, 0, 5);
        display.print(round(convertToFahrenheit(temperature)), 0);
    }

    // Print the degree sign after the numeric value
    drawIcon(display, Icon::degree_sign, display.getCursorX() + 1, TEXT_POS_Y);

    // Print a warning if oil temperature exceeds user set value
    if (temperature >= OIL_TEMP_WARNING_CELSIUS)
        drawWarning(display, TOP_HALF);
        
}

// Update the Oil pressure on the specified display with the provided value
// If the Bar selector jumper has been present during boot time, display the
// pressure in bar, display in PSI otherwise.
// display: An instance of the Adafruit_SSD1306 class representing the display
//          on wich the value will be displayed
// psi: The pressure to display, in PSI
// On display's second half
void updateOilPsi(Adafruit_SSD1306 &display, float psi)
{
    // To prevent LED flickering we only want to clear our alert if we're updating a display
    in_alert = false;
    current_oil_psi = psi;

    drawIcon(display, Icon::oil_pressure_icon, 0, 7 + DISPLAY_HALF_TWO);

    if (pressureUnitIsBar) {
        // Print the bar value
        // Move slightly the displayed value to the left if we are in warning state to give room for the warning sign
        if (psi <= OIL_PSI_WARNING_LOW || psi >= OIL_PSI_WARNING_HIGH) {
            display.setCursor(TEXT_POS_X - 6, TEXT_POS_Y + DISPLAY_HALF_TWO + 24);
            display.print(convertToBar(psi), 2);
        } else {
            display.setCursor(TEXT_POS_X, TEXT_POS_Y + DISPLAY_HALF_TWO + 24);
            display.print(convertToBar(psi), 2);
            // Print the bar sign
            drawIcon(display, Icon::bar_sign, display.getCursorX() + 1, TEXT_POS_Y + DISPLAY_HALF_TWO);
        }  
    } else {
        // Print the PSI value
        // Move slightly the displayed value to the left if we are in warning state to give room for the warning sign
        if (psi <= OIL_PSI_WARNING_LOW || psi >= OIL_PSI_WARNING_HIGH) {
            display.setCursor(TEXT_POS_X - 6, TEXT_POS_Y + DISPLAY_HALF_TWO + 24);
            display.print(psi, 1);
            if (psi < 10) {
                // Print the PSI sign
                drawIcon(display, Icon::psi_sign, display.getCursorX() + 1, TEXT_POS_Y + DISPLAY_HALF_TWO);
            }
        } else {
            display.setCursor(TEXT_POS_X, TEXT_POS_Y + DISPLAY_HALF_TWO + 24);
            display.print(psi, 1);
            // Print the PSI sign
            drawIcon(display, Icon::psi_sign, display.getCursorX() + 1, TEXT_POS_Y + DISPLAY_HALF_TWO);
        }
    }
    
    // Print a warning if oil psi is too low or too high
    if (psi >= OIL_PSI_WARNING_HIGH || psi <= OIL_PSI_WARNING_LOW)
        drawWarning(display, BOTTOM_HALF);
}

// Update the coolant temperature on the specified display with the specified temperature.
// If the Fahrenheit selector jumper was present during boot time, display the
// temperature in Fahrenheit, otherwise display in Celsius.
// display: An instance of the Adafruit_SSD1306 class representing the display
//          on which the value will be displayed
// temperature: The temperature to display, in Celsius
// On display's first half
void updateCoolantTemp(Adafruit_SSD1306 &display, float temperature)
{
    // To prevent LED flickering we only want to clear our alert if we're updating a display
    in_alert = false;
    current_coolant_temp = temperature;

    // Print the coolant value and the icon according to the desired units
    display.setCursor(TEXT_POS_X, TEXT_POS_Y + 24);
    if (!temperatureUnitIsFahrenheit) {
        // Jumper not present, Display in Celsius
        drawIcon(display, Icon::coolant_icon_c, 0, 5);
        display.print(round(temperature), 0);
    } else {
        // Jumper present. Convert to Fahrenheit
        drawIcon(display, Icon::coolant_icon_f, 0, 5);
        display.print(round(convertToFahrenheit(temperature)), 0);
    }

    // Print the degree sign after the numeric value
    drawIcon(display, Icon::degree_sign, display.getCursorX() + 1, TEXT_POS_Y);

    // Print a warning if coolant temperature exceeds user set value
    if (temperature >= COOLANT_TEMP_WARNING_CELSIUS)
        drawWarning(display, TOP_HALF);      
}

/*
// Update the Coolant PSI on the specified display with the provided value
// display: An instance of the Adafruit_SSD1306 class representing the display
//          on wich the value will be displayed
// psi: The pressure in PSI
// Unused
void updateCoolantPsi(Adafruit_SSD1306 &display, float psi)
{
    current_coolant_psi = psi;

    drawIcon(display, Icon::radiator_pressure_icon, 0, 3);

    // Print the PSI value
    // Move slightly the displayed value to the left if we are in warning state to give room for the warning sign
    display.setCursor(TEXT_POS_X - (psi < COOLANT_PSI_WARNING ? 0 : 10), TEXT_POS_Y + 24);
    display.print(psi, 1);

    // Print the PSI sign
    drawIcon(display, Icon::psi_sign, display.getCursorX() + 1, TEXT_POS_Y);
    
    if (psi >= COOLANT_PSI_WARNING)
        drawWarning(display, BOTTOM_HALF);
}
*/

// Update the supply voltage on the specified display with the provided value
// display: An instance of the Adafruit_SSD1306 class representing the display
//          on wich the value will be displayed
// voltage: The voltage value to display
// On display's second half
void updateSupplyVoltage(Adafruit_SSD1306 &display, float voltage)
{
    // To prevent LED flickering we only want to clear our alert if we're updating a display
    in_alert = false;
    current_supply_voltage = voltage;

    drawIcon(display, Icon::voltage_icon, 6, 3 + DISPLAY_HALF_TWO);

    display.setCursor(TEXT_POS_X - (voltage < 10.0 ? 0 : 8), TEXT_POS_Y + DISPLAY_HALF_TWO + 24);
    display.print(voltage, 1);
     
    // Print the voltage sign
    drawIcon(display, Icon::voltage_sign, display.getCursorX() + 1, TEXT_POS_Y + DISPLAY_HALF_TWO);

    // Print a warning if voltage is too low or too high
    if (voltage < BATTERY_VOLTAGE_LOW_WARNING || voltage > BATTERY_VOLTAGE_HIGH_WARNING) {
        drawWarning(display, BOTTOM_HALF);
        /*if (!voltage_warn_happened) {
            voltage_warn_happened = true;
            buzzer_currently_playing = true;
            buzzerTimer.begin(handleBuzzer, 2000000);
        }*/
    }
          
}

// Display a small animation at start up
// The logo width must be a multiple of 8. Typically, 8 bits in an unsigned char
void displayIntro()
{
    // The icon to use for the animation
    const Icon icon = Icon::rx8_logo;

    // Compute the size of an unsigned char in bits
    const uint8_t u_char_bits_size = sizeof(unsigned char) * __CHAR_BIT__;

    // Get a pointer to the logo bitmap array. We will need it to display only some parts of the image
    const unsigned char* logo_ptr { epd_bitmap_allArray[(uint8_t)icon] };

    // Get the logo size in pixels
    uint8_t width = pgm_read_byte(&iconSize[(uint8_t)icon].width);
    uint8_t height = pgm_read_byte(&iconSize[(uint8_t)icon].height);

    for (size_t x = u_char_bits_size - 1; x < width; x = x + u_char_bits_size)
    {
        display_1.clearDisplay();
        display_2.clearDisplay();

        for (size_t y = 0; y < height; y++)
        {
            display_1.drawBitmap(0, y + (display_1.height() - height) / 2, logo_ptr + y * width / u_char_bits_size  + (width - x - 1) / u_char_bits_size, x + 1, 1 ,1);
            display_2.drawBitmap(0, y + (display_2.height() - height) / 2, logo_ptr + y * width / u_char_bits_size  + (width - x - 1) / u_char_bits_size, x + 1, 1 ,1);
        }

        display_1.display();
        display_2.display();

        // Adjust the delay to have a smooth animation.
        // As more parts of the image is drawn, the more time it take to transfer it with i2c.
        delay((width - x) / 3);
        processDayLight();
    }

    delay(3000);
}

// Configures the Teensy IO pins
// All unused pins are put in three state with pull-ups
void configureIOs()
{
    // Unused pins
    pinMode(UNUSED_PIN_0, INPUT_PULLUP);
    pinMode(UNUSED_PIN_1, INPUT_PULLUP);
    //pinMode(UNUSED_PIN_2, INPUT_PULLUP);
    //pinMode(UNUSED_PIN_3, INPUT_PULLUP);
    //pinMode(UNUSED_PIN_5, INPUT_PULLUP);
    //pinMode(UNUSED_PIN_6, INPUT_PULLUP);
    //pinMode(UNUSED_PIN_7, INPUT_PULLUP);
    //pinMode(UNUSED_PIN_8, INPUT_PULLUP);
    pinMode(UNUSED_PIN_9, INPUT_PULLUP);
    //pinMode(UNUSED_PIN_10, INPUT_PULLUP);
    pinMode(UNUSED_PIN_11, INPUT_PULLUP);
    pinMode(UNUSED_PIN_12, INPUT_PULLUP);
    //pinMode(UNUSED_PIN_13, INPUT_PULLUP);
    //pinMode(UNUSED_PIN_14, INPUT_PULLUP);
    //pinMode(UNUSED_PIN_15, INPUT_PULLUP);
    //pinMode(UNUSED_PIN_16, INPUT_PULLUP);
    //pinMode(UNUSED_PIN_17, INPUT_PULLUP);
    //pinMode(UNUSED_PIN_18, INPUT_PULLUP);
    //pinMode(UNUSED_PIN_19, INPUT_PULLUP);
    //pinMode(UNUSED_PIN_20, INPUT_PULLUP);
    //pinMode(UNUSED_PIN_21, INPUT_PULLUP);
    //pinMode(UNUSED_PIN_22, INPUT_PULLUP);
    //pinMode(UNUSED_PIN_23, INPUT_PULLUP);

    // In use output digtal pins
    pinMode(OIL_THERMISTOR_REFERENCE_SELECT_OUTPUT_PIN, OUTPUT);
    pinMode(COOLANT_THERMISTOR_REFERENCE_SELECT_OUTPUT_PIN, OUTPUT);
    pinMode(WARNING_LED_OUTPUT_PIN, OUTPUT);
    pinMode(ALERT_BUZZER_OUTPUT_PIN, OUTPUT);

    // In use input digtal pins
    pinMode(TEMPERATURE_UNIT_SELECTOR_INPUT_PIN, INPUT_PULLUP);
    pinMode(PRESSURE_UNIT_SELECTOR_INPUT_PIN, INPUT_PULLUP);
    pinMode(HALL_EFFECT_SENSOR_INPUT_PIN, INPUT_PULLUP);

    // Analogue inputs have to be set to input (no pull-ups)
    pinMode(OIL_PSI_ANALOG_INPUT_PIN, INPUT);
    pinMode(OIL_ANALOG_INPUT_PIN, INPUT);
    pinMode(COOLANT_ANALOG_INPUT_PIN, INPUT);
    pinMode(VOLTAGE_ANALOG_INPUT_PIN, INPUT);
    pinMode(ILLUMINATION_ANALOG_INPUT_PIN, INPUT);
}

// Initialise the specified display, set font, size and colour
// display: An instance of the Adafruit_SSD1306 class representing the display
//          to be initialised
void initDisplay(Adafruit_SSD1306 &display)
{
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setFont(&FreeSans18pt7bNum);
    display.clearDisplay();
    display.display();
    forceDisplayRefresh();
}

void setup()
{
    configureIOs();
    initDisplay(display_1);
    initDisplay(display_2);

    // Start with the high reference pull-down value
    setThermistorHighReferenceOil(true);
    setThermistorHighReferenceCoolant(true);
    
    // Read the onboard jumper.
    // Jupmer absent = Celsius
    // Jupmer present = Fahrenheit
    temperatureUnitIsFahrenheit = !digitalRead(TEMPERATURE_UNIT_SELECTOR_INPUT_PIN);

    // Read the onboard jumper.
    // Jupmer absent = PSI
    // Jupmer present = Bar
    pressureUnitIsBar = !digitalRead(PRESSURE_UNIT_SELECTOR_INPUT_PIN);

    // Pending a redesign of the PCB to add a second jumper, value is hardcoded at compile time.
    #if (USE_BAR)
    pressureUnitIsBar = true;
    #endif //USE_BAR

    displayIntro();
}

void loop()
{
    float oil_temp;
    float oil_psi;
    float coolant_temp;
    float supply_voltage;
    int err;
    int err2;

    // The following value are used to compute and enforce the refresh rate
    uint64_t startMs;
    int32_t elapsedMs;
    int32_t waitMs;

    // Sample the current timer counter
    startMs = millis();

    // Get oil pressure and temp and display
    // Also handle any faults that we've caught and display the appropriate message
    // in the appropriate place
    err2 = getFluidPsi(oil_psi, PRESSURE_SENSOR_2131_100, OIL_PSI_ANALOG_INPUT_PIN);
    err = getFluidTempCelsius(oil_temp, OIL_ANALOG_INPUT_PIN);
    // We still want to process data when the lid is closed, but we don't want to display this on the screen.
    if (lidClosed) {
        if (err != ENOERR || err2 != ENOERR) {
            digitalWrite(WARNING_LED_OUTPUT_PIN, HIGH);
            in_alert = true;
        } else {
            if (oil_temp >= OIL_TEMP_WARNING_CELSIUS) {
                digitalWrite(WARNING_LED_OUTPUT_PIN, HIGH);
                in_alert = true;
            } else if (oil_psi >= OIL_PSI_WARNING_HIGH || oil_psi <= OIL_PSI_WARNING_LOW) {
                digitalWrite(WARNING_LED_OUTPUT_PIN, HIGH);
                in_alert = true;
            }
        }
    } else {
        if (err == ENOERR && err2 == ENOERR) {
            if (oil_psi != current_oil_psi || oil_temp != current_oil_temp) {
                display_1.clearDisplay();
                updateOilTemp(display_1, oil_temp);
                updateOilPsi(display_1, oil_psi);
                display_1.display();
            }
        } else if (err != ENOERR) {
            display_1.clearDisplay();
            displayFault(display_1, TOP_HALF);
            if (err2 != ENOERR) {
                displayFault(display_1, BOTTOM_HALF);
            } else {
                updateOilPsi(display_1, oil_psi);
            }
            display_1.display();
        } else {
            display_1.clearDisplay();
            updateOilTemp(display_1, oil_temp);
            displayFault(display_1, BOTTOM_HALF);
            display_1.display();
        }
    }
    

    // Get coolant temp and supply voltage and display
    // Also handle any faults that we've caught and display the appropriate message
    // in the appropriate place
    err = getFluidTempCelsius(coolant_temp, COOLANT_ANALOG_INPUT_PIN);
    err2 = getSupplyVoltage(supply_voltage);
    // We still want to process data when the lid is closed, but we don't want to display this on the screen.
    if (lidClosed) {
        // Save some processing if we are already in an alert state
        if (!in_alert) {
            if (err != ENOERR || err2 != ENOERR) {
                digitalWrite(WARNING_LED_OUTPUT_PIN, HIGH);
                in_alert = true;
            } else {
                if (coolant_temp >= COOLANT_TEMP_WARNING_CELSIUS) {
                    digitalWrite(WARNING_LED_OUTPUT_PIN, HIGH);
                    in_alert = true;
                } else if (supply_voltage >= BATTERY_VOLTAGE_HIGH_WARNING || supply_voltage <= BATTERY_VOLTAGE_LOW_WARNING) {
                    digitalWrite(WARNING_LED_OUTPUT_PIN, HIGH);
                    in_alert = true;
                }
            }
        }
    } else {
        if (err == ENOERR && err2 == ENOERR) {
            if (coolant_temp != current_coolant_temp) {
                display_2.clearDisplay();
                updateCoolantTemp(display_2, coolant_temp);
                updateSupplyVoltage(display_2, supply_voltage);
                display_2.display();
            }
        } else if (err != ENOERR) {
            display_2.clearDisplay();
            displayFault(display_2, TOP_HALF);
            if (err2 != ENOERR) {
                displayFault(display_2, BOTTOM_HALF);
            } else {
                updateSupplyVoltage(display_2, supply_voltage);
            }
            display_2.display();
        } else {
            display_2.clearDisplay();
            updateCoolantTemp(display_2, coolant_temp);
            displayFault(display_2, BOTTOM_HALF);
            display_2.display();
        }
    }
    
    // Now we check if the lid is closed, handling it appropriately.
    processLidStatus();
    // Now we check if the car has switched on/off lights, and handle state changes appropriately.
    processDayLight();

    #if ENABLE_WARNING_LEDS
    // Fault not detected so we swtich the LEDs off
    if (!in_alert)
        digitalWrite(WARNING_LED_OUTPUT_PIN, LOW);
    #endif

    // Wait the correct amount of time to respect the desired refresh rate
    // Note: There is no needs to take the timer overflow into account here. The timer overflows every
    //       50 days, which is much longer than a car would run continuously
    elapsedMs = (int32_t)(millis() - startMs);
    waitMs = int32_t((int32_t)((1.0 / (float)DISPLAY_REFRESH_RATE_HZ) * 1000.0) - (int32_t)elapsedMs);
    if (waitMs > 0)
        delay(waitMs);
}
