#include "fraction.h"

class StepMotor{
  private:
    bool mot = 0;
    Fraction time_delay = Fraction(0);
    Fraction time_move = Fraction(0);
    Fraction last_move = Fraction(0);
    Fraction stop_time = Fraction(0);
    long tick_from_rot = 0;
    long tick_target = 0; // how much to tick
    long reset_time = 10000;
    int error_count = 0;
    int motor_num;
    int step_time_sec = 10;
    int gear_ratio = 1;
    int microstepping = 1;

  public:
    long stride_num = 400;
    bool is_rotating = 0;
    bool is_enabled = 0;
    int step_pin;
    int dir_pin;
    int enable_pin;
    int target_num = 0; // target number to move
    bool mode;
    Fraction speed;
    void calculate_time_delay();
    void Start(int target);
    void Stop();
    void Speed(Fraction sp);
    void calc();
    void error_print();
    Fraction quadratic_move(Fraction total_time, Fraction y);
    Chrono chrono;
    StepMotor(int motor_num, int enable, int step, int dir, int mode = 0, int step_time_sec = 10, int gear_ratio = 1, int microstepping = 1): motor_num(motor_num), step_pin(step), dir_pin(dir), enable_pin(enable), mode(mode), step_time_sec(step_time_sec), gear_ratio(gear_ratio), speed(Fraction(0)), microstepping(microstepping) {
      calculate_time_delay(); pinMode(step, OUTPUT); pinMode(dir, OUTPUT); pinMode(enable, OUTPUT); digitalWrite(enable, HIGH);
    }
};

Fraction StepMotor::quadratic_move(Fraction total_time, Fraction y){
  double x = 0;
  if(y < Fraction(1, 2)){
    x = sqrt(y.to_double() / 2.0);
  }
  else{
    x = 1 - sqrt((1.0 - y.to_double()) / 2.0);
  }
  return Fraction((int)(x * total_time.to_double() * 999), 1000);
}

void StepMotor::calc(){
  Fraction time = Fraction(chrono.elapsed(), 1000);
  if(!is_rotating && is_enabled && time >= stop_time){
    is_enabled = 0;
    digitalWrite(enable_pin, HIGH);
    Serial.println("Motor Disabled");
  }
  if(time > time_move && is_rotating && time - last_move >= Fraction(1, 500) / Fraction(microstepping)){
    if(mot) digitalWrite(step_pin, HIGH);
    else digitalWrite(step_pin, LOW);
    if(speed > 0) digitalWrite(dir_pin, LOW);
    else digitalWrite(dir_pin, HIGH);  
    mot = !mot;
    last_move = time;
    tick_from_rot += 1;
    if(mode == 1) time_move = time_move + time_delay;
    else{ //use curve
      time_move = quadratic_move(time_delay * tick_target, Fraction(tick_from_rot, tick_target));
      //time_move.serial_print();
      //Serial.println();
    }

    /*if(tick_from_rot % 40 == 0){
      time.serial_print();
      Serial.print(" ");
      time_move.serial_print();
      Serial.print(" ");
      time_delay.serial_print();
      Serial.println();
    }*/ //for check
    if(tick_from_rot == tick_target){
      Stop();
      Serial.print((String)"Motor " + motor_num +  ": Moved to " + target_num);
      Serial.print(" Moved Time ");
      time_move.serial_print();
      Serial.println();
    }
    if(time > time_move){Serial.println((String)"Motor " + motor_num + ": Excedeed the time"); error_count += 1;} // if we set motor too fast, processor would not make it
  }
  
  if(time > reset_time){ //we set denominator & numerator as long(32bit), so if exceeded, reset it
    time = time - reset_time;
    chrono.restart((long)((time * 1000).to_double()));
    time_move = time_move - reset_time;
  }
}
void StepMotor::calculate_time_delay(){
  if(speed == 0) time_delay = Fraction(0);
  else{
    time_delay = speed / Fraction(stride_num);
  }
}

void StepMotor::Start(int target = -1){
  //if(time_move == -1){
  if(mode == 0){
    if(target != -1 && !is_rotating && target != target_num){   
      chrono.restart(0);
      time_move = Fraction(1, 100);
      last_move = Fraction(1, 100);
      is_rotating = 1;
      int need_move = (target - target_num); //how much need to move(in num)
      if(need_move == 0 && tick_target == tick_from_rot) is_rotating = 0;
      //else
      if(need_move < 0) need_move += 10;
      tick_target = stride_num / 10 * need_move * gear_ratio * microstepping + (tick_target - tick_from_rot); //how much need to move(in tick) & *4 for jinsoo's clock
      tick_from_rot = 0;
      //clear_frame();
      //add_to_frame(target_num, 0);
      //add_to_frame('>', 4);
      //add_to_frame(target, 8);
      //display_frame();
      Serial.println(motor_num, is_rotating);
      if(is_rotating){
        digitalWrite(enable_pin, LOW);
        is_enabled = 1;
      }
      target_num = target;
    }
    else{
      if(target == 1 && is_rotating) is_rotating = 0;
    }
    if(is_rotating) Serial.println((String) "Motor " + motor_num + " Moving");
  }
  else{
    if(target != -1 && target_num != target){   
      chrono.restart(0);
      last_move = Fraction(0);
      time_move = 0;
      is_rotating = 1;
      int need_move = (target - target_num); //how much need to move(in num)
      //if(need_move == 0) need_move = 10;
      //else
      if(need_move < 0) need_move += 10;
      //Serial.println((String) tick_target + " : " + tick_from_rot);
      if(tick_target != tick_from_rot) Serial.println((String) "Motor " + motor_num + " Can't Reach");
      tick_target = stride_num / 10 * need_move * gear_ratio * microstepping + (tick_target - tick_from_rot); //how much need to move(in tick) & *4 for jinsoo's clock
      //Serial.println((String) tick_target +":"+ tick_from_rot+":"+ need_move+":"+ stride_num);
      
      //Serial.println((String)"Motor " + motor_num + " : "+ (double)step_time_sec * 400 / tick_target);
      Speed(max(Fraction(step_time_sec * 400, tick_target),  Fraction(1, microstepping)));
      Serial.print((String) "Motor " + motor_num + " Moving, Speed : ");
      speed.serial_print();
      Serial.println();
      target_num = target;
      tick_from_rot = 0;
    }
  }
}
void StepMotor::Stop(){
  is_rotating = 0;
  //clear_frame();
  //add_to_frame(target_num, 4);
  //display_frame();
  //time_move = time_move + Fraction(9, 10);
  stop_time = Fraction(chrono.elapsed(), 1000) + Fraction(1, 2);
}

void StepMotor::Speed(Fraction sp){
  speed = sp;
  calculate_time_delay();
}

void StepMotor::error_print(){
  Serial.println((String) "Motor Num: "+ motor_num +"Error - Processor loses: " + error_count + " times\n");
}