#include "wifi_time.h"
#include "step_motor.h"
#include <EEPROM.h>

#define USE_EEPROM 0
#define EEPROM_PIN D0

#include "led-matrix.h"
#include "Arduino_LED_Matrix.h"
#define USE_LED_MATRIX 0

TimeNow timemodule(0);
int number_array[6] = {0, 0, 0, 0, 0, 0};

ArduinoLEDMatrix matrix;
position first = {5, 0}; // position of first digit
position second = {0, 0}; // etc.
position third = {5, 7};
position fourth = {0, 7};
byte currentFrame[NO_OF_ROWS][NO_OF_COLS];
byte rotatedFrame[NO_OF_ROWS][NO_OF_COLS];
void setDigit(position digitPosition = {0, 0}, const byte digit[][5] = {}){
  for(byte r = 0; r < 3; r++){
    for(byte c = 0; c < 5; c++){
      currentFrame[r+digitPosition.row][c+digitPosition.col] = digit[r][c];
    }
  }
}
void rotateFrame(){
  for(byte r = 0; r < NO_OF_ROWS; r++){
    for(byte c = 0; c < NO_OF_COLS; c++){
      rotatedFrame[r][c] = currentFrame[NO_OF_ROWS-1-r][NO_OF_COLS-1-c];
    }
  }
  memcpy(currentFrame, rotatedFrame, sizeof rotatedFrame);
}

StepMotor motors[4] = {
  StepMotor(1, 2, 3, 4),
  StepMotor(2, 5, 6, 7),
  StepMotor(3, 8, 9, 10),
  StepMotor(4, 11, 12, 13),//6, 12, 13, 1, 1, 4
};
int base_speed[4] = {5, 5, 5, 5};


bool program_running;
bool program_end;
void setup(){
  program_running = 1;
  program_end = 0;
  Serial.begin(921600);
  timemodule.init();
  pinMode(EEPROM_PIN, INPUT);
  if(USE_LED_MATRIX) matrix.begin();
  for(int i = 0; i < 4; i++){
    motors[i].Speed(base_speed[i]);
  }
  if(USE_EEPROM){
    for(int i = 0; i < 4; i++){
      int8_t tmp = EEPROM.read(i);
      motors[i].target_num = (int)tmp;
    }
  }
}

void loop(){
  if(timemodule.updateTime() && program_running){
    timemodule.time_now.update_array(number_array);
    timemodule.time_now.serial_print();
    if(USE_LED_MATRIX){
      setDigit(first, digits[number_array[2]]);
      setDigit(second, digits[number_array[3]]);
      setDigit(third, digits[number_array[4]]);
      setDigit(fourth, digits[number_array[5]]);
      rotateFrame();
      matrix.renderBitmap(currentFrame, NO_OF_ROWS, NO_OF_COLS); 
    }
    for(int i = 0; i < 4; i++){
      motors[i].Start(number_array[i]); //motor update
    }
  }
  for(int i = 0; i < 4; i++){
    motors[i].calc();
  }
  /*if(digitalRead(EEPROM_PIN) == HIGH){
    program_running = 0;
  }*/
  if(!program_running && !program_end){
    bool flag = 1;
    for(int i = 0; i < 4; i++){
      motors[i].Speed(Fraction(2));
      if(motors[i].is_rotating) flag = 0;
    }
    if(flag){
      for(int i = 0; i < 4; i++){
        EEPROM.write(i, motors[i].target_num);
      }
      Serial.println("All nums saved in EEPROM");
      program_end = 1;
      Serial.println("Program Ended");
    }
  }
  delay(1);
}