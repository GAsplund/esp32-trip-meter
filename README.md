# ESP32 Trip Meter
An ESP32-based trip meter primarily made for measuring fuel efficiency.

## Features
- Measure fuel efficiency (L/100km)
- Measure velocity (Km/h)
- Measure fuel injection speed (L/h)

## Connections
- The negative side of the injector is connected to corresponding input on the board
  - NOTE: Must be a saturation-type injector (read below)
- A pulsed signal of the vehicle speed sensor (VSS) is connected to VSS+
  - Variable reluctance (VR) speed sensors do not seem to work.

### Power and ground
The ground reference and power is currently given by the USB port on the ESP32.
This may change later due to being a limiting factor.

## Injector types
This project is mainly made to support saturation-type injectors.
If your engine uses other types of injectors, such as peak-and-hold injectors, this project **will not work**.

The easiest way to check this is to measure the resistance of an injector;
If it has a resistance of around 12Ω, it is a saturation-type injector.
If it has a resistance of around 2-3Ω, it is likely a **peak-and-hold** injector.

**Also note that** injectors that are connected for batch injection may be connected in parallel, and may in some cases display a resistance that is lower than normal.

The reason why peak-and-hold injectors will not work, is that they work differently from saturation injectors.
While saturation injectors simply open while the signal is active, a peak-and-hold injector, like its name suggests, sends a high voltage signal which forces the injector open (peak).
After that, a lower-voltage signal (normally around 12 volts) is pulsed through the remainder of the opening time (hold).
