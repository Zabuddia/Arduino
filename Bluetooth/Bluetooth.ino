#include "Arduino.h"
#include <SoftwareSerial.h>
#include <NewPing.h>

#define GO_RIGHT 2
#define GO_LEFT 4
#define GO_FORWARD 1
#define GO_BACKWARD 3

//Collision detector
#define COLLISION_INPUT A4
#define TRIGGER_PIN 12
#define ECHO_PIN 10
#define MAX_DISTANCE 200
#define DETECT_COLLISION 10
#define COLLISION_ON   0
#define COLLISION_OFF  1

int SensedCollision = 0;
int ActionCollision = COLLISION_OFF;
static NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

#define H_BRIDGE_ENA 5
#define H_BRIDGE_ENB 3
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11

//Motor speed definitions
#define SPEED_STOP      0
#define SPEED_LOW       (int) (255 * 0.45)
#define SPEED_MED       (int) (255 * 0.75)
#define SPEED_HIGH      (int) (255 * 1)
int ActionRobotSpeed = SPEED_HIGH;

//Driving direction definitions
#define DRIVE_STOP      0
#define DRIVE_LEFT      1
#define DRIVE_RIGHT     2
#define DRIVE_STRAIGHT  3
#define DRIVE_BACKWARD  4
int ActionRobotDrive = DRIVE_STOP;

int LeftButtonPushed = 0;
int RightButtonPushed = 0;
int UpButtonPushed = 0;
int DownButtonPushed = 0;

int ChangeSpeed = 0;

SoftwareSerial BTSerial(0, 1); // RX | TX

void setup() {
  Serial.begin(9600);
  BTSerial.begin(38400); // HC-05 default speed in AT command more

  pinMode(H_BRIDGE_ENA, OUTPUT);
  pinMode(H_BRIDGE_ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  RobotPerception();
  RobotPlanning();
  RobotAction();
}

void RobotPerception() {

  if (BTSerial.available()) {
    char c = BTSerial.read();
    Serial.println(c);
    switch (c) {
      case '1':
        UpButtonPushed = 1;
        ChangeSpeed = 0;
        break;
      case '2':
        RightButtonPushed = 1;
        ChangeSpeed = 0;
        break;
      case '3':
        DownButtonPushed = 1;
        ChangeSpeed = 0;
        break;
      case '4':
        LeftButtonPushed = 1;
        ChangeSpeed = 0;
        break;
      case '0':
        UpButtonPushed = 0;
        RightButtonPushed = 0;
        DownButtonPushed = 0;
        LeftButtonPushed = 0;
        ChangeSpeed = 0;
        break;
      case '5':
        ChangeSpeed = 1;
      default:
        // Optional: Handle any other characters or do nothing
        break;
    }
  }
  
  //Collision Sensor
  if (isCollision()) {
    SensedCollision = 1;
  } else {
    SensedCollision = 0;
  }
}

bool isCollision() {
  int sonar_distance = sonar.ping_cm();
  if(sonar_distance != 0){ 
    return (sonar_distance < DETECT_COLLISION);
  } else {
    return false;
  }
}

void RobotPlanning(void) {
  fsmCollisionDetection();
  fsmSpeedControl();
}

void fsmCollisionDetection() {
  static int collisionDetectionState = 0;
 
  switch (collisionDetectionState) {
    case 0:
      ActionCollision = COLLISION_ON;
      ActionRobotDrive = DRIVE_STOP;
      fsmSteerRobot(1);
      if (SensedCollision == 0) {
        collisionDetectionState = 1;
      }
      break;
    case 1:
      ActionCollision = COLLISION_OFF;
      fsmSteerRobot(0);
      if (SensedCollision == 1) {
        collisionDetectionState = 0;
      }
      break;
    default:
      {
        collisionDetectionState = 0;
      }
      break;
  }
}

void fsmSteerRobot(int collision) {
  static int steerRobotState = 0;
  switch (steerRobotState) {
    case 0:
      ActionRobotDrive = DRIVE_STOP;
      if (collision == 1) {
        if (LeftButtonPushed == 1) {
          steerRobotState = 0;
        } else if (RightButtonPushed == 1) {
          steerRobotState = 0;
        } else if (UpButtonPushed == 1) {
          steerRobotState = 0;
        } else if (DownButtonPushed == 1) {
          steerRobotState = 4;
        }
      } else {
        if (LeftButtonPushed == 1) {
          steerRobotState = 1;
        } else if (RightButtonPushed == 1) {
          steerRobotState = 2;
        } else if (UpButtonPushed == 1) {
          steerRobotState = 3;
        } else if (DownButtonPushed == 1) {
          steerRobotState = 4;
        }
      }
      break;
    case 1:
      ActionRobotDrive = DRIVE_LEFT;
      if (LeftButtonPushed == 0) {
        steerRobotState = 0;
      }
      break;
    case 2:
      ActionRobotDrive = DRIVE_RIGHT;
      if (RightButtonPushed == 0) {
        steerRobotState = 0;
      }
      break;
    case 3:
      ActionRobotDrive = DRIVE_STRAIGHT;
      if (UpButtonPushed == 0) {
        steerRobotState = 0;
      } else if (RightButtonPushed == 1) {
        steerRobotState = 2;
      } else if (LeftButtonPushed == 1) {
        steerRobotState = 1;
      }
      break;
    case 4:
      ActionRobotDrive = DRIVE_BACKWARD;
      if (DownButtonPushed == 0) {
        steerRobotState = 0;
      }
      break;
    default:
    {
      steerRobotState = 0;
    }
  }
}

void fsmSpeedControl() {
  static int toggleSpeedState = 0;

  switch (toggleSpeedState) {
    case 0:
      if (ChangeSpeed) {
        toggleSpeedState = 1;
      }
      break;
    case 1:
      if (!ChangeSpeed) {
        toggleSpeedState = 2;
      }
      break;
    case 2:
      fsmChangeSpeed();
      toggleSpeedState = 0;
      break;
  }
}

void fsmChangeSpeed() {
  static int changeSpeedState = 0;
 
  switch (changeSpeedState){
    case 0:
      ActionRobotSpeed = SPEED_STOP;
      changeSpeedState = 1;
      break;
    case 1:
      ActionRobotSpeed = SPEED_LOW;
      changeSpeedState = 2;
      break;
    case 2:
      ActionRobotSpeed = SPEED_MED;
      changeSpeedState = 3;
      break;
    case 3:
      ActionRobotSpeed = SPEED_HIGH;
      changeSpeedState = 0;
      break;
    default:{
      ActionRobotSpeed = SPEED_STOP;
    }
  }
}

void RobotAction() {
  switch(ActionRobotDrive) {
    case DRIVE_STOP:
      analogWrite(H_BRIDGE_ENA, 0);
      analogWrite(H_BRIDGE_ENB, 0);
      break;
    case DRIVE_STRAIGHT:
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      analogWrite(H_BRIDGE_ENA, ActionRobotSpeed);
      analogWrite(H_BRIDGE_ENB, ActionRobotSpeed);
      break;
    case DRIVE_RIGHT:
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      analogWrite(H_BRIDGE_ENA, ActionRobotSpeed);
      analogWrite(H_BRIDGE_ENB, ActionRobotSpeed);
      break;
    case DRIVE_LEFT:
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      analogWrite(H_BRIDGE_ENA, ActionRobotSpeed);
      analogWrite(H_BRIDGE_ENB, ActionRobotSpeed);
      break;
    case DRIVE_BACKWARD:
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      analogWrite(H_BRIDGE_ENA, ActionRobotSpeed);
      analogWrite(H_BRIDGE_ENB, ActionRobotSpeed);
  }
}
