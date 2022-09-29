# s3 - A minimal interpreter for PCs and Development boards

## What is s3?
The following statements can be made about s3. 
```
s3 ...

... is a full-featured, interactive, stack-based, interpreter/VM, implemented in under 250 lines of C code.
... has 100% human-readable (and understandable!) "machine code".
... has no compilation phase; it executes the source code directly.
... supports up to 1023 (MAX_FN) function variable-length definitions.
... provides 26 registers, [rA..rZ].
... supports locals, 10 at a time, [r0..r9].
... supports file operations, open, read, write, close, load.
... supports floating point math.
... runs under Windows
... runs under Linux
... TODO: runs on development boards via the Arduino IDE
```
## Why s3?
Many interpreted environments a large SWITCH statement with cases in a loop to execute the user's program. In these systems, the the "machine code"'s opcodes (the cases in the SWITCH statement) are often arbitrarily assigned and are not human-readable, so they have no meaning to the programmer when looking at the code that is actually being executed. Additionally a compiler and/or interpreter are included to create and execute the programs in that environment. In these enviromnents, there is a steep learning curve ... the programmer needs to learn the programming language, the hundreds (or thousands) of user functions in the libraries (or "WORDS" in Forth), how to use the compiler, and how to use the interpreter. I wanted to avoid as much as that as possible, and have only one thing to learn: the "machine code".

## Goals for s3
1. No need for a multiple gigabyte tool chain and the edit/compile/run paradigm for developing everyday programs.

2. A simple, minimal, and interactive programming environment that can be modified easily.

3. An environment that can be easily configured for and deployed to many different types of development boards via the Arduino IDE.

4. To be able to use the same environment on my personal computer(s) as well as development boards.

5. Short and intuitive commands, so there is not a lot of typing needed.

## Building s3
```
- Windows: I use Visual Studio, the s3.sln. Use the x86 configuration, 64-bit doesn't work.
- Linux: There is a simple ./make shell script. GCC or CLANG, your choice; 32 or 64 bit both work.
- Development Boards: Use the Arduino IDE, edit the BSZ, ISZ and FILES_SZ as appropriate.
  - NOTE: this part is currently under development
```

## Some s3 examples
```
; To enter a comment: 
    0( here is a comment )
  
; If/Else
    s3 code:          rC #("Yes")~("No")
    Forth equivalent: C @ IF ."Yes" ELSE ."No" THEN
    C equivalent:     if (C) { printf("Yes") } else { printf("No") }

 ; x = (a == b) ? c : d;
    s3 code:          rA rB=#(rC$)~(rD)sX;
    Forth equivalent: a @ b @ = IF c @ ELSE d @ THEN x !
    C equivalent:     x = (a == b) ? c : d;

; To make sure A < B
    S3 code:             %%>($)
    Forth equivalent:    OVER OVER > IF SWAP THEN
    C equivalent:        if (f > t) { int x = f; f = t; t = x; }

; To do something N times (in this case, execute Sub):
    S3 code:             rN 0[Sub]
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
    :CopyFTN 0(f t n--) l+ s3 s2 s1 r3 0[r1 c@ r2 c! i1 i2] l-;

; A simple benchmark for a 100 million FOR loop:
    :Mil 1000 # * *;
    :Bench 0(n--) t $ 0[] t $ -;
    100 Mil Bench . "usec"

; A simple benchmark for a 100 million WHILE loop:
    :Mil 1000#**;
    :Bench 0(n--) t${1-#}\t$-." usec";
    100 Mil Bench ." usec" 0(note that spaces are optional)

; Define a word to display the currently defined code:
    :Code 0(--) 0@1[nc@#58=(n1-c@59=(13,10,),];

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
^   (a b--r)      r: MODULO(a, b)
&   (a b--q r)    q: DIV(a,b), r: MODULO(a,b) - /MOD


*** BIT MANIPULATION ***
b&  (a b--n)      n: a AND b
b|  (a b--n)      n: a OR  b
b^  (a b--n)      n: a XOR b
b~  (a--b)        b: NOT a (ones-complement, e.g - 101011 => 010100)


*** STACK ***
#  (a--a a)       Duplicate TOS                    (DUP)
\  (a b--a)       Drop TOS                         (DROP)
$  (a b--b a)     Swap top 2 stack items           (SWAP)
%  (a b--a b a)   Push 2nd                         (OVER)
_  (a--b)         b: -a                            (Negate)
a  (a--b)         b: abs(a)                        (Absolute)
i  (x--y)         y: x+1                           (Increment)
d  (x--y)         y: x-1                           (Decrement)


*** MEMORY ***
@     (a--n)      Fetch CELL n from CELL address a
!     (n a--)     Store CELL n to CELL address a
c@    (a--n)      Fetch BYTE n from BYTE address a
c!    (n a--)     Store BYTE n to BYTE address a
m@    (a--n)      Fetch BYTE n from ABSOLUTE address a
m!    (n a--)     Store BYTE n to ABSOLUTE address a


*** REGISTERS and LOCALS ***
        NOTES: - s3 allocates 10 locals at a time, [r0..r9].
               - Register names are 1 UPPERCASE character: [A..Z]
l+    (--)        Allocate 10 locals, [r0..r9]
r3    (--n)       n: value of local #3
s3    (n--)       n: value to store in local #3
i3    (--)        Increment local #3
d3    (--)        Decrement local #3
l-    (--)        De-allocate the current set of locals.
rC    (--n)       n: value of register C
sC    (n--)       n: value to store in register C
iC    (--)        Increment register C
dC    (--)        Decrement register C


*** WORDS/FUNCTIONS ***
        NOTE: Word names are variable-length alphabetic (A-z) and start with a Capital letter.
:     (--)        Define word. Copy chars to (HERE++) until closing ';'.
Abc   (--)        Execute/call word Abc
;     (--)        End of word definition. Also Exit word.
^     (--)        Exit word.
        NOTE: To exit a word while inside of a loop, use 'xU^'.
              example: :LoopTest 100 0[n. n32=("-out" xU^)", "];


*** INPUT/OUTPUT ***
NNN   (--n)       Scan DECIMAL number. For multiple numbers, separate them by space (47 33).
        NOTEs: (1) To enter a negative number, use "negate" (eg - 490_).
               (2) To enter a float, end with 'e' (eg - 1234e).
'x     (--n)      n: the ASCII value of x
hXXX   (--h)      Scan XXX as a HEX number (0-9, A-F, a-f).
.      (N--)      Output N as decimal number.
,      (N--)      Output N an ASCII character.
b       (--)      Output a single SPACE.
"      (?--?)     Output a formatted string until the next '"'.
    NOTES: - %d outputs TOS as an integer
           - %X outputs TOS as a hex number (A-F are uppercase)
           - %x outputs TOS as a hex number (A-F are lowercase)
           - %c outputs TOS as a character
           - %n outputs CR/LF
           - %e outputs an ESCAPE (27)
           - %q outputs a QUOTE
           - %<x> outputs <x> (eg - %% outputs %)
`XXX`  (--)       Calls system("XXX").
xY     (a--)      Calls system(a).
|XXX|  (a--b)     Copies XXX to address a, b is the next address after the NULL terminator.
z      (a--)      ZTYPE: Output the formatted string at address a (see ").
t      (a--)      TYPE: Output the string at address a (no formatting).
xK?    (--f)      TODO: f: 1 if a character is waiting in the input buffer, else 0.
xK@    (--c)      TODO: c: next character from the input buffer. If no character, wait.


*** CONDITIONS/LOOPS/FLOW CONTROL ***
<     (a b--f)    f: (a < b) ? 1 : 0;
=     (a b--f)    f: (a = b) ? 1 : 0;
>     (a b--f)    f: (a > b) ? 1 : 0;
~     (n -- f)    f: (a = 0) ? 1 : 0; (Logical NOT)
[     (T F--)     For: start a For/Next loop.
n     (--n)       n: the index of the current FOR loop
]     (--)        NEXT: increment index (n) and stop if (T<=n)
x]    (N--)       +NEXT: Add N to the index (n) and stop if (n==T) or (n crosses T)
{     (--)        BEGIN While loop
}     (f--)       WHILE: if (f != 0) jump back to BEGIN, else continue
xU    (--)        UNLOOP: Unwind the LOOP stack
(     (f--)       IF: if (f == 0), skip to next ')'.


*** OTHER ***
xL    (a--)       Load from file a (eg - 1000#|tests|\xL).
xPI   (p--)       Arduino: Pin Input  (pinMode(p, INPUT))
xPU   (p--)       Arduino: Pin Pullup (pinMode(p, INPUT_PULLUP))
xPO   (p--)       Arduino: Pin Output (pinMode(p, OUTPUT)
xPRA  (p--n)      Arduino: Pin Read Analog  (n = analogRead(p))
xPRD  (p--n)      Arduino: Pin Read Digital (n = digitalRead(p))
xPWA  (n p--)     Arduino: Pin Write Analog  (analogWrite(p, n))
xPWD  (n p--)     Arduino: Pin Write Digital (digitalWrite(p, n))
xR    (--r)       r: a random number in the range [0..0xFFFFFFFF]
xT    (--n)       Milliseconds (Arduino: millis(), Windows/Linux: clock())
xN    (--n)       Microseconds (Arduino: micros(), Windows/Linux: clock())
xW    (n--)       TODO: Wait (Arduino: delay(),  Windows: Sleep())
0@    (--n)       Value of HERE variable
xY    (A--)       Sends string at A to system() (example: 1000#|ls|\xY).
xX    (--)        s3 system reset/clear.
xQ    (--)        PC: Exit s3
```
