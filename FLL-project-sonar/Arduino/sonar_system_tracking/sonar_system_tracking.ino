// Includes the Servo library
#include <Servo.h>

/* For Sonar Sensor */
// Defines Trig and Echo pins of the Ultrasonic Sensor
int trigPin = 10; // sending, analog pin 10
int echoPin = 11; // receiving, analog pin 11
// Variables for the duration and the distance
long duration = 0;
int distance = 0;

/* For Servo Motor */
Servo myServo; // Creates a servo object for controlling the servo motor

/* Initialization, must be done before running the main loop function */
void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);  // Sets the echoPin as an Input
  Serial.begin(9600);       // Set Baud Rate.
  myServo.attach(12);       // Defines on which pin the servo motor is attached, analog pin 12
}

void loop() {
  // rotates the servo motor from 15 to 165 degrees, 15 degs <= rotation <= 165 degs
  /* swing from left to right */
  for (int i = 15; i <= 165; i++) {
    if (checkObstacle()) {
      followObstacle();
      return; // Exit the loop to prevent further movement
    }
    myServo.write(i);
    delay(30);
    logServoPosition(i);
  }
  
  // Repeats the previous lines from 165 to 15 degrees
  /* swing from right to left */
  for (int i = 165; i > 15; i--) {
    if (checkObstacle()) {
      followObstacle();
      return; // Exit the loop to prevent further movement
    }
    myServo.write(i);
    delay(30);
    logServoPosition(i);
  }
}

// Function for calculating the distance measured by the Ultrasonic sensor
int calculateDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); // Reads the echoPin, returns the sound wave travel time in microseconds
  distance = duration * 0.034 / 2;
  return distance;
}

// Function to check for obstacles
bool checkObstacle() {
  distance = calculateDistance();
  Serial.print("Distance: ");
  Serial.println(distance);
  return distance < 20; // Change this threshold as needed (20 cm in this case)
}

// Function to follow the obstacle
void followObstacle() {
  while (true) {
    distance = calculateDistance();
    Serial.print("Following, Distance: ");
    Serial.println(distance);
    
    // Simple logic to move the servo based on distance
    if (distance < 20) {
      myServo.write(90); // Stop servo at center position when too close
    } else if (distance < 50) {
      myServo.write(165); // Move to right if the obstacle is far
    } else {
      myServo.write(15); // Move to left if the obstacle is very far
    }
    delay(500); // Adjust the delay as needed
    if (!checkObstacle()) {
      break; // Exit following when no obstacle is detected
    }
  }
}

// Function to log servo position and distance
void logServoPosition(int position) {
  Serial.print(position);
  Serial.print(",");
  Serial.print(distance);
  Serial.print(".");
}
