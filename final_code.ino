/*
* Object avoidance bot
* 
* Yipee
* 
* This robot sets the angle of the servo position (between 0 and 160 degrees) 
* to the maximum distance sensed by the ultrasonic distance sensor.
* The robot then checks the maximum distance to see where to turn (L < 80, R > 80)
* or whether to go straight. Troubleshooting tips would include checking PWM pins,
* descreasing/increasing latency in the ultrasonic distance sensor readings, 
* increasing/decreasing robot motor speed/delay and checking for syntax issues. 
*/

#include <Servo.h>

//motor pins
const int AIN1 = 13;           
const int AIN2 = 12;

//changed from 10 to 3 bcs servo library disables 10
const int PWMA = 11;            
const int PWMB = 3;           

const int BIN2 = 9;           
const int BIN1 = 8;           

//sensor pins
const int trigPin = 6;
const int echoPin = 5;

//pin for on/off switch
int switchPin = 7;

Servo myservo;

int servoPosition = 0;         //set init servo pos

//from example code
int backupTime = 200;           
int turnTime = 100;             
float distance = 0;

float measDist[160];

/********************************************************************************/

void setup() {
  
  pinMode(trigPin, OUTPUT);           //set the trig pin as output
  pinMode(echoPin, INPUT);            //set the echo pin as input
  pinMode(switchPin, INPUT_PULLUP);   //set the switch pin as input pull-up

  // Set motor control pins as outputs
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);

  myservo.attach(10); //set servo to pin 10

  Serial.begin(9600);  //begin serial communication
  Serial.print("Okaaaay let's go...");
}

/********************************************************************************/

void loop() {
  //read switch, LOW is on HIGH is off
  if (digitalRead(switchPin) == LOW) {
    
    //reset servo position to front
    myservo.write(90);

    //move forward a bit (default), slow
    rightMotor(60);
    leftMotor(66);
    delay(20);
    
    //read dist sensor
    distance = getDistance();
    
    //if object is within 10 cm
    if (distance < 13) {
      
      //halt! (pause) a vagrant
      rightMotor(0);
      leftMotor(0);
      delay(500);
      
      //find biggest distance
      int maxIndex = findMaxDist();
      
      //turn to biggest distance
      if (maxIndex < 80) {
        
        turnLeft();
      
      } else if (maxIndex > 80) {
        
        turnRight();

      } else {
        
        //by default move forward
        rightMotor(30);
        leftMotor(36);
      }
      
      //wait after turn to re-adjust sensor positioning
      delay(500);
    }
  } else {
    //switch not on LOW --> stop
    Serial.print("STOP the motors!");
    rightMotor(0);
    leftMotor(0);
  }
}

/********************************************************************************/

int findMaxDist() {
  
  int maxIndex = 0;         //index of the maximum distance
  float maxDistance = 0;    //the actual maximum distance

  //sweep servo from 0 to 160 degrees (not 180, as per booklet, puts too much stress on sensor)
  for (int pos = 160; pos >= 0; pos--) {
    
    myservo.write(pos);               //move servo to the current position
    delay(20);                        //delay by 20 ms 
    measDist[pos] = getDistance();    

    //check if distance is the maximum
    if (measDist[pos] > maxDistance) {
      
      maxDistance = measDist[pos];
      maxIndex = pos;
    }
  }

  return maxIndex;
}

/********************************************************************************/

float getDistance() {
  //send pulse to trigger ultrasonic sensor
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(8);
  digitalWrite(trigPin, LOW);

  //measure duration of pulse on echo pin
  float echoTime = pulseIn(echoPin, HIGH);

  //calculate the distance based on the time (from example code)
  float distance = echoTime / 148.0;  
  return distance;
}

/********************************************************************************/

void rightMotor(int motorSpeed) {

  if (motorSpeed > 0) {

    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);

  } else if (motorSpeed < 0) {

    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);

  } else {

    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);

  }

  analogWrite(PWMA, abs(motorSpeed));
}

/********************************************************************************/

void leftMotor(int motorSpeed) {

  if (motorSpeed > 0) {

    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);

  } else if (motorSpeed < 0) {

    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);

  } else {

    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);

  }

  analogWrite(PWMB, abs(motorSpeed));
}

/********************************************************************************/

void turnLeft() {

  rightMotor(50);
  leftMotor(-50);
  delay(500);

  rightMotor(0);
  leftMotor(0);
}

/********************************************************************************/

void turnRight() {

  rightMotor(-50);
  leftMotor(50);
  delay(500);
  
  rightMotor(0);
  leftMotor(0);
}

