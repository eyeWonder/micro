#include <Arduino.h>

static void color(char foreground);
static void cheer();
static void putdec( int16_t n );
static void gabber(char front);
static bool glyph(char cha);
void dancer();
void setup();
void loop();
#line 1 "src/orc/orc.ino"
#define PARSE      0
#define LAMBDA     1
#define STRING     2
#define ESC        3
#define COMMENT    4
#define JANK       5

#define BLACK      0        // doubt I'll use this...
#define RED        1
#define GREEN      2
#define YELLOW     3
#define BLUE       4        
#define MAGENTA    5
#define CYAN       6
#define WHITE      7        // or this
#define RESET      9        // preferring this.

#define GABMAX     16

#define LETTER     0
#define RUNE       1
#define PEL        2
#define NUM        3
#define PER        4


const unsigned char HI[] = "@rk!";

const char CLR[] = "\33[30m"; // 7 byte - not aligned

char bracecount = 0; 
unsigned char state = 0;             // reuse aggressively
unsigned char gab[GABMAX+1];         // we can freeze it to EEPROM when it spills. 
bool was_glyph = false; 

bool online = false ; // useless replace with state

static void color(char foreground) { // prints a foreground color, for now
  // bum: bitfield: @@@@$$$$
  // @: color value (number to sub) (low!)
  // $: type of attribute (other number to sub). Defaults to foreground.
  char clr[7];
  strcpy(clr,CLR);
  clr[3] = foreground + 48 ;
  Serial.print(clr);  
}

static void cheer() { // detects the Cheer; make async
 //   online = Serial.find("W@g!");
  char bite;
  if (Serial.available() && (bite = Serial.read())) {
    switch(bite) {
 //      case 'W' : state = 1 ; break;
 //      case '@' : state == 1? state = 2 : state = 0; break;
 //      case 'g' : state == 2? state = 3 : state = 0; break;
       case '!' : online = true ; 
 //: state == 3? online = true : state = 0; // falls thru
       default  : state = 0;        
    }
  }
}

static void putdec( int16_t n ) {
        unsigned char d4, d3, d2, d1, d0, q;
        if (n < 0) {
            Serial.print( '-' );
            n = -n;
        }
        d1 = (n>>4)  & 0xF;
        d2 = (n>>8)  & 0xF;
        d3 = (n>>12) & 0xF;
        d0 = 6*(d3 + d2 + d1) + (n & 0xF);
        q = (d0 * 0xCD) >> 11;
        d0 = d0 - 10*q;
        d1 = q + 9*d3 + 5*d2 + d1;
        q = (d1 * 0xCD) >> 11;
        d1 = d1 - 10*q;
        d2 = q + 2*d2; 
        q = (d2 * 0x1A) >> 8;
        d2 = d2 - 10*q;
        d3 = q + 4*d3;
        d4 = (d3 * 0x1A) >> 8;
        d3 = d3 - 10*d4;
      if (d4) {Serial.print( d4 + '0' );};
      if (d3) {Serial.print( d3 + '0' );};
      if (d2) {Serial.print( d2 + '0' );};
      if (d1) {Serial.print( d1 + '0' );};
      Serial.print( d0 + '0' );
}

static void gabber(char front) {
    if (gab[0] < GABMAX) {
      gab[++gab[0]] = front;
    } else {
    //  color(RED); Serial.print('*'); color(RESET);
      gab[0] = 1;
      gab[1] = front;
   }
}

static bool glyph(char cha) {
  //determines a glyph.  
  if ((32 < cha) && (cha < 48) && !(cha == '(' || cha == ')'))   { return true; }
  if ((90 < cha) && (cha < 97))   { return true; }
  if ((57 < cha) && (cha < 65))   { return true; }
  if ((122 < cha) && (cha < 127)) { return true; }
  return false;
}

void dancer() { // first of the reindeer
   char bite;
   if (Serial.available() && (bite = Serial.read())) {
     color(RESET); //should become superfluous, remove.
     gabber(bite);
     switch(bite){ 
       case '(' :
         if (++bracecount > 0) {
           color(bracecount % 8 + 1); 
         }
         break;
       case ')' :
         if (bracecount > 0) {
            color(bracecount % 8 + 1);
            bracecount--;
         } else {
           color(9);
           bracecount = 0;
         }
         break;
       case '\r' :
          Serial.print("\r\n"); //jumpcall
          break;                
     }
     if ((47 < bite) & (bite < 58)) {
        color(CYAN);
     }
     bool is_glyph = glyph(bite);
     if (is_glyph || was_glyph) {
        color(GREEN); 
    }
    if (is_glyph && ! was_glyph) {
      // backup, recolor, print one from gab.
      Serial.print("\33[D"); // generalize
      color(GREEN);
      Serial.print(char(gab[gab[0]-1]));      
    }
    was_glyph = is_glyph;
    Serial.print(bite); 
    // Serial.print(char(gab[gab[0]])); should be same
  }
}

void setup() {
  // setup µlisp
  gab[0] = '\0';
  state = 0;
  Serial.begin(57600);
}
void loop() {
  // if there's any serial available, read it:
    if (!online) { cheer(); }
    if (online) {
        if (state == 0) {
          Serial.print("@rk!\r\n(");
          state = 1;
        } else {
          dancer();
        }
    }
}








