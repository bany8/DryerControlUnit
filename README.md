# DryerControlUnit

## Description
The electronics of the **WARTMANN WM-2006 DH** food dryer (used for mushrooms, fruits, etc.) got wet and rusted.  
I decided to repair it and share how I did it.

The dryer already included:
- 230 VAC power input socket  
- power switch  
- 230 VAC fan  
- 230 VAC heater  

All schematics are in the **/Docs** folder, and photos are in **/Photos**.  
I could have reused the original resistive temperature sensor, but I didn’t notice it at first.<br>
Code is prepared in PlatformIO (VSCode extension) and you can find it in **/Dryer arduino/src**.

---

## Bill of Materials (BOM)
- Arduino Nano 3.0,
- 2× Songle SRD-05VDC-SL-C 5-pin 10 A / 5 V relay,
- 230 V → 5 V / 700 mA AC–DC converter,
- 2× 2N2222 NPN transistor (0.8 A, TO-92),
- 0.96" OLED display (SSD1306, I²C) 4 buttons,
- DS18B20 OneWire temperature sensor,
- fuse form original pcb.
