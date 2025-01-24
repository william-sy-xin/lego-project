// Includes the Servo library
#include <Servo.h>

/* For Sonar Sensor */
// Defines Tirg and Echo pins of the Ultrasonic Sensor
const int trigPin = 10;  // sending, analog pin 10
const int echoPin = 11;  // receiving, analog pin 11
// Variables for the duration and the distance
long duration;
int distance;

/* For Servo Motor */
Servo myServo;  // Creates a servo object for controlling the servo motor

// LED pins
const int green_led_pin = 4;
const int blue_led_pin = 5;
const int yellow_led_pin = 6;
const int red_led_pin = 7;

/* Initialization, must be done before run the main loop function */
void setup() {
  pinMode(trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);   // Sets the echoPin as an Input
  Serial.begin(9600);        // Set Baut Rate.
  myServo.attach(12);        // Defines on which pin is the servo motor attached, analog pin 12

  // initialize led pins as output
  pinMode(green_led_pin, OUTPUT);
  pinMode(blue_led_pin, OUTPUT);
  pinMode(yellow_led_pin, OUTPUT);
  pinMode(red_led_pin, OUTPUT);
}

void loop() {
  // rotates the servo motor from 15 to 165 degrees, 15 degs <= rotation <= 165 degs
  /* swing from left to right */
  for (int i = 15; i <= 165; i++) {
    myServo.write(i);
    delay(30);
    distance = calculateDistance();  // Calls a function for calculating the distance measured by the Ultrasonic sensor for each degree

    Serial.print(i);         // Sends the current degree into the Serial Port
    Serial.print(",");       // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
    Serial.print(distance);  // Sends the distance value into the Serial Port
    Serial.print(".");       // Sends addition character right next to the previous value needed later in the Processing IDE for indexing

    controlLEDs(distance);

    if (distance < 20) {
      trackingAndFollowingObject(i);
    }
  }

  // Repeats the previous lines from 165 to 15 degrees
  /* swing from right to left */
  for (int i = 165; i > 15; i--) {
    myServo.write(i);
    delay(30);
    distance = calculateDistance();
    Serial.print(i);
    Serial.print(",");
    Serial.print(distance);
    Serial.print(".");

    controlLEDs(distance);

    if (distance < 20) {
      trackingAndFollowingObject(i);
    }
  }
}

// Function for calculating the distance measured by the Ultrasonic sensor
int calculateDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);  // Reads the echoPin, returns the sound wave travel time in microseconds
  distance = duration * 0.034 / 2;
  return distance;
}

// tracking and following an object
void trackingAndFollowingObject(int current_angle) {
  while (true) {
    distance = calculateDistance();
    controlLEDs(distance);
    if (distance >= 20) {
      break;  // exit the loop if the object is no longer within 20 cm
    }
    // Adjust the servo to follow the object
    int new_angle = current_angle;
    if (distance < 20) {
      // move the servo to follow the object
      if (new_angle < 165) {
        new_angle++;
      }
      else {
        if (new_angle > 15) {
          new_angle--;
        }
      }
    }
    myServo.write(new_angle);
    delay(30);
    current_angle = new_angle;
  }
}

// control LEDs
void controlLEDs(int distance) {
  if (distance <= 10) {
    digitalWrite(red_led_pin, HIGH);
    digitalWrite(yellow_led_pin, LOW);
    digitalWrite(blue_led_pin, LOW);
    digitalWrite(green_led_pin, LOW);
  }
  else if(distance > 10 && distance <= 20) {
    digitalWrite(red_led_pin, LOW);
    digitalWrite(yellow_led_pin, HIGH);
    digitalWrite(blue_led_pin, LOW);
    digitalWrite(green_led_pin, LOW);
  }
  else if(distance > 20 && distance <= 30) {
    digitalWrite(red_led_pin, LOW);
    digitalWrite(yellow_led_pin, LOW);
    digitalWrite(blue_led_pin, HIGH);
    digitalWrite(green_led_pin, LOW);
  }
  else {
    digitalWrite(red_led_pin, LOW);
    digitalWrite(yellow_led_pin, LOW);
    digitalWrite(blue_led_pin, LOW);
    digitalWrite(green_led_pin, HIGH);
  }   
}
