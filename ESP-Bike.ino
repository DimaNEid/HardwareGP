#include <Wire.h>
#include <MPU6050.h>
#include <WiFi.h>
#include <WebServer.h>
#include <FirebaseESP32.h>

// WiFi credentials
const char *ssid = "Bike-ESP32";
const char *password = "12345678";

// Firebase credentials
#define WIFI_SSID "DamdoOmLab"
#define WIFI_PASSWORD "58)00wX7"
#define FIREBASE_HOST "https://testdim-aa3cf-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "Yv7AHSdRYSyeVIXSKbm6lkLseSsI8MCJV9zta5js"

FirebaseData firebaseData;
FirebaseConfig config;
FirebaseAuth auth;

// MPU6050 object
MPU6050 mpu;

// Web server object
WebServer server(80);
String direction = "none";

// GPIO for relays
const int leftRelay = 16;  // GPIO for left relay IN1
const int rightRelay = 17; // GPIO for right relay IN2
const int stopRelayPin = 18; // Relay connected to GPIO 18 that controls the 12V LED

// Threshold and sustained detection settings
const int turnThreshold = 3000; // Adjust this value as needed
const int requiredReadings = 5;  // Number of consecutive readings to confirm a turn

// Moving average filter variables
const int filterSize = 10;
int gzReadings[filterSize];
int gzIndex = 0;
long gzSum = 0;
int gzAverage = 0;

// Sustained detection counters
int rightTurnCounter = 0;
int leftTurnCounter = 0;

// LDR variables
const int ldrPin = 34;     // LDR connected to GPIO 34 (ADC1 channel)
int ldrValue = 0;
int previousLdrValue = 0;
const int stabilityRange = 100; // Range within which the LDR value is considered stable
bool isBikeStopped = false; // To track if the bike is stopped

void handleRoot() {
  String response = direction;
  if (isBikeStopped) {
    response += ",STOP";
  } else {
    response += ",GO";
  }
  server.send(200, "text/plain", response);
}

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize I2C communication
  Wire.begin(21, 22); // SDA, SCL pins for ESP32

  // Initialize MPU6050
  mpu.initialize();
  if (!mpu.testConnection()) {
    //Serial.println("MPU6050 connection failed");
  }

  // Initialize relay pins
  pinMode(leftRelay, OUTPUT);
  pinMode(rightRelay, OUTPUT);
  pinMode(stopRelayPin, OUTPUT);
  digitalWrite(leftRelay, HIGH); // Ensure relay is off initially (active LOW)
  digitalWrite(rightRelay, HIGH); // Ensure relay is off initially (active LOW)
  digitalWrite(stopRelayPin, HIGH); // Ensure relay is off initially (active LOW)

  // Initialize moving average filter
  for (int i = 0; i < filterSize; i++) {
    gzReadings[i] = 0;
  }

  // Set up WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 // Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  //Serial.println(" connected");

  // Initialize Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Set up WiFi access point
  WiFi.softAP(ssid, password);
  delay(100);
  //Serial.println("Access Point Started");
  //Serial.print("IP Address: ");
  //Serial.println(WiFi.softAPIP());

  // Set up web server
  server.on("/", handleRoot);
  server.begin();
 // Serial.println("HTTP server started");

  // Monitor the timer value in Firebase
  monitorTimer();
}

void loop() {
  // Handle client requests
  server.handleClient();

  // Read gyro data
  int16_t gx, gy, gz;
  mpu.getRotation(&gx, &gy, &gz);

  // Update the moving average filter
  gzSum -= gzReadings[gzIndex];
  gzReadings[gzIndex] = gz;
  gzSum += gz;
  gzIndex = (gzIndex + 1) % filterSize;
  gzAverage = gzSum / filterSize;

  // Determine turn direction
  if (gzAverage > turnThreshold) {
    rightTurnCounter++;
    leftTurnCounter = 0;
  } else if (gzAverage < -turnThreshold) {
    leftTurnCounter++;
    rightTurnCounter = 0;
  } else {
    rightTurnCounter = 0;
    leftTurnCounter = 0;
  }

  // Activate relays and set direction based on sustained detection
  if (rightTurnCounter >= requiredReadings) {
    digitalWrite(rightRelay, LOW); // Activate right relay (turn on right LED)
    digitalWrite(leftRelay, HIGH);  // Deactivate left relay (turn off left LED)
    direction = "right";
  } else if (leftTurnCounter >= requiredReadings) {
    digitalWrite(leftRelay, LOW); // Activate left relay (turn on left LED)
    digitalWrite(rightRelay, HIGH); // Deactivate right relay (turn off right LED)
    direction = "left";
  } else {
    digitalWrite(leftRelay, HIGH);  // Deactivate both relays (turn off both LEDs)
    digitalWrite(rightRelay, HIGH);
    direction = "none";
  }

  // Read LDR value and check if bike is stopped
  ldrValue = analogRead(ldrPin);
  if (abs(ldrValue - previousLdrValue) <= stabilityRange) {
    isBikeStopped = true;
    digitalWrite(stopRelayPin, LOW); // Turn the relay on (which powers the 12V LED)
    //Serial.println("Bike Stopped: Relay ON");
   // delay(1000);
  } else {
    isBikeStopped = false;
    digitalWrite(stopRelayPin, HIGH); // Turn the relay off (which turns off the 12V LED)
    //Serial.println("Bike Moving: Relay OFF");
   // delay(2000);
  }

  previousLdrValue = ldrValue; // Update the previous LDR value for the next comparison

  // Debug output
  //Serial.println("Direction: " + direction);

  delay(50); // Adjust the delay as needed
}

// Function to monitor the timer value in Firebase
void monitorTimer() {
  String path = "/bikeRentalSystem/timer";

  // Set up a listener for changes to the timer value
  if (!Firebase.beginStream(firebaseData, path)) {
    //Serial.println("Failed to start stream: " + firebaseData.errorReason());
  } else {
    Firebase.setStreamCallback(firebaseData, streamCallback, streamTimeoutCallback);
  }
}

// Callback function to handle timer changes
void streamCallback(StreamData data) {
  if (data.dataType() == "int") {
    int timerValue = data.intData();
    if (timerValue == 1) {
      Serial.println("lock");
    } else if (timerValue == 0) {
      Serial.println("unlock");
    }
  } else {
   // Serial.println("Unexpected data type.");
  }
}

// Callback function for stream timeout
void streamTimeoutCallback(bool timeout) {
  if (timeout) {
    //Serial.println("Stream timed out, resuming...");
    monitorTimer(); // Re-establish the stream if it times out
  }
}