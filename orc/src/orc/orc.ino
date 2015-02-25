// colors are semantic.
#define BLACK      0        // doubt I'll use this...
#define RED        1
#define GREEN      2
#define YELLOW     3
#define BLUE       4
#define MAGENTA    5
#define CYAN       6
#define WHITE      7        // or this
#define RESET      9        // preferring this.

#define GABMAX     64       // not found in blitzen
#define DRPMAX     64

//Phonemes
#define LETTER     1        // pair letter and rune
#define RUNE       2
#define PEL        3
#define DIGIT      4
#define PER        5
#define SPACE      6

//Lexemes
#define HEAD       8       // 
#define TAIL       16
#define CAR        32       // pair car and cdr
#define CDR        64

//Parsemes
#define SYMBOL     17
#define NUMBER     18
#define STRING     19
#define COMMENT    20
#define ESCAPE     21
#define JANK       22

/*
typedef slot {  // holds userdata
 // union type, 32 bits
 // with type table, one bit per
}
*/
const char HI[] = "@rk!";
const char CLR[] = "\33[30m";

char bracecount = 0;
char state = 0;                // online or not
char phoneme = 0;              // type of letter detected
char lexeme  = 0;              // place in symbol order.
char parseme = 0;
char gab[GABMAX];
char drp[DRPMAX];
char gibber = 0;
char derp = 0;
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

static void clear() { // fold into comprehensive color printer
    Serial.print("\33[0m");
}

static void cheer() { // detects the Cheer
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
    // shamelessly stolen from
    // http://www.piclist.com/techref/language/ccpp/convertbase.htm
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

static void herpderp(char front) {
    if (derp < DRPMAX-1) {
        drp[++derp] = front;
    } else {
        derp   = 1;
        drp[0] = drp[DRPMAX-1];
        drp[1] = front;
    }
}

static bool rune(char cha) {
    //determines a glyph.
    if ((32 < cha) && (cha < 48) && !(cha == '(' || cha == ')')) {
        return true;
    }
    if ((90 < cha)  && (cha < 97))  {
        return true;
    }
    if ((57 < cha)  && (cha < 65))  {
        return true;
    }
    if ((122 < cha) && (cha < 127)) {
        return true;
    }
    return false;
}

/*
void dancer() { // first of the reindeer
    char bite;
    bool tail = true; // the next character is a head by default
    char is_cha = 0;
    if (Serial.available() && (bite = Serial.read())) {
        gabber(bite);
        herpderp(bite);
        // Read
        if (bite == ' ' || bite == ',') {
            is_cha = SPACE;
        }
        if (('0' <= bite) && (bite <= '9')) {
            is_cha = NUMBER;
            color(CYAN);
        }
        if (rune(bite)) {
            is_cha = RUNE;
            tail = ! head;
        }
        if (('A' <= bite) && (bite <= 'z') && is_cha != RUNE) {
            is_cha = LETTER;
            tail = ! head;
            color(RESET);
        }
        if (is_cha == RUNE || (was_cha == RUNE && is_cha == LETTER && !head)) {
            color(GREEN);
        }
        switch(bite) {
        case '(' :
            is_cha = PEL;
            state = CAR;
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
            is_cha = SPACE;
            state = CAR;
            Serial.print("\r\n"); //jumpcall
            for (byte i = 0 ; i < gibber; i++) {
                Serial.print(char(gab[i])); //diagnostic
            }
            gibber = 0;
            gab[0] = '(' ; // now there's a dirty hack
            Serial.print("\r\n");
            break;
        case '\127' : // delete key
            is_cha = gab[--gibber] ;
            state = CAR ; //Fuck: copy gab forward by one and reparse using gab.
            Serial.print(">>>>");
        }

        // Report
        if (is_cha == RUNE && was_cha == LETTER && !head) {
            // backup, recolor, print one from gab.
            Serial.print("\33[D"); // generalize jump command
            color(GREEN);
            Serial.print(char(gab[gibber-1]));
        }

        if (state == CAR && (is_cha != PEL)) {
            Serial.print("\33[4m"); // underline
            state = CDR;
        }
        if (was_cha == NUMBER && is_cha != NUMBER && !head) {
            Serial.print("\33[0m");
            state = CDR;
        }
        // Serial.print(bite); faster
        Serial.print(char(gab[gibber])); //keeps us honest

        // Setup
        if ((state == CAR && !head)  ) {
            Serial.print("\33[0m"); // clear
            state = CDR;
        }

        was_cha = is_cha;
        head = tail ;
        // Loop
    }
} */

void dancer() { // first of the reindeer, 0.2
// let's try this again.
// dancer is a parser.
// it sets up the gab and derp, determines semantics, and
// moves forward.
// dancer consumes one letter at a time.
    char bite;
    bool tail = true; // the next character is a head by default
    char is_cha = 0;
    if (Serial.available() && (bite = Serial.read())) {
        gabber(bite);
        herpderp(bite);
        if (true) {    // parseme == symbol
            if (rune(bite)) {
                is_cha = RUNE;
                phoneme = RUNE;
                parseme = SYMBOL;
            }
            if (('0' <= bite) && (bite <= '9')) {
                is_cha = NUMBER;
                phoneme = DIGIT;
                parseme = NUMBER;
            }
            switch(bite) {
            case '(' :
                is_cha = PEL;
                phoneme = PEL;
                lexeme = CAR + HEAD;
                ++bracecount;
                break;
            case ')' :
                is_cha = PER;
                phoneme = PEL;
                parseme = SYMBOL;
                if (bracecount > 0) {
                    --bracecount;
                } else {
                    bracecount = 0;
                }
                break;
            case '\r' :
                is_cha = SPACE;
                parseme = SYMBOL;
                Serial.print("\r\n"); //jumpcall
                for (byte i = 0 ; i < gibber; i++) {
                    Serial.print(char(gab[i])); //diagnostic
                }
                gibber = 0;
                gab[0] = '(' ; // now there's a dirty hack
                lexeme = CAR + HEAD ;
                Serial.print("\r\n");
                break;
            case '\127' : // delete key
            //    is_cha = gab[--gibber] ;
                Serial.print(">>>>");
            }
        }
        switch(phoneme) {
        case LETTER :
            color(RESET);
            break;
        case RUNE   :
            color(GREEN);
            break;
        case PEL    :
            color(bracecount % 8);
            break;
        case DIGIT  :
            color(CYAN);
            break;
        case PER    :
            color(bracecount % 8);
            break;
        }
        if (phoneme != PEL) {
            Serial.print("λ");
            clear();
        }
        Serial.print(gab[gibber]);


    }
}
void setup() {
    // setup µlisp
    gab[0] = '(';
    drp[0] = '(';
    state = 0;
    Serial.begin(9600);
}

void loop() {
    if (state < 4 && online == false) { // state should be negative when not interacting
        cheer();
    }
    if (online) {
        if (state == 0) {
            Serial.print(HI);
            Serial.print("\r\n(");
            state = CAR;
        } else {
            dancer();
        }
    }
}








