segment	.text
align	4
global	_main:function
_main:
	push	ebp
	mov	ebp, esp
	sub	esp, 0
	push	dword 0
	push	dword [esp]
segment	.data
align	4
i:
	dd	0
segment	.text
	push	dword $i
	pop	ecx
	pop	eax
	mov	[ecx], eax
	add	esp, 4
_L1:
	push	dword $i
	pop	eax
	push	dword [eax]
	push	dword 10
	pop	eax
	xor	ecx, ecx
	cmp	[esp], eax
	setl	cl
	mov	[esp], ecx
	pop	eax
	cmp	eax, byte 0
	je	near _L2
segment	.rodata
align	4
_L3:
	db	"hello", 0
segment	.text
	push	dword $_L3
	call	prints
	add	esp, 4
	call	println
	push	dword $i
	pop	eax
	push	dword [eax]
	push	dword 1
	pop	eax
	add	dword [esp], eax
	push	dword [esp]
	push	dword $i
	pop	ecx
	pop	eax
	mov	[ecx], eax
	add	esp, 4
	jmp	dword _L1
_L2:
	push	dword 0
	pop	eax
	leave
	ret
extern	readi
extern	printi
extern	prints
extern	println
