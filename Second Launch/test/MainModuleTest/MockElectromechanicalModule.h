#define GREEN_PIN 0
#define RED_PIN 0
#define PYRO_PIN 0

class ElectromechanicalModule {
  bool pyroFired = false;

  public:

    void init() { 
      pinMode(GREEN_PIN, OUTPUT);
      pinMode(RED_PIN, OUTPUT);
      pinMode(PYRO_PIN, OUTPUT);
      Logger::log("Electromechanical Module initialized.");
    }

    void deployParachute() {
      Logger::log("Deploying parachute...");
      
      digitalWrite(GREEN_PIN, !digitalRead(GREEN_PIN));
      delay(500);
      digitalWrite(GREEN_PIN, !digitalRead(GREEN_PIN));
      
      digitalWrite(PYRO_PIN, HIGH); //FIRE!
      pyroFired = true;
      Logger::log("Charges fired, parachute deployed!");
    }

};
