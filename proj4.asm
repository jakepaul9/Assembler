;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;; PARTS 1 & 2 : RECURSIVE FIB FUNCTION AND ADDRESSING MODE :::::::::::::
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

            LDR R7  COUNT 

DOWHILE     TRP 2               ;GET NUMBER FROM INPUT
            BRZ R3  CONTINUE
            STR R3  N
            LDA R6  NUM_ARR
            ADD R6  R7
            STR R3  R6
            ADI R7  4

            MOV R2  FP          ;CREATE ACTIVATION RECORD FOR CALL TO FIB
            MOV FP  SP
            ADI SP  -4
            STR R2  SP
            ADI SP  -4
            STR R3  SP
            ADI SP  -4
            MOV R3  PC
            ADI R3  36
            STR R3  FP
            JMP FIB             ;CALL FIB FUNCTION

            LDR R3  SP          ;GET FINAL RESULT OFF TOP OF STACK 
            STR R3  RESULT1
            LDA R6  NUM_ARR
            ADD R6  R7
            STR R3  R6
            ADI R7  4
            STR R7  COUNT
            
            LDA R5  STR1
            LDR R2  STR1_SIZE
LOOP1       BRZ R2  PCONT1
            LDB R3  R5
            TRP 3
            ADI R5  1
            ADI R2  -1
            JMP LOOP1

PCONT1      LDR R3  N             
            TRP 1
            LDB R3  SPACE
            TRP 3
            LDB R3  i 
            TRP 3               
            LDB R3  s 
            TRP 3
            LDB R3  SPACE
            TRP 3
            LDR R3  RESULT1
            TRP 1
            LDB R3  NEWL
            TRP 3
            LDR R2  COUNT
            LDR R3  INC         
            LDR R4  CNT
            DIV R2  R3
            SUB R2  R4
            BRZ R2  CONTINUE
            JMP DOWHILE


;;;;;;;;;;;;;;;FIB(N);;;;;;;;;;;;;;;

FIB         LDR R3  ITER 
            STR R3  SP
            ADI SP  -12

            MOV R1  FP
            ADI R1  -8      
            LDR R2  R1
            MOV R6  R2
            LDR R3  ONE
            CMP R6  R3
            BGT R6  WHILE
            

            MOV SP  FP
            MOV R5  SP
            CMP R5  SB
            BGT R5  UNDERFLOW
            LDR R5  FP              
            MOV R4  FP 
            ADI R4  -4
            LDR FP  R4
            STR R2  SP
            JMR R5  ;50

WHILE       MOV R1 FP
            ADI R1 -12
            LDR R0  R1          ;GET LOCAL VARIABLE ITER 
            BRZ R0  RETURN
            ADI R1  4           
            LDR R2  R1          ;GET PARAM N
            SUB R2  R0          ;N - ITER 
            
            MOV R5 SP
            ADI R5 -8
            CMP R5 SL
            BLT R5 OVERFLOW

            MOV R3  FP          ;CREATE ACTIVATION RECORD FOR RECURSIVE CALL
            MOV FP  SP
            ADI SP  -4
            STR R3  SP          ;STORE RETURN ADDRESS (PFP)
            ADI SP  -4
            STR R2  SP          ;SET PARAM N
            ADI SP  -4
            MOV R3  PC
            ADI R3  36
            STR R3  FP
            JMP FIB

            MOV R1  FP
            ADI R1  -12         ;CHECK WHICH ITERATION
            LDR R0  R1
            MOV R3  R0
            LDR R6  ONE
            CMP R3  R6
            BRZ R3  TEMP2       
            ADI R0  -1          ;DECREMENT AND SAVE ITER 
            STR R0  R1
            ADI R1  -4          ;STORE RETURNED VALUE OF TOP OF STACK TO TEMP VAR TEMP1    
            LDR R3  SP
            STR R3  R1          ;STORE RESULT FOR F(N-2) INTO TEMP1 VAR
            JMP WHILE

TEMP2       ADI R0  -1          ;DECREMENT AND SAVE ITER 
            STR R0  R1
            ADI R1  -8          ;STORE RETURNED VALUE OF TOP OF STACK TO TEMP VAR TEMP1
            LDR R3  SP
            STR R3  R1          ;STORE RESULT FOR F(N-1) INTO TEMP2 VAR
            JMP WHILE

RETURN      MOV R1  FP          ;ADD TEMP1 AND TEMP2
            ADI R1  -16
            LDR R4  R1
            ADI R1  -4
            LDR R5  R1
            ADD R4  R5
            
            MOV SP  FP          ;DEALOCATE ACTIVATION RECORD
            MOV R3  SP          
            CMP R3  SB
            BGT R3  UNDERFLOW
            LDR R5  FP
            MOV R2  FP
            ADI R2  -4
            LDR FP  R2
            STR R4  SP          ;PUT RESULT ON STACK
            JMR R5              ;END FIB FUNCTION

CONTINUE    LDA R6  NUM_ARR     ;PART 2 ADDRESSING MODE TEST
            LDR R3  R6
            MOV R7  R6
            ADI R7  -4
            LDR R0  COUNT
            BRZ R0  PCONT2
            ADD R7  R0
            LDR R1  INC
            DIV R0  R1      

LOOP2       LDR R3  R6          ;PRINT OUT ARRAY
            TRP 1
            LDB R3  COMMA       
            TRP 3
            LDB R3 SPACE
            TRP 3
            LDR R3  R7
            TRP 1
            ADI R0  -2
            ADI R7  -4
            ADI R6  4
            BRZ R0  PCONT2
            LDB R3  COMMA
            TRP 3
            LDB R3 SPACE
            TRP 3
            JMP LOOP2

PCONT2      LDB R3  NEWL
            TRP 3

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;; PART 3: MULTI-THREADED VM :::::::::::::::
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

            LDR R3  ZERO
            STR R3  COUNT
            LCK ThreadMutex
            TRP 2               ;GET X
WHILE2      BRZ R3  ENDWHILE2
            RUN R0  NewThread
            TRP 2
            JMP WHILE2

ENDWHILE2   ULK ThreadMutex
            BLK

            LDA R6  NUM_ARR     ;PART 2 ADDRESSING MODE TEST
            LDR R3  R6
            MOV R7  R6
            ADI R7  -4
            LDR R0  COUNT
            ADD R7  R0
            LDR R1  INC
            DIV R0  R1      

LOOP3       LDR R3  R6          ;PRINT OUT ARRAY
            TRP 1
            LDB R3  COMMA
            TRP 3
            LDB R3 SPACE        
            TRP 3
            LDR R3  R7
            TRP 1
            ADI R0  -2
            ADI R7  -4
            ADI R6  4
            BRZ R0  PCONT3
            LDB R3  COMMA
            TRP 3
            LDB R3 SPACE
            TRP 3
            JMP LOOP3

PCONT3      LDB R3  NEWL
            TRP 3

            TRP 0 ;

NewThread   LCK ThreadMutex
            ULK ThreadMutex
            MOV R7  R3
            MOV R2  FP          ;CREATE ACTIVATION RECORD FOR CALL TO FIB
            MOV FP  SP
            ADI SP  -4
            STR R2  SP
            ADI SP  -4
            STR R3  SP
            ADI SP  -4
            MOV R3  PC
            ADI R3  36
            STR R3  FP
            JMP FIB 

            LDR R1  SP

            LDA R5  STR1
            LDR R2  STR1_SIZE
LOOP4       BRZ R2  PCONT4
            LDB R3  R5
            TRP 3
            ADI R5  1
            ADI R2  -1
            JMP LOOP4

PCONT4      MOV R3  R7             
            TRP 1
            LDB R3  SPACE
            TRP 3
            LDB R3  i 
            TRP 3
            LDB R3  s 
            TRP 3
            LDB R3  SPACE
            TRP 3
            LDR R3  SP
            TRP 1
            LDB R3  NEWL
            TRP 3

            LCK ArrayMutex
            LDR R4 COUNT
            LDA R6 NUM_ARR
            ADD R6  R4
            STR R7 R6
            ADI R6  4
            STR R1  R6
            ADI R4  8
            STR R4  COUNT
            ULK ArrayMutex
            END

UNDERFLOW   TRP 0

OVERFLOW    TRP 0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;GLOBALS;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ZERO        .INT    0
ONE         .INT    1
ITER        .INT    2
RESULT1     .INT    0
STR1_SIZE   .INT    13
N           .INT    0
CNT         .INT    30
COUNT       .INT    0
INC         .INT    4
ThreadMutex .INT    -1
ArrayMutex  .INT    -1


NUM_ARR     .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0
            .INT    0 

i           .BYT    'i'
s           .BYT    's'
NEWL        .BYT    '\n'
SPACE       .BYT    ' '
COMMA       .BYT    ','

STR1        .BYT    'F'
            .BYT    'i'
            .BYT    'b'
            .BYT    'o'
            .BYT    'n'
            .BYT    'a'
            .BYT    'c'
            .BYT    'c'
            .BYT    'i'
            .BYT    ' '
            .BYT    'o'
            .BYT    'f'
            .BYT    ' '