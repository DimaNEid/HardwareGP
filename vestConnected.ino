#include <WiFi.h>
#include <HTTPClient.h>

const char *ssid = "Bike-ESP32";
const char *password = "12345678";
const int leftRelay = 16;   // GPIO number for left LED relay
const int rightRelay = 17;  // GPIO number for right LED relay
const int stopRelayPin = 18; // GPIO number for stop LED relay

void setup() {
  Serial.begin(115200);
  pinMode(leftRelay, OUTPUT);
  pinMode(rightRelay, OUTPUT);
  pinMode(stopRelayPin, OUTPUT);
  digitalWrite(leftRelay, HIGH); // Ensure relay is off initially (active LOW)
  digitalWrite(rightRelay, HIGH); // Ensure relay is off initially (active LOW)
  digitalWrite(stopRelayPin, HIGH); // Ensure relay is off initially (active LOW)

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://192.168.4.1/");
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("HTTP response code: " + String(httpCode));
      Serial.println("Payload: " + payload);

      if (payload.indexOf("left") != -1) {
        digitalWrite(leftRelay, LOW); // Turn on the left relay (active LOW)
        digitalWrite(rightRelay, HIGH); // Turn off the right relay
      } else if (payload.indexOf("right") != -1) {
        digitalWrite(leftRelay, HIGH);  // Turn off the left relay
        digitalWrite(rightRelay, LOW); // Turn on the right relay (active LOW)
      } else {
        digitalWrite(leftRelay, HIGH);  // Turn off both relays
        digitalWrite(rightRelay, HIGH); // Turn off both relays
      }

      if (payload.indexOf("STOP") != -1) {
        digitalWrite(stopRelayPin, LOW); // Turn on the stop relay (active LOW)
      } else {
        digitalWrite(stopRelayPin, HIGH); // Turn off the stop relay
      }
    } else {
      Serial.println("Error on HTTP request: " + String(httpCode));
    }

    http.end();
  } else {
    Serial.println("WiFi not connected");
  }

  delay(200); // Check every 200 milliseconds
}