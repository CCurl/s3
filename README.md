# s3 - A minimal interpreter for PCs and Development boards

## What is s3?
The following statements can be made about s3. 
```
s3 ...

... is a full-featured, interactive, stack-based, and minimal interpreter/VM.
... has 100% human-readable (and understandable!) "machine code".
... has no compilation phase; it executes the source code directly.
... provides 26 registers, [rA..rZ].
... supports locals, 10 at a time, [r0..r9].
... supports floating point math.
... supports file operations, open, read, write, close, load.
... supports up to CODE_SZ (default is 1 MB) bytes.
... supports up to VARS_SZ (default is 1 MB) cells (32- or 64-bit).
... supports variable-length function names.
... supports up to MAX_FN (default is 4096) functions.
... supports multi-line functions.
... supports anonymous functions.
... runs under Windows
... runs under Linux
... runs on development boards via the Arduino IDE
```
## Why s3?
Many interpreted environments have a large SWITCH statement with cases in a loop to execute the user's program. In these systems, the "virtual machine language" opcodes (the cases in the SWITCH statement) are often arbitrarily assigned and are not human-readable, so they have no meaning to the programmer when looking at the code that is actually being executed. Additionally, a compiler and/or interpreter are needed to create and execute programs in that environment. In those enviromnents, there is a steep learning curve ... the programmer needs to learn the programming language, the hundreds (or thousands) of user functions in the libraries (or "WORDS" in Forth), how to use the compiler, and how to use the interpreter. I wanted to avoid as much as that as possible, and have only one thing to learn: the "virtual machine language".

## Goals for s3
1. Freedom from a multiple gigabyte tool chain and the edit/compile/run paradigm for developing everyday programs.

2. A simple, minimal, and interactive programming environment that can be extended and modified easily.

3. An environment that can be easily configured for and deployed to many different types of development boards via the Arduino IDE.

4. To be able to use the same environment on my personal computer(s) as well as development boards.

5. Short and intuitive commands, so there is not a lot of typing needed.

## Building s3
```
- Windows: I use Visual Studio, the s3.sln. Use the x86 configuration, 64-bit doesn't work.

- Linux: There is a simple ./make shell script. GCC or CLANG, your choice; 32 or 64 bit both work.

- Development Boards: Use the Arduino IDE, edit the *_SZ, defines as appropriate for the board.
```

## Some s3 examples
```
; To enter a comment: 
    0(here is a comment)
  
; Hello World example
    "Hello World!"
    
; Print numbers from 1 to 100
    :NUMS 101 1[n . b]; NUMS
    0(or) 101 1[n.b]
    0(or) 1{#.b1+#101<}\
 
; Print numbers from 100 to 1
    100 {# . b 1 - #} \
    0(or) 0 100[n.b1_x]
 
; Print the ASCII table
    127 32[n#"%d: [%c]%n"]
 
; If/Else
    s3 code:          rC #("Yes")~("No")
    Forth equivalent: C @ IF ."Yes" ELSE ."No" THEN
    C equivalent:     if (C) { printf("Yes") } else { printf("No") }

; A simple benchmark for a 100 million FOR/NEXT (aka - DO/LOOP) loop:
    :MIL 1000 # * *;
    :BENCH 0(n--) xT $ 0[] xT $ - . "ms";
    100 MIL BENCH

; A simple benchmark for a 100 million WHILE loop:
    :MIL 1000#**;
    :BENCH 0(n--) xT${d#}\xT$-"%dms";
    100 MIL BENCH 0(note that spaces are optional)

; Define a word to display the currently defined code:
    :CODE 0l@1[nc@#58=(ndc@59=("%n"),];

; x = (a == b) ? c : d;
    s3 code:          rA rB=#(rC$)~(rD)sX;
    Forth equivalent: a @ b @ = IF c @ ELSE d @ THEN x !
    C equivalent:     x = (a == b) ? c : d;

; To make sure A < B
    S3 code:             %%>($)
    Forth equivalent:    OVER OVER > IF SWAP THEN
    C equivalent:        if (f > t) { int x = f; f = t; t = x; }

; To do something N times (in this case, execute SUB):
    S3 code:             rN 0[SUB]
    Forth equivalent:    N @ 0 DO Sub LOOP
    C equivalent:        for (int i=0; i<N; i++) { Sub() }

; Increment X, decrement Y, then print them
    S3 code:             iX dY rY rX "X: %d, Y: %d"
    Forth equivalent:    1 X +! -1 Y +! ."X: " X @ . .", Y: " Y @ . 
    C equivalent:        X++; Y--; printf("X: %d, Y:%d", X, Y);
    
; To print numbers from F to T:
    S3 code:             rT rF[n.b]
    Forth equivalent:    T @ F @ DO I . LOOP
    C equivalent:        for (int i = F; i < T; i++)) { printf("%d ", i); }

; One way to copy N bytes from A to B (n f t--)
    :COPY 0(f t n--) l+ s3 s2 s1 r3 0[r1 c@ r2 c! i1 i2] l-;
    rA rB rN Copy

; Creating a jump-table using anonymous words
    10000:vJUMPTABLE;
    :JTSET  0(a n--) 1l@*vJUMPTABLE+!;
    :JTGET  0(n--a)  1l@*vJUMPTABLE+@;
    :JTEXEC 0(n--)   JTGETq<;
    :_"-this is A-"; 'A JTSET
    :_"-this is B-"; 'B JTSET
    'A JTEXEC
    'B JTEXEC

; Exit S3:
    xQ
```

## s3 Reference
```
*** ARITHMETIC ***
+   (a b--n)      n: a+b - addition
-   (a b--n)      n: a-b - subtraction
*   (a b--n)      n: a*b - multiplication
/   (a b--q)      q: a/b - division
x%  (a b--r)      r: MODULO(a, b)
&   (a b--q r)    q: DIV(a,b), r: MODULO(a,b) - /MOD


*** BIT MANIPULATION ***
b&  (a b--n)      n: a AND b
b|  (a b--n)      n: a OR  b
b^  (a b--n)      n: a XOR b
b~  (a--b)        b: NOT a (ones-complement, e.g - 101011 => 010100)


*** STACK ***
        NOTES: - The stacks are located in CELL addresses 4-64.
               - The data/parameter stack starts at 4 and grows upward.
               - The return stack starts at 64 and grows downward.
#  (a--a a)       Duplicate TOS                    (Forth: DUP)
\  (a b--a)       Drop TOS                         (Forth: DROP)
$  (a b--b a)     Swap top 2 stack items           (Forth: SWAP)
%  (a b--a b a)   Push 2nd                         (Forth: OVER)
_  (a--b)         b: -a                            (Negate)
a  (a--b)         b: ABS(a)                        (Absolute)
i  (x--y)         y: x+1                           (Increment)
d  (x--y)         y: x-1                           (Decrement)
xS (--)           Print the contents of the stack  (Forth: .S)
q< (n--)          Move TOS to return stack         (Forth: >R)
q@ (--n)          Copy return stack TOS            (Forth: R@)
q> (--n)          Move return stack TOS back       (Forth: R>)


*** FLOATING POINT ***
123e  (--F)       F: 123 as a floating point number
12.34 (--F)       F: 12.34
ff    (n--F)      n: integer, F: float
fi    (F--n)      F: float, n: integer
f_    (F--N)      N: -F (Negate)
f.    (F--)       Output F
f@    (a--F)      Fetch float F from CELL address a
f!    (F a--)     Store float F to CELL address a
f+    (a b--n)    n: a+b - addition
f-    (a b--n)    n: a-b - subtraction
f*    (a b--n)    n: a*b - multiplication
f/    (a b--n)    n: a/b - division
f<    (a b--f)    f: (a < b) ? -1 : 0;
f>    (a b--f)    f: (a > b) ? -1 : 0;
fS    (a--n)      n: SQR(a)
fT    (a--n)      n: TANH(a)


*** MEMORY ***
        NOTES: - There are 3 memory areas in s3: CELL, BYTE, and ABSOLUTE.
               - An address in CELL memory is an index into an array of CELLs (32- or 64-bit).
               - An address in BYTE memory is an index into an array of BYTES.
               - An address in ABSOLUTE memory is an address in the system's memory.
               - The stacks use CELL addresses 4->64.
               - The registers CELL addresses 65->90.
               - CELL addresses 91->VARS_SZ are free.
               - BYTE memory is used for CODE as well. Code starts at address 1.
               - To get the last allocated CODE address (aka - HERE), use 0 l@.
               - To size of a CELL in bytes, use 1 l@.
@     (a--n)      Fetch CELL n from CELL address a.
!     (n a--)     Store CELL n to CELL address a.
c@    (a--b)      Fetch BYTE b from BYTE address a (char fetch).
c!    (b a--)     Store BYTE b to BYTE address a (char store).
l@    (a--n)      Fetch CELL n from BYTE address a (long fetch).
l!    (n a--)     Store CELL n to BYTE address a (long store).
w@    (a--w)      Fetch WORD w from BYTE address a.
w!    (w a--)     Store WORD w to BYTE address a.
m@    (a--b)      Fetch BYTE b from ABSOLUTE address a.
m!    (b a--)     Store BYTE b to ABSOLUTE address a.


*** REGISTERS and LOCALS ***
        NOTES: - Register names are 1 UPPERCASE character: [A..Z].
               - Registers are stored in CELL addresses 65-90 (rA='Al@, rZ='Zl@).
               - s3 allocates 10 locals at a time, [r0..r9].
rX    (--n)       n: value of register/local #3.
sX    (n--)       n: value to store in register/local #3.
iX    (--)        Increment register/local #3.
dX    (--)        Decrement register/local #3.
l+    (--)        Allocate 10 locals, [r0..r9].
l-    (--)        De-allocate the current set of locals.


*** WORDS/FUNCTIONS ***
        NOTES: - Function/word names are variable-length UPPERCASE words.
               - Use ":_ 0(code);" to create a function with no name.
:FUNC (--)        Define word FUNC. Skip until next ';'.
:_    (--A)       Define an anonymous word. A: current HERE. Skip until next ';'.
FUNC  (--)        Execute/call word FUNC.
;     (--)        End of word definition. Exits word at run-time.
^     (--)        Exit word immediately.
        NOTE: To exit a word while inside a loop, use 'xU^'.
              example: :LOOPTEST 100 0[n.b n71=("-out" xU^)", "];
x?FN (--a h)      a: BYTE address of FN, h: hash for "FN"


*** VARIABLES/CONSTANTS ***
        NOTES: - Variable/constant names are variable-length UPPERCASE words starting with 'v'.
               - s3 uses the same hash table is used for constant and function names.
               - A variable reference is really just a CONSTANT interpreted as an address.
               - vFUNCNAME will push the execution address of FUNCNAME on the stack.
N:vXYZ; (N--)     N: Define variable address/constant vXYZ to be N.
vXYZ    (--N)     N: value/address of vXYZ.
vFUNC   (--N)     N: execution address of FUNC.
XYZ     (--)      Execute the code at the address referred to by vXYZ.


*** INPUT/OUTPUT ***
0-9    (--N)      Scan decimal number N. For multiple numbers, separate them by space (47 33).
        NOTES: - To enter a negative number, use "negate" (eg - 490_).
               - To enter a negative floating point number, use "Fnegate" (eg - 490.34f_).
NNNe   (--F)      Scan floating point number (e.g. - 355e)
NN.dd  (--F)      Scan floating point number (e.g. - 3.14159)
'x     (--N)      N: the ASCII value of x.
hHHH   (--N)      Scan HEX number N. H:[0..9 or A..F].
b%BBB  (--N)      Scan BINARY number N. B:[0..1].
.      (N--)      Output N as decimal number.
f.     (F--)      Output F as floating point number.
,      (N--)      Output N an ASCII character.
b      (--)       Output a single SPACE (NOTE: bit ops take precedence).
"      (?--?)     Output a formatted string until the next '"'.
    NOTES: - %d outputs TOS as an integer
           - %b outputs TOS as a binary number
           - %B outputs NOS as a number in base TOS (e.g. - 1234 8"%B")
           - %c outputs TOS as a character
           - %e outputs an ESCAPE (27)
           - %f outputs TOS as a float
           - %n outputs CR/LF
           - %q outputs a QUOTE
           - %x outputs TOS as a hex number (A-F are uppercase)
           - %<x> outputs <x> (e.g. - "%%" outputs %)
`XXX`  (--)       Calls system("XXX"). (e.g. - `ls -l`)
xY     (A--)      Sends string at BYTE address A to system() (example: 1000#|ls|\xY).
|XXX|  (a--b)     Copies XXX to BYTE address a, b is the next address after the NULL terminator.
x|XXX| (--a n)    a: BYTE address of XXX, n:number of chars.
z      (a--)      ZTYPE: Output the formatted string at BYTE address a (see ").
t      (a--)      TYPE: Output the NULL-terminated string at BYTE address a (faster, no formatting).
k?     (--f)      f: 1 if a character is waiting in the input buffer, else 0. (TODO: Linux)
k@     (--c)      c: next character from the input buffer. If no character, wait. (TODO: Linux)


*** CONDITIONS/LOOPS/FLOW CONTROL ***
<     (a b--f)    f: (a < b) ? 1 : 0;
=     (a b--f)    f: (a = b) ? 1 : 0;
>     (a b--f)    f: (a > b) ? 1 : 0;
~     (n -- f)    f: (a = 0) ? 1 : 0; (Logical NOT)
[     (T F--)     DO: start a DO/LOOP (aka-FOR/NEXT) loop.
n     (--n)       n: the index of the current DO loop
]     (--)        LOOP: increment index (n) and stop if (T<=n)
x]    (N--)       +LOOP: Add N to the index (n) and stop if (n==T) or (n crosses T)
{     (--)        Start a BEGIN/WHILE loop
}     (f--)       WHILE: if (f != 0) jump back to BEGIN, else continue
xU    (--)        UNLOOP: Unwind the LOOP stack (either DO or WHILE loops, use with '^')
(     (f--)       IF: if (f == 0), skip to next ')'.


*** FILES ***
fO    (a m--h)    Open: a=filename, m=mode (0=read, else write), h=handle (eg- 1000#|filename|\0fO)
fC    (h--)       Close: h=handle
fR    (h--c n)    Read: h=handle, c=char, n=0 if error/eof, else 1
fW    (c h--)     Write: h=handle, c=char
fD    (a--)       a: Address of filename to delete
fL    (--)        Output list of file names


*** OTHER ***
1 l@  (--N)       N: size of a CELL in bytes.
xL    (NM--)      Load from file NM (eg - 0 l@ 10+#|tests|\xL)
xPI   (p--)       Arduino: Open Pin Input (pinMode(p, INPUT))
xPU   (p--)       Arduino: Open Pin Pullup (pinMode(p, INPUT_PULLUP))
xPO   (p--)       Arduino: Open Pin Output (pinMode(p, OUTPUT)
xPRA  (p--n)      Arduino: Pin Read Analog  (n = analogRead(p))
xPRD  (p--n)      Arduino: Pin Read Digital (n = digitalRead(p))
xPWA  (n p--)     Arduino: Pin Write Analog  (analogWrite(p, n))
xPWD  (n p--)     Arduino: Pin Write Digital (digitalWrite(p, n))
xR    (--N)       N: a random 32-bit number
xT    (--n)       n: Current Milliseconds (Arduino: millis())
xN    (--n)       n: Current Microseconds (Arduino: micros())
xW    (n--)       n: MS to wait (Arduino: delay(),  Windows: Sleep(), Linux: usleep())
0@    (--n)       n: Value of HERE variable
xX    (--)        s3 system reset/clear.
xQ    (--)        PC: Exit s3
```
