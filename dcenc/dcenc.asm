; $Id: dcenc.asm,v 1.7 2002/07/05 03:12:07 j10 Exp $
; Dictionary Coding Routine for FE4
; Modifed for 65816 assembler

curptr equ $186A
savptr equ $187C

table1 equ $8294F0
table2 equ $8296F0
table3 equ $8298F0
table4 equ $829AF0

; IPS Magic
dcb "PATCH"
; Address
dcb $11,$84,$98
dcb {patchend1-patchstart1}/256, {patchend1-patchstart1}

org $918298

patchstart1
   jsl >override
   nop
patchend1

dcb $11,$FC,$B0
dcb {patchend2-patchstart2}/256, {patchend2-patchstart2}
org $91FAB0

patchstart2

override
   lda [$24],y
   and #$00FF
   cmp #$00FB
   beq lookup1
   cmp #$00FC
   beq lookup2
   cmp #$00FD
   beq lookup3pre
   cmp #$00FE
   beq lookup4pre
   cmp #$00FF
   beq returnpre
   rtl

lookup3pre
   jmp lookup3
lookup4pre
   jmp lookup4
returnpre
   jmp return
lookup1
   ; save current pointer
   lda <$24
   sta savptr
   lda <$25
   sta savptr+1
   ; increment pointer by 2
   inc savptr
   inc savptr

   ; read next byte
   iny
   lda [$24],y
   and #$00FF
   asl

   ; load new pointer
   phx
   tax
   lda >table1,x
   sta curptr
   sta <$24
;   inx
;   lda >table1,x
;   sta curptr+1
;   sta <$25
   sep #$20
   lda #<{table1/65536}
   sta curptr+2
   sta <$26
   rep #$20
   plx

   ldy #$0000
   lda [$24],y
   and #$00FF
   rtl

lookup2
   ; save current pointer
   lda <$24
   sta savptr
   lda <$25
   sta savptr+1
   ; increment pointer by 2
   inc savptr
   inc savptr

   ; read next byte
   iny
   lda [$24],y
   and #$00FF
   asl

   ; load new pointer
   phx
   tax
   lda >table2,x
   sta curptr
   sta <$24
   sep #$20
   lda #<{table2/65536}
   sta curptr+2
   sta <$26
   rep #$20
   plx

   ldy #$0000
   lda [$24],y
   and #$00FF
   rtl

lookup3
   ; save current pointer
   lda <$24
   sta savptr
   lda <$25
   sta savptr+1
   ; increment pointer by 2
   inc savptr
   inc savptr

   ; read next byte
   iny
   lda [$24],y
   and #$00FF
   asl

   ; load new pointer
   phx
   tax
   lda >table3,x
   sta curptr
   sta <$24
   sep #$20
   lda #<{table3/65536}
   sta curptr+2
   sta <$26
   rep #$20
   plx

   ldy #$0000
   lda [$24],y
   and #$00FF
   rtl

lookup4
   ; save current pointer
   lda <$24
   sta savptr
   lda <$25
   sta savptr+1
   ; increment pointer by 2
   inc savptr
   inc savptr

   ; read next byte
   iny
   lda [$24],y
   and #$00FF
   asl

   ; load new pointer
   phx
   tax
   lda >table4,x
   sta curptr
   sta <$24
   sep #$20
   lda #<{table4/65536}
   sta curptr+2
   sta <$26
   rep #$20
   plx

   ldy #$0000
   lda [$24],y
   and #$00FF
   rtl

return
   lda savptr
   sta curptr
   sta <$24
   lda savptr+1
   sta curptr+1
   sta <$25
   stz savptr
   stz savptr+1

   ldy #$0000
   jmp >override
patchend2
dcb "EOF"
