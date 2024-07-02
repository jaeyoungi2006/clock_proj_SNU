#include <Chrono.h>
#include <TimeLib.h>
using namespace std;

class StepMotor{
  private:
    bool mot = 0;
    double time_delay;
    double time_move;
  public:
    int stride_num = 200;
    bool is_rotating = 0;
    int step_pin;
    int dir_pin;
    double speed;
    void calculate_time_delay();
    void Start();
    void Stop();
    void Speed(double sp);
    void calc();
    Chrono chrono;
    StepMotor(int step, int dir): step_pin(step), dir_pin(dir), speed(1){calculate_time_delay(); pinMode(step, OUTPUT); pinMode(dir, OUTPUT);}
};
void StepMotor::calc(){
  double time = (double) chrono.elapsed() / 1000;
  if(time > time_move && is_rotating){
    if(mot) digitalWrite(step_pin, HIGH);
    else digitalWrite(step_pin, LOW);
    if(speed > 0) digitalWrite(dir_pin, HIGH);
    else digitalWrite(dir_pin, LOW);  
    mot = !mot;
    time_move += time_delay;
  }
}
void StepMotor::calculate_time_delay(){
  if(speed == 0) time_delay = 0;
  else{
    time_delay = speed / stride_num / 2;
  }
}

void StepMotor::Start(){
  time_move = (double) chrono.elapsed() / 1000;
  is_rotating = 1;
}
void StepMotor::Stop(){
  is_rotating = 0;
}
void StepMotor::Speed(double sp){
  speed = sp;
  calculate_time_delay();
}


StepMotor motor1(12, 13);
void setup() {
  // put your setup code here, to run once:
  motor1.Start();
  motor1.Speed((double)10/8); // 60/8 sec for 1 rotation
}


void loop() {
  // put your main code here, to run repeatedly:
  motor1.calc();
}
