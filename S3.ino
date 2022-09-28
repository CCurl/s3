#include "s3.h"

#if __SERIAL__
    int charAvailable() { return mySerial.available(); }
    int getChar() { 
        while (!charAvailable()) {}
        return mySerial.read();
    }
    void printSerial(const char* str) { mySerial.print(str); }
#else
    int charAvailable() { return 0; }
    int getChar() { return 0; }
    void printSerial(const char* str) { }
#endif

int isOTA = 0;

void printString(const char* str) { 
    printSerial(str);
}

void printChar(const char ch) { 
    char b[2] = { ch, 0 };
    printString(b);
}

CELL getSeed() { return millis(); }

#define __GAMEPAD__
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

addr doGamePad(byte ir, addr pc) {
    ir = *(pc++);
    switch (ir) {
    case 'X': Gamepad.xAxis(pop());          break;
    case 'Y': Gamepad.yAxis(pop());          break;
    case 'P': gp_PressRelease((int)pop());   break;
    case 'R': Gamepad.release(pop());        break;
    case 'A': Gamepad.dPad1(pop());          break;
    case 'B': Gamepad.dPad2(pop());          break;
    case 'L': Gamepad.releaseAll();          break;
    case 'W': Gamepad.write();               break;
    default:
        isError = 1;
        printString("-notGamepad-");
    }
    return pc;
}
#else
addr doGamePad(addr pc) { printString("-noGamepad-"); return pc; }
void gamePadBegin() { }
#endif

addr doPin(addr pc) {
    CELL pin = pop();
    byte ir = *(pc++);
    switch (ir) {
    case 'I': pinMode(pin, INPUT);          break;
    case 'O': pinMode(pin, OUTPUT);         break;
    case 'U': pinMode(pin, INPUT_PULLUP);   break;
    case 'R': ir = *(pc++);
        if (ir == 'A') { push(analogRead(pin));  }
        if (ir == 'D') { push(digitalRead(pin)); }
        break;
    case 'W': ir = *(pc++);
        if (ir == 'A') { analogWrite(pin,  (int)pop()); }
        if (ir == 'D') { digitalWrite(pin, (int)pop()); }
        break;
    default:
        isError = 1;
        printString("-notPin-");
    }
    return pc;
}

addr doCustom(byte ir, addr pc) {
    switch (ir) {
    case 'G': pc = doGamePad(ir, pc);       break;
    case 'N': push(micros());               break;
    case 'P': pc = doPin(pc);               break;
    case 'T': push(millis());               break;
    case 'W': delay(pop());                 break;
    case 'L': push(LED_BUILTIN);            break;
    default:
        isError = 1;
        printString("-notExt-");
    }
    return pc;
}

void loadCode(const char* src) {
    addr here = HERE;
    addr here1 = HERE;
    while (*src) {
        *(here1++) = *(src++);
    }
    *here1 = 0;
    run(here);
}

void input_push(FILE *fp) { }
FILE *input_pop() { return NULL; }

// ********************************************
// * HERE is where you load your default code *
// ********************************************

#define SOURCE_STARTUP \
    X(1000, ":C \"%n\"xIAU xIH1-[rIc@#,';=(rI1+c@':=(\"%n\"))];") \

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

void ok() {
    printString("\r\ns3:"); 
    dumpStack(); 
    printString(">");
}

// NB: tweak this depending on what your terminal window sends for [Backspace]
// E.G. - PuTTY sends a 127 for Backspace
int isBackSpace(char c) { 
  // printStringF("(%d)",c);
  return (c == 127) ? 1 : 0; 
}

void handleInput(char c) {
    static addr here = (addr)NULL;
    static addr here1 = (addr)NULL;
    if (here == NULL) {
        here = (addr)HERE;
        here1 = here;
    }
    if (c == 13) {
        printString(" ");
        *(here1) = 0;
        run(here);
        here = (addr)NULL;
        ok();
        return;
    }

    if (isBackSpace(c) && (here < here1)) {
        here1--;
        if (!isOTA) {
          char b[] = {8, 32, 8, 0};
          printString(b);
        }
        return;
    }
    if (c == 9) { c = 32; }
    if (BetweenI(c, 32, 126)) {
        *(here1++) = c;
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
    vmInit();
    gamePadBegin();
}

void do_autoRun() {
    addr fa = 0;
    if (fa) { run(fa); }
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
        ok();
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
        handleInput(getChar()); 
    }
    do_autoRun();
}
