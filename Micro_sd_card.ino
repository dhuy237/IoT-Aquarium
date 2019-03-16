
#include <SPI.h>
#include <SD.h>
int temp = 0;
int dept = 0;
int a = 1;
File myFile;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ;
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  if (SD.exists("example.txt")) {
    Serial.println("example.txt exists.");
  } else {
    Serial.println("example.txt doesn't exist.");
  }


  Serial.println("Creating example.txt...");
  myFile = SD.open("example.txt", FILE_READ);
  char r;
  while(myFile.available())
  {
    r = myFile.read();
    if (r == '*') 
    {a = a+1; continue;}
    else
    switch(a){
    case 1: {temp = temp * 10 + int(r) - '0';break;}
    case 2: {dept = dept * 10 + int(r) - '0';break;}
    }
  }
  myFile.close();

  
  // Check to see if the file exists:
  if (SD.exists("example.txt")) {
    Serial.println("example.txt exists.");
  } else {
    Serial.println("example.txt doesn't exist.");
  }

  Serial.print("Temperature: ");
  Serial.println(temp, DEC);
  Serial.print("Dept: ");
  Serial.print(dept,DEC);

}
void loop() {
  // nothing happens after setup finishes.
}
