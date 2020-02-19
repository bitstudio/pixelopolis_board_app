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

////////// EEPROM ///////////

////////// RAM ///////////
#define REG_TORQUE_ENABLE       0x40
#define REG_LED_ENABLE          0x41
#define REG_GOAL_VELOCITY       0x68
#define REG_PRESENT_VELOCITY    0x80

////////// INSTRUCTION ///////////
#define PING                    0x01
#define READ_DATA               0x02
#define WRITE_DATA              0x03
#define REBOOT                  0x08

////////// ERROR ///////////
#define RESULT_FAIL             0x01
#define INS_ERR                 0x02
#define CRC_ERR                 0x03
#define DATA_RANGE_ERR          0x04
#define DATA_LENGTH_ERR         0x05
#define DATA_LIMIT_ERR          0x06
#define ACCESS_ERR              0x07

////////// VARIABLE ///////////
#define DELAY_TIME 0.03


class Dynamixels{

public:

    Dynamixels(PinName tx, PinName rx, int baud, PinName di);

    void SetGoalVelocity(uint8_t ID,uint16_t value_velocity, uint8_t direction);
    void DisableTorque(uint8_t ID);
    void EnableTorque(uint8_t ID);
    void DisableLED(uint8_t ID);
    void EnableLED(uint8_t ID);
    uint16_t GetPresentVelocity(uint8_t ID);
    unsigned short update_crc(unsigned short crc_accum, unsigned char* data_blk_prt, unsigned short data_blk_size);


private :

    Serial _dynamixels;
    DigitalOut digitalOut_RS485DirectionPin;
    int _baud;
    uint8_t RxBuf[13];

    void write(uint8_t ID, uint16_t  address, uint16_t bytes, char* data);
    void read(uint8_t ID, uint16_t address, uint16_t bytes);
};
