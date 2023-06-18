#include <Adafruit_BMP280.h>
#include "TinyGPS++.h"
#include "KalmanFilter.h"

const int simulatedPressureValues[139] = {100833, 100833, 100833, 100833, 100833, 100833, 100833, 100833, 100833, 100833, 100833, 100833, 100833, 98522, 97353, 96081, 95049, 94231, 93621, 93137, 92812, 92578, 92645, 92868, 92756, 92924, 92913, 93059, 93115, 93216, 93250, 93384, 93441, 97539, 93655, 93700, 93790, 93880, 93948, 94072, 94378, 94231, 94299, 94378, 94503, 94617, 94651, 94810, 96334, 94947, 95015, 95118, 95278, 95323, 98264, 95495, 95587, 95678, 95782, 95862, 96127, 96023, 96069, 96150, 96207, 96288, 96357, 96438, 96519, 96623, 96669, 96738, 96831, 96900, 96981, 97121, 97179, 97190, 97295, 97341, 97458, 97598, 97621, 97563, 97609, 97691, 97761, 97796, 97843, 97889, 97936, 98018, 98393, 104180, 98287, 98240, 98393, 98393, 98440, 98522, 98616, 98639, 98698, 98745, 98816, 98875, 98957, 99075, 99512, 99264, 99299, 100523, 99406, 99489, 99571, 99702, 99749, 99809, 99880, 99963, 100034, 100129, 100368, 100463, 101927, 100487, 100582, 100606, 100726, 100762, 100821, 100893, 100893, 100893, 100893, 100893, 100893, 100893};

struct MockGPSLocation {
  double (*lat)();
  double (*lng)();
};

double getMockLat() {
  return -34.4355487;
}

double getMockLng() {
  return -58.9219291;
}

struct MockGPS {
  MockGPSLocation location;
};

class SensorModule {
  public:
  float bmpBasePressureHPa = 1008;
  KalmanFilter pressureKalmanFilter = KalmanFilter(1, 1, 0.01);
  int currentPressureValIndex = 0;
  MockGPS gps;


  void init() {
    Logger::log("Mock Sensor Module initialized.");

    gps.location.lat = &getMockLat;
    gps.location.lng = &getMockLng;
  }


  float getAltitude() {
    if (currentPressureValIndex > 138) {
      Logger::log("Consumed all simulated pressure values, altitude will return 0");
    }
    float alt = altitudeFromPressure(simulatedPressureValues[currentPressureValIndex++]); // bmp280.readAltitude(bmpBasePressureHPa);
    float est_alt = pressureKalmanFilter.updateEstimate(alt);
    Logger::log("Getting altitude: " + String(est_alt));
    return est_alt;
  }

  float getAcceleration() {
    Logger::debug("Getting acceleration");
    return 0;
  }

  float altitudeFromPressure(float pressure) {
    float pressureHPa = pressure / 100;
    return 44330 * (1 - pow(pressureHPa / bmpBasePressureHPa, 0.1903));
  }

};
