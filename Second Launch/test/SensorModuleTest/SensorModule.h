#include <Adafruit_BMP280.h>
#include "TinyGPS++.h"
#include "KalmanFilter.h"


//GPS pins
#define GPS_RX 4
#define GPS_TX 3
#define GPS_BAUD_RATE 9600

// BMP280 pins
#define BMP_PIN 118 //0x76

class SensorModule {
  public:
  float bmpBasePressureHPa = 1008;
  KalmanFilter pressureKalmanFilter = KalmanFilter(1, 1, 0.01);

  TinyGPSPlus gps;
  Adafruit_BMP280 bmp280;

  void init() {
    bmp280.begin(BMP_PIN);
    bmp280.setSampling(Adafruit_BMP280::MODE_NORMAL,     // Operating Mode. 
                     Adafruit_BMP280::SAMPLING_X2,     // Temp. oversampling 
                     Adafruit_BMP280::SAMPLING_X1,    // Pressure oversampling 
                     Adafruit_BMP280::FILTER_OFF,      // Filtering. 
                     Adafruit_BMP280::STANDBY_MS_1); // Standby time. 
    bmpBasePressureHPa = bmp280.readPressure() / 100;
  
    Logger::log("Sensor Module initialized.");
  }


  float getAltitude() {
    //  if (bmpBasePressureHPa == -1) bmpBasePressureHPa = bmp280.readPressure() / 100;
    //  Serial.println(bmpBasePressureHPa);
    float alt = bmp280.readAltitude(bmpBasePressureHPa);
    float pressure = bmp280.readPressure();
    float est_alt = pressureKalmanFilter.updateEstimate(alt);
    Logger::debug("Getting altitude");
    return 0;
  }

  float getAcceleration() {
    Logger::debug("Getting acceleration");
    return 0;
  }

};
