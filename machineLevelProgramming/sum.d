
sum:	file format mach-o 64-bit x86-64


Disassembly of section __TEXT,__text:

0000000100003ed0 <_plus>:
100003ed0: 55                          	pushq	%rbp
100003ed1: 48 89 e5                    	movq	%rsp, %rbp
100003ed4: 01 f7                       	addl	%esi, %edi
100003ed6: 48 63 c7                    	movslq	%edi, %rax
100003ed9: 5d                          	popq	%rbp
100003eda: c3                          	retq
100003edb: 0f 1f 44 00 00              	nopl	(%rax,%rax)

0000000100003ee0 <_sumstore>:
100003ee0: 55                          	pushq	%rbp
100003ee1: 48 89 e5                    	movq	%rsp, %rbp
100003ee4: 53                          	pushq	%rbx
100003ee5: 50                          	pushq	%rax
100003ee6: 48 89 d3                    	movq	%rdx, %rbx
100003ee9: e8 e2 ff ff ff              	callq	0x100003ed0 <_plus>
100003eee: 48 89 03                    	movq	%rax, (%rbx)
100003ef1: 48 83 c4 08                 	addq	$8, %rsp
100003ef5: 5b                          	popq	%rbx
100003ef6: 5d                          	popq	%rbp
100003ef7: c3                          	retq
100003ef8: 0f 1f 84 00 00 00 00 00     	nopl	(%rax,%rax)

0000000100003f00 <_main>:
100003f00: 55                          	pushq	%rbp
100003f01: 48 89 e5                    	movq	%rsp, %rbp
100003f04: 41 56                       	pushq	%r14
100003f06: 53                          	pushq	%rbx
100003f07: 48 83 ec 10                 	subq	$16, %rsp
100003f0b: 48 89 f3                    	movq	%rsi, %rbx
100003f0e: 48 8b 7e 08                 	movq	8(%rsi), %rdi
100003f12: e8 41 00 00 00              	callq	0x100003f58 <dyld_stub_binder+0x100003f58>
100003f17: 4c 63 f0                    	movslq	%eax, %r14
100003f1a: 48 8b 7b 10                 	movq	16(%rbx), %rdi
100003f1e: e8 35 00 00 00              	callq	0x100003f58 <dyld_stub_binder+0x100003f58>
100003f23: 48 63 d8                    	movslq	%eax, %rbx
100003f26: 48 8d 55 e8                 	leaq	-24(%rbp), %rdx
100003f2a: 4c 89 f7                    	movq	%r14, %rdi
100003f2d: 48 89 de                    	movq	%rbx, %rsi
100003f30: e8 ab ff ff ff              	callq	0x100003ee0 <_sumstore>
100003f35: 48 8b 4d e8                 	movq	-24(%rbp), %rcx
100003f39: 48 8d 3d 48 00 00 00        	leaq	72(%rip), %rdi  # 100003f88 <dyld_stub_binder+0x100003f88>
100003f40: 4c 89 f6                    	movq	%r14, %rsi
100003f43: 48 89 da                    	movq	%rbx, %rdx
100003f46: 31 c0                       	xorl	%eax, %eax
100003f48: e8 11 00 00 00              	callq	0x100003f5e <dyld_stub_binder+0x100003f5e>
100003f4d: 31 c0                       	xorl	%eax, %eax
100003f4f: 48 83 c4 10                 	addq	$16, %rsp
100003f53: 5b                          	popq	%rbx
100003f54: 41 5e                       	popq	%r14
100003f56: 5d                          	popq	%rbp
100003f57: c3                          	retq

Disassembly of section __TEXT,__stubs:

0000000100003f58 <__stubs>:
100003f58: ff 25 a2 40 00 00           	jmpq	*16546(%rip)  # 100008000 <dyld_stub_binder+0x100008000>
100003f5e: ff 25 a4 40 00 00           	jmpq	*16548(%rip)  # 100008008 <dyld_stub_binder+0x100008008>

Disassembly of section __TEXT,__stub_helper:

0000000100003f64 <__stub_helper>:
100003f64: 4c 8d 1d a5 40 00 00        	leaq	16549(%rip), %r11  # 100008010 <__dyld_private>
100003f6b: 41 53                       	pushq	%r11
100003f6d: ff 25 8d 00 00 00           	jmpq	*141(%rip)  # 100004000 <dyld_stub_binder+0x100004000>
100003f73: 90                          	nop
100003f74: 68 00 00 00 00              	pushq	$0
100003f79: e9 e6 ff ff ff              	jmp	0x100003f64 <__stub_helper>
100003f7e: 68 0c 00 00 00              	pushq	$12
100003f83: e9 dc ff ff ff              	jmp	0x100003f64 <__stub_helper>
