	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 11, 0	sdk_version 12, 1
	.globl	__Z7absdiffll                   ## -- Begin function _Z7absdiffll
	.p2align	4, 0x90
__Z7absdiffll:                          ## @_Z7absdiffll
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, %rax
	movq	%rdi, %rcx
	subq	%rsi, %rcx
	negq	%rcx
	subq	%rsi, %rax
	cmovleq	%rcx, %rax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
.subsections_via_symbols
