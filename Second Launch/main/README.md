# Ada Flight Computer v2

## Rerquirements to run the flight software tests
- Arduino IDE
- Adafruit_BMP280 library
- Preferences library

## Important clarifications
These folders are intended to test and iterate over each software module, but due to Arduino's import system, we cannot test the modules that are located in the `main` folder directly. Thus, after adecuate testing of any given module, we must copy the working, fully tested version of the module onto the `main` folder to be used in the flight computer. This is ugly, but it's the best way to do it currently.