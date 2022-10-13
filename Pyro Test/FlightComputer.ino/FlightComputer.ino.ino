
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include "Adafruit_BMP280.h"
// #include "MPU9250.h"
#include "KalmanFilter.h"

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include <Preferences.h>


void appendFile(fs::FS &fs, const char * path, String message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}


// =============================================
// ===          MISC Global Vars             ===
// =============================================

//Timers Vars
unsigned long loggingMillis = 0;
unsigned long loggingMillisInterval = 0;
unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;
unsigned long landprev = 0;
unsigned long currentMillis;
unsigned long ledblinkMillis = 0;
bool launch = true;
bool pyro = false;
bool pyroFired = false;
bool landed = false;
bool ABORT = false;
int lcc = 0; // land check coutner
float fallr = -0.5;
int adelay = 100; //ms



// =================================
//           Pin set-up
// =================================

int pyroPin = 23;
int greenPin = 13;
int redPin = 12;

// =============================================
// ===              MPU Vars                 ===
// =============================================

// MPU9250 accelgyro(i2c0, 0x68);

int16_t ax, ay, az;
int16_t gx, gy, gz;
int8_t threshold, count;

double roll = 0.00;
double pitch = 0.00;

float temp;
bool zero_detect;
bool TurnOnZI = false;

bool XnegMD, XposMD, YnegMD, YposMD, ZnegMD, ZposMD;

 // ================================================================
// ===                         TIMERS                            ===
// ================================================================


hw_timer_t *My_timer = NULL;
void IRAM_ATTR onTimer()
{
//digitalWrite(greenPin, LOW);
}

// =================================
//           Barometer
// =================================

Adafruit_BMP280 bmp280;
KalmanFilter pressureKalmanFilter(1, 1, 0.01);
const float sealvl = 1008;
static float alt = 0;
float est_alt;
float lastAlt = 0;
float temperature;
float pascal = 0;
float base_alt;
bool sdon = true;


void setup() 
{
  Serial.begin(9600); // opens serial port, sets data rate to 115200 bps
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(5, OUTPUT);
  
  // digitalWrite(pyroPin, HIGH); //FIRE!

  //Timers 
  My_timer = timerBegin(0, 80, true);
  timerAttachInterrupt(My_timer, &onTimer, true);
  timerAlarmWrite(My_timer, 12000000, true);
  timerAlarmEnable(My_timer);
  //Initialize MPU
  
  //initializeMPU(); 
  
  //Initialize Barometer
  initializeBMP();
  
  //Initialize SD card reader
  
  //initializeSD();
  if(!SD.begin(5)){
      Serial.println("Card Mount Failed");
      sdon = false;
  }
  if(sdon) {
    appendFile(SD, "/logs.txt", "\n\nFLIGHT COMPUTER ON");
    loggingMillis = millis();
  }
    
}
 

 // ================================================================
// ===                         MAIN LOOP                         ===
// ================================================================
void loop() 
{

  if((millis() - loggingMillisInterval) > 200) {
    loggingMillisInterval = millis();
    
    if(sdon) {
      appendFile(SD, "/logs.txt", "\n");
      appendFile(SD, "/logs.txt", String(millis()-loggingMillis));
      appendFile(SD, "/logs.txt", " - Alt: ");
      appendFile(SD, "/logs.txt", String(alt));
      
      appendFile(SD, "/logs.txt", "\n");
      appendFile(SD, "/logs.txt", String(millis()-loggingMillis));
      appendFile(SD, "/logs.txt", " - Pres: ");
      appendFile(SD, "/logs.txt", String(pascal));
    }
  }
  
  if (launch == false && pyro == false && landed == false)  
  {
    //Green signal, everything ok (ground)
  }
  else if(launch == true && pyro == false && landed == false)
  {
    //Signal of launch
  }
  else if (landed == true) 
  {
    //Signal of landing
  }

  //=== Flight steps ===
  //
  //   Detect launch
  //   Enable ABORT - for extreame tilt. ??
  //   Detect Apogee
  //   Fire Pyros
  //   Flight Log
  
  //getAccMotion();
  getAlt();

  //Apogee detection algorithm
   if (est_alt - lastAlt <= fallr && pyro == false && launch == true && pyroFired == false)
   {
     //check for drop
      //Store time of Apogee Trigger 1
      delay(adelay);
      getAlt();
      Serial.println("Point 1");
      if(sdon) {
        appendFile(SD, "/logs.txt", "\n");
        appendFile(SD, "/logs.txt", String(millis()-loggingMillis));
        appendFile(SD, "/logs.txt", " - Point 1");
      }

      if (est_alt - lastAlt <= fallr) 
      {
        //check for  drop
        //Store time of Apogee Trigger 1

        delay(adelay);
        getAlt();
        Serial.println("Point 2");
        if(sdon) {
          appendFile(SD, "/logs.txt", "\n");
          appendFile(SD, "/logs.txt", String(millis()-loggingMillis));
          appendFile(SD, "/logs.txt", " - Point 2");
        }

        if (est_alt - lastAlt <= fallr)
        {
          //PASS 3
          //Store time of Apogee Pyro Fire
          //Fire Pyros!
          pyro = true;
          if((millis() - ledblinkMillis) > 750) {
            ledblinkMillis = millis();
            digitalWrite(redPin, !digitalRead(redPin));
          }
          Serial.println("Point 3");
          if(sdon) {
            appendFile(SD, "/logs.txt", "\n");
            appendFile(SD, "/logs.txt", String(millis()-loggingMillis));
            appendFile(SD, "/logs.txt", " - Point 3");
          }
        } 
        else 
        {
          lastAlt = est_alt;
        }
      }
      else 
      {
        lastAlt = est_alt;
      }
    }
    else
    {
      lastAlt = est_alt;
    }


  //Deploy Parachutes
  if(pyro == true && launch == true && pyroFired == false && landed == false) 
  {
    Serial.println("Fire charges!");
    if(sdon) {
      appendFile(SD, "/logs.txt", "\n");
      appendFile(SD, "/logs.txt", String(millis()-loggingMillis));
      appendFile(SD, "/logs.txt", " - Fire charges");
    }
    
    digitalWrite(greenPin, !digitalRead(greenPin));
    delay(500);
    digitalWrite(greenPin, !digitalRead(greenPin));
    
    digitalWrite(pyroPin, HIGH); //FIRE!
    pyroFired = true;
  }


}
// =================================
//             MPU unit
// =================================

void initializeMPU() 
{
  Wire.begin(0x76);
  int status = 0; // accelgyro.begin();
  if (!status)
  {} 
  //set filter to filter noise
  //ser thresholds
  //set control LED pins
}

// ================================================================
// ===                         BAROMETER                       ===
// ================================================================

void initializeBMP() 
{

  Serial.print(F("BMP"));

  bmp280.begin(0x76);

  //if (!bmp280.begin()) {
  //  Serial.println(F("0"));
  //  while (1);
  //}


  // Default settings from datasheet. 
  bmp280.setSampling(Adafruit_BMP280::MODE_NORMAL,     // Operating Mode. 
                     Adafruit_BMP280::SAMPLING_X2,     // Temp. oversampling 
                     Adafruit_BMP280::SAMPLING_X1,    // Pressure oversampling 
                     Adafruit_BMP280::FILTER_OFF,      // Filtering. 
                     Adafruit_BMP280::STANDBY_MS_1); // Standby time. 
  
  Serial.println(F("BMPInit1"));
}

void getAlt()
{
  alt = bmp280.readAltitude(sealvl);
  Serial.println(alt);
  pascal = bmp280.readPressure();
  Serial.println(pascal);
  est_alt = pressureKalmanFilter.updateEstimate(alt);
  Serial.println(est_alt);
}


// ================================================================
// ===                         CONTROL SIGNALS                  ===
// ================================================================









// ================================================================
// ===                 Address check algorithm                  ===
// ================================================================

/***
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
***/
/*
#include <Wire.h>
 
void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial.println("\nI2C Scanner");
}
 
void loop() {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  delay(5000);          
}

/***************************************************************************
  This is a library for the BMP280 humidity, temperature & pressure sensor
  Designed specifically to work with the Adafruit BMP280 Breakout
  ----> http://www.adafruit.com/products/2651
  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface.
  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!
  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/
/*
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>

#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

Adafruit_BMP280 bmp2; // I2C
//Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
//Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

void setup() {
  Serial.begin(9600);
  while ( !Serial ) delay(100);   // wait for native usb
  Serial.println(F("BMP280 test"));
  unsigned status;
  //status = bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID);
  status = bmp2.begin(0x76);
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp2.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }

  
  bmp2.setSampling(Adafruit_BMP280::MODE_NORMAL,     
                  Adafruit_BMP280::SAMPLING_X2,     
                  Adafruit_BMP280::SAMPLING_X16,    
                  Adafruit_BMP280::FILTER_X16,      
                  Adafruit_BMP280::STANDBY_MS_500); 
}

void loop() {
    Serial.print(F("Temperature = "));
    Serial.print(bmp2.readTemperature());
    Serial.println(" *C");

    Serial.print(F("Pressure = "));
    Serial.print(bmp2.readPressure());
    Serial.println(" Pa");

    Serial.print(F("Approx altitude = "));
    Serial.print(bmp2.readAltitude(1013.25)); 
    Serial.println(" m");

    Serial.println();
    delay(2000);
}
*/
