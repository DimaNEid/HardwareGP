// #include <TM1637Display.h>
// #include <FirebaseESP32.h> // Or FirebaseESP8266 if using ESP8266

// // Define the pins for TM1637 display
// #define CLK 18  // Clock pin connected to GPIO18
// #define DIO 19  // Data pin connected to GPIO19

// TM1637Display display(CLK, DIO);

// // Firebase setup
// #define FIREBASE_HOST "https://testdim-aa3cf-default-rtdb.firebaseio.com/"
// #define FIREBASE_AUTH "Yv7AHSdRYSyeVIXSKbm6lkLseSsI8MCJV9zta5js"
// FirebaseData firebaseData;
// FirebaseConfig config;

// String counterTimer = ""; // Variable to store timer string from Firebase

// void setup() {
//   Serial.begin(115200);
  
//   // Initialize the TM1637 display
//   display.setBrightness(0x0f);  // Set brightness level
//   display.clear();  // Clear the display

//   // Initialize Firebase connection
//   config.host = FIREBASE_HOST;
//   config.signer.tokens.legacy_token = FIREBASE_AUTH;
//   Firebase.begin(&config, &firebaseData);
//   Firebase.reconnectWiFi(true);
// }

// void loop() {
//   // Retrieve the timer value from Firebase (assuming the path is '/counterTimer')
//   if (Firebase.getString(firebaseData, "/counterTimer")) {
//     counterTimer = firebaseData.stringData();
//     Serial.println("Counter Timer from Firebase: " + counterTimer);
    
//     // Extract minutes and seconds from the "00:00" format
//     int minutes = counterTimer.substring(0, 2).toInt();
//     int seconds = counterTimer.substring(3, 5).toInt();
    
//     // Display on the TM1637 7-segment display
//     displayTime(minutes, seconds);
//   } else {
//     Serial.println("Failed to retrieve timer value from Firebase: " + firebaseData.errorReason());
//   }

//   delay(10000);  // Fetch the timer every 10 seconds
// }

// void displayTime(int minutes, int seconds) {
//   // Combine minutes and seconds into a single value
//   int displayValue = (minutes * 100) + seconds; // Format as MMSS

//   // Display the value on the 7-segment display in MM:SS format
//   display.showNumberDecEx(displayValue, 0x40, true); // Display in MM:SS format (0x40 for colon ":")
// }
//*****************************
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <TM1637Display.h> // For the TM1637 Display

// Firebase project details
#define FIREBASE_HOST "https://testdim-aa3cf-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "Yv7AHSdRYSyeVIXSKbm6lkLseSsI8MCJV9zta5js"

// Replace with your network credentials
#define WIFI_SSID "DamdoOmLab"
#define WIFI_PASSWORD "58)00wX7"

// Firebase objects
FirebaseData firebaseData;
FirebaseConfig config;
FirebaseAuth auth;  // FirebaseAuth object added


// Pin definitions for the TM1637 Display
#define CLK_PIN  18  // Example GPIO for CLK
#define DIO_PIN  19  // Example GPIO for DIO

TM1637Display display(CLK_PIN, DIO_PIN);

void setup() {
  Serial.begin(9600);

  // Initialize Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to Wi-Fi");

  // Initialize Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);  // Now passing the auth object
  Firebase.reconnectWiFi(true);

  // Initialize TM1637 display
  display.setBrightness(0x0f);  // Set brightness to max
}

void loop() {
  // Reading the timerCounter data from Firebase
  if (Firebase.getString(firebaseData, "/bikeRentalSystem/timerCounter")) {
    String timerValue = firebaseData.stringData();
    Serial.println("Timer value from Firebase: " + timerValue);

    // Convert the time (HH:MM) to display on the TM1637
    if (timerValue.length() == 5 && timerValue.charAt(2) == ':') {
      int minutes = timerValue.substring(3).toInt();
      int hours = timerValue.substring(0, 2).toInt();

      // Display the hours and minutes
      display.showNumberDecEx(hours * 100 + minutes, 0b01000000, true);  // ":" separator on
    }
  } else {
    Serial.println("Failed to get value: " + firebaseData.errorReason());
  }

  delay(1000);  // Delay between reads
}
// #include <WiFi.h>
// #include <FirebaseESP32.h>
// #include <TM1637Display.h> // For the TM1637 Display

// // Firebase project details
// #define FIREBASE_HOST "https://testdim-aa3cf-default-rtdb.firebaseio.com/"
// #define FIREBASE_AUTH "Yv7AHSdRYSyeVIXSKbm6lkLseSsI8MCJV9zta5js"

// // Replace with your network credentials
// #define WIFI_SSID "DamdoOmLab"
// #define WIFI_PASSWORD "58)00wX7"

// // Firebase objects
// FirebaseData firebaseData;
// FirebaseConfig config;
// FirebaseAuth auth;  // FirebaseAuth object added

// // Pin definitions for the TM1637 Display
// #define CLK_PIN  18  // Example GPIO for CLK
// #define DIO_PIN  19  // Example GPIO for DIO

// TM1637Display display(CLK_PIN, DIO_PIN);

// // Variables to store last and current time
// String lastTimerValue = "";
// unsigned long lastUpdateTime = 0;  // To track time between updates

// void setup() {
//   Serial.begin(9600);

//   // Initialize Wi-Fi
//   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//   }

//   // Initialize Firebase
//   config.host = FIREBASE_HOST;
//   config.signer.tokens.legacy_token = FIREBASE_AUTH;
//   Firebase.begin(&config, &auth);  // Now passing the auth object
//   Firebase.reconnectWiFi(true);

//   // Initialize TM1637 display
//   display.setBrightness(0x0f);  // Set brightness to max
// }

// void loop() {
//   // Only check Firebase every 1 second
//   if (millis() - lastUpdateTime > 1000) {
//     lastUpdateTime = millis();  // Update last time we checked Firebase

//     // Reading the timerCounter data from Firebase
//     if (Firebase.getString(firebaseData, "/bikeRentalSystem/timerCounter")) {
//       String timerValue = firebaseData.stringData();

//       // Only update the display if the timer value has changed
//       if (timerValue != lastTimerValue) {
//         lastTimerValue = timerValue;  // Update the last known value

//         // Convert the time (HH:MM) to display on the TM1637
//         if (timerValue.length() == 5 && timerValue.charAt(2) == ':') {
//           int minutes = timerValue.substring(3).toInt();
//           int hours = timerValue.substring(0, 2).toInt();

//           // Display the hours and minutes on the 7-segment display
//           display.showNumberDecEx(hours * 100 + minutes, 0b01000000, true);  // ":" separator on
//         }
//       }
//     } else {
//       // Handle Firebase retrieval error if needed
//       Serial.println("Failed to get value: " + firebaseData.errorReason());
//     }
//   }
// }

