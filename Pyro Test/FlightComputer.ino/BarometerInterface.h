// #ifndef BAROMETERINTERFACE_H
// #define BAROMETERINTERFACE_H
// #include <BMP280_DEV.h>

// class BarometerInterface
// {
// public:
//     /**
//      * Common barometer readings.
//      */
//     typedef struct
//     {
//         float pressure;
//         float temperature;
//         float altitude;
//     } Data_t;

//     BarometerInterface();

//     //One time setup logic
//     bool init1();

//     // Collects most recent data from barometer
//     bool run1();

//     // Gets more recent pressure reading
//     float getPressure1();

//     //Gets more recent Temperature reading
//     float getTemperature1();

//     // Gets more recent Altitude reading   
//     float getAltitude1();

//     float printData();
    
// protected:
    
//     //most recent data read from barometer
//     Data_t mData;
// };



// #endif