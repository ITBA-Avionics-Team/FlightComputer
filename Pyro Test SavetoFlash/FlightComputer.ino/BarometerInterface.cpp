// #include "BarometerInterface.h"

// BMP280_DEV bmp;

// BarometerInterface::BarometerInterface()
// {

// }

// bool BarometerInterface::init1()
// {
//   bmp.begin(BMP280_I2C_ALT_ADDR);
//   bmp.setTimeStandby(TIME_STANDBY_2000MS);     // Set the standby time to 2 seconds
//   bmp.startNormalConversion();
// }

// float BarometerInterface::getPressure1()
// {
//     return mData.pressure;
// }

// float BarometerInterface::getTemperature1()
// {
//     return mData.temperature;
// }

// float BarometerInterface::getAltitude1()
// {
//     return mData.altitude;
// }

// bool BarometerInterface::run1()
// {
//   bmp.getMeasurements(this->mData.temperature, this->mData.pressure, this->mData.altitude);
// }

// float BarometerInterface::printData()
// {
  
// }