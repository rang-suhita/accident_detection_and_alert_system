// Pin Definitions
#define EYE_SENSOR_PIN A0
#define ALCOHOL_SENSOR_PIN A1
#define TEMP_SENSOR_PIN A2
#define X_PIN A3
#define Y_PIN A4
#define Z_PIN A5
#define ALERT_PIN 7   // Digital pin to send signal to ESP32

void setup() {
  Serial.begin(9600);

  // Pin modes
  pinMode(EYE_SENSOR_PIN, INPUT);
  pinMode(ALCOHOL_SENSOR_PIN, INPUT);
  pinMode(TEMP_SENSOR_PIN, INPUT);
  pinMode(ALERT_PIN, OUTPUT);

  digitalWrite(ALERT_PIN, LOW); // Default alert signal is LOW
}

void loop() {
  // Read sensor values
  int eyeBlink = analogRead(EYE_SENSOR_PIN);
  int alcohol = analogRead(ALCOHOL_SENSOR_PIN);
  int temp = analogRead(TEMP_SENSOR_PIN);
  int x = analogRead(X_PIN);
  int y = analogRead(Y_PIN);
  int z = analogRead(Z_PIN);

  // Detect alert conditions
  bool isCrash = (x > 900 || y > 900 || z > 900);   // Adjust threshold
  bool isDrunk = (alcohol > 500);                  // Adjust threshold
  bool isDrowsy = (eyeBlink < 100);                // Adjust threshold
  bool isOverheat = ((temp * 0.488) > 60);         // Convert LM35 to Celsius

  // Send signal to ESP32
  if (isCrash || isDrunk || isDrowsy || isOverheat) {
    digitalWrite(ALERT_PIN, HIGH);
  } else {
    digitalWrite(ALERT_PIN, LOW);
  }

  // Print readings
  Serial.println("------ Sensor Readings ------");
  Serial.print("Eye Blink: "); Serial.println(eyeBlink);
  Serial.print("Alcohol: "); Serial.println(alcohol);
  Serial.print("Temperature (C): "); Serial.println(temp * 0.488);
  Serial.print("Accelerometer - X: "); Serial.println(x);
  Serial.print("Y: "); Serial.println(y);
  Serial.print("Z: "); Serial.println(z);
  Serial.println("-----------------------------");

  delay(1000);
}
