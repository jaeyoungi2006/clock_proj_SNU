// #include <Chrono.h>
// #include <TimeLib.h>

// long gcd(long a, long b){
//     if(a % b == 0) return b;
//     return gcd(b, a % b);
// }

// long lcm(long a, long b){
//     long c = gcd(a, b);
//     long a_ = a / c;
//     long b_ = b / c;
//     return a_ * b_ * c;
// }

// class Fraction{
//     public:
//         long numerator;
//         long denominator;
//         Fraction(long n, long d = 1){
//             long g = gcd(n, d);
//             numerator = n / g;
//             denominator = d / g;
//             if(denominator < 0){
//                 numerator = -numerator;
//                 denominator = -denominator;
//             }
//         }

//         // Addition
//         Fraction operator+(const Fraction & other) const{
//             long num = numerator * other.denominator + other.numerator * denominator;
//             long den = denominator * other.denominator;
//             return Fraction(num, den);
//         }

//         Fraction operator+(long other) const{
//             return *this + Fraction(other);
//         }

//         friend Fraction operator+(long lhs, const Fraction& rhs){
//             return rhs + lhs;
//         }

//         // Negation
//         Fraction operator-() const {
//             return Fraction(-numerator, denominator);
//         }

//         // Subtraction
//         Fraction operator-(const Fraction& other) const {
//             return *this + (-other);
//         }

//         Fraction operator-(long other) const {
//             return *this - Fraction(other);
//         }

//         friend Fraction operator-(long lhs, const Fraction& rhs) {
//             return -rhs + lhs;
//         }

//         // Multiplication
//     Fraction operator*(const Fraction& other) const {
//         long num = numerator * other.numerator;
//         long den = denominator * other.denominator;
//         return Fraction(num, den);
//     }

//     Fraction operator*(long other) const {
//         return *this * Fraction(other);
//     }

//     friend Fraction operator*(long lhs, const Fraction& rhs) {
//         return rhs * lhs;
//     }

//     // Division
//     Fraction operator/(const Fraction& other) const {
//         return *this * Fraction(other.denominator, other.numerator);
//     }

//     Fraction operator/(long other) const {
//         return Fraction(numerator, denominator * other);
//     }

//     friend Fraction operator/(long lhs, const Fraction& rhs) {
//         return Fraction(lhs) / rhs;
//     }

//     // Equality
//     bool operator==(const Fraction& other) const {
//         return numerator == other.numerator && denominator == other.denominator;
//     }

//     bool operator==(long other) const {
//         return *this == Fraction(other);
//     }

//     friend bool operator==(long lhs, const Fraction& rhs) {
//         return rhs == lhs;
//     }

//     // Inequality
//     bool operator!=(const Fraction& other) const {
//         return !(*this == other);
//     }

//     bool operator!=(long other) const {
//         return !(*this == other);
//     }

//     friend bool operator!=(long lhs, const Fraction& rhs) {
//         return !(rhs == lhs);
//     }

//     // Less than
//     bool operator<(const Fraction& other) const {
//         return numerator * other.denominator < denominator * other.numerator;
//     }

//     bool operator<(long other) const {
//         return *this < Fraction(other);
//     }

//     friend bool operator<(long lhs, const Fraction& rhs) {
//         return Fraction(lhs) < rhs;
//     }

//     // Less than or equal
//     bool operator<=(const Fraction& other) const {
//         return *this == other || *this < other;
//     }

//     bool operator<=(long other) const {
//         return *this <= Fraction(other);
//     }

//     friend bool operator<=(long lhs, const Fraction& rhs) {
//         return Fraction(lhs) <= rhs;
//     }

//     // Greater than
//     bool operator>(const Fraction& other) const {
//         return !(*this <= other);
//     }

//     bool operator>(long other) const {
//         return *this > Fraction(other);
//     }

//     friend bool operator>(long lhs, const Fraction& rhs) {
//         return Fraction(lhs) > rhs;
//     }

//     // Greater than or equal
//     bool operator>=(const Fraction& other) const {
//         return !(*this < other);
//     }

//     bool operator>=(long other) const {
//         return *this >= Fraction(other);
//     }

//     friend bool operator>=(long lhs, const Fraction& rhs) {
//         return Fraction(lhs) >= rhs;
//     }

//     double to_double() const {
//         return (double) numerator / denominator;
//     }

//     void serial_print() const{
//       Serial.print(numerator);
//       Serial.print("/");
//       Serial.print(denominator);
//     }
// };


// using namespace std;

// class StepMotor{
//   private:
//     bool mot = 0;
//     Fraction time_delay = Fraction(0);
//     Fraction time_move = Fraction(0);
//     long tick_from_rot = 0;
//     long reset_time = 100;
//     int error_count = 0;
//   public:
//     long stride_num = 400;
//     bool is_rotating = 0;
//     int step_pin;
//     int dir_pin;
//     Fraction speed;
//     void calculate_time_delay();
//     void Start();
//     void Stop();
//     void Speed(Fraction sp);
//     void calc();
//     void error_print();
//     Chrono chrono;
//     StepMotor(int step, int dir): step_pin(step), dir_pin(dir), speed(Fraction(0)){calculate_time_delay(); pinMode(step, OUTPUT); pinMode(dir, OUTPUT);}
// };

// void StepMotor::calc(){
//   Fraction time = Fraction(chrono.elapsed(), 1000);
//   if(time > time_move && is_rotating){
//     if(mot) digitalWrite(step_pin, HIGH);
//     else digitalWrite(step_pin, LOW);
//     if(speed > 0) digitalWrite(dir_pin, HIGH);
//     else digitalWrite(dir_pin, LOW);  
//     mot = !mot;
//     time_move = time_move + time_delay;
//     tick_from_rot += 1;

//     /*if(tick_from_rot % 40 == 0){
//       time.serial_print();
//       Serial.print(" ");
//       time_move.serial_print();
//       Serial.print(" ");
//       time_delay.serial_print();
//       Serial.println();
//     }*/ //for check
//     if(time > time_move){Serial.println("Excedeed the time"); error_count += 1;} // if we set motor too fast, processor would not make it
//   }
  
//   if(time > reset_time){ //we set denominator & numerator as long(32bit), so if exceeded, reset it
//     time = time - reset_time;
//     chrono.restart((long)((time * 1000).to_double()));
//     time_move = time_move - reset_time;
//   }
// }
// void StepMotor::calculate_time_delay(){
//   if(speed == 0) time_delay = Fraction(0);
//   else{
//     time_delay = speed / Fraction(stride_num);
//   }
// }

// void StepMotor::Start(){
//   time_move = Fraction(chrono.elapsed(), 1000);
//   tick_from_rot = 0;
//   is_rotating = 1;
// }
// void StepMotor::Stop(){
//   is_rotating = 0;
// }
// void StepMotor::Speed(Fraction sp){
//   speed = sp;
//   calculate_time_delay();
// }

// void StepMotor::error_print(){
//   Serial.println((String) "Error - Processor loses: " + error_count + " times\n");
// }


// StepMotor motor1(12, 13);
// int check = 0;
// void setup() {
//   // put your setup code here, to run once:
//   motor1.Start();
//   motor1.Speed(Fraction(10)); // 60/8 sec for 1 rotation
//   Serial.begin(9600);
// }


// void loop() {
//   // put your main code here, to run repeatedly:
//   motor1.calc();
//   check += 1;
//   if(check % 1000000 == 0) check = 0;
//   if(check % 1000 == 0 && Serial.available()){
//       char input = Serial.read();
//       if(input == 'e') motor1.error_print();
//   }
// }
