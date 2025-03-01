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

/* LED pins */
const int green_led_pin = 4;
const int blue_led_pin = 5;
const int yellow_led_pin = 6;
const int red_led_pin = 7;

/* For robot arm */
Servo myServo1;  // for joint 1
Servo myServo2;  // for joint 2

/* Joystick pins */
const int joy_x = A0;   // Joystick button X-axis, analog pin 0
const int joy_y = A1;   // Joystick button Y-axis, analog pin 1
const int button_pin = 2;   // Joystick button, digital pin 2

/* For laser sensor */
const int laser_pin = 3; // laser sensor pin 3

// variable for joystick values
int x_val = 0;
int y_val = 0;
int servo1_angle = 0;
int servo2_angle = 0;

// button state
bool last_button_state = HIGH;
bool current_button_state;

/* Initialization, must be done before run the main loop function */
void setup() {
  // robot arm servo to pins
  myServo1.attach(9);
  myServo2.attach(8);

  // scanning platform servo
  myServo.attach(12);        // Defines on which pin is the servo motor attached, analog pin 12

  // Joystick button pin
  pinMode(button_pin, INPUT_PULLUP);

  // laser sensor
  pinMode(laser_pin, OUTPUT);
  digitalWrite(laser_pin, LOW); // initialization, turning off the laser at the very beginning

  // sonar sensor
  pinMode(trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);   // Sets the echoPin as an Input
  
  // LED
  pinMode(green_led_pin, OUTPUT);
  pinMode(blue_led_pin, OUTPUT);
  pinMode(yellow_led_pin, OUTPUT);
  pinMode(red_led_pin, OUTPUT);

  Serial.begin(9600);        // Set Baut Rate.
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
    controlRobotArm();
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

    controlRobotArm();
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
  if (distance <= 10) { // very close to the obstacle! Mayday!!! Mayday!!!
    digitalWrite(red_led_pin, HIGH);
    digitalWrite(yellow_led_pin, LOW);
    digitalWrite(blue_led_pin, LOW);
    digitalWrite(green_led_pin, LOW);
  }
  else if(distance > 10 && distance <= 20) { // getting close to the obstacle! Danger!!
    digitalWrite(red_led_pin, LOW);
    digitalWrite(yellow_led_pin, HIGH);
    digitalWrite(blue_led_pin, LOW);
    digitalWrite(green_led_pin, LOW);
  }
  else if(distance > 20 && distance <= 30) { // An obstacle is proceeding, Alert ~
    digitalWrite(red_led_pin, LOW);
    digitalWrite(yellow_led_pin, LOW);
    digitalWrite(blue_led_pin, HIGH);
    digitalWrite(green_led_pin, LOW);
  }
  else { // maybe some object there, but far away from us, relax...
    digitalWrite(red_led_pin, LOW);
    digitalWrite(yellow_led_pin, LOW);
    digitalWrite(blue_led_pin, LOW);
    digitalWrite(green_led_pin, HIGH);
  }
}

// Robot arm control:
void controlRobotArm() {
   // Read joystick values
  x_val = analogRead(joy_x);
  y_val = analogRead(joy_y);

  // map joystick value to servo anagles(0-180 degs)
  servo1_angle = map(x_val, 0, 1023, 0, 180);
  servo2_angle = map(y_val, 0, 1023, 0, 180);

  // move servo to the new angles
  myServo1.write(servo1_angle);
  myServo2.write(servo2_angle);

  // current button state
  current_button_state = digitalRead(button_pin);

  // checkc whether button is pressed 
  if (last_button_state == HIGH && current_button_state == LOW) {
    // button was pressed
    Serial.println("Button Pressed!");
    // turning on the laser
    laserActivate(true);
  }
  else if(last_button_state == LOW && current_button_state == HIGH) {
    // button was pressed
    Serial.println("Button Released!");
    // turning on the laser
    laserActivate(false);
  }
  // Update the last_button_state:
  last_button_state = current_button_state;
  // Delay for stability
  delay(50);
}

// laser activate
void laserActivate(bool flag) {
  digitalWrite(laser_pin, flag ? HIGH : LOW); // turned on/off the laser pointer
  if (flag) {
    Serial.println("Laser Activated!");
  }
  else {
    Serial.println("Laser Deactivated!");
  }
  delay(50);
}
