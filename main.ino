#include <NTP.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <time.h>
#include <sche2.h>
#include <ACROBOTIC_SSD1306.h>
#include <buttonana.h>
#include <pump_and_feeding.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>


//Declare global variable:
sche2 Sche;
void getdata(char * topic, byte *content, unsigned int length_of_content) {
    Sche.getdata(topic, content, length_of_content);
} 
int state[4] = {0}; // 
int state_num[4] = {8,3,4,4};
int curs = 0;
int mouse;
int wifiset = 0;
String screen[4][8];
int _cmd;
String unit[3] = {"C", "F", "K"};
void display_screen() {
  Serial.println(state[3] * 100 + state[2] * 10 + _cmd + 1000);
  oled.setTextXY(0,0);
  oled.putString("Wifi: ");
  oled.putString(WiFi.status()!=WL_CONNECTED? " Disconnected" : " Connected");
  oled.setTextXY(1,1);
  oled.putString(screen[0][state[0]]);
  oled.setTextXY(2,1);
  oled.putString(screen[1][state[1]].substring(0,20));
  oled.setTextXY(3,1);
  oled.putString(screen[2][state[2]]);
  oled.setTextXY(4,1);
  oled.putString(screen[3][state[3]]);
  oled.setTextXY(curs + 1,0);
  oled.putString(">");
  for(int i=1;i<=4;i++) 
    if(i != curs + 1) {
      oled.setTextXY(i,0);
      oled.putString(" ");
  }   
}
int Current_second;
int diw;
int _time;
SoftwareSerial s(3,1);
buttonana switc(A0, 200, 500, 950);
buttonana select(A0, 200, 950, 1300);
WiFiUDP u;
WiFiClient _client;
NTPClient timeClient(u,"0.asia.pool.ntp.org",25200);
PubSubClient a("m16.cloudmqtt.com", 12224, getdata, _client);
pump_and_feeding pf(D4, D5, D6, D7, D8, 22, 44, 50);
OneWire oneWire(D4);
DallasTemperature sensors(&oneWire);

//------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  s.begin(115200);
  Wire.begin(D2,D3);
  oled.init();                      
  oled.clearDisplay();    
  for(int i=0;i<15;i++) {
    oled.drawFrame(i);
    delay(i * 100);
  }          
  WiFiManager wifiManager;
  wifiManager.setConfigPortalTimeout(30);
  oled.clearDisplay();
  oled.setTextXY(1, 0);
  oled.putString("configuring");
  oled.setTextXY(2, 0);
  oled.putString("Wifi...");
  wifiManager.autoConnect("AquaWifi");
  Serial.println("Connected!");
  a.connect("test4", "dqdnpxhd", "hmuo2Lwny6jq");
  a.subscribe("test");
  timeClient.begin();
  timeClient.update();
  timeClient.setRTCTime();
  //oled.drawFrame(i++);
  //oled.clearDisplay();
  //oled.drawFrame(i++);                          
  screen[1][0] = "Temp: 29";
  screen[2][0] = "Pump: Off        ";
  screen[2][1] = "Pump: Pumping out ";
  screen[2][2] = "Pump: Pumping in ";
  screen[2][3] = "Pump: Auto       ";
  screen[3][0] = "Feeding machine: Off    ";
  screen[3][1] = "Feeding machine: Opening";
  screen[3][2] = "Feeding machine: Closing";
  screen[3][3] = "Feeding machine: Auto   ";
  oled.clearDisplay();
  oled.setTextXY(1, 0);
  oled.putString("establishment");
  oled.setTextXY(2, 0);
  oled.putString("is");
  oled.setTextXY(3, 0);
  oled.putString("completed");
  delay(500);   
  oled.clearDisplay(); 
  oled.setFont(font5x7); 
}
//------------------------------------------------------------------

void loop() { 
  if(WiFi.status()!= WL_CONNECTED) {
    wifiset = 1;
  }
  else if(wifiset == 1) {
    //Serial.println(WiFi.status()== WL_CONNECTED);
    wifiset = 0;
    timeClient.update();
    timeClient.setRTCTime();
  }
  if(Current_second != timeClient.getRTCTime(screen[0][1], screen[0][0], diw)) {
    Current_second = timeClient.getRTCTime(screen[0][1], screen[0][0], diw);
    sensors.gettemp(screen[1][0],screen[1][1],screen[1][2]);
    if(Sche.diw != diw) {
      Sche.diw = diw;
      //Sche.upda();
    }
    display_screen();
  }
    a.loop();
    _cmd = Sche.pop(screen[0][0]);
    if(_cmd != 0)
      Serial.println(state[3] * 100 + state[2] * 10 + _cmd + 1000);
    switch(curs) {
        case 0: {
          switch(state[0]) {
                case 0: {
                    switc.pres(curs, 4, display_screen);
                    select.pres(state[0], state_num[0], display_screen);
                    break;
                }
                case 1: {
                    switc.pres(curs, 4, display_screen);
                    select.pres(state[0], state_num[0], display_screen);
                    break;
                }
                case 2: {
                    screen[0][3] = screen[0][0].substring(0,16) + 'Y' + 'N';
                    mouse = 0;
                    state[0]++;
                    display_screen();
                    break;
                }
                case 3: {
                    select.pres(mouse, 12, display_screen);
                    switch(mouse) {
                        case 8: {
                          mouse += 2;
                          break;
                        }
                        case 10: {
                          int cu = 0;
                          switc.pres(cu, 2, display_screen);
                            if (cu == 1)
                              state[0] = 4;
                            break;
                        }
                        case 11: {
                          int cu = 0;
                          switc.pres(cu, 2, display_screen);
                            if (cu == 1)
                              state[0] = 5;
                            break;
                        }
                        default: {
                          int cu = screen[0][3][mouse + 6] - '0';
                          if(cu != ':' - '0' && mouse <8)
                            switc.pres(cu, 10, display_screen);
                          screen[0][3][mouse + 6] = cu + '0';
                          break;
                        }
                    }
                        oled.setTextXY(1,mouse + 7);
                        oled.putString("_");
                    break;
                }
                case 4: {
                    timeClient.setRTCTime_offline(screen[0][1], screen[0][3]);
                    state[0]++;
                    break;
                }
                case 5: {
                    screen[0][6] = screen[0][1].substring(0,16) + 'Y' + 'N';
                    mouse = 0;
                    state[0]++;
                    display_screen();
                    break;
                }
                case 6: {
                    select.pres(mouse, 12, display_screen);
                    switch(mouse) {
                        case 10: {
                          int cu = 0;
                          switc.pres(cu, 2, display_screen);
                            if (cu == 1)
                              state[0] = 7;
                            break;
                        }
                        case 11: {
                          int cu = 0;
                          switc.pres(cu, 2, display_screen);
                            if (cu == 1)
                              state[0] = 0;
                            break;
                        }
                        default: {
                          int cu = screen[0][6][mouse + 6] - '0';
                          if(cu != '-' - '0' && mouse <10)
                            switc.pres(cu, 10, display_screen);
                          screen[0][6][mouse + 6] = cu + '0';
                          break;
                        }
                    }
                        oled.setTextXY(1,mouse + 7);
                        oled.putString("_");
                    break;
                }
                case 7: {
                    timeClient.setRTCTime_offline(screen[0][6], screen[0][0]);
                    state[0] = 0;
                    break;
                }
          }
          break;
        }
        default: {
          switc.pres(curs, 4, display_screen);
          select.pres(state[curs], state_num[curs], [&]{display_screen();Serial.println(state[3] * 100 + state[2] * 10 + _cmd + 1000);});
          break;
        }
    }
}
