# ESP32 SIM800 Wiring Guide

## Visual Connection Diagram

```
                 ESP32 DevKit v1                    SIM800L Module
                ┌─────────────────┐                ┌─────────────────┐
                │                 │                │                 │
                │     GPIO 16  ───┼────────────────┼─── TX           │
                │     GPIO 17  ───┼────────────────┼─── RX           │
                │     GND      ───┼────────────────┼─── GND          │
                │     3.3V/VIN ───┼────────────────┼─── VCC          │
                │     GPIO 4   ───┼────────────────┼─── RST (opt)    │
                │     GPIO 2   ───┼────────────────┼─── PWR (opt)    │
                │                 │                │                 │
                └─────────────────┘                └─────────────────┘
                        │                                   │
                        │                                   │
                     USB Cable                          Antenna
                                                      Connection
```

## Detailed Pin Mapping

| Function | ESP32 Pin | SIM800 Pin | Wire Color | Notes |
|----------|-----------|------------|------------|-------|
| Serial RX | GPIO 16 | TX | Blue | Data from SIM800 to ESP32 |
| Serial TX | GPIO 17 | RX | Green | Data from ESP32 to SIM800 |
| Ground | GND | GND | Black | Common ground reference |
| Power | 3.3V/VIN | VCC | Red | Power supply (see notes) |
| Reset | GPIO 4 | RST | Yellow | Optional hardware reset |
| Power Ctrl | GPIO 2 | PWR | Orange | Optional power control |

## Power Supply Requirements

### Critical Power Specifications
- **Operating Voltage**: 3.4V - 4.4V DC
- **Sleep Current**: 1-2 mA
- **Active Current**: 300-400 mA
- **Peak Current**: 2A (during transmission bursts)
- **Duration of Peaks**: 577 μs bursts every 4.6ms

### Power Supply Options

#### Option 1: External 5V Power Supply (Recommended for Testing)
```
External 5V PSU → Buck Converter (5V→4.2V) → SIM800L VCC
                                          → ESP32 VIN
Common GND for all components
```

#### Option 2: Li-ion Battery (Best for Portability)
```
3.7V Li-ion Battery (2000mAh+) → SIM800L VCC
                               → ESP32 VIN (through voltage divider if needed)
```

#### Option 3: USB Power (Requires Capacitor Bank)
```
USB 5V → Buck Converter → 4.2V → Large Capacitors → SIM800L
                                  (1000μF + 100μF)
```

## Antenna Connections

### GSM Antenna
- **SIM800L**: Usually has u.FL connector
- **Cable**: u.FL to SMA pigtail cable  
- **Antenna**: 900/1800MHz GSM antenna
- **Placement**: Away from other electronics, vertical orientation

### GPS Antenna (if using GPS-enabled SIM800)
- **Connection**: Separate u.FL connector on module
- **Type**: Active GPS antenna (3.3V powered)
- **Placement**: Clear view of sky, away from metal objects

## Breadboard Layout

```
                    Breadboard Layout
    ┌─────────────────────────────────────────────┐
    │  +  a  b  c  d  e     f  g  h  i  j  -     │
    │ 1│  •  •  •  •  •     •  •  •  •  •  │1    │
    │ 2│  •  •  •  •  •     •  •  •  •  •  │2    │
    │ 3│  •  •ESP32 •  •     •  •  •  •  •  │3    │
    │ 4│  •  •  •  •  •     •  •  •  •  •  │4    │
    │ 5│  •  •  •  •  •     •  •SIM800  •  │5    │
    │ 6│  •  •  •  •  •     •  •  •  •  •  │6    │
    │ 7│  •  •  •  •  •     •  •  •  •  •  │7    │
    │   ═══════════════     ═══════════════      │
    │   Power Rails         Power Rails          │
    └─────────────────────────────────────────────┘
```

### Breadboard Connections
1. Place ESP32 on left side (pins 1-15)
2. Place SIM800L on right side (pins 1-8)
3. Use power rails for VCC and GND distribution
4. Connect jumper wires as per pin mapping table

## PCB Design Considerations

### Trace Width and Spacing
- **Power traces**: Minimum 20mil (0.5mm) width
- **Signal traces**: 10mil (0.25mm) width  
- **Ground plane**: Use solid ground plane
- **Via size**: 12mil minimum for power

### Component Placement
- Keep SIM800 away from ESP32's crystal oscillator
- Place power capacitors close to SIM800 VCC pin
- Route antenna traces as 50Ω controlled impedance
- Use ground keepout around antenna connections

### EMI Considerations
- Use ferrite beads on power lines
- Add bypass capacitors (100nF ceramic + 10μF tantalum)
- Shield sensitive circuits if possible
- Keep digital switching away from RF sections

## Troubleshooting Connections

### Testing Continuity
1. **Power**: Measure 3.4-4.4V at SIM800 VCC pin
2. **Ground**: Verify continuity between all ground points
3. **Serial**: Check for 3.3V levels on TX/RX lines
4. **Reset**: Should be HIGH (3.3V) normally

### Common Wiring Issues

#### Problem: No Response from SIM800
```
Checks:
□ VCC voltage within 3.4-4.4V range
□ GND connections secure
□ TX/RX not swapped (ESP32 TX → SIM800 RX)
□ Baud rate set to 9600
□ Power supply can deliver 2A peaks
```

#### Problem: Random Resets/Crashes
```
Likely Causes:
□ Insufficient power supply current
□ Poor power supply regulation
□ Missing bypass capacitors
□ Loose connections

Solutions:
□ Add 1000μF electrolytic + 100nF ceramic caps
□ Use dedicated power supply (not USB)
□ Shorten power wires
□ Check all solder joints
```

#### Problem: Poor Signal Quality
```
Antenna Issues:
□ Antenna not connected
□ Poor antenna placement
□ Damaged antenna cable
□ Wrong frequency antenna

Test with:
□ Different antenna
□ Move to different location
□ Check u.FL connection
```

## Safety Warnings

⚠️ **Power Supply**: Never exceed 4.4V on SIM800 VCC
⚠️ **Current**: Ensure power supply can handle 2A bursts  
⚠️ **Heat**: SIM800 may get warm during operation
⚠️ **ESD**: Use anti-static precautions when handling modules
⚠️ **RF**: Follow local regulations for GSM/GPS antenna usage

## Tools Required

### Basic Tools
- Soldering iron (if making permanent connections)
- Multimeter for voltage/continuity testing
- Wire strippers
- Small screwdrivers

### Recommended Tools
- Oscilloscope (for signal debugging)
- Logic analyzer (for serial communication debugging)
- RF power meter (for antenna testing)
- Hot air station (for SMD work)

## Final Verification Checklist

Before powering up:

□ All connections match the pin mapping table
□ No short circuits between VCC and GND
□ Power supply voltage is 3.4-4.4V
□ Power supply current rating ≥ 2A
□ Antenna properly connected
□ SIM card inserted correctly
□ All jumper wires secure

Power-on checks:

□ SIM800 power LED illuminates
□ No excessive heat generation
□ Serial communication working (AT commands)
□ Network registration successful
□ GPS antenna has sky view (if testing GPS)

## Advanced Connections

### Level Shifters (if needed)
Some SIM800 modules operate at 5V TTL levels:
```
ESP32 3.3V ← Logic Level Shifter → SIM800 5V
```

### External SIM Card Socket
For easier SIM card access:
```
SIM800 SIM pins → External SIM card socket
```

### Status LEDs
Add visual indicators:
```
GPIO pins → LEDs → Current limiting resistors → GND
```

This completes the comprehensive wiring guide for ESP32-SIM800 integration.