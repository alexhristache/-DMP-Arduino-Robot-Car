#include <SoftwareSerial.h>
#define rx 3
#define tx 2

#define trigPinR 11
#define echoPinR 4
#define trigPinL 13
#define echoPinL 12

#define dirPin A0

SoftwareSerial btConnection = SoftwareSerial(rx, tx);

int speedA = 5;
int dir1A = 6;
int dir2A = 7;

int speedB = 10;
int dir1B = 8;
int dir2B = 9;

int currSpeed, currSpeedA, currSpeedB;
bool way, newWay, wayA, wayB;
char btCmd;

void engineA(bool way, int currSpeed)
{
  if (way)
  {
    digitalWrite(dir1A, HIGH);
    digitalWrite(dir2A, LOW);
  } 
  else
  {
    digitalWrite(dir1A, LOW);
    digitalWrite(dir2A, HIGH);
  }
  if (currSpeed >= 0 && currSpeed <= 150)
  {
    analogWrite(speedA, currSpeed);
  } 
}

void engineB(bool way, int currSpeed)
{
  if (way)
  {
    digitalWrite(dir1B, HIGH);
    digitalWrite(dir2B, LOW);
  } 
  else
  {
    digitalWrite(dir1B, LOW);
    digitalWrite(dir2B, HIGH);
  }
  if (currSpeed >= 0 && currSpeed <= 150)
  {
    analogWrite(speedB, currSpeed);
  } 
}

long readDistance(bool side)
{
  long duration, distance;
  int trigPin, echoPin;
  if (side) // Right Side Sensor
  {
    trigPin = trigPinR;
    echoPin = echoPinR;
  }
  else // Left Side Sensor
  {
    trigPin = trigPinL;
    echoPin = echoPinL;
  }

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);

  distance = duration / 2 / 29.1;

  return min(distance, 55);
}

void setup() {
  Serial.begin(9600);
  btConnection.begin(9600);
  pinMode(echoPinL, INPUT);
  pinMode(echoPinR, INPUT);
  pinMode(trigPinR, OUTPUT);
  pinMode(trigPinL, OUTPUT);

  pinMode(dir1A, OUTPUT);
  pinMode(dir2A, OUTPUT);
  pinMode(speedA, OUTPUT);
  pinMode(dir1B, OUTPUT);
  pinMode(dir2B, OUTPUT);
  pinMode(speedB, OUTPUT);

  pinMode(dirPin, INPUT);
}

void loop() {
  
  float dist = 4800 / (analogRead(dirPin) - 20);
  Serial.println(dist);
  
  if (way && currSpeed != 0 && (readDistance(true) < 25 || readDistance(false) < 25 || dist < 25))
  {
    delay(25);
    currSpeed = 0;
    currSpeedA = currSpeedB = currSpeed;
  }

  else if (btConnection.available())
  {
    btCmd = btConnection.read();
    if (currSpeed == 0)
      way = newWay;
check_state:
    switch (btCmd)
    {
      case 'f': //inainte
        newWay = true;
        if (way == newWay)
        {
          if (currSpeed == 0)
          {
            currSpeed = 80;
          }
          else
          {
            currSpeed += 20;
          }
        }
        else if (currSpeed > 0)
        {
          currSpeed -= 20;
          if (currSpeed < 80)
          {
            currSpeed = 0;
          }
        }
        else
        {
          way = newWay;
          goto check_state;
        }
        currSpeedA = currSpeed;
        currSpeedB = currSpeed;
        wayA = wayB = way;
        break;

      case 'b':
        newWay = false;
        if (way == newWay)
        {
          if (currSpeed == 0)
          {
            currSpeed = 80;
          }
          else
          {
            currSpeed += 20;
          }
        }
        else if (currSpeed > 0)
        {
          currSpeed -= 20;
          if (currSpeed < 80)
          {
            currSpeed = 0;
          }
        }
        else
        {
          way = newWay;
          goto check_state;
        }
        currSpeedA = currSpeedB = currSpeed;
        wayA = wayB = way;
        break;
        
      case 'r':
        if (currSpeed == 0)
        {
          currSpeedA = 90;
          currSpeedB = 90;
          wayA = way;
          wayB = !way;
        }
        else
        {
          currSpeedA = 0;
          currSpeedB = currSpeed;
          wayA = wayB = way;
        }
        break;
        
      case 'l':

        if (currSpeed == 0)
        {
          currSpeedA = 90;
          currSpeedB = 90;
          wayA = !way;
          wayB = way;
        }
        else
        {
          currSpeedA = currSpeed;
          currSpeedB = 0;
          wayA = wayB = way;
        }
        break;
        
      case 's':
        currSpeed = 0;
        currSpeedA = currSpeedB = currSpeed;
        break;
    }
  }
  engineA(wayA, currSpeedA);
  engineB(wayB, currSpeedB);
}
