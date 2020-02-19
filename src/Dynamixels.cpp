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


Dynamixels::Dynamixels(PinName tx, PinName rx, int baud, PinName di): _dynamixels(tx, rx), digitalOut_RS485DirectionPin(di){
    _baud = baud;
    _dynamixels.baud(_baud);
}

void Dynamixels::SetGoalVelocity(uint8_t ID,uint16_t value_velocity, uint8_t direction){
    char velocity[4];
    uint16_t speed = (((1 - direction) * ~value_velocity) + (direction * value_velocity));
    uint16_t first_bit = ((1 - direction) * 0xff) + (direction * 0x00);

    velocity[0] = speed & 0xff;
    velocity[1] = speed >> 8;
    velocity[2] = first_bit;
    velocity[3] = first_bit;

    write(ID, REG_GOAL_VELOCITY, 4, velocity);
    wait(DELAY_TIME);
}

uint16_t Dynamixels::GetPresentVelocity(uint8_t ID){
  read(ID, REG_PRESENT_VELOCITY, 4);
  switch(RxBuf[8]){
        case 0x00 : //no error
            return (uint16_t)RxBuf[9] | (((uint16_t)RxBuf[10]<<8)&0xff00);

        default :   //error occurred
            return (uint16_t)RxBuf[8]; //return error status
  }
}

void Dynamixels::DisableTorque(uint8_t ID){
    char data[1];
    data[0] = 0;
    write(ID, REG_TORQUE_ENABLE, 1, data);
    wait(DELAY_TIME);
}

void Dynamixels::EnableTorque(uint8_t ID){
    char data[1];
    data[0] = 1;
    write(ID, REG_TORQUE_ENABLE, 1, data);
    wait(DELAY_TIME);
}

void Dynamixels::DisableLED(uint8_t ID){
    char data[1];
    data[0] = 0;
    digitalOut_RS485DirectionPin = 1;
    write(ID, REG_LED_ENABLE, 1, data);
    wait(DELAY_TIME);
    digitalOut_RS485DirectionPin = 0;
}

void Dynamixels::EnableLED(uint8_t ID){
    char  data[1];
    data[0] = 1;
    digitalOut_RS485DirectionPin = 1;
    write(ID, REG_LED_ENABLE, 1, data);
    wait(DELAY_TIME);
    digitalOut_RS485DirectionPin = 0;
}

unsigned short Dynamixels::update_crc(unsigned short crc_accum, unsigned char* data_blk_prt, unsigned short data_blk_size){
    unsigned short i, j;
    unsigned short crc_table[256] = {
        0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
        0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022,
        0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072,
        0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041,
        0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2,
        0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
        0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1,
        0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
        0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192,
        0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1,
        0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1,
        0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
        0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151,
        0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162,
        0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
        0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101,
        0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312,
        0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
        0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371,
        0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342,
        0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1,
        0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
        0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2,
        0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
        0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291,
        0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2,
        0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2,
        0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1,
        0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
        0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
        0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231,
        0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202
    };

    for(j = 0; j < data_blk_size; j++)
    {
        i = ((unsigned short)(crc_accum >> 8) ^ data_blk_prt[j]) & 0xFF;
        crc_accum = (crc_accum << 8) ^ crc_table[i];
    }

    return crc_accum;
}

void Dynamixels::write(uint8_t ID, uint16_t address, uint16_t bytes, char* data){
    // 0xff, 0xff, 0xfd, 0x00, ID, Len_L, Len_H, Instruction, Address, Param(s), CRC_L, CRC_H
    int length = bytes + 5;

    unsigned char TxBuf[length + 7];
    unsigned short crc = 0;

    TxBuf[0] = 0xff;                // Header1
    TxBuf[1] = 0xff;                // Header2
    TxBuf[2] = 0xfd;                // Header3
    TxBuf[3] = 0x00;                // Reserved
    TxBuf[4] = ID;                  // Packet ID
    TxBuf[5] = length & 0xff;       // Len_L // bottom 8 bits //Packet Length = number of Parameters + 3
    TxBuf[6] = length >> 8;         // Len_H // top 8 bits //
    TxBuf[7] = WRITE_DATA;          // Instruction
    TxBuf[8] = address & 0xff;      // Address L
    TxBuf[9] = address >> 8;        // Address H

    for (int i=0; i<bytes ; i++) {
        TxBuf[10+i] = data[i];
    }

    crc = update_crc(0, TxBuf, length + 5);

    TxBuf[10+bytes] = crc & 0x00ff;         // CRC_L // CRC_L = (CRC & 0x00FF);
    TxBuf[11+bytes] = (crc >> 8) & 0x00ff;  // CRC_H // first dummy //

    //send Instruction Packet
    wait_ms(1);     //wait for 74HC126 enable pin transition delay
    digitalOut_RS485DirectionPin = 1;
    // Transmit the packet in one burst with no pausing
    for (int i = 0; i < (length + 7) ; i++) {
        _dynamixels.putc(TxBuf[i]);
    }

    // Wait for data to transmit
    wait (0.00002);
    digitalOut_RS485DirectionPin = 0;
}


void Dynamixels::read(uint8_t ID, uint16_t address, uint16_t bytes){
  int length = 7 ;  //[intruction][address_LSB][address_MSB][DataLength_LSB][DataLength_MSB][CRC_LSB][CRC_MSB]
  uint8_t TxBuf[14];
  unsigned short crc = 0;

  TxBuf[0] = 0xff;                // Header1
  TxBuf[1] = 0xff;                // Header2
  TxBuf[2] = 0xfd;                // Header3
  TxBuf[3] = 0x00;                // Reserved
  TxBuf[4] = ID;                  // Packet ID
  TxBuf[5] = length & 0xff;       // Len_L // bottom 8 bits //Packet Length = number of Parameters + 3
  TxBuf[6] = length >> 8;         // Len_H // top 8 bits //
  TxBuf[7] = READ_DATA;           // Instruction
  TxBuf[8] = address & 0xff;      // Address L
  TxBuf[9] = address >> 8;        // Address H
  TxBuf[10] = bytes & 0xff;      // Address L
  TxBuf[11] = bytes >> 8;        // Address H

  crc = update_crc(0, TxBuf, length + 5);

  TxBuf[12] = crc & 0x00ff;         // CRC_L // CRC_L = (CRC & 0x00FF);
  TxBuf[13] = (crc >> 8) & 0x00ff;  // CRC_H // first dummy //

  //send Instruction Packet
  wait_ms(1);     //wait for 74HC126 enable pin transition delay
  digitalOut_RS485DirectionPin = 1;

  // Transmit the packet in one burst with no pausing
  for (int i = 0; i < (length + 7) ; i++) {
      _dynamixels.putc(TxBuf[i]);
  }
  // Wait for data to transmit
  wait (0.00002);
  digitalOut_RS485DirectionPin = 0;

  /*---------------------------------------------------------------*/

  int i = 0;
  const unsigned int TimeOut = 500;
  bool wait = true;

  wait_us(180);
  digitalOut_RS485DirectionPin = 1;

  while (!_dynamixels.readable() && wait) {
    wait_us(1);
    i++;
    if(i == TimeOut){
      wait = false;
      // No return data packet
    }
  }
  wait_ms(4);

  i = 0;
  while(_dynamixels.readable() && (i < 13)){
    RxBuf[i] = _dynamixels.getc();
    i++;
  }
  digitalOut_RS485DirectionPin = 0;
}
