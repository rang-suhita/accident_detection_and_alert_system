#include <WiFi.h>
#include <HTTPClient.h>
#include <TinyGPSPlus.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Twilio credentials
const char* account_sid = "YOUR_TWILIO_SID";
const char* auth_token = "YOUR_TWILIO_AUTH_TOKEN";
const char* from_number = "whatsapp:+14155238886";  // Twilio sandbox
const char* to_number = "whatsapp:+91XXXXXXXXXX";   // Your number

// GPS and alert
const int alertPin = 27;
bool alertSent = false;

TinyGPSPlus gps;
HardwareSerial SerialGPS(2); // UART2 RX=16 TX=17

void setup() {
  Serial.begin(115200);
  SerialGPS.begin(9600, SERIAL_8N1, 16, 17);

  pinMode(alertPin, INPUT);

  connectToWiFi();
}

void loop() {

  while (SerialGPS.available()) {
    gps.encode(SerialGPS.read());
  }

  int alertState = digitalRead(alertPin);

  if (alertState == HIGH && !alertSent) {
    Serial.println("Alert received from Arduino. Reading GPS...");

    if (gps.location.isValid()) {
      sendWhatsAppAlert(gps.location.lat(), gps.location.lng());
      alertSent = true;
    } else {
      Serial.println("Waiting for valid GPS...");
    }
  }

  if (alertState == LOW) {
    alertSent = false;
  }

  delay(1000);
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected.");
}

void sendWhatsAppAlert(double lat, double lng) {
  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;

    String url = "https://api.twilio.com/2010-04-01/Accounts/" + 
                 String(account_sid) + "/Messages.json";

    http.begin(url);
    http.setAuthorization(account_sid, auth_token);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String googleMapsLink =
      "https://www.google.com/maps/search/?api=1&query=" +
      String(lat, 6) + "," + String(lng, 6);

    String body =
      "From=" + String(from_number) +
      "&To=" + String(to_number) +
      "&Body=Accident Alert! Location: " + googleMapsLink;

    int httpResponseCode = http.POST(body);

    if (httpResponseCode > 0) {
      Serial.println("WhatsApp alert sent with location!");
    } else {
      Serial.print("Failed to send alert. Code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected.");
  }
}
