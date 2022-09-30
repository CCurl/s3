// s3.cpp - inspired by STABLE from Sandor Schneider
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define btw(a,b,c) ((b<=a) && (a<=c))
#define TOS     st.i[s]
#define NOS     st.i[s-1]
#define PUSH    st.i[++s]
#define POP     st.i[s--]
#define R0      st.i[r]
#define R1      st.i[r+1]
#define R2      st.i[r+2]
#define BSZ    (64*1024)
#define ISZ      (BSZ/4)
#define FSZ        10
#define MAX_FN  0x3FF
#define putC(ch) putc(ch, stdout)
#define getC() fgetc(stdin)

union fib { float f[ISZ]; long i[ISZ]; }; static union fib st;
static char *y, stb[BSZ]; 
static long c, cb=1, h, sb=4, rb=64, rg=68, lb=125, r, s, t, u, fpSp;
static long fn, fa, funcs[MAX_FN+1], p;
static long sd, fp;
long fpStk[FSZ];

void init() {
    s=sb-1; h=cb; u=ISZ-500;
    for (int i=0; i<ISZ; i++) { st.i[i]=0; }
    for (int i=0; i<BSZ; i++) { stb[i]=0; }
    for (int i=0; i<MAX_FN; i++) { funcs[i]=0; }
    st.i[0] = h;
}
int funcN(int x) { 
    unsigned long hh = 5381;
    while (btw(stb[x], 'A', 'Z') || btw(stb[x], 'a', 'z') || (btw(stb[x], '0', '9'))) {
            hh=((hh<<4)^hh)+(stb[x++]-96);
    }
    fn=(hh & MAX_FN); fa=funcs[fn];
    return x;
}
void fSystem() { system((char*)POP); }
void fOpen() { t=POP; y=&stb[TOS]; TOS=(long)fopen(y, (t) ? "wb" : "rb"); }
void fClose() { if (TOS) { fclose((FILE*)TOS); } s--; }
void fLoad() { 
    PUSH=0; fOpen(); t=POP;
    if (t) { if ((FILE*)fp != stdin) { fpStk[++fpSp] = fp; } fp = t; }
    else { printf("-loadFail-"); }
}
void dotQ(int delim) {
    y = (char*)&stb[p];
    if (delim==0) { y = (char*)POP; }
    while (*y && (*y != delim)) {
        c = *(y++); if (delim) { ++p; }
        if (c == '%') {
            c = *(y++); if (delim) { ++p; }
            if (c=='d') { printf("%ld", POP); }
            else if (c=='c') { putC((int)POP); }
            else if (c=='e') { putC(27); }
            else if (c=='f') { printf("%g", st.f[s--]); }
            else if (c=='n') { putC(13); putC(10); }
            else if (c=='q') { putC('"'); }
            else if (c=='X') { printf("%lX", POP); }
            else if (c=='x') { printf("%lx", POP); }
            else { putC(c); }
        } else { putC(c); }
    }
    if (delim) { ++p; }
}
void X() { if (u && (u != 10)) printf("-IR %ld (%c)?", u, (char)u); p = 0; }
void N() {}
void fStore() { st.i[TOS]=NOS; s-=2; }
void fDotQ() { dotQ('"'); }
void fDup() { t = TOS; PUSH = t; }
void fSwap() { t = TOS; TOS = NOS; NOS = t; }
void fOver() { t = NOS; PUSH = t; }
void fSlMod() { u=NOS; t=TOS; NOS=u/t; TOS=u%t; }
void fAscii() { PUSH = stb[p++]; }
void fIf() { if (POP == 0) { while (stb[p++] != ')'); } }
void fMult() { NOS *= TOS; s--; }
void fAdd() { NOS += TOS; s--; }
void fEmit() { putC(POP); }
void fSub() { NOS -= TOS; s--; }
void fDot() { printf("%ld", POP); }
void fDiv() { NOS /= TOS; s--; }
void n09() {
    PUSH = (u - '0'); 
    while (btw(stb[p], '0', '9')) { TOS = (TOS * 10) + stb[p++] - '0'; }
    if (stb[p] == 'e') { ++p; st.f[s] = (float)TOS; }
}
void fCreate() {
    p=funcN(p); if (fa) { printf("-redef:%ld at %ld-", fn, fa); }
    while (stb[p]==' ') { ++p; } funcs[fn]=p;
    while (stb[p++]!=';') {}
    if (h<p) { h=p; } st.i[0]=h;
}
void fRet() { p = st.i[r++]; if (rb < r) { r = rb; p = 0; } }
void fLT() { NOS= (NOS<TOS) ? -1:0; s--; }
void fEq() { NOS= (NOS==TOS)? -1:0; s--; }
void fGT() { NOS= (NOS>TOS) ? -1:0; s--; }
void fLookup() { p=funcN(p); PUSH=fa; PUSH=fn; }
void fFetch() { TOS = st.i[TOS]; }
void AZ() { p=funcN(p-1); if (fa) { st.i[--r]=p; p=fa; } }
void fDrop() { --s; }
void fDo() { r -= 3; st.i[r+2]=p; st.i[r]=POP; st.i[r+1]=POP; }
void fLoopS(int x) {
    if ((x==1) && (R0<R1)) { p=st.i[r+2]; return; }
    if ((x==0) && (R0>R1)) { p=R2; return; }
    r+=3;
}
void fLoop() { ++st.i[r]; fLoopS(1); }
void fLeave() { p=st.i[r++]; }
void fNeg() { TOS = -TOS; }
void fSys() { c = BSZ-100; t=c;
    while ((31 < stb[p]) && (stb[p] != '`')) { stb[t++] = stb[p++]; }
    stb[t]=0; ++p; PUSH=(long)&stb[c]; fSystem();
}
void fAbs() { TOS=(TOS<0)?-TOS:TOS; }
void fBit() {
    u = stb[p++]; if (u == '~') { TOS = ~TOS; }
    else if (u == '&') { NOS &= TOS; s--; }
    else if (u == '|') { NOS |= TOS; s--; }
    else if (u == '^') { NOS ^= TOS; s--; }
    else { putc(32, stdout); --p; }
}
void fCOp() { u=stb[p++]; if (u=='@') { TOS=stb[TOS]; } else if (u=='!') { stb[TOS]=(char)NOS; s-=2; } }
void fRegDec() { u = stb[p++]; 
        if (btw(u, 'A', 'Z')) { st.i[u+32]--; } 
        else if (btw(u, '0', '9')) { st.i[lb+u-'0']--; } 
        else { --p; --TOS; }
}
void fExec() { st.i[--r] = p; p = POP; }
void fFloat() { u = stb[p++]; // printf("-flt:%c-",u);
    if (u == '.') { printf("%g", st.f[s--]); }
    else if (u == '@') { st.f[s] = st.f[TOS]; }
    else if (u == '!') { st.f[TOS] = st.f[s - 1]; s -= 2; }
    else if (u == '+') { st.f[s - 1] += st.f[s]; s--; }
    else if (u == '<') { TOS = (st.f[s - 1] < st.f[s]) ? -1 : 0; }
    else if (u == '-') { st.f[s - 1] -= st.f[s]; s--; }
    else if (u == '>') { TOS = (st.f[s - 1] > st.f[s]) ? -1 : 0; }
    else if (u == '*') { st.f[s - 1] *= st.f[s]; s--; }
    else if (u == 'i') { TOS = (int)st.f[s]; }
    else if (u == '/') { st.f[s-1] /= st.f[s]; s--; }
    else if (u == 'f') { st.f[s] = (float)TOS; }
    // else if (u == 's') { st.f[s] = (float)sqrt(st.f[s]); }
    // else if (u == 't') { st.f[s] = (float)tanh(st.f[s]); }
    else if (u == 'O') { fOpen(); }
    else if (u == 'C') { fClose(); }
    else if (u == 'R') { 
        t = TOS; TOS=0; PUSH=0;
        if (t) { TOS=fread((void*)&NOS, 1, 1, (FILE*)t);  }
    }
    else if (u == 'W') { if (TOS) { fwrite((void*)&NOS, 1, 1, (FILE*)TOS); } s -= 2; }
}
void fHex() {
    PUSH=0; while (1) {
    if (btw(stb[p],'0','9')) { TOS=(TOS*16)+stb[p++]-'0'; }
    else if (btw(stb[p],'A','F')) { TOS=(TOS*16)+stb[p++]-'A'+10; }
    else if (btw(stb[p],'a','f')) { TOS=(TOS*16)+stb[p++]-'a'+10; }
    else { return; } }
}
void fRegInc() { u = stb[p++]; 
    if (btw(u, 'A', 'Z')) { st.i[u+32]++; } 
    else if (btw(u, '0', '9')) { st.i[lb+u-'0']++; } 
    else { --p; ++TOS; }
}
void fLoc() {
    u=stb[p++]; if (u=='+') { lb += (lb < 185) ? 10 : 0; }
    else if (u=='-') { lb -= (125 < lb) ? 10 : 0; }
}
void fKey() {
    u=stb[p++]; if (u=='?') { PUSH = 0; /*TODO!*/ }
    else if (u=='@') { PUSH = getC(); }
}
void fMOp() { u=stb[p++]; if (u=='@') { TOS=*(char*)TOS; } } // else if (u=='!') { *(char*)TOS=(char)NOS; s-=2; } }
void fIndex() { PUSH = R0; }
void fDotS() { for (int i = sb; i <= s; i++) { printf("%c%ld", (i == sb) ? 0 : 32, st.i[i]); } }
void fRegGet() { u = stb[p++]; PUSH=0;
        if (btw(u, 'A', 'Z')) { TOS=st.i[u+32]; } 
        else if (btw(u, '0', '9')) { TOS=st.i[lb+u-'0']; }
}
void fRegSet() { u = stb[p++]; t=POP;
        if (btw(u, 'A', 'Z')) { st.i[u+32]=t; } 
        else if (btw(u, '0', '9')) { st.i[lb+u-'0']=t; }
}
void fType() { y=(char*)&stb[POP]; puts(y); }
void fExt() { u = stb[p++];
    if (u == '%') { NOS %= TOS; s--; } // MOD
    else if (u == ']') { u=(R0<R1)?1:0; R0+=POP; fLoopS(u); } // +LOOP
    else if (u == 'R') { if (!sd) { sd=(long)(cb+y)+clock(); } // RAND
        sd=(sd<<13)^sd; sd=(sd>>17)^sd; sd = (sd<<5)^sd;
        PUSH=sd; } 
    else if (u == 'L') { fLoad(); } // ABS
    else if (u == 'Y') { TOS=(long)&stb[TOS]; fSystem(); } // system
    else if (u == 'T') { PUSH = clock(); } // TIMER/MILLIS
    else if (u == 'N') { PUSH = clock()*1000; } // TIMER/MICRO
    else if (u == 'U') { r+=3; } // UNLOOP
    else if (u == 'W') { printf("<wait:todo>"); } // WAIT
    else if (u == 'X') { init(); p=0; } // Reset
    else if (u == 'Q') { exit(0); } // Exit s3
}
void fZType() { TOS=(long)&stb[TOS]; dotQ(0); }
void fBegin() { r-=3; R0=p; }
void fQt() { while (stb[p]!='|') { stb[TOS++]=stb[p++]; } stb[TOS++]=0; ++p; }
void fWhile() { if (POP) { p=R0; } else { r+=3; } }
void fLNot() { TOS=(TOS)?0:-1; }

void (*q[128])() = { 
    X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,                     //   0:31
    N,fStore,fDotQ,fDup,fSwap,fOver,fSlMod,fAscii,fIf,N,fMult,fAdd,fEmit,fSub,fDot,fDiv, //  32:47
    n09,n09,n09,n09,n09,n09,n09,n09,n09,n09,fCreate,fRet,fLT,fEq,fGT,fLookup,            //  48:63
    fFetch,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,                                 //  64:79
    AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,fDo,fDrop,fLoop,fLeave,fNeg,                        //  80:95
    fSys,fAbs,fBit,fCOp,fRegDec,fExec,fFloat,X,fHex,fRegInc,X,fKey,fLoc,fMOp,fIndex,X,   //  96:111
    X,fDotS,fRegGet,fRegSet,fType,X,X,X,fExt,X,fZType,fBegin,fQt,fWhile,fLNot,X };       // 112:127

void R(int x) { s=(s<sb)?(sb-1):s; r=rb; p=x; while (p) { u=stb[p++]; q[u](); } }
void H(char* s) { FILE* fp = fopen("h.txt", "at"); if (fp) { fprintf(fp, "%s", s); fclose(fp); } }
void L() {
    if (feof((FILE*)fp)) {
        if (fp==(long)stdin) { exit(0); }
        PUSH=fp; fClose();
        fp = (0<fpSp) ? fpStk[--fpSp] : (long)stdin;
    }
    if (fp == (long)stdin) { printf("\ns3:("); fDotS(); printf(")>"); }
    fgets(&stb[h], 128, (FILE*)fp);
    if (fp == (long)stdin) { H(&stb[h]); }
    R(h);
}
int main(int argc, char* argv[]) {
    init();
    st.i[lb] = argc;
    u = 1000;
    fp = (long)stdin; fpSp = 0;
    for (int i=0; i<FSZ; i++) { fpStk[i]=0; }
    for (int i=1; i<argc; ++i) {
        y=argv[i]; t=atoi(y);
        if ((t) || (y[0] == '0' && y[1]==0)) { st.i[lb+i]=t; }
        else { st.i[lb+i]=u; for (int j=0; y[j]; j++) { stb[u++]=y[j]; } stb[u++]=0; }
    }
    if ((argc>1) && (argv[1][0]!='-')) {
        fp = (long)fopen(argv[1], "rb");
        if (!fp) { fp=(long)stdin; }
    }
    while (1) { L(); }
    return 0;
}
