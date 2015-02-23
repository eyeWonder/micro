#include <Arduino.h>
#include <SoftwareSerial.h>               // 632  
void setup();
void loop();
#line 1 "src/query/tricketcosts/tricketcosts.ino"
                                          // 338 (314 16 mHz)
//#include <SoftwareSerial.h>               // 632  

SoftwareSerial s(0,1);                    // 2054

void setup() {   
s.begin(9600);                            // 2542
}// set baud rate to 9600 baud      
void loop() 
{
s.write(12);                              // 2550
} 
