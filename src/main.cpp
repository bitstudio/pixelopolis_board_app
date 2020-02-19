/*
 * Copyright 2020 Google LLC
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     https://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include "Dynamixels.h"
#include "serialCommunicate.h"


// Car ID
#define CAR_ID 2
// Dynamixels Parameters
#define XL430_RPM_PER_UNIT 0.229    // RPM per unit
// Car Parameters
#define LENGTH_BTW_WHEEL 0.136      // Meters
#define WHEEL_RADIUS 0.0335         // Meters

#define MOTOR_LEFT 0                // Mortor ID
#define MOTOR_RIGHT 1               // Mortor ID

using namespace std;


void getData();
void InitialDynamixel();
void ReleaseDynamixel();
// void irCalibration();
int getDistance(float irValue);

AnalogIn battery(A3);
AnalogIn ir(A4);
DigitalIn InitialButton(D10);
DigitalOut torqueEnableLED(D9);
Dynamixels XL430(PA_2, PA_15, 9600, D7);

serialCommunicate sCom;
Ticker ticker_;
Timer timer_sendBattery;

bool isReceive = false;
bool isCalibrate = false;
int initialDistance = 0;
uint16_t leftWheelSpeed, rightWheelSpeed;


int main() {
  int counter = 0;
  int distance = 0;
  int buttonState = 0;
  int lastButtonState = 0;

  timer_sendBattery.start();
  sCom.setup();

  // wait(2.0);
  // irCalibration();/

  while(true) {
    // Check press user button
    buttonState = InitialButton;
    if(buttonState != lastButtonState){
      if(buttonState == 0){
        counter++;
        if((counter % 2) !=0){
          InitialDynamixel();
          torqueEnableLED = 1;
        }
        else{
          ReleaseDynamixel();
          torqueEnableLED = 0;
        }
      }
    }
    lastButtonState = buttonState;

    // Receive command from PC
    sCom.update();
    if(sCom.isSerialReceive()){
        isReceive = true;
        sCom.setSerialReceive(false);
        getData();
    }

    // get distance between car and obstacles
    distance = getDistance(ir.read());
    // pc.printf("\nr%d\n", distance);

    // read voltage of battery from circuit
    // required: AREF connect 5v
    // send voltage of battery every 5 secound
    if(timer_sendBattery.read() > 5){
      pc.printf("\nb%d\n", int(battery.read() * 100));
      pc.printf("\nid%d\n", CAR_ID);
      timer_sendBattery.reset();
    }
  }
}


void InitialDynamixel(){
    pc.printf("Enable Torque Mode\n");
    XL430.EnableTorque(MOTOR_LEFT);
    XL430.EnableTorque(MOTOR_RIGHT);
    XL430.EnableLED(MOTOR_LEFT);
    XL430.EnableLED(MOTOR_RIGHT);
}


void ReleaseDynamixel(){
    pc.printf("Disable Torque Mode\n");
    XL430.DisableTorque(MOTOR_RIGHT);
    XL430.DisableTorque(MOTOR_LEFT);
    XL430.DisableLED(MOTOR_LEFT);
    XL430.DisableLED(MOTOR_RIGHT);
}


void getData(){

    leftWheelSpeed = (uint16_t)sCom.getLeftWheel();
    rightWheelSpeed = (uint16_t)sCom.getRightWheel();
    pc.printf("%d-%d || %d-%d\n",leftWheelSpeed,sCom.getDirection(0),rightWheelSpeed,sCom.getDirection(1));

    //bug when leftWheelSpeed !=0 && rightWheelSpeed = 0 motor will set left = 0
    if(leftWheelSpeed == 0 && rightWheelSpeed == 0){
      pc.printf("STOP LEFT RIGHT\n");
      XL430.SetGoalVelocity(MOTOR_RIGHT, 0, sCom.getDirection(1));
      XL430.SetGoalVelocity(MOTOR_LEFT, 0, sCom.getDirection(0));
    }
    else{
      XL430.SetGoalVelocity(MOTOR_RIGHT, rightWheelSpeed, sCom.getDirection(1));
      XL430.SetGoalVelocity(MOTOR_LEFT, leftWheelSpeed, sCom.getDirection(0));
    }
}


// void irCalibration() {
//   /* code */
//   for (size_t i = 0; i < 5; i++) {
//     int readDistance = getDistance(ir.read());
//     initialDistance = initialDistance + readDistance;
//     pc.printf("Calibrate with %d\n", readDistance);
//     wait(1.0);
//   }
//   initialDistance = int(initialDistance / 5);
//   isCalibrate = true;
//   pc.printf("initialDistance is %d\n", initialDistance);
//   wait(3.0);
// }


int getDistance(float irValue){
  int irDistance;
  float irVolts;

  // read voltage from ir sensor
  // assume maximum voltage is 5.0 volts
  irVolts = irValue * 5.0;
  // from datasheet graph and convert cm to mm
  irDistance = 130 * pow(irVolts, -1);
  // if (isCalibrate) {
  //   return initialDistance - irDistance;
  // }
  // else{
  //   return irDistance;
  // }
  return irDistance;
}
