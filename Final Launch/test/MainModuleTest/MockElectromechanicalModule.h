#define PYRO_PIN_DROGUE 0
#define PYRO_PIN_MAIN 0

#define GREEN_PIN 0
#define RED_PIN 0

class ElectromechanicalModule {
  bool drogueDeployed = false;
  bool mainDeployed = false;

  public:

    void init() { 
      pinMode(PYRO_PIN_DROGUE, OUTPUT);
      pinMode(PYRO_PIN_MAIN, OUTPUT);
      Logger::log("Electromechanical Module initialized.");
    }

   void deployDrogueParachute() {
      Logger::log("Deploying drogue parachute...");
      
      digitalWrite(PYRO_PIN_DROGUE, HIGH); //FIRE!
      drogueDeployed = true;
      Logger::log("Charges fired, drogue parachute deployed!");
    }

    void deployMainParachute() {
      Logger::log("Deploying main parachute...");
      
      digitalWrite(PYRO_PIN_MAIN, HIGH); //FIRE!
      mainDeployed = true;
      Logger::log("Charges fired, main parachute deployed!");
    }

};
