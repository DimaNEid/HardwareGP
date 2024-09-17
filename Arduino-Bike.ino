// Define the pins for the stepper motor
const int dirPin = 9;   // Direction pin for the stepper motor
const int pulPin = 8;   // Pulse pin for the stepper motor
#define trigPin 10      // Trig pin for ultrasonic sensor
#define echoPin 11      // Echo pin for ultrasonic sensor
#define relayPin 12     // Relay pin for controlling 12V LED

void setup() {
  // Set the direction and pulse pins as outputs for the stepper motor
  pinMode(dirPin, OUTPUT);
  pinMode(pulPin, OUTPUT);
  
  // Set up ultrasonic sensor and relay
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relayPin, OUTPUT);

  // Initialize serial communication for debugging and communication with ESP32
  Serial.begin(9600);
  Serial.println("Arduino ready");
}

void loop() {
  // Check if a command is received from the ESP32
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    Serial.print("Received: ");
    Serial.println(command);
command.trim();
    if (command == "unlock") {
      open();   // Rotate clockwise for 3 seconds
      Serial.println("rotating ccw");
    } else if (command == "lock") {
      close();  // Rotate counterclockwise for 3 seconds
       Serial.println("rotating cw");
    }
  }

  // Measure the distance using the ultrasonic sensor
  long duration, distance;

  // Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  distance = (duration * 0.034) / 2;

  // Print the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);

  // Check if the distance is below the threshold (e.g., 65 cm)
  if (distance < 65) {
    // Turn on the relay (which will turn on the 12V LED)
    digitalWrite(relayPin, HIGH);
    delay(250);
    digitalWrite(relayPin, LOW);
    delay(250);
  } else {
    // Ensure the relay is off if the distance is above the threshold
    digitalWrite(relayPin, LOW);
  }

  delay(500); // Adjust the delay as needed
}

// Function to rotate the stepper motor clockwise for 3 seconds
void open() {
  digitalWrite(dirPin, LOW);  // Set direction to clockwise
  unsigned long startTime = millis();
  while (millis() - startTime < 3000) {  // Run for 3000 milliseconds (3 seconds)
    digitalWrite(pulPin, HIGH);
    delayMicroseconds(1000);  // Adjust this delay to control speed
    digitalWrite(pulPin, LOW);
    delayMicroseconds(1000);  // Adjust this delay to control speed
  }
}

// Function to rotate the stepper motor counterclockwise for 3 seconds
void close() {
  digitalWrite(dirPin, HIGH);  // Set direction to counterclockwise
  unsigned long startTime = millis();
  while (millis() - startTime < 3000) {  // Run for 3000 milliseconds (3 seconds)
    digitalWrite(pulPin, HIGH);
    delayMicroseconds(1000);  // Adjust this delay to control speed
    digitalWrite(pulPin, LOW);
    delayMicroseconds(1000);  // Adjust this delay to control speed
  }
}