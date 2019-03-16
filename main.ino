//Khoa Truong
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <Servo.h>
#include <time.h>
#include <WiFiUdp.h> 
#include <NTPClient.h>
//--------------------------------------------------------------------------------------------------//
//                                        SUMMARIZE ALL METHODS
//--------------------------------------------------------------------------------------------------//
void set_time(String temp_state);                                     // set time for programs' clock
int check_match_time();                                               // check time of real clock with time in plant for feeding
void count_clock();                                                   // count clock.
void decode_signal(String temp_state);                                // decode list time of plant for feeding from the server to save it in program
void auto_feeding();                                                  // call this method to feed fish
void reset_time_feeding();                                            // delete list time of auto feeding
void receive_Infor(char*tp, byte * nd, unsigned int length);          

//--------------------------------------------------------------------------------------------------//
//                                        ALL STRUCTS AND GLOBAL VARIANCES
//--------------------------------------------------------------------------------------------------//
struct timeFeeding{
    int* elements;
    int num_of_time=0;
};
timeFeeding time_feeding;                                             // list time of auto feeding include the number and list

struct time_tank{
  int hour=0;
  int minute=0;
  int second=0;
};                                                            
time_tank tot;                                                        // clock
int check_time=0;     // check whether you update real time for program yet or not
int check_auto=0;     // if there is plant for feeding fish, check_auto=1 else=0
int allow_feed=1;     // used to feed fish follow plant. EX: when the clock reach the setup time for auto feeding, we call auto_feeding and allow_feed=0 to ensure
                      // auto_feeding called only once time. and when the clock change to next minute, allow_feed =1. 

//--------------------------------------------------------------------------------------------------//
//                                                  SET UP
//--------------------------------------------------------------------------------------------------//
WiFiUDP u;
Servo motor;
WiFiClient cli;
PubSubClient MQTT("m16.cloudmqtt.com",16357,receive_Infor,cli);

//--------------------------------------------------------------------------------------------------//
//                                                  SET_TIME
//--------------------------------------------------------------------------------------------------//
void Set_Time(String temp_state)
{
  tot.hour=0;
  tot.minute=0;
  tot.second=0;
  for (int i=0; i <=1; i++) tot.hour+=(temp_state[i]-48)*pow(10,1-i);
  for (int i=3; i <=4; i++) tot.minute+=(temp_state[i]-48)*pow(10,4-i); 
}
//--------------------------------------------------------------------------------------------------//
//                                                  CHECK_MATCH_TIME
//--------------------------------------------------------------------------------------------------//
int check_match_time()
{
  for (int i=0; i<time_feeding.num_of_time; i++)
  {
    int temp=tot.hour*100+tot.minute;
    if (temp==time_feeding.elements[i]) return 1;
  }
  return 0;
}
//--------------------------------------------------------------------------------------------------//
//                                                  COUNT_CLOCK
//--------------------------------------------------------------------------------------------------//
void count_clock()
{
  tot.second++;
  if (tot.second==60) 
  {
      tot.minute++;
    allow_feed=1;
    tot.second=0;
    if (tot.minute==60)
    {
      tot.hour++;
      tot.minute=0;
      if (tot.hour==24) tot.hour=0;  
    }
  }
}
//--------------------------------------------------------------------------------------------------//
//                                                  DECODE_SIGNAL
//--------------------------------------------------------------------------------------------------//
void decode_signal(String temp_state)
{
  check_auto=1;
  time_feeding.num_of_time =0;
  int dem = 0;
  int vtr_num_of_time = temp_state.indexOf('_');                          // get the location of the first '_' character
  int vtr_end_num_of_time = temp_state.indexOf('_',vtr_num_of_time+1);    // get the location of the second '_' character
  
  for (int i = vtr_num_of_time + 1; i <= vtr_end_num_of_time-1; i++)
      time_feeding.num_of_time += (temp_state[i]-48) * pow(10, vtr_end_num_of_time - 1 - i);    //get the number of time 
  
  time_feeding.elements= new int [time_feeding.num_of_time];              // create a matrix contained elements of auto-feeding time
  
  dem = -1;
  int* vtr;      // array contained the position of '_' from vtr_end_num_of_time to the end
  vtr= new int[time_feeding.num_of_time];
  for (int i=vtr_end_num_of_time+1; i<temp_state.length();i++)
    if (temp_state[i] == '_') 
      { dem++; 
        vtr[dem] = i; 
      }
  dem++;
  vtr[dem] = temp_state.length();
  
  int dem1 = 0;
  for (int i = 0; i < time_feeding.num_of_time; i++) time_feeding.elements[i] = 0;
  for (int i = vtr_end_num_of_time + 1; i < temp_state.length(); i++)
  {
    if (temp_state[i] != '_') time_feeding.elements[dem1] = time_feeding.elements[dem1]+ (temp_state[i] - 48)*pow(10, vtr[dem1] - 1 - i);
    else dem1++;
  }
 delete[]vtr;
}

//--------------------------------------------------------------------------------------------------//
//                                                  AUTO_FEEDING
//--------------------------------------------------------------------------------------------------//
void auto_feeding()
{
     /*//-----------open the box--------------
     for (int i=0; i<=25; i++)
     {
      motor.write(i);
      delay(25);
     }
      //----------------close the box-------------
     for (int i=25; i>=0; i--)
     {
      motor.write(i);
      delay(25);
     }
     //----------------------------------------time-------------*/
  
  char time_char[10]="";    // array demonstrate the real time
  int temp_hour=tot.hour;
  int temp_minute=tot.minute;
  int d=2;
  //------------------------------------------------------
  //              change the real time into array of char 
  //------------------------------------------------------
  if (temp_hour<10)
  {
    time_char[0]='0';
    time_char[1]=temp_hour+48;
  }else 
  do
  {
    d--;
    char i=(temp_hour%10)+48;
    time_char[d]=i;
    temp_hour=temp_hour/10;
  }while(temp_hour>0);
  //-----------------------------------------
  time_char[2]=':';
  d=5;
  //-----------------------------------------
  if (temp_minute<10)
  {
    time_char[3]='0';
    time_char[4]=temp_minute+48;
  }else
  do
  {
    d--;
    char i=(temp_minute%10)+48;
    time_char[d]=i;
    temp_minute=temp_minute/10;
  }while(temp_minute>0);
  //------------------------------------------
  //------------------------------------------
  char final_char[100]="You have fed your Fish at ";
  strcat(final_char,time_char);
  MQTT.publish("NOTE",final_char);
  Serial.println("...have fed fishes at.....");
  
}
//--------------------------------------------------------------------------------------------------//
//                                                  RESET_TIME_FEEDING
//--------------------------------------------------------------------------------------------------//
void reset_time_feeding()
{
  delete[]time_feeding.elements;
  time_feeding.num_of_time=0;   
  int check_auto=0;       // there is no plant for feeding
}
//--------------------------------------------------------------------------------------------------//
//                                                  RECEIVE_INFOR
//--------------------------------------------------------------------------------------------------//
void receive_Infor(char*tp, byte * nd, unsigned int length)
{
    Serial.println("There is a message from server:");
    Serial.print("number of time ");
    Serial.println(time_feeding.num_of_time);
    for (int i=0; i<time_feeding.num_of_time;i++) Serial.println(time_feeding.elements[i]);
    String topic(tp);
    String temp_state= String((char*)nd);
    temp_state.remove(length);
    
    int state = int(temp_state[0])-48;  
    if (topic=="Feeding_Fish")
    {
        if (state==1) auto_feeding();
        if (state==2) decode_signal(temp_state);
        if (state==3) reset_time_feeding();
    }
    else if (topic=="Set_Time") {Set_Time(temp_state); check_time=1;}
    Serial.print(tot.hour); Serial.print(":"); Serial.print(tot.minute); Serial.print(":"); Serial.println(tot.second);
}

void setup() {
   Serial.begin(115200);
  
  WiFi.begin("THREE O'CLOCK","3open24h");         
  while (WiFi.status() != WL_CONNECTED)
  {delay (50);
  Serial.print(".");
  }
  Serial.println("Wifi is connected");
  
  while (1)
  { delay (500);
    if (!MQTT.connect("Aquatic Tank","bfglrgpa","Xu7o8LYQCn4y")) 
    Serial.print(".");  
    else break;
  }
  Serial.println("Connected to server");
  MQTT.publish("List Features","Topic...................................Features");    
  MQTT.publish("             ","Set_Time....................hh:mm");                  
  MQTT.publish("             ","Feeding_Fish....................1: Auto Feeding");
  MQTT.publish("             ","Feeding_Fish....................2: Set time for feeding.EX:hhmm");
  MQTT.publish("             ","Feeding_Fish....................3: Reset time for feeding");
                      
  MQTT.publish("             ","Take_Temp.......................1: Take Temperature");
  MQTT.publish("             ","Pumper..........................1: Release Water");
  MQTT.publish("             ","Pumper..........................2: Pump Water");
  MQTT.subscribe("Feeding_Fish");   
  MQTT.subscribe("Set_Time");
  MQTT.subscribe("Take_Temp"); 
  MQTT.subscribe("Pumper");                                            
  
  //motor.attach(MOTOR_GATE);
}


void loop() { 
  MQTT.loop();
  if (check_time)
  {
  if (check_auto==1 && check_match_time()==1 && allow_feed==1) { auto_feeding(); allow_feed=0;}
  count_clock();
  delay(1000);
  }
}
