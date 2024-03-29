// s3.cpp - inspired by STABLE from Sandor Schneider

#include "s3.h"
// #define __DEBUG__

ST_T st;
static char *y;
uint8_t stb[CODE_SZ];
cell_t locs[LOCS_SZ+10], lb, lstk[LOOP_SZ+1], lsp;
cell_t h, sb = 4, rb = 64, r, s, t, u, fpSp;
cell_t fn, fa, p, sd, fp, funcs[MAX_FN+1], fpStk[FILE_SZ];

void Run(cell_t x, int clr);

void init(int files) {
    s = sb - 1; h = 1; lb = lsp = 0;
    for (int i = 0; i < CODE_SZ; i++) { stb[i] = 0; }
    for (int i = 0; i < VARS_SZ; i++) { st.i[i] = 0; }
    for (int i = 0; i <= MAX_FN; i++) { funcs[i] = 0; }
    if (files) { fpSp = 0; for (int i = 0; i < FILE_SZ; i++) { fpStk[i] = 0; } }
    STI(0) = h;
    STI(1) = sizeof(cell_t);
}
int funcN(int x) {
    cell_t hh = stb[x++];
    while (btw(stb[x], 'A', 'Z')) { hh = (hh*33) + stb[x++]; }
    fn = (hh & MAX_FN); fa = funcs[fn];
    return x;
}
void X() { u=STB(p-1); if (u) { printStringF("-IR %ld (%c)?", u, (char)u); } p = 0; }
void N() {}
void fSystem() { system((char*)POP); }
void fOpen() { t = POP; y = (char*)&stb[TOS]; TOS = doFopen(y, t); }
void fClose() { t = POP; if (t) { doFclose(t); } }
void fLoad() {
    PUSH(0); fOpen(); t = POP;
    if (t) { if (fp != (cell_t)stdin) { fpStk[++fpSp] = fp; } fp = t; }
    else { printString("-loadFail-"); }
}
void printBase(cell_t v, int b) {
    char x[65], *c=&x[64], n=((v<0) && (b==10))?1:0;
    ucell_t u=(n)?-v:v;
    *(c) = 0;
    do { *(--c)=(u%b)+'0'; if (*c>'9') *c+=7; u/=b; } while (u);
    if (n) { *(--c)='-'; }
    printString(c);
}
void dotQ(int delim) {
    y = (char*)&stb[p];
    if (delim == 0) { y = (char*)POP; }
    while (*y && (*y != delim)) {
        char c = *(y++); if (delim) { ++p; }
        if (c == '%') {
            c = *(y++); if (delim) { ++p; }
            if (c == 'd') { printBase(POP, 10); }
            else if (c == 'c') { putC((int)POP); }
            else if (c == 'e') { putC(27); }
            else if (c == 'f') { printStringF("%g", st.f[s--]); }
            else if (c == 'n') { putC(13); putC(10); }
            else if (c == 'q') { putC('"'); }
            else if (c == 's') { printString((char*)&stb[POP]); }
            else if (c == 'B') { t = POP; printBase(POP, t); }
            else if (c == 'b') { printBase(POP, 2); }
            else if (c == 'x') { printBase(POP, 16); }
            else { putC(c); }
        }
        else { putC(c); }
    }
    if (delim) { ++p; }
}
#ifdef NEEDS_ALIGN
void setCell(cell_t a, cell_t v) {
    for (int i = 0; i < STI(1); i++) { STB(a++) = v&0xff; v= v>>8; }
}
cell_t getCell(cell_t a) {
    cell_t ret = 0;
    for (int i = 0; i < STI(1); i++) { ret = (ret << 8) | STB(a++); }
    return ret;
}
#else
inline void setCell(cell_t a, cell_t v) { *(cell_t*)&STB(a) = v; }
inline cell_t getCell(cell_t a) { return *(cell_t*)&STB(a); }
#endif
void fStore() { st.i[TOS] = NOS; s -= 2; }
void fFetch() { TOS = STI(TOS); }
void fDotQ() { dotQ('"'); }
void fDup() { t = TOS; PUSH(t); }
void fSwap() { t = TOS; TOS = NOS; NOS = t; }
void fOver() { t = NOS; PUSH(t); }
void fDrop() { --s; }
void fSlMod() { u = NOS; t = TOS; NOS = u / t; TOS = u % t; }
void fAscii() { PUSH(stb[p++]); }
void fDot() { printBase(POP, 10); }
void fEmit() { putC(POP); }
void fIf() { if (POP == 0) { while (stb[p++] != ')'); } }
void fAdd() { NOS += TOS; s--; }
void fSub() { NOS -= TOS; s--; }
void fMult() { NOS *= TOS; s--; }
void fDiv() { NOS /= TOS; s--; }
void n09() {
    PUSH(STB(p-1) - '0');
    while (btw(stb[p],'0','9')) { TOS=(TOS*10)+stb[p++]-'0'; }
    if (stb[p] == 'e') { ++p; FTOS = (float)TOS; }
    else if (stb[p] == '.') { 
        ++p; FTOS = (float)TOS;
        float d = 10;
        while (btw(stb[p],'0','9')) { FTOS+=(stb[p++]-'0')/d; d*=10; }
    }
}
void fVar() { p = funcN(p); PUSH(fa); }
void fCreate() {
    cell_t cur=p, isVar=(stb[p]=='v'), isUs=(stb[p]=='_');
    if (isVar) { ++p; }
    if (isUs) { PUSH(++p); } else { p = funcN(p); }
    if ((!isUs) && fa) { printStringF("-redef:%ld to %ld,hash(%ld)-", fa, cur, fn); }
    while (stb[p] == ' ') { ++p; }
    if (isVar) { funcs[fn] = POP; }
    else { if (!isUs) { funcs[fn] = p; } }
    while (stb[p] != ';') {
        if (stb[p]) { if (stb[p]<32) { stb[p]=32; } ++p; }
        else {
            if (fp == (cell_t)stdin) { printString(": "); }
            doFgets((char*)&stb[p], 256, fp);
        }
    }
    if (h < (++p)) { h=p; st.i[0]=h; }
}
void fRet() { p = st.i[r++]; if (rb < r) { r = rb; p = 0; } }
void fLT() { NOS = (NOS < TOS) ? -1 : 0; s--; }
void fEq() { NOS = (NOS == TOS) ? -1 : 0; s--; }
void fGT() { NOS = (NOS > TOS) ? -1 : 0; s--; }
void fLookup() { p = funcN(p); PUSH(fa); PUSH(fn); }
void doExec(cell_t addr) {
    if (!addr) { printString("-noimpl-"); return; }
    if ((stb[p] != ';') && (stb[p] != '^')) { st.i[--r] = p; }
    p = addr;
}
void fGoto() { p = POP; }
void fExec() { doExec(POP); }
void AZ() { p = funcN(p - 1); doExec(fa); }
void fDo() { lsp += 3; L0 = POP; L1 = POP; L2 = p; }
void fLoopS(int x) {
    if ((x==0) && (L0>L1)) { p = L2; return; }
    if ((x==1) && (L0<L1)) { p = L2; return; }
    lsp -= 3;
}
void fLoop() { if (++L0 < L1) { p = L2; } else { lsp -= 3; } }
void fBegin() { lsp += 3; L0 = p; }
void fWhile() { if (POP) { p = L0; } else { lsp -= 3; } }
void fIndex() { PUSH(L0); }
void fIndex2() { PUSH(lstk[lsp-3]); }
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
    else if (u == '%') { PUSH(0); while (btw(stb[p], '0', '1')) { TOS = (TOS * 2) + stb[p++] - '0'; } }
    else { putC(32); --p; }
}
void fCOp() {
    u = stb[p++]; if (u == '@') { TOS = STB(TOS); }
    else if (u == '!') { STB(TOS) = (uint8_t)NOS; s -= 2; }
}
void fWord() {
    u = stb[p++];
    if (u == '@') { TOS = (stb[TOS + 1] << 8) | stb[TOS]; }
    else if (u == '!') { stb[TOS] = (uint8_t)NOS; stb[TOS + 1] = (uint8_t)(NOS >> 8); s -= 2; }
}
void fEOL() { putC(13); putC(10); }
void fFloat() {
    u = stb[p++];
    if (u == '.') { printStringF("%g", st.f[s--]); }
    else if (u == '@') { FTOS = st.f[TOS]; }
    else if (u == '!') { st.f[TOS] = FNOS; s -= 2; }
    else if (u == '+') { FNOS += FTOS; s--; }
    else if (u == '-') { FNOS -= FTOS; s--; }
    else if (u == '*') { FNOS *= FTOS; s--; }
    else if (u == '/') { FNOS /= FTOS; s--; }
    else if (u == '_') { FTOS = -FTOS; }
    else if (u == '<') { TOS = (FNOS < FTOS) ? -1 : 0; }
    else if (u == '>') { TOS = (FNOS > FTOS) ? -1 : 0; }
    else if (u == 'i') { TOS = (int)FTOS; }
    else if (u == 'f') { FTOS = (float)TOS; }
    else if (u == 'S') { FTOS = (float)sqrt(FTOS); }
    else if (u == 'T') { FTOS = (float)tanh(FTOS); }
    else if (u == 'O') { fOpen(); }
    else if (u == 'C') { fClose(); }
    else if (u == 'D') { doFdelete((char*)POP); }
    else if (u == 'L') { doFlist(); }
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
        else { return; }
    }
}
void fLoc() {
    u = stb[p++]; if (u == '+') { lb += (lb < LOCS_SZ) ? 10 : 0; }
    else if (u == '-') { lb -= (0<lb) ? 10 : 0; }
    else if (u == '@') { TOS = getCell(TOS); }
    else if (u == '!') { setCell(TOS, NOS); s -= 2; }
}
void fRegDec() {
    u = stb[p++];
    if (btw(u, 'A', 'Z')) { st.i[u]--; }
    else if (btw(u, '0', '9')) { locs[lb + u - '0']--; }
    else { --p; --TOS; }
}
void fRegInc() {
    u = stb[p++];
    if (btw(u, 'A', 'Z')) { st.i[u]++; }
    else if (btw(u, '0', '9')) { locs[lb + u - '0']++; }
    else { --p; ++TOS; }
}
void fRegGet() {
    u = stb[p++]; PUSH(0);
    if (btw(u, 'A', 'Z')) { TOS = st.i[u]; }
    else if (btw(u, '0', '9')) { TOS = locs[lb + u - '0']; }
    else if (u == '@') { TOS=R0; }
    else if (u == '<') { s--; st.i[--r]=POP; }
    else if (u == '>') { TOS=st.i[r++]; }
}
void fRegSet() {
    u = stb[p++]; 
    if (btw(u, 'A', 'Z')) { st.i[u] = POP; }
    else if (btw(u, '0', '9')) { locs[lb + u - '0'] = POP; }
    else if (u == 'a') { strcat((char*)&stb[NOS], (char*)&stb[TOS]); s--; }
    else if (u == 'c') { t=NOS+strlen((char*)&stb[NOS]); stb[t++]=TOS; stb[t]=0; s--; }
    else if (u == 'd') { t=NOS+strlen((char*)&stb[NOS]); TOS+=(TOS>9)?7:0; stb[t++]=TOS+'0'; stb[t] = 0; s--; }
    else if (u == 'e') { t=strlen((char*)&stb[TOS]); TOS+=t; }
    else if (u == 'f') { t=POP; u=TOS; TOS=0; while (stb[u]&&(TOS==0)) { if (stb[u]==t) { TOS=u; } else { u++; } } }
    else if (u == 'l') { t=strlen((char*)&stb[TOS]); PUSH(t); }
    else if (u == 'n') { char z[20]; sprintf(z,"%ld",POP); y=(char*)&stb[TOS]; strcat(y, z); }
    else if (u == 's') { strcpy((char*)&stb[NOS], (char*)&stb[TOS]); s--; }
    else if (u == 't') { stb[TOS]=0; }
}
void fKey() {
    u = stb[p++]; if (u == '?') { PUSH(charAvailable()); /*TODO!*/ }
    else if (u == '@') { PUSH(getC()); }
}
void fMOp() { u = stb[p++]; if (u == '@') { TOS = *(char*)TOS; } } // else if (u=='!') { *(char*)TOS=(char)NOS; s-=2; } }
void fDotS() { putC('('); for (int i=sb; i<=s; i++) { if (sb<i) { putC(32); } printBase(st.i[i],10); } putC(')'); }
void fType() { y = (char*)&stb[POP]; fputs(y, stdout); }
void fRand() {
    if (!sd) { sd = (cell_t)(y)+timerMS(); }
    sd = (sd << 13) ^ sd; sd = (sd >> 17) ^ sd; sd = (sd << 5) ^ sd;
    PUSH(sd);
}
void fCheckStk() {
        if (s<sb-1) { printStringF("-underflow at %ld-",p-1); p=0; }
        if (r<=s) { printStringF("-overflow at %ld-",p-1); p=0; }
}
void fExt() {
    u = stb[p++];
    if (u == '%') { NOS %= TOS; s--; } // MOD
    else if (u == 'R') { fRand(); }
    else if (u == ']') { u = (L0 < L1) ? 1 : 0; L0 += POP; fLoopS(u); } // +LOOP
    else if (u == '|') { PUSH(p); PUSH(0); while (stb[p++]!=u) { ++TOS; } }
    else if (u == 'L') { fLoad(); } // ABS
    else if (u == 'Y') { TOS = (cell_t)&stb[TOS]; fSystem(); } // system
    else if (u == 'T') { PUSH(timerMS()); } // TIMER/MILLIS
    else if (u == 'N') { PUSH(timerNS()); } // TIMER/MICROS
    else if (u == 'U') { lsp += 3; } // UNLOOP
    else if (u == 'W') { delay(POP); } // WAIT
    else if (u == 'S') { fDotS(); }
    else if (u == '?') { fLookup(); }
    else if (u == 'X') { init(0); p=0; } // Reset
    else if (u == 'V') { PUSH(10000); } // 1.0.0
    else if (u == 'E') { cell_t x=p; Run(POP, 0); p=x; }
    else if (u == 'Q') { exit(0); } // Exit s3
}
void fUser() { p = doUser(u, p); }
void fZType() { TOS = (cell_t)&stb[TOS]; dotQ(0); }
void fQt() { while (stb[p] != '|') { stb[TOS++] = stb[p++]; } stb[TOS++] = 0; ++p; }
void fLNot() { TOS = (TOS) ? 0 : -1; }

void (*jmpTbl[128])() = {
    X,X,X,X,X,X,X,X,X,X,N,X,X,N,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,                        //   0:31
    N,fStore,fDotQ,fDup,fSwap,fOver,fSlMod,fAscii,fIf,N,fMult,fAdd,fEmit,fSub,fDot,fDiv,    //  32:47
    n09,n09,n09,n09,n09,n09,n09,n09,n09,n09,fCreate,fRet,fLT,fEq,fGT,X,                     //  48:63
    fFetch,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,                                    //  64:79
    AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,AZ,fDo,fDrop,fLoop,fLeave,fNeg,                           //  80:95
    fSys,fAbs,fBit,fCOp,fRegDec,fEOL,fFloat,X,fHex,fRegInc,fIndex2,fKey,fLoc,fMOp,fIndex,X, //  96:111
    X,X,fRegGet,fRegSet,fType,fUser,fVar,fWord,fExt,X,fZType,fBegin,fQt,fWhile,fLNot,X };   // 112:127

void Run(cell_t x, int clr) {
    if (clr) {
        s=(s<sb)?(sb-1):s; 
        r=rb; lsp=0;
    }
    p = x;
    while (p) {
        jmpTbl[stb[p++]]();
#ifdef __DEBUG__
        fCheckStk();
#endif
    } 
}
#ifdef __PC__
void Hist(char *s) { FILE *fp = fopen("h.txt", "at"); if (fp) { fprintf(fp, "%s", s); fclose(fp); } }
void Loop() {
    if (feof((FILE*)fp)) {
        if (fp == (cell_t)stdin) { exit(0); }
        doFclose(fp);
        fp = (0 < fpSp) ? fpStk[--fpSp] : (cell_t)stdin;
    }
    if (fp == (cell_t)stdin) { printString("\ns3:"); fDotS(); putC('>'); }
    y = (char*)&stb[h]; *y = 0; doFgets(y, 128, fp);
    if (fp == (cell_t)stdin) { Hist(y); }
    Run(h, 1);
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
    if (!fp) { fp = (cell_t)stdin; }
    while (1) { Loop(); }
    return 0;
}
#endif // __PC__
