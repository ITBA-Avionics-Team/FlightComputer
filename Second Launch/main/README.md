# Flight Software (FSW) v1

## Rerquirements to run the flight software
- Arduino IDE
- Adafruit_BMP280 library
- Preferences library

## Developer Notes:
- You can enable or disable debug mode by modifying the `debugActivated` variable in the `Logger.h` file.
- You can prevent loading of previous state by uncommenting the indicated line on the `init()` function of the `StorageModule`.
- Currently, all methods in the `SensorModule` return mock values equal to 0.
- Currently, gps data is not passed to the telemetry string generation function on `main.ino`, because GPS connection hasn't been implemented nor mocked on the `SensorModule`.