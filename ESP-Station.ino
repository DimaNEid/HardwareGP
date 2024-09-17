// #include <WiFi.h>
// #include <FirebaseESP32.h>
// #include <SPI.h>
// #include <MFRC522.h>

// // Replace these with your network credentials
// #define WIFI_SSID "DamdoOmLab"
// #define WIFI_PASSWORD "58)00wX7"

// // Your Firebase project details
// #define FIREBASE_HOST "https://testdim-aa3cf-default-rtdb.firebaseio.com/"
// #define FIREBASE_AUTH "Yv7AHSdRYSyeVIXSKbm6lkLseSsI8MCJV9zta5js"

// // Define Firebase Data object
// FirebaseData firebaseData;
// FirebaseConfig config;

// // RFID and Ultrasonic Sensor Pins
// #define SS_PIN  5   // ESP32 pin GPIO5 
// #define RST_PIN 27  // ESP32 pin GPIO27 
// #define TRIG_PIN 4  // Pin for the ultrasonic sensor trigger
// #define ECHO_PIN 21  // Pin for the ultrasonic sensor echo

// // RFID instance
// MFRC522 rfid(SS_PIN, RST_PIN);

// // Variables for distance calculation
// long duration;
// float distance;
// String userID = "";
// String studentID = "";
// String studentName = "";
// String bikeID = "";
// String phoneNumber = ""; // To store the phone number
// String stationID = "station1"; // Hardcoded station ID

// void setup() {
//     Serial.begin(9600);
//     Serial2.begin(115200, SERIAL_8N1, 16, 17);  // Initialize Serial2 for communication with Arduino Mega

//     initializePeripherals();
//     connectToWiFi();
//     initializeFirebase();

//     Serial.println("System initialized and ready.");
// }

// void loop() {
//     handleSerialInput();
//     handleRFIDInput();
//     updateUltrasonicStatus();

//     delay(2000);  // Delay before next loop iteration
// }

// void initializePeripherals() {
//     initializeSPI();
//     initializeRFID();
//     initializeUltrasonic();
// }

// void initializeSPI() {
//     SPI.begin();
// }

// void initializeRFID() {
//     rfid.PCD_Init();
// }

// void initializeUltrasonic() {
//     pinMode(TRIG_PIN, OUTPUT);
//     pinMode(ECHO_PIN, INPUT);
// }

// void connectToWiFi() {
//     WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//     Serial.print("Connecting to Wi-Fi...");
//     while (WiFi.status() != WL_CONNECTED) {
//         delay(500);
//         Serial.print(".");
//     }
//     Serial.println(" connected");
// }

// void initializeFirebase() {
//     config.host = FIREBASE_HOST;
//     config.signer.tokens.legacy_token = FIREBASE_AUTH;
//     Firebase.begin(&config, NULL);
//     Firebase.reconnectWiFi(true);
// }

// void handleSerialInput() {
//     if (Serial2.available()) {
//         String receivedData = Serial2.readStringUntil('\n');
//         Serial.println("Received from Arduino Mega: " + receivedData);

//         if (isPhoneNumber(receivedData)) {
//             phoneNumber = extractPhoneNumber(receivedData);
//             Serial.println("Phone Number: " + phoneNumber);
//             lookupStudentInfoByPhone(phoneNumber);
//         } else {
//             userID = extractUID(receivedData);
//             lookupStudentInfoByRFID(userID);
//         }

//         if (isUserBikeRented(studentID)) {
//             denyAccess();
//         } else if (!studentID.isEmpty()) {
//             storeUserDataInFirebase();
//         }
//     }
// }

// bool isPhoneNumber(String data) {
//     return data.startsWith(",");
// }

// String extractPhoneNumber(String data) {
//     String phone = data.substring(1);
//     phone.trim(); // Remove any leading/trailing whitespace
//     return phone;
// }

// String extractUID(String data) {
//     int index = data.indexOf("Card UID: ");
//     if (index != -1) {
//         String uid = data.substring(index + 10); // Get the UID part
//         uid.trim();  // Remove any leading/trailing whitespace
//         uid.replace(" ", "");  // Remove spaces between hex values
//         uid.toUpperCase();  // Convert to uppercase to match Firebase entry
//         return uid;
//     }
//     return "";
// }

// void handleRFIDInput() {
//     if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
//         bikeID = getCardUID();
//         Serial.println("Bike ID (ESP32): " + bikeID);

//         storeStationDataInFirebase();
//         updateRentalStatusIfNeeded();

//         rfid.PICC_HaltA();
//         rfid.PCD_StopCrypto1();
//     }
// }

// String getCardUID() {
//     String uid = "";
//     for (int i = 0; i < rfid.uid.size; i++) {
//         uid += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
//         uid += String(rfid.uid.uidByte[i], HEX);
//     }
//     uid.toUpperCase();
//     return uid;
// }

// void lookupStudentInfoByRFID(String rfidTag) {
//     retrieveStudentInfo("/cardMappings/" + rfidTag);
// }

// void lookupStudentInfoByPhone(String phone) {
//     retrieveStudentInfo("/phoneMappings/" + phone);
// }

// void retrieveStudentInfo(String path) {
//     if (Firebase.get(firebaseData, path)) {
//         if (firebaseData.dataType() == "json") {
//             FirebaseJson& json = firebaseData.jsonObject();
//             FirebaseJsonData jsonData;

//             json.get(jsonData, "studentID");
//             if (jsonData.success) studentID = jsonData.stringValue;

//             json.get(jsonData, "name");
//             if (jsonData.success) studentName = jsonData.stringValue;

//             json.get(jsonData, "phoneNumber");
//             if (jsonData.success) phoneNumber = jsonData.stringValue;
//         }
//     } else {
//         Serial.println("Failed to retrieve student info: path: " + path + " reason: " + firebaseData.errorReason());
//         clearStudentInfo();
//     }
// }

// void clearStudentInfo() {
//     studentID = "";
//     studentName = "";
// }

// bool isUserBikeRented(String studentID) {
//     return getRentalStatus("/bikeRentalSystem/rented");
// }

// bool getRentalStatus(String path) {
//     bool rentedStatus = false;
//     if (Firebase.getBool(firebaseData, path)) {
//         rentedStatus = firebaseData.boolData();
//         Serial.println("Rented status: " + String(rentedStatus));
//     } else {
//         Serial.println("Failed to retrieve rented status from Firebase");
//     }
//     return rentedStatus;
// }

// void updateRentalStatusIfNeeded() {
//     String rentalBikeID = getRentalBikeID();
//     String stationBikeID = getStationBikeID();

//     if (rentalBikeID == stationBikeID && isUserBikeRented(studentID)) {
//         setRentalStatus(false);
//     }
// }

// String getRentalBikeID() {
//     return getStringData("/bikeRentalSystem/bikeID");
// }

// String getStationBikeID() {
//     return getStringData("/stations/" + stationID + "/bikeID");
// }

// String getStringData(String path) {
//     if (Firebase.getString(firebaseData, path)) {
//         return firebaseData.stringData();
//     } else {
//         Serial.println("Failed to retrieve data from Firebase");
//         return "";
//     }
// }

// void setRentalStatus(bool status) {
//     Firebase.setBool(firebaseData, "/bikeRentalSystem/rented", status);
//     Serial.println("Rental status set to " + String(status));
// }

// void updateUltrasonicStatus() {
//     measureDistance();
//     String status = determineStatus();
//     storeStationStatus(status);
// }

// void measureDistance() {
//     digitalWrite(TRIG_PIN, LOW);
//     delayMicroseconds(2);

//     digitalWrite(TRIG_PIN, HIGH);
//     delayMicroseconds(10);
//     digitalWrite(TRIG_PIN, LOW);

//     duration = pulseIn(ECHO_PIN, HIGH);
//     distance = duration * 0.034 / 2;

//    // Serial.print("Measured Distance: ");
//     //Serial.println(distance);
// }

// String determineStatus() {
//     return (distance < 10) ? "Not Available" : "Available";
// }

// void storeStationStatus(String status) {
//     String path = "/stations/" + stationID + "/";
//     Firebase.setString(firebaseData, path + "status", status);
//    // Serial.print("Station Status: ");
//     //Serial.println(status);
// }

// void storeUserDataInFirebase() {
//     storeStudentInfo();
//     storePhoneNumber();
//     storeBikeID();
// }

// void storeStudentInfo() {
//     if (!studentID.isEmpty()) {
//         Firebase.setString(firebaseData, "/bikeRentalSystem/studentID", studentID);
//         Firebase.setString(firebaseData, "/bikeRentalSystem/studentName", studentName);
//         Serial.println("Student ID and Name written to Firebase: " + studentID + ", " + studentName);
//     }
// }

// void storePhoneNumber() {
//     if (!phoneNumber.isEmpty()) {
//         Firebase.setString(firebaseData, "/bikeRentalSystem/phoneNumber", phoneNumber);
//         Serial.println("Phone Number written to Firebase: " + phoneNumber);
//     }
// }

// void storeBikeID() {
//     if (!bikeID.isEmpty()) {
//         Firebase.setString(firebaseData, "/bikeRentalSystem/bikeID", bikeID);
//         Serial.println("Bike ID written to Firebase: " + bikeID);
//         setRentalStatus(true);
//     }
// }

// void storeStationDataInFirebase() {
//     if (!bikeID.isEmpty()) {
//         Firebase.setString(firebaseData, "/stations/" + stationID + "/bikeID", bikeID);
//         Serial.println("Bike ID stored in station: " + bikeID);
//     }
// }

// void denyAccess() {
//     Serial.println("Access Denied: User already has a bike rented.");
// }
//**************
// #include <WiFi.h>
// #include <FirebaseESP32.h>
// #include <SPI.h>
// #include <MFRC522.h>

// // Replace these with your network credentials
// #define WIFI_SSID "DamdoOmLab"
// #define WIFI_PASSWORD "58)00wX7"

// // Your Firebase project details
// #define FIREBASE_HOST "https://testdim-aa3cf-default-rtdb.firebaseio.com/"
// #define FIREBASE_AUTH "Yv7AHSdRYSyeVIXSKbm6lkLseSsI8MCJV9zta5js"

// // Define Firebase Data object
// FirebaseData firebaseData;
// FirebaseConfig config;

// // RFID and Ultrasonic Sensor Pins
// #define SS_PIN  5   // ESP32 pin GPIO5 
// #define RST_PIN 27  // ESP32 pin GPIO27 
// #define TRIG_PIN 4  // Pin for the ultrasonic sensor trigger
// #define ECHO_PIN 21  // Pin for the ultrasonic sensor echo

// // RFID instance
// MFRC522 rfid(SS_PIN, RST_PIN);

// // Variables for distance calculation
// long duration;
// float distance;
// String userID = "";
// String studentID = "";
// String studentName = "";
// String bikeID = "";
// String phoneNumber = ""; // To store the phone number
// String stationID = "station1"; // Hardcoded station ID

// void setup() {
//     Serial.begin(9600);
//     Serial2.begin(115200, SERIAL_8N1, 16, 17);  // Initialize Serial2 for communication with Arduino Mega

//     initializePeripherals();
//     connectToWiFi();
//     initializeFirebase();

//     Serial.println("System initialized and ready.");
// }

// void loop() {
//     handleSerialInput();
//     handleRFIDInput();
//     updateUltrasonicStatus();

//     delay(2000);  // Delay before next loop iteration
// }

// void initializePeripherals() {
//     initializeSPI();
//     initializeRFID();
//     initializeUltrasonic();
// }

// void initializeSPI() {
//     SPI.begin();
// }

// void initializeRFID() {
//     rfid.PCD_Init();
// }

// void initializeUltrasonic() {
//     pinMode(TRIG_PIN, OUTPUT);
//     pinMode(ECHO_PIN, INPUT);
// }

// void connectToWiFi() {
//     WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//     Serial.print("Connecting to Wi-Fi...");
//     while (WiFi.status() != WL_CONNECTED) {
//         delay(500);
//         Serial.print(".");
//     }
//     Serial.println(" connected");
// }

// void initializeFirebase() {
//     config.host = FIREBASE_HOST;
//     config.signer.tokens.legacy_token = FIREBASE_AUTH;
//     Firebase.begin(&config, NULL);
//     Firebase.reconnectWiFi(true);
// }

// void handleSerialInput() {
//     if (Serial2.available()) {
//         String receivedData = Serial2.readStringUntil('\n');
//         Serial.println("Received from Arduino Mega: " + receivedData);

//         if (isPhoneNumber(receivedData)) {
//             phoneNumber = extractPhoneNumber(receivedData);
//             Serial.println("Phone Number: " + phoneNumber);
//             lookupStudentInfoByPhone(phoneNumber);
//         } else {
//             userID = extractUID(receivedData);
//             lookupStudentInfoByRFID(userID);
//         }

//         if (isUserBikeRented(studentID)) {
//             denyAccess();
//         } else if (!studentID.isEmpty()) {
//             allowAccess(); // Notify Arduino to allow access
//             storeUserDataInFirebase();
//         }
//     }
// }

// bool isPhoneNumber(String data) {
//     return data.startsWith(",");
// }

// String extractPhoneNumber(String data) {
//     String phone = data.substring(1);
//     phone.trim(); // Remove any leading/trailing whitespace
//     return phone;
// }

// String extractUID(String data) {
//     int index = data.indexOf("Card UID: ");
//     if (index != -1) {
//         String uid = data.substring(index + 10); // Get the UID part
//         uid.trim();  // Remove any leading/trailing whitespace
//         uid.replace(" ", "");  // Remove spaces between hex values
//         uid.toUpperCase();  // Convert to uppercase to match Firebase entry
//         return uid;
//     }
//     return "";
// }

// void handleRFIDInput() {
//     if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
//         bikeID = getCardUID();
//         Serial.println("Bike ID (ESP32): " + bikeID);

//         storeStationDataInFirebase();
//         updateRentalStatusIfNeeded();

//         rfid.PICC_HaltA();
//         rfid.PCD_StopCrypto1();
//     }
// }

// String getCardUID() {
//     String uid = "";
//     for (int i = 0; i < rfid.uid.size; i++) {
//         uid += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
//         uid += String(rfid.uid.uidByte[i], HEX);
//     }
//     uid.toUpperCase();
//     return uid;
// }

// void lookupStudentInfoByRFID(String rfidTag) {
//     retrieveStudentInfo("/cardMappings/" + rfidTag);
// }

// void lookupStudentInfoByPhone(String phone) {
//     retrieveStudentInfo("/phoneMappings/" + phone);
// }

// void retrieveStudentInfo(String path) {
//     if (Firebase.get(firebaseData, path)) {
//         if (firebaseData.dataType() == "json") {
//             FirebaseJson& json = firebaseData.jsonObject();
//             FirebaseJsonData jsonData;

//             json.get(jsonData, "studentID");
//             if (jsonData.success) studentID = jsonData.stringValue;

//             json.get(jsonData, "name");
//             if (jsonData.success) studentName = jsonData.stringValue;

//             json.get(jsonData, "phoneNumber");
//             if (jsonData.success) phoneNumber = jsonData.stringValue;
//         }
//     } else {
//         Serial.println("Failed to retrieve student info: path: " + path + " reason: " + firebaseData.errorReason());
//         clearStudentInfo();
//     }
// }

// void clearStudentInfo() {
//     studentID = "";
//     studentName = "";
// }

// bool isUserBikeRented(String studentID) {
//     return getRentalStatus("/bikeRentalSystem/rented");
// }

// bool getRentalStatus(String path) {
//     bool rentedStatus = false;
//     if (Firebase.getBool(firebaseData, path)) {
//         rentedStatus = firebaseData.boolData();
//         Serial.println("Rented status: " + String(rentedStatus));
//     } else {
//         Serial.println("Failed to retrieve rented status from Firebase");
//     }
//     return rentedStatus;
// }

// void updateRentalStatusIfNeeded() {
//     String rentalBikeID = getRentalBikeID();
//     String stationBikeID = getStationBikeID();

//     if (rentalBikeID == stationBikeID || isUserBikeRented(studentID)) {
//         // Set the rented status to false because the bike has been returned
//         setRentalStatus(false);
//         allowAccess();  // Notify Arduino to allow access
//     } 
// }


// String getRentalBikeID() {
//     return getStringData("/bikeRentalSystem/bikeID");
// }

// String getStationBikeID() {
//     return getStringData("/stations/" + stationID + "/bikeID");
// }

// String getStringData(String path) {
//     if (Firebase.getString(firebaseData, path)) {
//         return firebaseData.stringData();
//     } else {
//         Serial.println("Failed to retrieve data from Firebase");
//         return "";
//     }
// }

// void setRentalStatus(bool status) {
//     Firebase.setBool(firebaseData, "/bikeRentalSystem/rented", status);
//     Serial.println("Rental status set to " + String(status));
// }

// void updateUltrasonicStatus() {
//     measureDistance();
//     String status = determineStatus();
//     storeStationStatus(status);
// }

// void measureDistance() {
//     digitalWrite(TRIG_PIN, LOW);
//     delayMicroseconds(2);

//     digitalWrite(TRIG_PIN, HIGH);
//     delayMicroseconds(10);
//     digitalWrite(TRIG_PIN, LOW);

//     duration = pulseIn(ECHO_PIN, HIGH);
//     distance = duration * 0.034 / 2;
// }

// String determineStatus() {
//     return (distance < 10) ? "Not Available" : "Available";
// }

// void storeStationStatus(String status) {
//     String path = "/stations/" + stationID + "/";
//     Firebase.setString(firebaseData, path + "status", status);
// }

// void storeUserDataInFirebase() {
//     storeStudentInfo();
//     storePhoneNumber();
//     storeBikeID();
// }

// void storeStudentInfo() {
//     if (!studentID.isEmpty()) {
//         Firebase.setString(firebaseData, "/bikeRentalSystem/studentID", studentID);
//         Firebase.setString(firebaseData, "/bikeRentalSystem/studentName", studentName);
//         Serial.println("Student ID and Name written to Firebase: " + studentID + ", " + studentName);
//     }
// }

// void storePhoneNumber() {
//     if (!phoneNumber.isEmpty()) {
//         Firebase.setString(firebaseData, "/bikeRentalSystem/phoneNumber", phoneNumber);
//         Serial.println("Phone Number written to Firebase: " + phoneNumber);
//     }
// }

// void storeBikeID() {
//     if (!bikeID.isEmpty()) {
//         Firebase.setString(firebaseData, "/bikeRentalSystem/bikeID", bikeID);
//         Serial.println("Bike ID written to Firebase: " + bikeID);
//         setRentalStatus(true);
//     }
// }

// void storeStationDataInFirebase() {
//     if (!bikeID.isEmpty()) {
//         Firebase.setString(firebaseData, "/stations/" + stationID + "/bikeID", bikeID);
//         Serial.println("Bike ID stored in station: " + bikeID);
//     }
// }

// void denyAccess() {
//     Serial.println("Access Denied: User already has a bike rented.");
//     Serial2.println("DENY_ACCESS"); // Notify Arduino to deny access
//     Serial.println("deny");
// }

// void allowAccess() {
//     Serial2.println("ALLOW_ACCESS"); // Notify Arduino to allow access
//     Serial.println("allow");
// }
//*******
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <SPI.h>
#include <MFRC522.h>

// Replace these with your network credentials
#define WIFI_SSID "DamdoOmLab"
#define WIFI_PASSWORD "58)00wX7"

// Your Firebase project details
#define FIREBASE_HOST "https://testdim-aa3cf-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "Yv7AHSdRYSyeVIXSKbm6lkLseSsI8MCJV9zta5js"

// Define Firebase Data object
FirebaseData firebaseData;
FirebaseConfig config;

// RFID and Ultrasonic Sensor Pins
#define SS_PIN  5   // ESP32 pin GPIO5 

#define RST_PIN 27  // ESP32 pin GPIO27 
#define TRIG_PIN 4  // Pin for the ultrasonic sensor trigger
#define ECHO_PIN 21  // Pin for the ultrasonic sensor echo

// RFID instance
MFRC522 rfid(SS_PIN, RST_PIN);

// Variables for distance calculation
long duration;
float distance;
String userID = "";
String studentID = "";
String studentName = "";
String bikeID = "";
String phoneNumber = ""; // To store the phone number
String stationID = "station1"; // Hardcoded station ID

// Variable to track the last command sent to the Arduino
String lastCommand = "";

void setup() {
    Serial.begin(9600);
    Serial2.begin(115200, SERIAL_8N1, 16, 17);  // Initialize Serial2 for communication with Arduino Mega

    initializePeripherals();
    connectToWiFi();
    initializeFirebase();

    Serial.println("System initialized and ready.");
}

void loop() {
    checkStationControl();
    handleSerialInput();
    handleRFIDInput();
    updateUltrasonicStatus();

    delay(2000);  // Delay before next loop iteration
}

void initializePeripherals() {
    initializeSPI();
    initializeRFID();
    initializeUltrasonic();
}

void initializeSPI() {
    SPI.begin();
}

void initializeRFID() {
    rfid.PCD_Init();
}

void initializeUltrasonic() {
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

void connectToWiFi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" connected");
}

void initializeFirebase() {
    config.host = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&config, NULL);
    Firebase.reconnectWiFi(true);
}

void checkStationControl() {
    String controlPath = "/stations/" + stationID + "/stationAdminControl";

    // Retrieve the stationAdminControl status from Firebase
    Serial.println("Checking stationAdminControl node...");
    if (Firebase.getString(firebaseData, controlPath)) {
        String controlStatus = firebaseData.stringData();
        Serial.println("Station Control Status: " + controlStatus);

        // Only send the command to Arduino if the control status has changed
        if (controlStatus != lastCommand) {
            if (controlStatus == "Active") {
                // Send command to Arduino to activate
                Serial2.println("ACTIVATE_STATION");
                Serial.println("Sending ACTIVATE_STATION to Arduino.");
            } else if (controlStatus == "Deactivated") {
                // Send command to Arduino to deactivate
                Serial2.println("DEACTIVATE_STATION");
                Serial.println("Sending DEACTIVATE_STATION to Arduino.");
            }
            lastCommand = controlStatus; // Update the last command
        }
    } else {
        Serial.println("Failed to retrieve station control status: " + firebaseData.errorReason());
    }
}

void handleSerialInput() {
    if (Serial2.available()) {
        String receivedData = Serial2.readStringUntil('\n');
        Serial.println("Received from Arduino Mega: " + receivedData);

        if (isPhoneNumber(receivedData)) {
            phoneNumber = extractPhoneNumber(receivedData);
            Serial.println("Phone Number: " + phoneNumber);
            lookupStudentInfoByPhone(phoneNumber);
        } else {
            userID = extractUID(receivedData);
            lookupStudentInfoByRFID(userID);
        }

        if (isUserBikeRented(studentID)) {
            denyAccess();
        } else if (!studentID.isEmpty()) {
            storeUserDataInFirebase();
        }
    }
}

bool isPhoneNumber(String data) {
    return data.startsWith(",");
}

String extractPhoneNumber(String data) {
    String phone = data.substring(1);
    phone.trim(); // Remove any leading/trailing whitespace
    return phone;
}

String extractUID(String data) {
    int index = data.indexOf("Card UID: ");
    if (index != -1) {
        String uid = data.substring(index + 10); // Get the UID part
        uid.trim();  // Remove any leading/trailing whitespace
        uid.replace(" ", "");  // Remove spaces between hex values
        uid.toUpperCase();  // Convert to uppercase to match Firebase entry
        return uid;
    }
    return "";
}

void handleRFIDInput() {
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        bikeID = getCardUID();
        Serial.println("Bike ID (ESP32): " + bikeID);

        storeStationDataInFirebase();
        updateRentalStatusIfNeeded();

        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();
    }
}

String getCardUID() {
    String uid = "";
    for (int i = 0; i < rfid.uid.size; i++) {
        uid += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
        uid += String(rfid.uid.uidByte[i], HEX);
    }
    uid.toUpperCase();
    return uid;
}

void lookupStudentInfoByRFID(String rfidTag) {
    retrieveStudentInfo("/cardMappings/" + rfidTag);
}

void lookupStudentInfoByPhone(String phone) {
    retrieveStudentInfo("/phoneMappings/" + phone);
}

void retrieveStudentInfo(String path) {
    if (Firebase.get(firebaseData, path)) {
        if (firebaseData.dataType() == "json") {
            FirebaseJson& json = firebaseData.jsonObject();
            FirebaseJsonData jsonData;

            json.get(jsonData, "studentID");
            if (jsonData.success) studentID = jsonData.stringValue;

            json.get(jsonData, "name");
            if (jsonData.success) studentName = jsonData.stringValue;

            json.get(jsonData, "phoneNumber");
            if (jsonData.success) phoneNumber = jsonData.stringValue;
        }
    } else {
        Serial.println("Failed to retrieve student info: path: " + path + " reason: " + firebaseData.errorReason());
        clearStudentInfo();
    }
}

void clearStudentInfo() {
    studentID = "";
    studentName = "";
}

bool isUserBikeRented(String studentID) {
    return getRentalStatus("/bikeRentalSystem/rented");
}

bool getRentalStatus(String path) {
    bool rentedStatus = false;
    if (Firebase.getBool(firebaseData, path)) {
        rentedStatus = firebaseData.boolData();
        Serial.println("Rented status: " + String(rentedStatus));
    } else {
        Serial.println("Failed to retrieve rented status from Firebase");
    }
    return rentedStatus;
}

void updateRentalStatusIfNeeded() {
    String rentalBikeID = getRentalBikeID();
    String stationBikeID = getStationBikeID();

    if (rentalBikeID == stationBikeID && isUserBikeRented(studentID)) {
        setRentalStatus(false);
    }
}

String getRentalBikeID() {
    return getStringData("/bikeRentalSystem/bikeID");
}

String getStationBikeID() {
    return getStringData("/stations/" + stationID + "/bikeID");
}

String getStringData(String path) {
    if (Firebase.getString(firebaseData, path)) {
        return firebaseData.stringData();
    } else {
        Serial.println("Failed to retrieve data from Firebase");
        return "";
    }
}

void setRentalStatus(bool status) {
    Firebase.setBool(firebaseData, "/bikeRentalSystem/rented", status);
    Serial.println("Rental status set to " + String(status));
}

void updateUltrasonicStatus() {
    measureDistance();
    String status = determineStatus();
    storeStationStatus(status);
}

void measureDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    duration = pulseIn(ECHO_PIN, HIGH);
    distance = duration * 0.034 / 2;
}

String determineStatus() {
    return (distance < 10) ? "Available" : "Not Available";
}

// void storeStationStatus(String status) {
//     String path = "/stations/" + stationID + "/";
//     Firebase.setString(firebaseData, path + "status", status);
// }

void storeUserDataInFirebase() {
    storeStudentInfo();
    storePhoneNumber();
    storeBikeID();
}

void storeStudentInfo() {
    if (!studentID.isEmpty()) {
        Firebase.setString(firebaseData, "/bikeRentalSystem/studentID", studentID);
        Firebase.setString(firebaseData, "/bikeRentalSystem/studentName", studentName);
        Serial.println("Student ID and Name written to Firebase: " + studentID + ", " + studentName);
    }
}

void storePhoneNumber() {
    if (!phoneNumber.isEmpty()) {
        Firebase.setString(firebaseData, "/bikeRentalSystem/phoneNumber", phoneNumber);
        Serial.println("Phone Number written to Firebase: " + phoneNumber);
    }
}

void storeBikeID() {
    if (!bikeID.isEmpty()) {
        Firebase.setString(firebaseData, "/bikeRentalSystem/bikeID", bikeID);
        Serial.println("Bike ID written to Firebase: " + bikeID);
        setRentalStatus(true);
    }
}

void storeStationDataInFirebase() {
    if (!bikeID.isEmpty()) {
        Firebase.setString(firebaseData, "/stations/" + stationID + "/bikeID", bikeID);
        Serial.println("Bike ID stored in station: " + bikeID);
    }
}

void denyAccess() {
    Serial.println("Access Denied: User already has a bike rented.");
}

void storeStationStatus(String status) {
    String path = "/stations/" + stationID + "/";
    Firebase.setString(firebaseData, path + "status", status);
    Serial.println("Station Status: " + status);
    
    // Send the station status to Arduino
    if (status == "Not Available") {
        Serial2.println("STATION_NOT_AVAILABLE");
        Serial.println("Sending STATION_NOT_AVAILABLE to Arduino.");
    } else if (status == "Available") {
        Serial2.println("STATION_AVAILABLE");
        Serial.println("Sending STATION_AVAILABLE to Arduino.");
    }
}