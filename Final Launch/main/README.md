# Ada Flight Computer v2

## Rerquirements to run the flight software
- Arduino IDE
- Adafruit_BMP280 library
- Preferences library

## Developer Notes:
- You can enable or disable debug mode by modifying the `debugActivated` variable in the `Logger.h` file.
- You can prevent loading of previous state by uncommenting the indicated line on the `init()` function of the `StorageModule`.
- Currently, all methods in the `SensorModule` return mock values equal to 0.
- There is no GPS connection in the `SensorModule` yet, which might yield an error in `main.ino` execution.
