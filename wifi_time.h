#include "RTC.h"
#include <WiFiS3.h>
#include "arduino_secrets.h"
#include "time.h"
#include <Chrono.h>

#define TIMEZONE_OFFSET_HOURS 9
#define UPDATE_TIME_WIFI_PERIOD 'H' // 3600 sec -> 1 hour ('D': Day, 'H': Hour, '6': 6Hour, 'M':Minute)
#define RESTART_PERIOD 'D' // RESET By Day

char ssid[3][30] = {SECRET_SSID1, SECRET_SSID2, SECRET_SSID3};
char pass[3][30] = {SECRET_PASS1, SECRET_PASS2, SECRET_PASS3};

constexpr unsigned int LOCAL_PORT = 2390;  // local port to listen for UDP packets
constexpr int NTP_PACKET_SIZE = 48; // NTP timestamp is in the first 48 bytes of the message
int wifiStatus = WL_IDLE_STATUS;
IPAddress timeServer(162, 159, 200, 123); // pool.ntp.org NTP server

void reboot() {
  NVIC_SystemReset();
}

class TimeNow{
  private:
    bool connectToWifi(uint8_t maxTries = 3); // 1 if succeeded 0 if not
    void printWifiStatus();
    unsigned long sendNTPpacket(IPAddress& address);
    unsigned long getUnixTime(int8_t timeZoneOffsetHours = 0, uint8_t maxTries = 5);
    bool updateTimeWifi(); // 1 if succeeded 0 if not
    long sec_from_wifi_update;
    
    byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
    WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP
  public:
   Time time_now = Time(0, 0, 0);
   Chrono chrono;
   bool updateTime(); // 1 if changed 0 if not
   void init(Time time = Time(0, 0, 0));
   TimeNow(long sec): sec_from_wifi_update(sec) {};
};

bool TimeNow::connectToWifi(uint8_t maxTries){
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    return 0;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  uint8_t tries = 0;
  // attempt to connect to WiFi network:
  for(int i = 0; i < 3; i++){
    tries = 0;
    while (wifiStatus != WL_CONNECTED && tries < maxTries) {
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(ssid[i]);
      Serial.println(pass[i]);
      // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
      wifiStatus = WiFi.begin(ssid[i], pass[i]);
      tries++;
    }
    if(wifiStatus == WL_CONNECTED) break;
  }

  if(tries >= maxTries){
    Serial.println("Wifi connect Failed");
    return 0;
  }
  delay(2000);
  if(WiFi.RSSI() < -5){
    Serial.println("Connected to WiFi");
    printWifiStatus();
    return 1;
  }
  return 0;
}

void TimeNow::printWifiStatus(){
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

unsigned long TimeNow::sendNTPpacket(IPAddress& address){
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

unsigned long TimeNow::getUnixTime(int8_t timeZoneOffsetHours, uint8_t maxTries){
  // Try up to `maxTries` times to get a timestamp from the NTP server, then give up.
  for (size_t i = 0; i < maxTries; i++){
    sendNTPpacket(timeServer); // send an NTP packet to a time server
    // wait to see if a reply is available
    delay(2000);

    if (Udp.parsePacket()) {
      Serial.println("Packet received.");
      Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

      //the timestamp starts at byte 40 of the received packet and is four bytes,
      //or two words, long. First, extract the two words:
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      
      // Combine the four bytes (two words) into a long integer
      // this is NTP time (seconds since Jan 1 1900):
      unsigned long secsSince1900 = highWord << 16 | lowWord;

      // Now convert NTP time into everyday time:
      // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
      const unsigned long seventyYears = 2208988800UL;
      unsigned long secondsSince1970 = secsSince1900 - seventyYears + (timeZoneOffsetHours * 3600);
      return secondsSince1970 + 2; // +2 for delay(2000)
    }
    else {
      Serial.println("Packet not received. Trying again.");
    }
  }
  bool flag = connectToWifi();
  if(flag){ //retry
    Udp.begin(LOCAL_PORT);
    return getUnixTime(timeZoneOffsetHours = timeZoneOffsetHours, maxTries = maxTries);
  }
  return 0;
}

bool TimeNow::updateTimeWifi(){
  yield();
  Serial.println("\nStarting connection to NTP server...");
  auto unixTime = getUnixTime(TIMEZONE_OFFSET_HOURS, 5);
  if(unixTime == 0){
    Serial.println("Update Time using Wifi Failed");
    return 0;
  }
  Serial.print("Unix time = ");
  Serial.println(unixTime);
  RTCTime timeToSet = RTCTime(unixTime);
  int hour = timeToSet.getHour();
  int minute = timeToSet.getMinutes();
  int second = timeToSet.getSeconds();
  bool flag = 0;
  if(hour != time_now.hour || minute != time_now.minute || second != time_now.second) flag = 1;
  time_now.hour = hour;
  time_now.minute = minute;
  time_now.second = second;
  return 1;
}

bool TimeNow::updateTime(){
  if(chrono.elapsed() / 1000 > sec_from_wifi_update){
    time_now.add(1);
    sec_from_wifi_update += 1;
    if(time_now.is_update_period(UPDATE_TIME_WIFI_PERIOD)){//if(sec_from_wifi_update % UPDATE_TIME_WIFI_PERIOD == 0){
     bool succeeded = updateTimeWifi();
     if(succeeded){
       sec_from_wifi_update = 0;
       time_now.add(1);
       chrono.restart(0);
     }
    }
    if(time_now.is_update_period(RESTART_PERIOD)){
      reboot();
    }
    return 1;
  }
  return 0;
}

void TimeNow::init(Time time){
  bool flag = connectToWifi();
  if(flag){
    Udp.begin(LOCAL_PORT);
    updateTimeWifi();
  }
  else{
    time_now = time;
  }
  chrono.restart(0);
}