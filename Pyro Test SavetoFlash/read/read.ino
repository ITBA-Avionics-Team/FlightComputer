#include <Preferences.h>
Preferences preferences;

void setup() {
  // put your setup code here, to run once:
  preferences.begin("lanzamiento00", false);
  String logs = preferences.getString("logs");
  Serial.begin(115200);
  uint64_t lognum = preferences.getULong64("lognum");
  Serial.print("Found ");
  Serial.print(String(lognum));
  Serial.println(" logs!");
  Serial.println("Printing...");
  for(uint64_t i = 0; i <= lognum; i++) {
    Serial.print(String(i));
    Serial.print(": ");
    Serial.println(preferences.getString(String(i).c_str()));
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
