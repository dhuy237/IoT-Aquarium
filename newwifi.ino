#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

ESP8266WebServer sv(80);
void setup() {
WiFi.mode(WIFI_AP_STA);
WiFi.softAP("this is Khoa","12345678");  
WiFi.begin("Minh Dung", "07031998md");
while (WiFi.waitForConnectResult() != WL_CONNECTED)
  delay(100);
  Serial.begin(115200);
  Serial.println(WiFi.localIP());
  sv.on("/",HTTP_GET,[]{
    Serial.println(String("truy cap thanh cong"));
    sv.send(200,"text/html",
"<html>"
"<head>"
"<title>"
"Khoa ne may ban"
"</title>"
"</head>"
"<body>"
"This is Khoa website"
"<button type=(button)>"
"Click me!"
"</button>"
"</body>"
"</html>"
    
   );
  });
  sv.begin();
}

void loop() {
  sv.handleClient();
}
