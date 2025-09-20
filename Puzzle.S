#define __SFR_OFFSET 0x00
#include "avr/io.h"

.global asmDelay

.equ delayVal, 40000        ; inner loop counter (16 bit value)

asmDelay:                   ; function name
  outerLoop:
    LDI R30, lo8(delayVal)  ; first 8 bits of inner loop counter
    LDI R31, hi8(delayVal)  ; second 8 bits of inner loop counter
  innerLoop:
    SBIW R30, 1             ; sub 1 from inner loop counter
    BRNE innerLoop          ; if counter != 0, branch to inner loop
    SUBI R21, 1             ; sub 1 from outer loop counter passed to R21
    BRNE outerLoop          ; if counter != 0, branch to outer loop
  RET