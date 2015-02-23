#include <Arduino.h>

static void color(char foreground);
static void cheer();
static void putdec( int16_t n );
static void gabber(char front);
static bool rune(char cha);
void dancer();
void setup();
void loop();
#line 1 "src/orc/orc.ino"
#define PARSE      1
#define LAMBDA     2
#define STRING     3
#define ESC        4
#define COMMENT    5
#define JANK       6

#define BLACK      0        // doubt I'll use this...
#define RED        1
#define GREEN      2
#define YELLOW     3
#define BLUE       4
#define MAGENTA    5
#define CYAN       6
#define WHITE      7        // or this
#define RESET      9        // preferring this.

#define GABMAX     8

#define LETTER     1
#define RUNE       2
#define PEL        3
#define NUM        4
#define PER        5
#define SPAZ       6


const unsigned char HI[] = "@rk!";

const char CLR[] = "\33[30m"; // 7 byte - not aligned

char bracecount = 0;
unsigned char state = 0;
long pad = 45;
unsigned char gab[GABMAX];         // we can freeze it to EEPROM when it spills.
char gibber = -1;
char was_cha = 0;
bool head = true;

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
        case 'W' :
            state = 1 ;
            break;
        case '@' :
            state == 1? state = 2 : state = 0;
            break;
        case 'g' :
            state == 2? state = 3 : state = 0;
            break;
        case '!' :
            state == 3? online = true : state = 0; // falls thru
        default  :
            state = 0;
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
    if (d4) {
        Serial.print( d4 + '0' );
    };
    if (d3) {
        Serial.print( d3 + '0' );
    };
    if (d2) {
        Serial.print( d2 + '0' );
    };
    if (d1) {
        Serial.print( d1 + '0' );
    };
    Serial.print( d0 + '0' );
}

static void gabber(char front) {
    if (gibber < GABMAX-1) {
        gab[++gibber] = front;
    } else {
        //  color(RED); Serial.print('*'); color(RESET);
        gibber = 1;
        gab[0] = gab[GABMAX-1];
        gab[1] = front;
    }
}

static bool rune(char cha) {
    //determines a glyph.
    if ((32 < cha) && (cha < 48) && !(cha == '(' || cha == ')')) {
        return true;
    }
    if ((90 < cha) && (cha < 97)) {
        return true;
    }
    if ((57 < cha) && (cha < 65)) {
        return true;
    }
    if ((122 < cha) && (cha < 127)) {
        return true;
    }
    return false;
}

void dancer() { // first of the reindeer
    char bite;
    char is_cha = 0;
    if (Serial.available() && (bite = Serial.read())) {
        color(RESET); //should become superfluous, remove.
        gabber(bite);
        switch(bite) {
        case '(' :
            is_cha = PEL;
            if (++bracecount > 0) {
                color(bracecount % 8 + 1);
            }
            break;
        case ')' :
            is_cha = PER;
            if (bracecount > 0) {
                color(bracecount % 8 + 1);
                bracecount--;
            } else {
                color(9);
                bracecount = 0;
            }
            break;
        case '\r' :
            is_cha = SPAZ;
            Serial.print("\r\n"); //jumpcall
            for (byte i = 0 ; i < GABMAX; i++) {
                Serial.print(char(gab[i])); //diagnostic
            }
            Serial.print("\r\n");
            break;
        }
        if (bite == ' ' || bite == ',') {
            is_cha = SPAZ;
        }
        if (('0' <= bite) && (bite <= '9')) {
            is_cha = NUM;
            color(CYAN);
        }
        if (rune(bite)) {
            is_cha = RUNE;
        }
        if (('A' < bite) && (bite <'z') && is_cha != RUNE) {
          is_cha = LETTER;
        }
        if (is_cha == RUNE || was_cha == RUNE) {
            color(GREEN);
        }
        if (is_cha == RUNE && !head) {
            // backup, recolor, print one from gab.
            Serial.print("\33[D"); // generalize jump command
            color(GREEN);
            Serial.print(char(gab[gibber-1]));
        }

        if (head) {
            Serial.print("\33[4m");
        }
        // Serial.print(bite); faster
        Serial.print(char(gab[gibber])); //keeps us honest
        if (head) {
            Serial.print("\33[0m");
        }
        //Serial.print(head);
        // setup next loop
        if (is_cha <= RUNE) {
          head = ! head;
        }
        was_cha = is_cha;
    }
}

void setup() {
    // setup µlisp
    gab[0] = '\0';
    state = 0;
    Serial.begin(9600); // helps with resets?
    Serial.print(pad); //remove
}
void loop() {
    // if there's any serial available, read it:
    if (state < 4) {
        cheer();
    }
    if (online) {
        if (state == 0) {
            Serial.print("@rk!\r\n(");
            state = 5;
        } else {
            dancer();
        }
    }
}








