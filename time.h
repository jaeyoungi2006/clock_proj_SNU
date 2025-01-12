class Time{
  public:
    int hour;
    int minute;
    int second;
    Time(int h, int m, int s): hour(h), minute(m), second(s) {};
    void add(int time){
      second += time;
      minute += second / 60;
      hour += minute / 60;
      second %= 60;
      minute %= 60;
      hour %= 24;
    }
    void serial_print(){
      Serial.print(hour);
      Serial.print(":");
      Serial.print(minute);
      Serial.print(":");
      Serial.println(second);
    }
    void update_array(int*arr){
      arr[0] = hour / 10;
      arr[1] = hour % 10;
      arr[2] = minute / 10;
      arr[3] = minute % 10;
      arr[4] = second / 10;
      arr[5] = second % 10;
    }
    bool is_update_period(char mode){
      switch(mode){
        case 'D':
          return (hour == 0) && (minute == 0) && (second == 30); //00:00:30
        case '6':
          return (hour % 6 == 0) && (minute == 0) && (second == 30); //0,6,12,18:00:30
        case 'H':
          return (minute == 0) && (second == 30); //HH:00:30
        case 'M':
          return (second == 30); //HH:MM:30
        default:
          return 0; // no update
      }
    }
};
