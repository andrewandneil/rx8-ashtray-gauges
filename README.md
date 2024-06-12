# Mazda RX-8 Ashtray Gauges

As someone who likes knowing the vitals of my ever-so-fragile rotary engine, I wanted a way to monitor Oil Temperature, Oil Pressure, Coolant Temperature and Electrical System Voltage without needing big aftermarket gauges. I stumbled across Stephane Gilbert's work, where he built a small gauge system for coolant temperature, pressure and battery voltage, but it wasn't a perfect fit for me. I decided to take his basis, and modify it to my need. After several months of toiling around, I have finally produced this:

![RX8 Ashtray Gauges Installed](images/ashtray_installed.png)

This project doesn't use the OBDII port, instead relying on deidcated sensors. For both oil and coolant temperature AEM-30-2012 sensors were used and for oil pressure an AEM-2131-100 was used. Electrical system voltage is calculated using the supplied power on board.

The design uses two PCBs, one PCB that houses the electronics, and another PCB inside the ashtray cavity to breakout the connectors for screens and sensors.

## Features
- Coolant Temperature Monitoring & Warning.
- Oil Temperature and Pressure Monitoring & Warning.
- System Voltage Monitoring & Warning.
- Screen auto-off when ashtray lid is closed.
- Optional LED warning lights when temperature, pressure or voltage is out of user specified limits.
- Optional buzzer sound when temperature, pressure or voltage is out of user specified limits. [NOT IMPLEMENTED ATM]

## Sensors
#### My Oil Temperature Sensor Location:
A modified banjo bolt with a 1/8NPT thread cut into the top is used. Mounted to the banjo bolt on the front passanger side oil cooler.

![Oil Temp Sensor Location](images/oil_temp_location.png)

#### My Oil Pressure Sensor Location:
A modified banjo bolt with a 1/8NPT thread cut into the top is used. Mounted to the banjo bolt on the oil filter.

![Oil Temp Sensor Location](images/oil_pressure_location.png)

#### My Coolant Temperature Sensor Location:
A Racing Beat RX-8 Water Temp Sensor Hose Adapter is used with a 3/8 to 1/8 step down adapter to fit the 1/8NPT thread of the sensor. Fitted to the heater core hose located on the passanger side.

![Coolant Temp Sensor Location](images/cool_temp_location.png)

## 3D Printing
The cases for the electronics and the OLED screen holders are designed to be 3D printed by a standard FDM printer. 

### Electronics Housing
Recommended print settings:
- PETG or ABS/ASA (don't use PLA as it will warp when exposed to the sun or heat)
- 0.2mm Layer Height

You'll also need:
- x6 M3 Heat set inserts (OD: 4mm, Length: 4.2mm)
- x6 M3x10 Bolts
- x2 M3 Washers (for clamping the CPT power item to the board)

### OLED Screen Holder
Recommended print settings:
- PETG or ABS/ASA (don't use PLA as it will warp when exposed to the sun or heat)
- 0.1mm or 0.15mm Layer Height

You'll also need:
- x5 M3 Heat set inserts (OD: 4mm, Length: 4.2mm)
- x2 M3x10 Bolts
- x3 M3x4 Bolts

### OLED Screen Spacers
Print two, one for each oled screen.

Recommended print settings:
- PETG or ABS/ASA (don't use PLA as it will warp when exposed to the sun or heat)
- 0.2mm Layer Height

You'll also need:
- x8 M2.5x8 Dome Head Bolts (must be low profile)
- x8 M2.5 Nuts
- x8 M2.5 Washers

## Useful Links
A guide for adding the oil temperature and pressure sensors: https://www.rx8club.com/series-i-do-yourself-forum-73/diy-oil-temp-pressure-sender-install-modified-banjo-bolts-235867/
rx8-coolant-monitor by Stephane Gilbert (topolittle): https://github.com/topolittle/rx8-coolant-monitor