#include <Chrono.h>
using namespace std;

class StepMotor{
  private:
    bool mot = 0;
    double time_delay;
    double time_move;
  public:
    int stride_num = 400;
    bool is_rotating = 0;
    int step_pin;
    int dir_pin;
    double speed;
    void calculate_time_delay();
    void Start();
    void Stop();
    void Speed(int);
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
    time_delay = speed / stride_num;
  }
}

void StepMotor::Start(){
  time_move = (double) chrono.elapsed() / 1000;
  is_rotating = 1;
}
void StepMotor::Stop(){
  is_rotating = 0;
}
void StepMotor::Speed(int sp){
  speed = sp;
  calculate_time_delay();
}


StepMotor motor1(12, 13);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  motor1.Start();
  motor1.Speed(3600/8); // 60/8 sec for 1 rotation
}

void loop() {
  // put your main code here, to run repeatedly:
  motor1.calc();
}
