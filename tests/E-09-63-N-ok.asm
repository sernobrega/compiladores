; TEXT
segment	.text
; ALIGN
align	4
; GLOBAL _main, :function
global	_main:function
; LABEL _main
_main:
; ENTER 4
	push	ebp
	mov	ebp, esp
	sub	esp, 4
; INT 0
	push	dword 0
; LOCAL -4
	lea	eax, [ebp+-4]
	push	eax
; STINT
	pop	ecx
	pop	eax
	mov	[ecx], eax
        ;; before body 
        ;; section inclusive
; INT 1
	push	dword 1
; JZ _L3
	pop	eax
	cmp	eax, byte 0
	je	near _L3
        ;; FOR initialize
; INT 1
	push	dword 1
; DUP32
	push	dword [esp]
; LOCAL -8
	lea	eax, [ebp+-8]
	push	eax
; STINT
	pop	ecx
	pop	eax
	mov	[ecx], eax
        ;; FOR test
; LABEL _L4
_L4:
; LOCAL -8
	lea	eax, [ebp+-8]
	push	eax
; LDINT
	pop	eax
	push	dword [eax]
; INT 3
	push	dword 3
; LE
	pop	eax
	xor	ecx, ecx
	cmp	[esp], eax
	setle	cl
	mov	[esp], ecx
; JZ _L6
	pop	eax
	cmp	eax, byte 0
	je	near _L6
        ;; FOR instruction
; LOCAL -8
	lea	eax, [ebp+-8]
	push	eax
; LDINT
	pop	eax
	push	dword [eax]
        ;; int 
; CALL printi
	call	printi
; TRASH 4
	add	esp, 4
        ;; FOR increment
; LABEL _L5
_L5:
; LOCAL -8
	lea	eax, [ebp+-8]
	push	eax
; LDINT
	pop	eax
	push	dword [eax]
; INT 1
	push	dword 1
; ADD
	pop	eax
	add	dword [esp], eax
; DUP32
	push	dword [esp]
; LOCAL -8
	lea	eax, [ebp+-8]
	push	eax
; STINT
	pop	ecx
	pop	eax
	mov	[ecx], eax
        ;; FOR jump to test
; JMP _L4
	jmp	dword _L4
        ;; FOR end
; LABEL _L6
_L6:
; LABEL _L3
_L3:
; LABEL _L1
_L1:
        ;; after body 
; LABEL _L2
_L2:
; STFVAL32
	pop	eax
; LEAVE
	leave
; RET
	ret
; EXTERN printi
extern	printi
