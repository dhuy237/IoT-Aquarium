//
//  main.cpp
//  codeforce
//
//  Created by Khoa Nguyen on 4/9/19.
//  Copyright © 2019 Khoa Nguyen. All rights reserved.
//

#include <SPI.h>
#include <SD.h>
//String filename = "testSD";
class sdk {
public: 
sdk(){SD.begin(D8);}
~sdk(){}
String SDprint(String filename) //print data in file
{
    File myFile;
    String temp;
    myFile = SD.open(filename + ".txt");
    if (myFile) {
        // read from the file until there's nothing else in it:
            temp = myFile.readStringUntil('\n');
            //Serial.println(temp);
        myFile.close();
    }
    return temp;
}
void SDcheck()
{
    Serial.print("Initializing SD card...");
    
    if (!SD.begin(D8)) {
        Serial.println("initialization failed!");
        return;
    }
    Serial.println("initialization done.");
}
void fileCheck(String filename) // Check if file is existed then remove it
{
    File myFile;
    
    if (SD.exists(filename+".txt")) {
        Serial.println("file exists.");
        // delete the file:
        Serial.println("Removing file");
        SD.remove(filename+".txt");
        SD.rmdir(filename+".txt");
    } else {
        Serial.println("file doesn't exist.");
    }
}
void SDwrite(String filename,String content)// Write data to file
{
    File myFile;
    Serial.println("writing");
    myFile = SD.open(filename +".txt");
    if (myFile) {
        // read from the file until there's nothing else in it:
            String temp = myFile.readStringUntil('\n');
            for(int i=0;i<temp.length();i++)
                if(temp[i] == content[0])
                    return;
        myFile.close();
    }
    myFile = SD.open(filename + ".txt", FILE_WRITE);
    if(myFile) {
        myFile.print(content); //Very Important
	Serial.println("Writing succ");
    }
    else
        Serial.println("Writing error");
    myFile.close();
}
//////////////////////////
void SDdelete(String filename,String content) //Delete data in file
{
    File myFile;
    int i = 0;
    String temp;
    myFile = SD.open(filename+".txt");
    if (myFile)
    {
      //  while (myFile.available()) {
            temp = myFile.readStringUntil('\n');
       // }
        myFile.close();
    }
    fileCheck(filename);
    myFile = SD.open(filename+".txt", FILE_WRITE);
    if(myFile) {
        for(int j = 0; j< temp.length(); j++)
            if(temp[j] != content[0])
                myFile.print(temp[j]);
    }
    else
        Serial.println("Writing error");
    myFile.close();
}
};