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

#include "serialCommunicate.h"

void serialCommunicate::setup(){
    pc.baud(9600);
    isSerialReceive_ = false;
    direction[0] = 1;
    direction[0] = 1;
    inputInt[0] = 0;
    inputInt[1] = 0;
    map24baseNumber['0'] = 0;
    map24baseNumber['1'] = 1;
    map24baseNumber['2'] = 2;
    map24baseNumber['3'] = 3;
    map24baseNumber['4'] = 4;
    map24baseNumber['5'] = 5;
    map24baseNumber['6'] = 6;
    map24baseNumber['7'] = 7;
    map24baseNumber['8'] = 8;
    map24baseNumber['9'] = 9;
    map24baseNumber['a'] = 10;
    map24baseNumber['b'] = 11;
    map24baseNumber['c'] = 12;
    map24baseNumber['d'] = 13;
    map24baseNumber['e'] = 14;
    map24baseNumber['f'] = 15;
    map24baseNumber['g'] = 16;
    map24baseNumber['h'] = 17;
    map24baseNumber['i'] = 18;
    map24baseNumber['j'] = 19;
    map24baseNumber['k'] = 20;
    map24baseNumber['l'] = 21;
    map24baseNumber['m'] = 22;
    map24baseNumber['n'] = 23;
    map24baseNumber['o'] = 24;
    count = 0;
}

void serialCommunicate::serialReceive(int i){
    //send delay
    if(inputChar[0]=='l'){
        inputInt[0] = intTemp;
        direction[0] = i;
    }
    if(inputChar[0]=='r'){
        inputInt[1] = intTemp;
        isSerialReceive_ = true;
        direction[1] = i;
    }
}

void serialCommunicate::recevieData(){

    //printf("received");
    int LeftFirstIndex  = map24baseNumber.find(inputData[1])->second;
    int LeftSecondIndex = map24baseNumber.find(inputData[2])->second;
    int LSpeed = LeftFirstIndex*24+LeftSecondIndex - 288;
    int RightFirstIndex  = map24baseNumber.find(inputData[3])->second;
    int RightSecondIndex = map24baseNumber.find(inputData[4])->second;
    int RSpeed = RightFirstIndex*24+RightSecondIndex - 288;
    if(LSpeed>=0)
      direction[0] = 1;
    else
      direction[0] = 0;
    if(RSpeed>=0)
        direction[1] = 1;
    else
        direction[1] = 0;
    inputInt[0] = abs(LSpeed);
    inputInt[1] = abs(RSpeed);
    count = 0;
    isSerialReceive_ = true;
}

void serialCommunicate::update(){
    if(pc.readable()){
        pc.scanf("%s",inputData);
        pc.printf("read = %s\n",inputData);
        recevieData();
    }
    /*while(pc.readable()&&count < 100){
        inputData[count] = pc.getc();
        count++;
        if(inputData[count-1]=='s'){
            pc.printf("read = %s",inputData);
            recevieData();
        }
    }*/
}

int serialCommunicate::getLeftWheel(){
    return inputInt[0];
}
int serialCommunicate::getRightWheel(){
    return inputInt[1];
}

int serialCommunicate::getDirection(int i){
    return direction[i];
}

bool serialCommunicate::isSerialReceive(){
    return isSerialReceive_;
}

void serialCommunicate::setSerialReceive(bool s){
    isSerialReceive_ = s;
}
