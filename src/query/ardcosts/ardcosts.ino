
//bare setup() and loop(): 4,242 bytes(!) on Leonardo
// -- 314 bytes on the trinket. >.<

void setup() {
    Serial.begin(57600); // 4,256, so nothing
    Serial.available();  // 4,272
    Serial.available();  // 4,278, 3 words? 
    char foo = Serial.read(); // 4284
    foo = Serial.read();      // 4290
   // foo = sin(foo);  
   Serial.print(foo) ;       //  4314, 12 words 
   Serial.print(foo) ;       //  4228, 7 words
   Serial.print(foo) ;       //  4336, 4 words?
   Serial.print(foo) ;       //  4344  4 words now
   foo++ ;                   // a word as I'm an ape
   Serial.print(foo);        //  4354  5 words indeed
                             // printing should be two words
                             // and sometimes one.
}
void loop() {

  
}
