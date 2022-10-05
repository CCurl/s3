#include "s3.h"

#if __SERIAL__
    int charAvailable() { return mySerial.available(); }
    void printString(const char* str) { mySerial.print(str); }
    int getC() { 
        while (!charAvailable()) {}
        return mySerial.read();
    }
    void putC(int ch) { 
        char b[2] = { (char)ch, 0 };
        printString(b);
    }
#else
    int charAvailable() { return 0; }
    int getC() { return 0; }
    void putC(int c) { }
    void printString(const char* str) { }
#endif

int isOTA = 0;

#ifdef __GAMEPAD__
#include <HID-Project.h>
#include <HID-Settings.h>
void gamePadBegin() { Gamepad.begin(); }

void gp_PressRelease(int btn) {
    Gamepad.press(btn);  
    Gamepad.write();
    delay(100);
    Gamepad.release(btn);
    Gamepad.write();        
}

long doGamePad(long ir, long pc) {
    ir = stb[pc++];
    switch (ir) {
    case 'X': Gamepad.xAxis(POP);          break;
    case 'Y': Gamepad.yAxis(POP);          break;
    case 'P': gp_PressRelease((int)POP);   break;
    case 'R': Gamepad.release(POP);        break;
    case 'A': Gamepad.dPad1(POP);          break;
    case 'B': Gamepad.dPad2(POP);          break;
    case 'L': Gamepad.releaseAll();        break;
    case 'W': Gamepad.write();             break;
    default:
        printString("-notGamepad-");
    }
    return pc;
}
#else
long doGamePad(long ir, long pc) { printString("-noGamepad-"); return pc; }
void gamePadBegin() { }
#endif

long doPin(long pc) {
    long pin = POP;
    byte ir = stb[pc++];
    switch (ir) {
    case 'I': pinMode(pin, INPUT);          break;
    case 'O': pinMode(pin, OUTPUT);         break;
    case 'U': pinMode(pin, INPUT_PULLUP);   break;
    case 'R': ir = stb[pc++];
        if (ir == 'A') { PUSH(analogRead(pin));  }
        if (ir == 'D') { PUSH(digitalRead(pin)); }
        break;
    case 'W': ir = stb[pc++];
        if (ir == 'A') { analogWrite(pin,  (int)POP); }
        if (ir == 'D') { digitalWrite(pin, (int)POP); }
        break;
    default:
        printString("-notPin-");
    }
    return pc;
}

long doUser(long ir, long pc) {
    switch (ir) {
    case 'G': pc = doGamePad(ir, pc);       break;
    case 'N': PUSH(micros());               break;
    case 'P': pc = doPin(pc);               break;
    case 'T': PUSH(millis());               break;
    case 'W': delay(POP);                   break;
    case 'L': PUSH(LED_BUILTIN);            break;
    default:
        printString("-notExt-");
    }
    return pc;
}

void loadCode(const char* src) {
    long here = h;
    while (*src) {
        stb[here++] = *(src++);
    }
    stb[here] = 0;
    Run(h);
}

// ********************************************
// * HERE is where you load your default code *
// ********************************************

#define SOURCE_STARTUP \
    X(1000, ":Code 0@1[nc@#58=(n1-c@59=(13,10,),];") \

//#if __BOARD__ == ESP8266
#define X(num, val) const char str ## num[] = val;
//#else
//#define X(num, val) const PROGMEM char str ## num[] = val;
//#endif
SOURCE_STARTUP

#undef X
#define X(num, val) str ## num,
const char *bootStrap[] = {
    SOURCE_STARTUP
    NULL
};

void loadBaseSystem() {
    for (int i = 0; bootStrap[i] != NULL; i++) {
        loadCode(bootStrap[i]);
    }
}

void s3() {
    printString("\ns3:(");
    fDotS();
    printString(")>");
}

// NB: tweak this depending on what your terminal window sends for [Backspace]
// E.G. - PuTTY sends a 127 for Backspace
int isBackSpace(char c) { 
  // printStringF("(%d)",c);
  return (c == 127) ? 1 : 0; 
}

void handleInput(char c) {
    static long here = 0;
    if (here == 0) { here = h; }
    if (c == 13) {
        printString(" ");
        stb[here] = 0;
        Run(h);
        here = h;
        s3();
        return;
    }

    if (isBackSpace(c) && (h < here)) {
        here--;
        if (!isOTA) {
          char b[] = {8, 32, 8, 0};
          printString(b);
        }
        return;
    }
    if (c == 9) { c = 32; }
    if (btw(c, 32, 126)) {
        stb[here++] = c;
        if (!isOTA) { printChar(c); }
    }
}

void setup() {
#ifdef __SERIAL__
    mySerial.begin(19200);
    while (!mySerial) {}
    delay(500);
    // while (mySerial.available()) { char c = mySerial.read(); }
    ok();
#endif
    init(1);
    gamePadBegin();
}

void do_autoRun() {
    long fa = st.i[1];
    if (fa) { Run(fa); }
}

#undef LED_BUILTIN
#define LED_BUILTIN 16
void loop() {
    static int iLed = 0;
    static long nextBlink = 0;
    static int ledState = LOW;
    long curTm = millis();

    if (iLed == 0) {
        loadBaseSystem();
        s3();
        iLed = LED_BUILTIN;
        pinMode(iLed, OUTPUT);
    }
    if (nextBlink < curTm) {
        ledState = (ledState == LOW) ? HIGH : LOW;
        digitalWrite(iLed, ledState);
        nextBlink = curTm + 1000;
    }

    while (charAvailable()) { 
        isOTA = 0;
        handleInput(getC()); 
    }
    do_autoRun();
}
