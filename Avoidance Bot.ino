//Shela Qiu and Vicky Xu
//Avoidance Bot
//Code for a car that moves forward and is able to detect a box and drive around it automatically
//Uses a mini Arduino Uno board with standard DC motors for the wheels,
//as well as an ultrasonic sensor attached to a servo motor
//Motor and ultrasonic sensor methods were obtained from templates

#include <Servo.h>

//Ultrasonic sensor 
int pin = 8; //Pin connection

//Motors
//Standard PWM DC control
int E1 = 5;     //M1 Speed Control
int E2 = 6;     //M2 Speed Control
int M1 = 4;    //M1 Direction Control
int M2 = 7;    //M1 Direction
int turn=700; //Delay for turning the car - the higher the number, the sharper the turn (calibrate to vehicle - should be a 90 degree turn)

//Servo motor
Servo myservo; // create servo object to control a servo
int pos = 0; // variable to store the servo position

void setup()
{
  Serial.begin(9600); //For tracking/error-checking purposes
  //Set output pins
  pinMode(pin, OUTPUT); //Ultrasonic sensor
  digitalWrite(pin, HIGH); // Trig pin is normally HIGH (ultrasonic sensor)
  for(int i=4;i<=7;i++) //Motors
    pinMode(i, OUTPUT);  
  
  myservo.attach(13);//Attach servo
}

void stop(void)                    //Motor - stop
{
  digitalWrite(E1, LOW); //Turns off both motors
  digitalWrite(E2, LOW);
}

//Send in speed of the motors
void right(char a, char b)         //Motors - turn Right
{
  analogWrite (E1, a);     //PWM Speed Control
  digitalWrite(M1, HIGH);
  analogWrite (E2, b);
  digitalWrite(M2, HIGH);
}


//Send in speed of the motors
void left(char a, char b)         //Motors - turn Left
{
  analogWrite (E1, a);
  digitalWrite(M1, LOW);
  analogWrite (E2, b);
  digitalWrite(M2, LOW);
}

void advance (char a, char b)            //Motors - move forward
{
  analogWrite (E1, a);
  digitalWrite(M1, LOW);
  analogWrite (E2, b);
  digitalWrite(M2, HIGH);
}

//Analyze input from ultrasonic sensor to determine range from nearest object
float distance(int pin)
{
  unsigned long time;
  unsigned long sizeofpulse;
  float range;

  pinMode(pin, OUTPUT); //return digital pin to OUTPUT mode after reading
  digitalWrite(pin, LOW);
  delayMicroseconds(25);
  digitalWrite(pin, HIGH); //Trig pin pulsed LOW for 25usec
  time = micros(); //record timer
  pinMode(pin, INPUT); //change pin to INPUT to read the echo pulse
  sizeofpulse = pulseIn(pin, LOW, 18000); //should be approx 150usec, timeout at 18msec
  time = micros() - time - sizeofpulse; // amount of time elapsed since we sent the trigger pulse and detect the echo pulse, then subtract the size of the echo pulse
  range = (time * 340.29 / 2 / 10000) - 3; // convert to distance in centimeters
  return range;
}

//Function to call when car hits a box
void wServo()
{
  //Counter to see how far along the box the car moves
  int counter=0;

  //Turn the motor to the left to detect the block as the car moves along it
  pos = 180;
  myservo.write(pos);
  delay(1000);

  //Turn the car right - the ultrasonic sensor should now be pointed at the box
  right(0,100);
  delay(turn);
  stop();
  delay(1000);

  //While the sensor detects the box, it means the car hasn't cleared it - keep moving forward
  Serial.println("Going along box");
  float range=distance(pin);
  while (range < 25)
  {
    range=distance(pin);
    Serial.println(range);
    advance(100, 101);
    counter++; //Count how long it moves forward for so car knows how far back it has to go on the other side
  }

  //After it's cleared the box, keep moving forward a bit so it can turn into open space after
  advance(100, 101);
  delay(500);
  stop();
  delay(1500);
  
  //Turn left to go along next side of box
  Serial.println("Turning left");
  left(100, 0);
  delay(turn);
  stop();
  delay(1500);

  //The ultrasonic sensor is still pointed at the box
  //Move forward a little to account for open space
  Serial.println("go forward");
  advance(100,101);
  delay(1500);

  //Keep moving forward until the car clears the next side of the block
  Serial.println("Going along next side of box");
  range=distance(pin);
  while (range < 25)
  {
    range=distance(pin);
    Serial.println(range);
    advance(100, 101);
  }
  
  //After it's cleared the box, keep moving forward a bit so it can turn into open space after
  advance(100, 101);
  delay(500);
  stop();
  delay(1500);

  //Turn left
  Serial.println("Turn left");
  left(100, 0);
  delay(turn);
  stop();
  delay(1500);

  //Go back to the middle of the box using the counter from last time
  Serial.println("Go to middle");
  advance(100, 101);
  delay(counter*10);
  stop();
  delay(1500);

  //Turn right - after this, the car is ready for the next obstacle, as it should be on the exact opposite side from where it started
  Serial.println("Go back");
  right(0, 100);
  delay(turn);
  stop();
}

void loop()
{
  
  Serial.println("Start - reset servo");
  //Set the servo to face forward
  pos = 90;
  myservo.write(pos);
  //Delay to allow user to place car in the course
  delay(5000);

  //Start detecting objects
  float range = distance(pin);
  Serial.println(range);
  Serial.println("Looking for box");
  //Keep moving forward until it reaches a block
  while(range>16)
  {
    range=distance(pin);
    Serial.println(range);
    advance(100,101); //Motor speeds are different to accomodate discrepancies in motor stengths
  }
  //Stop after each movement for more precise motor control (before and after turns)
  stop();
  delay(1000);

  //Go to method when a block is hit
  wServo();

}



