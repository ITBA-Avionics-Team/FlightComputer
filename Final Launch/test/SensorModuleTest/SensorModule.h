
#include <Adafruit_BMP280.h>
#include "KalmanFilter.h"

// BMP280 pins
#define BMP_PIN 118 //0x76

// MPU6050

#define MPU_PIN 0 


class SensorModule {
  public:
  float bmpBasePressureHPa = 1008;

  KalmanFilter pressureKalmanFilter = KalmanFilter(1, 1, 0.01);

  Adafruit_BMP280 bmp280;

  void init() {
    Logger::log("Initializing sensor Module...");

    // Initialize BMP
    bmp280.begin(BMP_PIN);
    bmp280.setSampling(Adafruit_BMP280::MODE_NORMAL,     // Operating Mode. 
                     Adafruit_BMP280::SAMPLING_X2,     // Temp. oversampling 
                     Adafruit_BMP280::SAMPLING_X1,    // Pressure oversampling 
                     Adafruit_BMP280::FILTER_OFF,      // Filtering. 
                     Adafruit_BMP280::STANDBY_MS_1); // Standby time. 
    bmpBasePressureHPa = bmp280.readPressure() / 100;

    // Initialize MPU

  
    Logger::log("Sensor Module initialized.");
  }


  float getAltitude() {
    float alt = bmp280.readAltitude(bmpBasePressureHPa);
    float pressure = bmp280.readPressure();
    float est_alt = pressureKalmanFilter.updateEstimate(alt);
    Logger::debug("Getting altitude with Kalman filter applied: " + String(est_alt));
    return est_alt;
  }

  float getRawAltitude() {
    float alt = bmp280.readAltitude(bmpBasePressureHPa);
    float pressure = bmp280.readPressure();
    Logger::debug("Getting raw altitude: " + String(alt));
    return alt;
  }

  float getAcceleration() {
    Logger::debug("Getting acceleration");
    return 0;
  }

};
