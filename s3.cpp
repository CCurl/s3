// s3.cpp - inspired by STABLE from Sandor Schneider

#include "s3.h"

ST_T st;
static char *y;
BYTE stb[CODE_SZ];
long h, sb = 4, rb = 64, rg = 68, lb = 125, r, s, t, u, fpSp;
long fn, fa, p, sd, fp, funcs[MAX_FN + 1], fpStk[FILE_SZ];

void init(int files) {
    s = sb - 1; h = 1;
    for (int i = 0; i < CODE_SZ; i++) { stb[i] = 0; }
    for (int i = 0; i < VARS_SZ; i++) { st.i[i] = 0; }
    for (int i = 0; i <= MAX_FN; i++) { funcs[i] = 0; }
    if (files) { fpSp = 0; for (int i = 0; i < FILE_SZ; i++) { fpStk[i] = 0; } }
    st.i[0] = h;
}
int funcN(int x) {
    unsigned long hh = 5381;
    while (btw(stb[x], 'A', 'Z') || btw(stb[x], 'a', 'z') || (btw(stb[x], '0', '9'))) {
        hh = ((hh << 4) ^ hh) + (stb[x++] - 'A');
    }
    fn = (hh & MAX_FN); fa = funcs[fn];
    return x;
}
void X() { if (u) { printStringF("-IR %ld (%c)?", u, (char)u); } p = 0; }
void N() {}
void fSystem() { system((char*)POP); }
void fOpen() { t = POP; y = (char*)&stb[TOS]; TOS = doFopen(y, t); }
void fClose() { t = POP; if (t) { doFclose(t); } }
void fLoad() {
    PUSH(0); fOpen(); t = POP;
    if (t) { if (fp != (long)stdin) { fpStk[++fpSp] = fp; } fp = t; }
    else { printString("-loadFail-"); }
}
void dotQ(int delim) {
    y = (char*)&stb[p];
    if (delim == 0) { y = (char*)POP; }
    while (*y && (*y != delim)) {
        char c = *(y++); if (delim) { ++p; }
        if (c == '%') {
            c = *(y++); if (delim) { ++p; }
            if (c == 'd') { printStringF("%ld", POP); }
            else if (c == 'c') { putC((int)POP); }
            else if (c == 'e') { putC(27); }
            else if (c == 'f') { printStringF("%g", st.f[s--]); }
            else if (c == 'n') { putC(13); putC(10); }
            else if (c == 'q') { putC('"'); }
            else if (c == 's') { printString((char*)&stb[POP]); }
            else if (c == 'X') { printStringF("%lX", POP); }
            else if (c == 'x') { printStringF("%lx", POP); }
            else { putC(c); }
        }
        else { putC(c); }
    }
    if (delim) { ++p; }
}
void fStore() { st.i[TOS] = NOS; s -= 2; }
void fDotQ() { dotQ('"'); }
void fDup() { t = TOS; PUSH(t); }
void fSwap() { t = TOS; TOS = NOS; NOS = t; }
void fOver() { t = NOS; PUSH(t); }
void fDrop() { --s; }
void fSlMod() { u = NOS; t = TOS; NOS = u / t; TOS = u % t; }
void fAscii() { PUSH(stb[p++]); }
void fDot() { printStringF("%ld", POP); }
void fEmit() { putC(POP); }
void fIf() { if (POP == 0) { while (stb[p++] != ')'); } }
void fAdd() { NOS += TOS; s--; }
void fSub() { NOS -= TOS; s--; }
void fMult() { NOS *= TOS; s--; }
void fDiv() { NOS /= TOS; s--; }
void n09() {
    PUSH(u - '0');
    while (btw(stb[p], '0', '9')) { TOS = (TOS * 10) + stb[p++] - '0'; }
    if (stb[p] == 'e') { ++p; st.f[s] = (float)TOS; }
}
void fCreate() {
    if (stb[p] == '_') { PUSH(++p); u = 999; }
    else {
        p = funcN(p);
        if (fa) { printStringF("-redef:%ld to %ld,hash(%ld)-", fa, p, fn); }
    }
    while (stb[p] == ' ') { ++p; }
    if (u != 999) { funcs[fn] = p; }
    while (stb[p] != ';') {
        if (stb[p]) { if (stb[p] < 32) { stb[p] = 32; } ++p; }
        else {
            if (fp == (long)stdin) { printString(": "); }
            doFgets((char*)&stb[p], 128, fp);
        }
    }
    if (h < (++p)) { h = p; st.i[0] = h; }
}
void fRet() { p = st.i[r++]; if (rb < r) { r = rb; p = 0; } }
void fLT() { NOS = (NOS < TOS) ? -1 : 0; s--; }
void fEq() { NOS = (NOS == TOS) ? -1 : 0; s--; }
void fGT() { NOS = (NOS > TOS) ? -1 : 0; s--; }
void fLookup() { p = funcN(p); PUSH(fa); PUSH(fn); }
void fFetch() { TOS = st.i[TOS]; }
void doExec(long addr) {
    if (!addr) { printString("-noimpl-"); return; }
    if ((stb[p] != ';') && (stb[p] != '^')) { st.i[--r] = p; }
    p = addr;
}
void fGoto() { p = POP; }
void fExec() { doExec(POP); }
void AZ() { p = funcN(p - 1); doExec(fa); }
void fDo() { r -= 3; st.i[r + 2] = p; st.i[r] = POP; st.i[r + 1] = POP; }
void fLoopS(int x) {
    if ((x==1) && (R0 < R1)) { p = R2; return; }
    if ((x==0) && (R0 > R1)) { p = R2; return; }
    r += 3;
}
void fLoop() { if (++R0 < R1) { p = R2; } else { r += 3; } }
void fLeave() { p = st.i[r++]; }
void fNeg() { TOS = -TOS; }
void fSys() {
    int c = CODE_SZ - 100; t = c;
    while ((31 < stb[p]) && (stb[p] != '`')) { stb[t++] = stb[p++]; }
    stb[t] = 0; ++p; PUSH(&stb[c]); fSystem();
}
void fAbs() { TOS = (TOS < 0) ? -TOS : TOS; }
void fBit() {
    u = stb[p++]; if (u == '~') { TOS = ~TOS; }
    else if (u == '&') { NOS &= TOS; s--; }
    else if (u == '|') { NOS |= TOS; s--; }
    else if (u == '^') { NOS ^= TOS; s--; }
    else { putc(32, stdout); --p; }
}
void fCOp() {
    u = stb[p++];
    if (u == '@') { TOS = (BYTE)stb[TOS]; }
    if (u == '!') { stb[TOS] = (BYTE)NOS; s -= 2; }
}
void fWord() {
    u = stb[p++];
    if (u == '@') { TOS = (stb[TOS + 1] << 8) | stb[TOS]; }
    if (u == '!') { stb[TOS] = (BYTE)NOS; stb[TOS + 1] = (BYTE)(NOS >> 8); s -= 2; }
}
void fFloat() {
    u = stb[p++]; // printf("-flt:%c-",u);
    if (u == '.') { printStringF("%g", st.f[s--]); }
    else if (u == '@') { st.f[s] = st.f[TOS]; }
    else if (u == '!') { st.f[TOS] = st.f[s - 1]; s -= 2; }
    else if (u == '+') { st.f[s - 1] += st.f[s]; s--; }
    else if (u == '-') { st.f[s - 1] -= st.f[s]; s--; }
    else if (u == '*') { st.f[s - 1] *= st.f[s]; s--; }
    else if (u == '/') { st.f[s - 1] /= st.f[s]; s--; }
    else if (u == '<') { TOS = (st.f[s - 1] < st.f[s]) ? -1 : 0; }
    else if (u == '>') { TOS = (st.f[s - 1] > st.f[s]) ? -1 : 0; }
    else if (u == 'i') { TOS = (int)st.f[s]; }
    else if (u == 'f') { st.f[s] = (float)TOS; }
    else if (u == 'S') { st.f[s] = (float)sqrt(st.f[s]); }
    else if (u == 'T') { st.f[s] = (float)tanh(st.f[s]); }
    else if (u == 'O') { fOpen(); }
    else if (u == 'C') { fClose(); }
    else if (u == 'R') {
        t = TOS; TOS = 0; PUSH(0);
        if (t) { TOS = doFread((void*)&NOS, 1, 1, t); }
    }
    else if (u == 'W') { if (TOS) { doFwrite((void*)&NOS, 1, 1, TOS); } s -= 2; }
}
void fHex() {
    PUSH(0);
    while (1) {
        if (btw(stb[p], '0', '9')) { TOS = (TOS * 16) + stb[p++] - '0'; }
        else if (btw(stb[p], 'A', 'F')) { TOS = (TOS * 16) + stb[p++] - 'A' + 10; }
        else if (btw(stb[p], 'a', 'f')) { TOS = (TOS * 16) + stb[p++] - 'a' + 10; }
        else { return; }
    }
}
void fRegInc() {
    u = stb[p++];
    if (btw(u, 'A', 'Z')) { st.i[u]++; }
    else if (btw(u, '0', '9')) { st.i[lb + u - '0']++; }
    else { --p; ++TOS; }
}
void fRegDec() {
    u = stb[p++];
    if (btw(u, 'A', 'Z')) { st.i[u]--; }
    else if (btw(u, '0', '9')) { st.i[lb + u - '0']--; }
    else { --p; --TOS; }
}
void fLoc() {
    u = stb[p++]; if (u == '+') { lb += (lb < 185) ? 10 : 0; }
    else if (u == '-') { lb -= (125 < lb) ? 10 : 0; }
}
void fKey() {
    u = stb[p++]; if (u == '?') { PUSH(0); /*TODO!*/ }
    else if (u == '@') { PUSH(getC()); }
}
void fRegGet() {
    u = stb[p++]; PUSH(0);
    if (btw(u, 'A', 'Z')) { TOS = st.i[u]; }
    else if (btw(u, '0', '9')) { TOS = st.i[lb + u - '0']; }
}
void fRegSet() {
    u = stb[p++]; t = POP;
    if (btw(u, 'A', 'Z')) { st.i[u] = t; }
    else if (btw(u, '0', '9')) { st.i[lb + u - '0'] = t; }
}
void fMOp() { u = stb[p++]; if (u == '@') { TOS = *(char*)TOS; } } // else if (u=='!') { *(char*)TOS=(char)NOS; s-=2; } }
void fIndex() { PUSH(R0); }
void fDotS() { for (int i=sb; i<=s; i++) { if (sb<i) { putC(32); } printStringF("%ld", st.i[i]); } }
void fType() { y = (char*)&stb[POP]; fputs(y, stdout); }
void fRand() {
    if (!sd) { sd = (long)(y)+timerMS(); }
    sd = (sd << 13) ^ sd; sd = (sd >> 17) ^ sd; sd = (sd << 5) ^ sd;
    PUSH(sd);
}
void fExt() {
    u = stb[p++];
    if (u == '%') { NOS %= TOS; s--; } // MOD
    else if (u == ']') { u = (R0 < R1) ? 1 : 0; R0 += POP; fLoopS(u); } // +LOOP
    else if (u == 'R') { fRand(); }
    else if (u == 'L') { fLoad(); } // ABS
    else if (u == 'Y') { TOS = (long)&stb[TOS]; fSystem(); } // system
    else if (u == 'T') { PUSH(timerMS()); } // TIMER/MILLIS
    else if (u == 'N') { PUSH(timerNS()); } // TIMER/MICROS
    else if (u == 'U') { r += 3; } // UNLOOP
    else if (u == 'W') { printStringF("-wait:%ld-", POP); } // WAIT
    else if (u == 'X') { init(0); p = 0; } // Reset
    else if (u == 'Q') { exit(0); } // Exit s3
}
void fUser() { p = doUser(u, p); }
void fZType() { TOS = (long)&stb[TOS]; dotQ(0); }
void fBegin() { r -= 3; R0 = p; }
void fQt() { while (stb[p] != '|') { stb[TOS++] = stb[p++]; } stb[TOS++] = 0; ++p; }
void fWhile() { if (POP) { p = R0; } else { r += 3; } }
void fLNot() { TOS = (TOS) ? 0 : -1; }

void (*q[128])() = {
    X,X,X,X,X,X,X,X,X,X,N,X,X,N,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,                        //   0:31
    N,fStore,fDotQ,fDup,fSwap,fOver,fSlMod,fAscii,fIf,N,fMult,fAdd,fEmit,fSub,fDot,fDiv,    //  32:47
    n09,n09,n09,n09,n09,n09,n09,n09,n09,n09,fCreate,fRet,fLT,fEq,fGT,fLookup,               //  48:63
    fFetch,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,                                    //  64:79
    AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,fDo,fDrop,fLoop,fLeave,fNeg,                           //  80:95
    fSys,fAbs,fBit,fCOp,fRegDec,fExec,fFloat,fGoto,fHex,fRegInc,X,fKey,fLoc,fMOp,fIndex,X,  //  96:111
    X,fDotS,fRegGet,fRegSet,fType,fUser,X,fWord,fExt,X,fZType,fBegin,fQt,fWhile,fLNot,X };  // 112:127

void Run(int x) { s = (s < sb) ? (sb - 1) : s; r = rb; p = x; while (p) { u = stb[p++]; q[u](); } }
#ifdef __PC__
void Hist(char* s) { FILE* fp = fopen("h.txt", "at"); if (fp) { fprintf(fp, "%s", s); fclose(fp); } }
void Loop() {
    if (feof((FILE*)fp)) {
        if (fp == (long)stdin) { exit(0); }
        doFclose(fp);
        fp = (0 < fpSp) ? fpStk[--fpSp] : (long)stdin;
    }
    if (fp == (long)stdin) { printString("\ns3:("); fDotS(); printString(")>"); }
    stb[h] = 0; fgets((char*)&stb[h], 128, (FILE*)fp);
    if (fp == (long)stdin) { Hist((char*)&stb[h]); }
    Run(h);
}
int main(int argc, char* argv[]) {
    init(1);
    st.i[lb] = argc;
    u = 1000;
    for (int i = 1; i < argc; ++i) {
        y = argv[i]; t = atoi(y);
        if ((t) || (y[0] == '0' && y[1] == 0)) { st.i[lb + i] = t; }
        else { st.i[lb + i] = u; for (int j = 0; y[j]; j++) { stb[u++] = y[j]; } stb[u++] = 0; }
    }
    if ((argc > 1) && (argv[1][0] != '-')) { fp = doFopen(argv[1], 0); }
    if (!fp) { fp = doFopen("src.s3", 0); }
    if (!fp) { fp = (long)stdin; }
    while (1) { Loop(); }
    return 0;
}
#endif // __PC__
