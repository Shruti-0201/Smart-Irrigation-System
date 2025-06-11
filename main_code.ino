// Smart Irrigation + Manual Pump Control
#define BLYNK_TEMPLATE_ID "TMPL3W3q6XHva"
#define BLYNK_TEMPLATE_NAME "Smart irrigation system with esp32"
#define BLYNK_AUTH_TOKEN "TPsGRh9h7Vt5jF4aDFIoy1tR-8wUx70T"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#define SOIL_MOISTURE_PIN 34   // Analog pin
#define THRESHOLD_MOISTURE 30  // Moisture % threshold
#define PUMP_PIN 2             // Relay connected to GPIO 2

#define PUMP_SWITCH V6         // Blynk manual pump control
#define MOISTURE_DISPLAY V5    // Moisture display

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "your-wifi-name";
char pass[] = "your-wifi-name";

BlynkTimer timer;
bool manualPumpOn = false;

void sendSensorData() {
  int moistureValue = analogRead(SOIL_MOISTURE_PIN);
  int moisturePercent = map(moistureValue, 3500, 4095, 100, 0);
  moisturePercent = constrain(moisturePercent, 0, 100);

  Serial.print("Moisture: ");
  Serial.println(moisturePercent);

  Blynk.virtualWrite(MOISTURE_DISPLAY, moisturePercent);

  if (!manualPumpOn) {
    if (moisturePercent < THRESHOLD_MOISTURE) {
      digitalWrite(PUMP_PIN, HIGH);
      Serial.println("Auto Mode: Pump ON (Low moisture)");
      Blynk.logEvent("moisture_alert", "Soil moisture is below threshold!");
    } else {
      digitalWrite(PUMP_PIN, LOW);
      Serial.println("Auto Mode: Pump OFF");
    }
  }
}

BLYNK_WRITE(PUMP_SWITCH) {
  manualPumpOn = param.asInt();
  if (manualPumpOn) {
    digitalWrite(PUMP_PIN, HIGH);
    Serial.println("Manual Mode: Pump ON");
  } else {
    digitalWrite(PUMP_PIN, LOW);
    Serial.println("Manual Mode: Pump OFF");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW); // default OFF

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(3000L, sendSensorData);

  Blynk.syncVirtual(PUMP_SWITCH); // get current pump state
}

void loop() {
  Blynk.run();
  timer.run();
}