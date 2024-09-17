// #include <Wire.h>
// #include <LiquidCrystal_I2C.h>
// #include <Keypad.h>
// #include <SoftwareSerial.h>
// #include <SPI.h>
// #include <MFRC522.h>

// // Pin Definitions
// #define SS_PIN 9
// #define RST_PIN 8
// #define GREEN_LED_PIN 31 // Green LED for access granted
// #define RED_LED_PIN 32   // Red LED for access denied
// #define KEYPAD_RELAY_PIN 30 // Relay control pin for keypad

// // RFID and LCD
// MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
// LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 columns and 2 rows

// // Keypad Configuration
// const byte ROWS = 4; // Four rows
// const byte COLS = 4; // Four columns
// char keys[ROWS][COLS] = {
//   { '1', '2', '3', 'A' },
//   { '4', '5', '6', 'B' },
//   { '7', '8', '9', 'C' },
//   { '*', '0', '#', 'D' }
// };
// byte rowPins[ROWS] = { 22, 23, 24, 25 }; // Connect to the row pinouts of the keypad
// byte colPins[COLS] = { 26, 27, 28, 29 }; // Connect to the column pinouts of the keypad
// Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// // Global Variables
// String generatedCode = "";
// String enteredCode = "";
// String phoneNumber = "";
// bool codeGenerated = false;
// bool enteringPhoneNumber = false;

// byte authorizedUID1[] = {0xF6, 0x18, 0x9B, 0xF7}; // Replace with your actual first authorized UID
// byte authorizedUID2[] = {0x21, 0xA9, 0x0D, 0x1E}; // Replace with your actual second authorized UID

// void setup() {
//   initializeComponents();
//   initializeSIM800L();
//   initializeRFID();
// }

// void loop() {
//   handleRFID();
//   handleKeypad();
// }

// // Initialization Functions
// void initializeComponents() {
//   pinMode(KEYPAD_RELAY_PIN, OUTPUT);
//   digitalWrite(KEYPAD_RELAY_PIN, LOW); // Ensure the relay is initially locked
//   pinMode(GREEN_LED_PIN, OUTPUT);
//   pinMode(RED_LED_PIN, OUTPUT);
//   digitalWrite(GREEN_LED_PIN, LOW);
//   digitalWrite(RED_LED_PIN, LOW);

//   Serial.begin(9600);
//   Serial3.begin(115200);  
//   lcd.init(); // Initialize the LCD
//   lcd.backlight();
//   displayMessage("your card or #", "for sms");
// }

// void initializeSIM800L() {
//   Serial1.begin(9600); // Start Serial1 for GSM module communication
//   sendATCommand("AT", 1000, "OK");
//   sendATCommand("AT+CMGF=1", 1000, "OK"); // Set the SMS mode to text
//   sendATCommand("AT+CSCS=\"GSM\"", 1000, "OK"); // Set the character set to GSM
// }

// void initializeRFID() {
//   SPI.begin(); // Initiate SPI bus
//   mfrc522.PCD_Init(); // Initiate MFRC522
//   Serial.println("Place your card on the reader...");
// }

// // RFID Handling
// void handleRFID() {
//   if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
//     String uid = getCardUID();
//     Serial3.println("Card UID: " + uid);

//     if (compareUID(mfrc522.uid.uidByte, mfrc522.uid.size, authorizedUID1, sizeof(authorizedUID1))) {
//       grantAccess("12029448");
//     } else if (compareUID(mfrc522.uid.uidByte, mfrc522.uid.size, authorizedUID2, sizeof(authorizedUID2))) {
//       grantAccess("12029305");
//     } else {
//       denyAccess();
//     }
//     delay(1000); // Wait before reading another card
//   }
// }

// String getCardUID() {
//   String uid = "";
//   for (byte i = 0; i < mfrc522.uid.size; i++) {
//     uid += String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
//     uid += String(mfrc522.uid.uidByte[i], HEX);
//   }
//   return uid;
// }

// bool compareUID(byte *uid, byte uidSize, byte *compareTo, byte compareSize) {
//   if (uidSize != compareSize) {
//     return false;
//   }
//   for (byte i = 0; i < uidSize; i++) {
//     if (uid[i] != compareTo[i]) {
//       return false;
//     }
//   }
//   return true;
// }

// // Keypad Handling
// void handleKeypad() {
//   char key = keypad.getKey();
//   if (key) {
//     Serial.print("Key pressed: ");
//     Serial.println(key);

//     if (key == '#') {
//       startPhoneNumberEntry();
//     } else if (key == '*') {
//       resetSystem();
//     } else if (enteringPhoneNumber) {
//       handlePhoneNumberEntry(key);
//     } else if (codeGenerated) {
//       handleCodeEntry(key);
//     }
//   }
// }

// void startPhoneNumberEntry() {
//   if (!codeGenerated && !enteringPhoneNumber) {
//     enteringPhoneNumber = true;
//     displayMessage("Enter phone num:", "");
//   }
// }

// void handlePhoneNumberEntry(char key) {
//   if (key == 'D') {
//     deleteLastDigit();
//   } else if (key == 'A') {
//     processPhoneNumber();
//   } else if (phoneNumber.length() < 15) { // Maximum phone number length
//     phoneNumber += key;
//     lcd.setCursor(0, 1);
//     lcd.print(phoneNumber);
//   }
// }

// void deleteLastDigit() {
//   if (phoneNumber.length() > 0) {
//     phoneNumber.remove(phoneNumber.length() - 1);
//     lcd.setCursor(0, 1);
//     lcd.print("                "); // Clear the line
//     lcd.setCursor(0, 1);
//     lcd.print(phoneNumber);
//   }
// }

// void processPhoneNumber() {
//   Serial3.print(",");
//   Serial3.println(phoneNumber);
//   if (phoneNumber.length() > 0) {
//     generatedCode = generateCode();
//     sendSMS();
//     displayMessage("Code sent to", "your phone");
//     codeGenerated = true;
//     enteringPhoneNumber = false;
//     phoneNumber = "";
//     delay(3000); // Wait 3 seconds to show the message
//     displayMessage("Enter code:", "");
//   }
// }

// void handleCodeEntry(char key) {
//   if (enteredCode.length() < 4) {
//     enteredCode += key;
//   }
//   displayMessage("Enter code:", enteredCode);

//   if (enteredCode.length() == 4) {
//     if (enteredCode == generatedCode) {
//       grantAccessWithKeypad();
//     } else {
//       denyAccess();
//     }
//   }
// }

// // Access Control
// void grantAccess(String bikeID) {
//   displayAccessGrantedMessage(bikeID);
//   unlockBike(KEYPAD_RELAY_PIN);
//   resetSystem();
// }

// void grantAccessWithKeypad() {
//   displayMessage("Access Granted", "");
//   unlockBike(KEYPAD_RELAY_PIN);
//   delay(2000); // Keep the lock open for 15 seconds
//   resetSystem();
// }

// void denyAccess() {
//   displayMessage("Access Denied", "");
//   digitalWrite(RED_LED_PIN, HIGH); // Turn on red LED
//   delay(2000);
//   resetSystem();
// }

// // Helper Functions
// void unlockBike(int pin) {
//   digitalWrite(pin, HIGH); // Unlock the bike (relay deactivated)
//   delay(2000); // Keep the lock open for 20 seconds
//   displayMessage("Take the bike", "^_^");
//   delay(8000);
//   digitalWrite(pin, LOW); // Re-lock the bike (relay activated)
// }

// String generateCode() {
//   String code = "";
//   for (int i = 0; i < 4; i++) {
//     code += String(random(0, 10)); // Generate a random digit between 0 and 9
//   }
//   return code;
// }

// void sendSMS() {
//   String message = "Your access code: " + generatedCode;
//   Serial1.println("AT+CMGS=\"" + phoneNumber + "\""); // Use entered phone number
//   delay(100);
//   Serial1.print(message);
//   delay(100);
//   Serial1.write(26); // ASCII code for CTRL+Z to send the SMS
//   delay(1000); // Wait for the message to be sent
// }

// bool sendATCommand(String command, int timeout, String expectedResponse) {
//   Serial.print("Sending command: ");
//   Serial.println(command);
//   Serial1.println(command);
//   long int time = millis();
//   while ((time + timeout) > millis()) {
//     while (Serial1.available()) {
//       String response = Serial1.readString();
//       Serial.print("Response: ");
//       Serial.println(response);
//       if (response.indexOf(expectedResponse) != -1) {
//         return true;
//       }
//     }
//   }
//   Serial.println("Command timed out.");
//   return false;
// }

// void resetSystem() {
//   enteredCode = "";
//   codeGenerated = false;
//   enteringPhoneNumber = false;
//   phoneNumber = "";
//   digitalWrite(GREEN_LED_PIN, LOW); // Turn off green LED
//   digitalWrite(RED_LED_PIN, LOW); // Turn off red LED
//   displayMessage("Your card or #", "for sms");
// }

// void displayMessage(const String &line1, const String &line2) {
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print(line1);
//   lcd.setCursor(0, 1);
//   lcd.print(line2);
// }

// void displayAccessGrantedMessage(String bikeID) {
//   displayMessage("hello ", bikeID);
//   delay(2000); // Wait for 2 seconds
//   displayMessage("Take the ", "Bike ^_^");
//   digitalWrite(GREEN_LED_PIN, HIGH);
//   unlockBike(KEYPAD_RELAY_PIN);
//   delay(8000); // Display the second message for 8 seconds
// }
//*********************************
// #include <Wire.h>
// #include <LiquidCrystal_I2C.h>
// #include <Keypad.h>
// #include <SPI.h>
// #include <MFRC522.h>

// // Pin Definitions
// #define SS_PIN 9
// #define RST_PIN 8
// #define GREEN_LED_PIN 31 // Green LED for access granted
// #define RED_LED_PIN 32   // Red LED for access denied
// #define KEYPAD_RELAY_PIN 30 // Relay control pin for keypad

// // RFID and LCD
// MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
// LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 columns and 2 rows

// // Keypad Configuration
// const byte ROWS = 4; // Four rows
// const byte COLS = 4; // Four columns
// char keys[ROWS][COLS] = {
//   { '1', '2', '3', 'A' },
//   { '4', '5', '6', 'B' },
//   { '7', '8', '9', 'C' },
//   { '*', '0', '#', 'D' }
// };
// byte rowPins[ROWS] = { 22, 23, 24, 25 }; // Connect to the row pinouts of the keypad
// byte colPins[COLS] = { 26, 27, 28, 29 }; // Connect to the column pinouts of the keypad
// Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// // Global Variables
// String generatedCode = "";
// String enteredCode = "";
// String phoneNumber = "";
// bool codeGenerated = false;
// bool enteringPhoneNumber = false;
// bool accessAllowed = false; // Track access permission

// byte authorizedUID1[] = {0xF6, 0x18, 0x9B, 0xF7}; // Replace with your actual first authorized UID
// byte authorizedUID2[] = {0x21, 0xA9, 0x0D, 0x1E}; // Replace with your actual second authorized UID

// void setup() {
//   initializeComponents();
//   initializeSIM800L();
//   initializeRFID();

//   // Initialize Serial communication for ESP32
//   Serial3.begin(115200);
// }

// void loop() {
//   checkESPCommunication(); // Check if ESP32 has sent any commands
//   handleRFID();
//   handleKeypad();
// }

// // Initialization Functions
// void initializeComponents() {
//   pinMode(KEYPAD_RELAY_PIN, OUTPUT);
//   digitalWrite(KEYPAD_RELAY_PIN, LOW); // Ensure the relay is initially locked
//   pinMode(GREEN_LED_PIN, OUTPUT);
//   pinMode(RED_LED_PIN, OUTPUT);
//   digitalWrite(GREEN_LED_PIN, LOW);
//   digitalWrite(RED_LED_PIN, LOW);

//   Serial.begin(9600);
//   lcd.init(); // Initialize the LCD
//   lcd.backlight();
//   displayMessage("your card or #", "for sms");
// }

// void initializeSIM800L() {
//   Serial1.begin(9600); // Start Serial1 for GSM module communication
//   sendATCommand("AT", 1000, "OK");
//   sendATCommand("AT+CMGF=1", 1000, "OK"); // Set the SMS mode to text
//   sendATCommand("AT+CSCS=\"GSM\"", 1000, "OK"); // Set the character set to GSM
// }

// void initializeRFID() {
//   SPI.begin(); // Initiate SPI bus
//   mfrc522.PCD_Init(); // Initiate MFRC522
//   Serial.println("Place your card on the reader...");
// }

// void checkESPCommunication() {
//   if (Serial3.available()) {
//     String message = Serial3.readStringUntil('\n');
//     if (message == "DENY_ACCESS") {
//       accessAllowed = false;
//       denyAccess();
//       Serial.println("deny");
//     } else if (message == "ALLOW_ACCESS") {
//       accessAllowed = true;
//       Serial.println("allow");
//     }
//   }
  
// }

// // RFID Handling
// void handleRFID() {
//     if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
//         String uid = getCardUID();
//         uid.toUpperCase();  // Convert the UID to uppercase
//         Serial3.println("Card UID: " + uid);

//         if (compareUID(mfrc522.uid.uidByte, mfrc522.uid.size, authorizedUID1, sizeof(authorizedUID1))) {
//             grantAccess("12029448");
//         } else if (compareUID(mfrc522.uid.uidByte, mfrc522.uid.size, authorizedUID2, sizeof(authorizedUID2))) {
//             grantAccess("12029305");
//         } else {
//             denyAccess();
//             Serial.println("d1");

//         }
//         delay(1000); // Wait before reading another card
//     }
// }

// String getCardUID() {
//   String uid = "";
//   for (byte i = 0; i < mfrc522.uid.size; i++) {
//     uid += String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
//     uid += String(mfrc522.uid.uidByte[i], HEX);
//   }
//   return uid;
// }

// bool compareUID(byte *uid, byte uidSize, byte *compareTo, byte compareSize) {
//   if (uidSize != compareSize) {
//     return false;
//   }
//   for (byte i = 0; i < uidSize; i++) {
//     if (uid[i] != compareTo[i]) {
//       return false;
//     }
//   }
//   return true;
// }

// // Keypad Handling
// void handleKeypad() {
//   char key = keypad.getKey();
//   if (key) {
//     Serial.print("Key pressed: ");
//     Serial.println(key);

//     if (key == '#') {
//       startPhoneNumberEntry();
//     } else if (key == '*') {
//       resetSystem();
//     } else if (enteringPhoneNumber) {
//       handlePhoneNumberEntry(key);
//     } else if (codeGenerated) {
//       handleCodeEntry(key);
//     }
//   }
// }

// void startPhoneNumberEntry() {
//   if (!codeGenerated && !enteringPhoneNumber) {
//     enteringPhoneNumber = true;
//     displayMessage("Enter phone num:", "");
//   }
// }

// void handlePhoneNumberEntry(char key) {
//   if (key == 'D') {
//     deleteLastDigit();
//   } else if (key == 'A') {
//     processPhoneNumber();
//   } else if (phoneNumber.length() < 15) { // Maximum phone number length
//     phoneNumber += key;
//     lcd.setCursor(0, 1);
//     lcd.print(phoneNumber);
//   }
// }

// void deleteLastDigit() {
//   if (phoneNumber.length() > 0) {
//     phoneNumber.remove(phoneNumber.length() - 1);
//     lcd.setCursor(0, 1);
//     lcd.print("                "); // Clear the line
//     lcd.setCursor(0, 1);
//     lcd.print(phoneNumber);
//   }
// }

// void processPhoneNumber() {
//   Serial3.print(",");
//   Serial3.println(phoneNumber);
//   if (phoneNumber.length() > 0) {
//     generatedCode = generateCode();
//     sendSMS();
//     displayMessage("Code sent to", "your phone");
//     codeGenerated = true;
//     enteringPhoneNumber = false;
//     phoneNumber = "";
//     delay(3000); // Wait 3 seconds to show the message
//     displayMessage("Enter code:", "");
//   }
// }

// void handleCodeEntry(char key) {
//   if (enteredCode.length() < 4) {
//     enteredCode += key;
//   }
//   displayMessage("Enter code:", enteredCode);

//   if (enteredCode.length() == 4) {
//     if (enteredCode == generatedCode) {
//       grantAccessWithKeypad();
//     } else {
//       denyAccess();
//     }
//   }
// }

// // Access Control
// void grantAccess(String bikeID) {
//   if (accessAllowed) {
//     displayAccessGrantedMessage(bikeID);
//     unlockBike(KEYPAD_RELAY_PIN);
//     resetSystem();
//   } else {
//     denyAccess();
//     Serial.println("d2");
//   }
// }

// void grantAccessWithKeypad() {
//   if (accessAllowed) {
//     displayMessage("Access Granted", "");
//     unlockBike(KEYPAD_RELAY_PIN);
//     delay(2000); // Keep the lock open for 15 seconds
//     resetSystem();
//   } else {
//     denyAccess();
//   }
// }

// void denyAccess() {
//   displayMessage("Access Denied", "");
//   digitalWrite(RED_LED_PIN, HIGH); // Turn on red LED
//   delay(2000);
//   resetSystem();
// }

// // Helper Functions
// void unlockBike(int pin) {
//   digitalWrite(pin, HIGH); // Unlock the bike (relay deactivated)
//   delay(2000); // Keep the lock open for 20 seconds
//   displayMessage("Take the bike", "^_^");
//   delay(8000);
//   digitalWrite(pin, LOW); // Re-lock the bike (relay activated)
// }

// String generateCode() {
//   String code = "";
//   for (int i = 0; i < 4; i++) {
//     code += String(random(0, 10)); // Generate a random digit between 0 and 9
//   }
//   return code;
// }

// void sendSMS() {
//   String message = "Your access code: " + generatedCode;
//   Serial1.println("AT+CMGS=\"" + phoneNumber + "\""); // Use entered phone number
//   delay(100);
//   Serial1.print(message);
//   delay(100);
//   Serial1.write(26); // ASCII code for CTRL+Z to send the SMS
//   delay(1000); // Wait for the message to be sent
// }

// bool sendATCommand(String command, int timeout, String expectedResponse) {
//   Serial.print("Sending command: ");
//   Serial.println(command);
//   Serial1.println(command);
//   long int time = millis();
//   while ((time + timeout) > millis()) {
//     while (Serial1.available()) {
//       String response = Serial1.readString();
//       Serial.print("Response: ");
//       Serial.println(response);
//       if (response.indexOf(expectedResponse) != -1) {
//         return true;
//       }
//     }
//   }
//   Serial.println("Command timed out.");
//   return false;
// }

// void resetSystem() {
//   enteredCode = "";
//   codeGenerated = false;
//   enteringPhoneNumber = false;
//   phoneNumber = "";
//   accessAllowed = false; // Reset accessAllowed for the next cycle
//   digitalWrite(GREEN_LED_PIN, LOW); // Turn off green LED
//   digitalWrite(RED_LED_PIN, LOW); // Turn off red LED
//   displayMessage("Your card or #", "for sms");
// }

// void displayMessage(const String &line1, const String &line2) {
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print(line1);
//   lcd.setCursor(0, 1);
//   lcd.print(line2);
// }

// void displayAccessGrantedMessage(String bikeID) {
//   displayMessage("hello ", bikeID);
//   delay(2000); // Wait for 2 seconds
//   displayMessage("Take the ", "Bike ^_^");
//   digitalWrite(GREEN_LED_PIN, HIGH);
//   unlockBike(KEYPAD_RELAY_PIN);
//   delay(8000); // Display the second message for 8 seconds
// }
//************************
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>

// Pin Definitions
#define SS_PIN 9
#define RST_PIN 8
#define GREEN_LED_PIN 31 // Green LED for access granted
#define RED_LED_PIN 32   // Red LED for access denied
#define KEYPAD_RELAY_PIN 30 // Relay control pin for keypad

// RFID and LCD
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 columns and 2 rows

// Keypad Configuration
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 22, 23, 24, 25 }; // Connect to the row pinouts of the keypad
byte colPins[COLS] = { 26, 27, 28, 29 }; // Connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Global Variables
String generatedCode = "";
String enteredCode = "";
String phoneNumber = "";
bool codeGenerated = false;
bool enteringPhoneNumber = false;

byte authorizedUID1[] = {0xF6, 0x18, 0x9B, 0xF7}; // Replace with your actual first authorized UID
byte authorizedUID2[] = {0x21, 0xA9, 0x0D, 0x1E}; // Replace with your actual second authorized UID

// Variable to store the last received command from ESP32
String lastESPCommand = "";

void setup() {
  lastESPCommand = "ACTIVATE_STATION";
  initializeComponents();
  initializeSIM800L();
  initializeRFID();
  Serial3.begin(115200); // Communication with ESP32
}

void loop() {
  handleSerialInputFromESP();
  handleRFID();
  handleKeypad();
}

// Initialization Functions
void initializeComponents() {
  pinMode(KEYPAD_RELAY_PIN, OUTPUT);
  digitalWrite(KEYPAD_RELAY_PIN, LOW); // Ensure the relay is initially locked
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);

  Serial.begin(9600);
  lcd.init(); // Initialize the LCD
  lcd.backlight();
  displayMessage("your card or #", "for sms");
}

void initializeSIM800L() {
  Serial1.begin(9600); // Start Serial1 for GSM module communication
  sendATCommand("AT", 1000, "OK");
  sendATCommand("AT+CMGF=1", 1000, "OK"); // Set the SMS mode to text
  sendATCommand("AT+CSCS=\"GSM\"", 1000, "OK"); // Set the character set to GSM
}

void initializeRFID() {
  SPI.begin(); // Initiate SPI bus
  mfrc522.PCD_Init(); // Initiate MFRC522
  Serial.println("Place your card on the reader...");
}

// Handle Serial Input from ESP32
void handleSerialInputFromESP() {
  if (Serial3.available()) {
    String receivedData = Serial3.readStringUntil('\n');
    receivedData.trim(); // Remove any trailing whitespace or newlines
    Serial.println("Received from ESP32: " + receivedData);

    if (receivedData == "ACTIVATE_STATION") {
      // Activate the station (e.g., turn on the relay, etc.)
      lastESPCommand = "ACTIVATE_STATION";
      displayMessage("Station Active", "");
      digitalWrite(GREEN_LED_PIN, HIGH); // Example action
      digitalWrite(RED_LED_PIN, LOW);
      delay(2000);
      resetSystem();
    } else if (receivedData == "DEACTIVATE_STATION") {
      // Deactivate the station (e.g., turn off the relay, etc.)
      lastESPCommand = "DEACTIVATE_STATION";
      displayMessage("Out of Service", "");
      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(RED_LED_PIN, HIGH); // Example action
    }
  }
}

// RFID Handling
void handleRFID() {
  // Only process RFID when the station is active
  if (lastESPCommand == "ACTIVATE_STATION") {
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      String uid = getCardUID();
      Serial3.println("Card UID: " + uid);

      if (compareUID(mfrc522.uid.uidByte, mfrc522.uid.size, authorizedUID1, sizeof(authorizedUID1))) {
        grantAccess("12029448");
      } else if (compareUID(mfrc522.uid.uidByte, mfrc522.uid.size, authorizedUID2, sizeof(authorizedUID2))) {
        grantAccess("12029305");
      } else {
        denyAccess();
      }
      delay(1000); // Wait before reading another card
    }
  }
}

String getCardUID() {
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  return uid;
}

bool compareUID(byte *uid, byte uidSize, byte *compareTo, byte compareSize) {
  if (uidSize != compareSize) {
    return false;
  }
  for (byte i = 0; i < uidSize; i++) {
    if (uid[i] != compareTo[i]) {
      return false;
    }
  }
  return true;
}

// Keypad Handling
void handleKeypad() {
  // Only process keypad when the station is active
  if (lastESPCommand == "ACTIVATE_STATION") {
    char key = keypad.getKey();
    if (key) {
      Serial.print("Key pressed: ");
      Serial.println(key);

      if (key == '#') {
        startPhoneNumberEntry();
      } else if (key == '*') {
        resetSystem();
      } else if (enteringPhoneNumber) {
        handlePhoneNumberEntry(key);
      } else if (codeGenerated) {
        handleCodeEntry(key);
      }
    }
  }
}

void startPhoneNumberEntry() {
  if (!codeGenerated && !enteringPhoneNumber) {
    enteringPhoneNumber = true;
    displayMessage("Enter phone num:", "");
  }
}

void handlePhoneNumberEntry(char key) {
  if (key == 'D') {
    deleteLastDigit();
  } else if (key == 'A') {
    processPhoneNumber();
  } else if (phoneNumber.length() < 15) { // Maximum phone number length
    phoneNumber += key;
    lcd.setCursor(0, 1);
    lcd.print(phoneNumber);
  }
}

void deleteLastDigit() {
  if (phoneNumber.length() > 0) {
    phoneNumber.remove(phoneNumber.length() - 1);
    lcd.setCursor(0, 1);
    lcd.print("                "); // Clear the line
    lcd.setCursor(0, 1);
    lcd.print(phoneNumber);
  }
}

void processPhoneNumber() {
  Serial3.print(",");
  Serial3.println(phoneNumber);
  if (phoneNumber.length() > 0) {
    generatedCode = generateCode();
    sendSMS();
    displayMessage("Code sent to", "your phone");
    codeGenerated = true;
    enteringPhoneNumber = false;
    phoneNumber = "";
    delay(3000); // Wait 3 seconds to show the message
    displayMessage("Enter code:", "");
  }
}

void handleCodeEntry(char key) {
  if (enteredCode.length() < 4) {
    enteredCode += key;
  }
  displayMessage("Enter code:", enteredCode);

  if (enteredCode.length() == 4) {
    if (enteredCode == generatedCode) {
      grantAccessWithKeypad();
    } else {
      denyAccess();
    }
  }
}

// Access Control
void grantAccess(String bikeID) {
  displayAccessGrantedMessage(bikeID);
 // unlockBike(KEYPAD_RELAY_PIN);
  resetSystem();
}

void grantAccessWithKeypad() {
  displayMessage("Access Granted", "");
  displayAccessGrantedMessage("12029448");
 // unlockBike(KEYPAD_RELAY_PIN);
  delay(2000); // Keep the lock open for 15 seconds
  resetSystem();
}

void denyAccess() {
  displayMessage("Access Denied", "");
  digitalWrite(RED_LED_PIN, HIGH); // Turn on red LED
  delay(2000);
  resetSystem();
}

// Helper Functions
void unlockBike(int pin) {
  digitalWrite(pin, HIGH); // Unlock the bike (relay deactivated)
  delay(2000); // Keep the lock open for 20 seconds
  displayMessage("Take the bike", "^_^");
 delay(8000);
  digitalWrite(pin, LOW); // Re-lock the bike (relay activated)
}

String generateCode() {
  String code = "";
  for (int i = 0; i < 4; i++) {
    code += String(random(0, 10)); // Generate a random digit between 0 and 9
  }
  return code;
}

void sendSMS() {
  String message = "Your access code: " + generatedCode;
  Serial1.println("AT+CMGS=\"" + phoneNumber + "\""); // Use entered phone number
  delay(100);
  Serial1.print(message);
  delay(100);
  Serial1.write(26); // ASCII code for CTRL+Z to send the SMS
  delay(1000); // Wait for the message to be sent
}

bool sendATCommand(String command, int timeout, String expectedResponse) {
  Serial.print("Sending command: ");
  Serial.println(command);
  Serial1.println(command);
  long int time = millis();
  while ((time + timeout) > millis()) {
    while (Serial1.available()) {
      String response = Serial1.readString();
      Serial.print("Response: ");
      Serial.println(response);
      if (response.indexOf(expectedResponse) != -1) {
        return true;
      }
    }
  }
  Serial.println("Command timed out.");
  return false;
}

void resetSystem() {
  enteredCode = "";
  codeGenerated = false;
  enteringPhoneNumber = false;
  phoneNumber = "";
  digitalWrite(GREEN_LED_PIN, LOW); // Turn off green LED
  digitalWrite(RED_LED_PIN, LOW); // Turn off red LED
  displayMessage("Your card or #", "for sms");
}

void displayMessage(const String &line1, const String &line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void displayAccessGrantedMessage(String bikeID) {
  displayMessage("hello ", bikeID);
  delay(2000); // Wait for 2 seconds
  displayMessage("Take the ", "Bike ^_^");
  digitalWrite(GREEN_LED_PIN, HIGH);
  unlockBike(KEYPAD_RELAY_PIN); resetSystem();
  delay(8000); // Display the second message for 8 seconds
 
  
}
