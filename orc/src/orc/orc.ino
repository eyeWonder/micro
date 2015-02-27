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
#define LETTER     0b00000001          // 1
#define RUNE       0b00000010          // 2
#define PEL        0b00000100          // 4
#define DIGIT      0b00001000          // 8
#define PER        0b00010000          // 16
#define SPACE      0b00100000          // 32
#define QUOTE      0b01000000          // 64

#define GLYPH      0b00000011          // LETTER | RUNE     

//Lexemes
#define CAR        12
#define CDR        14
#define ERROR      15

//Parsemes
#define SYMBOL     17
#define NUMBER     18
#define STRING     20
#define COMMENT    21
#define ESCAPE     27
#define JANK       29
#define LAMBDA     93
#define FAIL       42

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
char lexeme  = CAR;              // place in symbol order.
char parseme = SYMBOL;              // parser at work
char phoneme = 0;
bool head = false;           // head or tail of symbol
char gab[GABMAX];
char drp[DRPMAX];
char gibber = 0;
char derp = 0;
char was_cha = 0;

bool online = true ; // useless replace with state

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

void printabove(char above) {
    // a helper function to print stuff above.
    Serial.print("\33[A"); // up
    Serial.print(above);
    Serial.print("\33[D\33[B"); // back/down
}

void dancer(char bite) { // first of the reindeer, 0.2
// dancer is a colorful parser.
// dancer consumes one letter at a time.
chew:  // Djikstra forgive me. Knuth would understand.
    phoneme = 0;
    gabber(bite);
//     herpderp(bite);    // forget the derp for now

parse:
    if (bite == 127) { // delete key
        // move to own function, protect against deletes past zero!
        --gibber; // walk back to last cha
        if (gab[gibber] == '(') {
            --bracecount;
        }
        if (gab[gibber] == ')') {
            ++bracecount;
        }
        --gibber; // prior to last cha
        Serial.print("\33[D \33[D");
        goto chew;
    }
    switch (parseme) {
    case SYMBOL:         // parseme == symbol
        if (('0' <= bite) && (bite <= '9')) {
            phoneme = DIGIT;
            parseme = NUMBER;
            head = false;
        }
        if (rune(bite)) {
            phoneme = RUNE;
        }
        if (('A' <= bite) && (bite <= 'z') && phoneme != RUNE) {
            phoneme = LETTER;
        }
        switch(bite) {
        case '(' :
            phoneme = PEL;
            lexeme = CAR;
            head = true;
            ++bracecount;
            break;
        case ')' :
            phoneme = PER;
            if (bracecount >= 0) {
                --bracecount;
            } else {
                bracecount = -1; // lower bound -1
            }
            break;
        case '"' :
            parseme = STRING;
            phoneme = ESCAPE; // reused for jump
            head = false;
            goto parse;
        case ';' :
            parseme = COMMENT;
            head = false;
            goto parse;
            break;
        case '\r' :
            phoneme = SPACE;
            //<diagnostic>
            clear();
            Serial.print("\r\n"); //jumpcall
            for (byte i = 0 ; i < gibber; i++) {
                Serial.print(char(gab[i]));
            }
            //</diagnostic>
            gibber = -1;
            Serial.print("\r\n");
            break;
        case ' ' :
            phoneme = SPACE;
            head = false;
            break;
        }
        break;
    case NUMBER:
        if (!('0' <= bite) || !(bite <= '9')) {
            parseme = SYMBOL;
            goto parse;
        }
        break;
    case STRING: // include escaping logic, for now, close on "
        if (phoneme == ESCAPE) {
            phoneme = QUOTE;
        } else {
            if (bite == '"') {
                parseme = SYMBOL;
                color(YELLOW);
                goto send_bite;
            }
        }
        break;
    case COMMENT:
        if (bite == '\r') {
            parseme = SYMBOL;
            goto parse;
        }
        if (bite == ';') {
            color(BLACK);
            goto send_bite;
        }
        break;
    case ESCAPE:
        // handle ANSI escape sequences
        // 'minimal compliance'
        // see http://en.wikipedia.org/wiki/ANSI_escape_code
        break;
    }   // ends switch(parseme)
    if (phoneme & GLYPH) {
        if (was_cha & (LETTER | RUNE)) {
            head = !head;
        } else {
            head = true;
        }
    }
    if ((phoneme == RUNE) && (was_cha == LETTER) && !head) {
        Serial.print("\33[D"); // generalize jump command
        color(GREEN);
        Serial.print(char(gab[gibber-1]));
    }
    if(parseme != SYMBOL || phoneme == SPACE) {
        lexeme = CDR;
    }

report:
    switch(phoneme) {
    case LETTER :
        if (was_cha == RUNE && !head) {
            color(GREEN);
        }
        else {
            color(WHITE);
        }
        break;
    case RUNE   :
        color(GREEN);
        break;
    case PEL    :
        if (bracecount <= 0) {
            color(RESET);
        } else {
            color(bracecount % 8);
        }
        break;
    case DIGIT  :
        color(CYAN);
        break;
    case QUOTE :
        color(YELLOW);
        break;
    case PER    :
        if (bracecount < 0) { // this is a syntax error, later.
            color(RESET);
        } else {
            color((bracecount % 8)+1);
        }
        break;
    }
    switch(lexeme) {
    case CAR:
        Serial.print("\33[40m");
        break;
    case CDR:
        Serial.print("\33[49m");
        break;
    }

send_bite:
    if (head) {
        Serial.print("\33[4m");
    } else {
        Serial.print("\33[24m");
    }
    Serial.print(gab[gibber]);

next:
    was_cha = phoneme;
    if(!head && (was_cha & GLYPH)) {
        lexeme = CDR;
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
    char bite;
    if (state < 4 && online == false) { // state should be negative when not interacting
        cheer();
    }
    if (online) {
        if (state == 0) {
            Serial.print(HI);
            Serial.print("\r\n\n(");
            state = CAR;
        } else {
            if (Serial.available() && (bite = Serial.read())) {
                dancer(bite);
            }
        }
    }
}








