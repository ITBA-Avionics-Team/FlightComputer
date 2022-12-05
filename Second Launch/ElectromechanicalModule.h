#include <Wire.h>

#define GREEN_PIN 0
#define RED_PIN 0
#define PYRO_PIN 0

class ElectromechanicalModule {

  public:

    void init() { 
      pinMode(GREEN_PIN, OUTPUT);
      pinMode(RED_PIN, OUTPUT);
      pinMode(PYRO_PIN, OUTPUT);
      Serial.println("Electromechanical Module initialized.");
    }

    void deployParachute() {
      Serial.println("Fire charges!");
      
      digitalWrite(greenPin, !digitalRead(greenPin));
      delay(500);
      digitalWrite(greenPin, !digitalRead(greenPin));
      
      digitalWrite(pyroPin, HIGH); //FIRE!
      pyroFired = true;
    }

};
