  
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include "Adafruit_BMP280.h"
// #include "MPU9250.h"
#include "KalmanFilter.h"

#include <Preferences.h>
#include <nvs_flash.h>



const float MIN_LAUNCH_ALT = 50; // metros sobre el piso
const float MIN_LAUNCH_ACC = 10; // Gs




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
bool launch = false;
bool pyro = false;
bool pyroFired = false;
bool landed = false;
bool ABORT = false;
int lcc = 0; // land check coutner
float fallr = -0.5;
int adelay = 100; //ms
Preferences preferences;



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

KalmanFilter pressureKalmanFilter(1, 1, 0.01);
const float sealvl = 1008;
static float alt = 0;
float est_alt;
float est_base_alt;
float est_ground_alt;
float lastAlt = 0;
float temperature;
float pascal = 0;
float base_alt;
Adafruit_BMP280 bmp280;


void setup() 
{
  delay(3000);
  nvs_flash_erase(); // erase the NVS partition and...
  nvs_flash_init(); // initialize the NVS partition.
  
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
  preferences.begin("lanzamiento00", false);
  logToParams("Flight computer ON!");
  
  getAlt();
  est_base_alt = est_alt;
  logToParams("Base: " + String(est_base_alt));
}
 

 // ================================================================
// ===                         MAIN LOOP                         ===
// ================================================================
void loop() 
{

  if((millis() - loggingMillisInterval) > 500) {
    loggingMillisInterval = millis();
    
    logToParams(String(est_ground_alt) + "," + String(pascal));
  }
  
  if (launch == false && pyro == false && landed == false)  
  {
    //Green signal, everything ok (ground)
    if(est_ground_alt > MIN_LAUNCH_ALT) {
      launch = true;
      Serial.println("Launch detected");
      logToParams("Launch detected");
    }
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
      logToParams("Point 1");

      if (est_alt - lastAlt <= fallr) 
      {
        //check for  drop
        //Store time of Apogee Trigger 1

        delay(adelay);
        getAlt();
        Serial.println("Point 2");
        logToParams("Point 2");

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
          logToParams("Point 3, pyro true");
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
    logToParams("Fire parachute charges");
    
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
  est_ground_alt = est_alt - est_base_alt;
}

void logToParams(String str) {
  static uint64_t prefCounter = 0;
  Serial.print("Saving ");
  Serial.print((String(millis()-loggingMillis) + " - " + str).c_str());
  Serial.print(" in ");
  Serial.println(("log" + String(prefCounter)).c_str());
  preferences.putString(("log" + String(prefCounter++)).c_str(), (String(millis()-loggingMillis) + "," + str).c_str());
  preferences.putULong64("lognum", prefCounter);
}
