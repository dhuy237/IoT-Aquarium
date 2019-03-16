//Viet Khoa
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define DHTPIN D4
#define DHTTYPE DHT11
void getdata(char * tp, byte *nd, unsigned int length){}
DHT dht(DHTPIN, DHTTYPE);
WiFiClient c;
PubSubClient TempControl("m16.cloudmqtt.com", 15419 ,getdata,c);
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
WiFi.begin("Minh Dung","07031998md");
while (WiFi.status() != WL_CONNECTED)
{
  Serial.print("connecting");
  delay(100);
}
TempControl.connect("Temperature control", "yusinxjw", "HUJ8CugaQYaJ");
dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
float t=dht.readTemperature();
float h=dht.readHumidity();
Serial.print("Nhiet do la: ");
Serial.print(t);
TempControl.publish("nhiet do",String(t).c_str());
TempControl.publish("do am",String(h).c_str());
delay(5000);
}
