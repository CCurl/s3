#include <Arduino.h>
#include <stdarg.h>
#include "s3.h"

int32_t timerMS() { return millis(); }
int32_t timerNS() { return micros(); }

#ifdef mySerial
    void SerialInit() {
        mySerial.begin(19200);
        while (!mySerial) {}
        delay(500);
    }
    int charAvailable() { return mySerial.available(); }
    void printString(const char* str) { mySerial.print(str); }
    int getC() { 
        while (!charAvailable()) {}
        return mySerial.read();
    }
    void putC(int ch) { 
        mySerial.print((char)ch);
    }
    void printStringF(const char* fmt, ...) {
        char buf[64];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        printString(buf);
    }
#else
    void SerialInit() { }
    int charAvailable() { return 0; }
    int getC() { return 0; }
    void putC(int c) { }
    void printString(const char* str) { }
    void printStringF(const char* fmt, ...) { }
#endif // __mySerial__

#ifdef __FILES__
#include "files.h"
#else
int32_t doFopen(const char *fn, int mode) { return 0; }
void doFclose(int32_t fh) { }
char *doFgets(char *buf, int sz, int32_t fh) { *buf = 0; return buf; }
int doFread(void* buf, int sz, int num, int32_t fh) { return 0; }
int doFwrite(void* buf, int sz, int num, int32_t fh) { return 0; }
#endif // __FILES__

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

int32_t doGamePad(int32_t ir, int32_t pc) {
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
int32_t doGamePad(int32_t ir, int32_t pc) { printString("-noGamepad-"); return pc; }
void gamePadBegin() { }
#endif // __GAMEPAD__

int32_t doPin(int32_t pc) {
    int32_t pin = POP;
    uint8_t ir = stb[pc++];
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

int32_t doUser(int32_t ir, int32_t pc) {
    switch (ir) {
    case 'G': pc = doGamePad(ir, pc);       break;
    case 'P': pc = doPin(pc);               break;
    case 'W': delay(POP);                   break;
    case 'L': PUSH(LED_BUILTIN);            break;
    default:
        printString("-notExt-");
    }
    return pc;
}

void loadCode(const char* src) {
    int32_t x = h;
    printString(src);
    printString("\r\n");
    while (*src) { stb[x++] = *(src++); }
    stb[x] = 0;
    Run(h);
}

// ********************************************
// * HERE is where you load your default code *
// ********************************************

void loadBaseSystem() {
  loadCode(":CODE 0@1[nc@#58=(n1-c@59=(13,10,),];");
  loadCode(":MIL 1000**;");
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
    static int32_t here = 0;
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
        if (!isOTA) { putC(c); }
    }
}

void setup() {
    SerialInit();
    init(1);
    // s3();
    gamePadBegin();
}

void do_autoRun() {
    // int32_t x = st.i[1];
    // if (x) { Run(x); }
}

// #undef LED_BUILTIN
// #define LED_BUILTIN 16
void loop() {
    static int iLed = 0;
    static int32_t nextBlink = 0;
    static int ledState = LOW;
    int32_t curTm = timerMS();

    if (iLed == 0) {
        loadBaseSystem();
        printString("-base system loaded-\r\n");
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
