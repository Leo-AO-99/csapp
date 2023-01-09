# bombLab

os:Linux

```shell
objdump -d bomd > bomb.asm
```

or

```shell
gdb bomb
disas <function_name>
```

#### 炸弹一

```assembly
0000000000400ee0 <phase_1>:
  400ee0:	48 83 ec 08          	sub    $0x8,%rsp
  400ee4:	be 00 24 40 00       	mov    $0x402400,%esi
  400ee9:	e8 4a 04 00 00       	callq  401338 <strings_not_equal>
  400eee:	85 c0                	test   %eax,%eax
  400ef0:	74 05                	je     400ef7 <phase_1+0x17>
  400ef2:	e8 43 05 00 00       	callq  40143a <explode_bomb>
  400ef7:	48 83 c4 08          	add    $0x8,%rsp
  400efb:	c3                   	retq   
```

mov    $0x402400,%esi

将0x402400地址的内容传入esi

调用strings_not_equal

将我们输入的字符串与esi的字符串做对比

得到0x402400下的字符串内容即可

```shell
gdb bomb
x/s 0x402400
“Border relations with Canada have never been better.”
```

##### phase_1

Border relations with Canada have never been better.

#### 炸弹二

先分析read_six_numbers

```assembly
000000000040145c <read_six_numbers>:
  40145c:	48 83 ec 18          	sub    $0x18,%rsp
  401460:	48 89 f2             	mov    %rsi,%rdx
  401463:	48 8d 4e 04          	lea    0x4(%rsi),%rcx
  401467:	48 8d 46 14          	lea    0x14(%rsi),%rax
  40146b:	48 89 44 24 08       	mov    %rax,0x8(%rsp)
  401470:	48 8d 46 10          	lea    0x10(%rsi),%rax
  401474:	48 89 04 24          	mov    %rax,(%rsp)
  401478:	4c 8d 4e 0c          	lea    0xc(%rsi),%r9
  40147c:	4c 8d 46 08          	lea    0x8(%rsi),%r8
  401480:	be c3 25 40 00       	mov    $0x4025c3,%esi
  401485:	b8 00 00 00 00       	mov    $0x0,%eax
  40148a:	e8 61 f7 ff ff       	callq  400bf0 <__isoc99_sscanf@plt>
  40148f:	83 f8 05             	cmp    $0x5,%eax
  401492:	7f 05                	jg     401499 <read_six_numbers+0x3d>
  401494:	e8 a1 ff ff ff       	callq  40143a <explode_bomb>
  401499:	48 83 c4 18          	add    $0x18,%rsp
  40149d:	c3                   	retq   
```

内部调用sscanf，查看esi内容

```shell
x/s 0x4025c3
"%d %d %d %d %d %d"
```

按照函数参数在寄存器中的顺序结合函数名称可知，第三到六个参数对应第一到四个数字

分析可知（过程省略，下面的加法为rsi内的地址的加法）

rdx=rsi 
rdx=rsi+4
r8=rsi+8
r9=rsi+12

超出六个的存在栈内，因为保存地址，操作环境为64位，一次八字节

```assembly
401467:	48 8d 46 14          	lea    0x14(%rsi),%rax
40146b:	48 89 44 24 08       	mov    %rax,0x8(%rsp)
401470:	48 8d 46 10          	lea    0x10(%rsi),%rax
401474:	48 89 04 24          	mov    %rax,(%rsp)
```

```assembly
0000000000400efc <phase_2>:
  400efc:	55                   	push   %rbp
  400efd:	53                   	push   %rbx
  400efe:	48 83 ec 28          	sub    $0x28,%rsp
  400f02:	48 89 e6             	mov    %rsp,%rsi
  400f05:	e8 52 05 00 00       	callq  40145c <read_six_numbers>
  400f0a:	83 3c 24 01          	cmpl   $0x1,(%rsp)
  400f0e:	74 20                	je     400f30 <phase_2+0x34>
  400f10:	e8 25 05 00 00       	callq  40143a <explode_bomb>
  400f15:	eb 19                	jmp    400f30 <phase_2+0x34>
  400f17:	8b 43 fc             	mov    -0x4(%rbx),%eax
  400f1a:	01 c0                	add    %eax,%eax
  400f1c:	39 03                	cmp    %eax,(%rbx)
  400f1e:	74 05                	je     400f25 <phase_2+0x29>
  400f20:	e8 15 05 00 00       	callq  40143a <explode_bomb>
  400f25:	48 83 c3 04          	add    $0x4,%rbx
  400f29:	48 39 eb             	cmp    %rbp,%rbx
  400f2c:	75 e9                	jne    400f17 <phase_2+0x1b>
  400f2e:	eb 0c                	jmp    400f3c <phase_2+0x40>
  400f30:	48 8d 5c 24 04       	lea    0x4(%rsp),%rbx
  400f35:	48 8d 6c 24 18       	lea    0x18(%rsp),%rbp
  400f3a:	eb db                	jmp    400f17 <phase_2+0x1b>
  400f3c:	48 83 c4 28          	add    $0x28,%rsp
  400f40:	5b                   	pop    %rbx
  400f41:	5d                   	pop    %rbp
  400f42:	c3                   	retq   
```



```assembly
400f02:	48 89 e6             	mov    %rsp,%rsi
```

已知rsi指向的地址为六个数字的第一个数字的地址，而地址来自rsp，调用函数结束后，rsp复原，可知当前rsp内存储的地址为六个数字的第一个数字的地址



调用read_six_numbers，需要输入六个数字

```assembly
400f0a:	83 3c 24 01          	cmpl   $0x1,(%rsp)
```

可以确定第一个数字为1



```assembly
400f0e:	74 20                	je     400f30 <phase_2+0x34>
```

如果相同，跳转到0x400f30



```assembly
400f30:	48 8d 5c 24 04       	lea    0x4(%rsp),%rbx
```

一个整型4字节，0x4(%rsp)为第二个整型地址



```assembly
400f35:	48 8d 6c 24 18       	lea    0x18(%rsp),%rbp
```

0x18(%rsp)为第六个整型地址结尾，标示着结束循环，或者理解为指向数组结尾

0x400f2e行用来判断是否结束循环



```assembly
400f3a:	eb db                	jmp    400f17 <phase_2+0x1b>
```

跳转到0x400f17



```assembly
400f17:	8b 43 fc             	mov    -0x4(%rbx),%eax
```

 -0x4(%rbx)实际的地址依然为%rsp，因此是1，eax内存入1



```assembly
400f1a:	01 c0                	add    %eax,%eax
400f1c:	39 03                	cmp    %eax,(%rbx)
```

eax内为2，因此克制rbx指向的值为2，第二个整型为2



```assembly
400f25:	48 83 c3 04          	add    $0x4,%rbx
```

rbx指向第三个整型



```assembly
400f29:	48 39 eb             	cmp    %rbp,%rbx
400f2c:	75 e9                	jne    400f17 <phase_2+0x1b>
```

跳转到0x400f17

重复上面步骤，第三个整型为4，第四个整型为8，第五个整型为16，第六个整型为32，最后结束循环



##### phase_2

1 2 4 8 16 32



#### 炸弹三

```assembly
0000000000400f43 <phase_3>:
  400f43:	48 83 ec 18          	sub    $0x18,%rsp
  400f47:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx
  400f4c:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx
  400f51:	be cf 25 40 00       	mov    $0x4025cf,%esi
  400f56:	b8 00 00 00 00       	mov    $0x0,%eax
  400f5b:	e8 90 fc ff ff       	callq  400bf0 <__isoc99_sscanf@plt>
  400f60:	83 f8 01             	cmp    $0x1,%eax
  400f63:	7f 05                	jg     400f6a <phase_3+0x27>
  400f65:	e8 d0 04 00 00       	callq  40143a <explode_bomb>
  400f6a:	83 7c 24 08 07       	cmpl   $0x7,0x8(%rsp)
  400f6f:	77 3c                	ja     400fad <phase_3+0x6a>
  400f71:	8b 44 24 08          	mov    0x8(%rsp),%eax
  400f75:	ff 24 c5 70 24 40 00 	jmpq   *0x402470(,%rax,8)
  400f7c:	b8 cf 00 00 00       	mov    $0xcf,%eax
  400f81:	eb 3b                	jmp    400fbe <phase_3+0x7b>
  400f83:	b8 c3 02 00 00       	mov    $0x2c3,%eax
  400f88:	eb 34                	jmp    400fbe <phase_3+0x7b>
  400f8a:	b8 00 01 00 00       	mov    $0x100,%eax
  400f8f:	eb 2d                	jmp    400fbe <phase_3+0x7b>
  400f91:	b8 85 01 00 00       	mov    $0x185,%eax
  400f96:	eb 26                	jmp    400fbe <phase_3+0x7b>
  400f98:	b8 ce 00 00 00       	mov    $0xce,%eax
  400f9d:	eb 1f                	jmp    400fbe <phase_3+0x7b>
  400f9f:	b8 aa 02 00 00       	mov    $0x2aa,%eax
  400fa4:	eb 18                	jmp    400fbe <phase_3+0x7b>
  400fa6:	b8 47 01 00 00       	mov    $0x147,%eax
  400fab:	eb 11                	jmp    400fbe <phase_3+0x7b>
  400fad:	e8 88 04 00 00       	callq  40143a <explode_bomb>
  400fb2:	b8 00 00 00 00       	mov    $0x0,%eax
  400fb7:	eb 05                	jmp    400fbe <phase_3+0x7b>
  400fb9:	b8 37 01 00 00       	mov    $0x137,%eax
  400fbe:	3b 44 24 0c          	cmp    0xc(%rsp),%eax
  400fc2:	74 05                	je     400fc9 <phase_3+0x86>
  400fc4:	e8 71 04 00 00       	callq  40143a <explode_bomb>
  400fc9:	48 83 c4 18          	add    $0x18,%rsp
  400fcd:	c3                   	retq   

```

查看sscanf输入格式

```shell
x/s 0x4025cf
"%d %d"
```

可知炸弹三需要两个输入整型

scanf返回值为输入的项数，可知eax内为2，跳过explode_bomb

```assembly
400f47:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx
400f4c:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx
```

第一个数保存在0x8(%rsp)，第二个数保存在0xc(%rsp)

可以看出使用了switch



```assembly
400f6a:	83 7c 24 08 07       	cmpl   $0x7,0x8(%rsp)
400f6f:	77 3c                	ja     400fad <phase_3+0x6a>
```

可以看出第一个数小于7大于等于0



```assembly
400f71:	8b 44 24 08          	mov    0x8(%rsp),%eax
400f75:	ff 24 c5 70 24 40 00 	jmpq   *0x402470(,%rax,8)
```

```shell
p/x *(int*)(0x402470 + 8 * $rax)
```

rax内保存第一个数，可以计算跳转地址



```assembly
400fbe:	3b 44 24 0c          	cmp    0xc(%rsp),%eax
```

可以看出第二个数字需要等于%rax里面的值，同时每个case下都有mov    <HEX>,%eax



以第一个数为2时为例

```shell
p/x *(int*)(0x402470 + 8 * 2)
0x400f83
```

```assembly
400f83:	b8 c3 02 00 00       	mov    $0x2c3,%eax
400f88:	eb 34                	jmp    400fbe <phase_3+0x7b>
```

因此第二个数字为0x2c3（707）

##### phase_3

（任选一组数字）

0  207

1 311

2 707

3 256

4 389

5 206

6 682



#### 炸弹四

```assembly
000000000040100c <phase_4>:
  40100c:	48 83 ec 18          	sub    $0x18,%rsp
  401010:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx
  401015:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx
  40101a:	be cf 25 40 00       	mov    $0x4025cf,%esi
  40101f:	b8 00 00 00 00       	mov    $0x0,%eax
  401024:	e8 c7 fb ff ff       	callq  400bf0 <__isoc99_sscanf@plt>
  401029:	83 f8 02             	cmp    $0x2,%eax
  40102c:	75 07                	jne    401035 <phase_4+0x29>
  40102e:	83 7c 24 08 0e       	cmpl   $0xe,0x8(%rsp)
  401033:	76 05                	jbe    40103a <phase_4+0x2e>
  401035:	e8 00 04 00 00       	callq  40143a <explode_bomb>
  40103a:	ba 0e 00 00 00       	mov    $0xe,%edx
  40103f:	be 00 00 00 00       	mov    $0x0,%esi
  401044:	8b 7c 24 08          	mov    0x8(%rsp),%edi
  401048:	e8 81 ff ff ff       	callq  400fce <func4>
  40104d:	85 c0                	test   %eax,%eax
  40104f:	75 07                	jne    401058 <phase_4+0x4c>
  401051:	83 7c 24 0c 00       	cmpl   $0x0,0xc(%rsp)
  401056:	74 05                	je     40105d <phase_4+0x51>
  401058:	e8 dd 03 00 00       	callq  40143a <explode_bomb>
  40105d:	48 83 c4 18          	add    $0x18,%rsp
  401061:	c3                   	retq   
```

看到sscanf，分析输入

```assembly
401010:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx
401015:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx
```

```shell
x/s 0x4025cf
"%d %d"
```



```assembly
40102e:	83 7c 24 08 0e       	cmpl   $0xe,0x8(%rsp)
401033:	76 05                	jbe    40103a <phase_4+0x2e>
```

第一个数字需要小于14（还要大于等于0，测试出来，不知道汇编如何看出来的，而且小于0，func4好像也会爆栈）



```assembly
401051:	83 7c 24 0c 00       	cmpl   $0x0,0xc(%rsp)
401056:	74 05                	je     40105d <phase_4+0x51>
```

第二个数字需要等于0



```assembly
40103a:	ba 0e 00 00 00       	mov    $0xe,%edx
40103f:	be 00 00 00 00       	mov    $0x0,%esi
401044:	8b 7c 24 08          	mov    0x8(%rsp),%edi
401048:	e8 81 ff ff ff       	callq  400fce <func4>
40104d:	85 c0                	test   %eax,%eax
40104f:	75 07                	jne    401058 <phase_4+0x4c>
```

调用func4，第一个参数为输入的第一个数，第二个参数为0，第三个参数为14，返回值要求为0



```assembly
0000000000400fce <func4>:
  400fce: 48 83 ec 08                  	subq	$8, %rsp
  400fd2: 89 d0                        	movl	%edx, %eax
  400fd4: 29 f0                        	subl	%esi, %eax
  400fd6: 89 c1                        	movl	%eax, %ecx
  400fd8: c1 e9 1f                     	shrl	$31, %ecx
  400fdb: 01 c8                        	addl	%ecx, %eax
  400fdd: d1 f8                        	sarl	%eax
  400fdf: 8d 0c 30                     	leal	(%rax,%rsi), %ecx
  400fe2: 39 f9                        	cmpl	%edi, %ecx
  400fe4: 7e 0c                        	jle	0x400ff2 <func4+0x24>
  400fe6: 8d 51 ff                     	leal	-1(%rcx), %edx
  400fe9: e8 e0 ff ff ff               	callq	0x400fce <func4>
  400fee: 01 c0                        	addl	%eax, %eax
  400ff0: eb 15                        	jmp	0x401007 <func4+0x39>
  400ff2: b8 00 00 00 00               	movl	$0, %eax
  400ff7: 39 f9                        	cmpl	%edi, %ecx
  400ff9: 7d 0c                        	jge	0x401007 <func4+0x39>
  400ffb: 8d 71 01                     	leal	1(%rcx), %esi
  400ffe: e8 cb ff ff ff               	callq	0x400fce <func4>
  401003: 8d 44 00 01                  	leal	1(%rax,%rax), %eax
  401007: 48 83 c4 08                  	addq	$8, %rsp
  40100b: c3                           	retq
```

参数分别为<第一个输入的数>，0，14



```c
#include <stdio.h>
#include <stdlib.h>
int func4(int edi, int esi, int edx)
{
    int eax = edx;
    eax -= esi;
    int ecx = eax;
    ecx >>= 31;
    eax += ecx;
    eax >>= 1;
    ecx = eax + 1 * esi;
    if (edi < ecx)
    {
        edx = ecx - 1;
        eax = func4(edi, esi, edx);
        eax += eax;
        return eax;
    }
    else
    {
        // ecx <= edi
        eax = 0;
        if (edi > ecx)
        {
            esi = ecx + 1;
            eax = func4(edi, esi, edx);
            eax = eax + 1 * eax + 1;
            return eax;
        }
        else
        {
            return eax;
        }
    }
}
int main()
{
    for (int n = 0; n <= 14; n++)
    {
        int ret = func4(n, 0, 14);
        if (ret == 0)
            printf("%d ", n);
    }
    return 0;
}


/****
0 1 3 7
****/
```



##### phase_4

（任选一组数字）

0 0

1 0

3 0

7 0



#### 炸弹五

```assembly
0000000000401062 <phase_5>:
  401062:	53                   	push   %rbx
  401063:	48 83 ec 20          	sub    $0x20,%rsp
  401067:	48 89 fb             	mov    %rdi,%rbx
  40106a:	64 48 8b 04 25 28 00 	mov    %fs:0x28,%rax
  401071:	00 00 
  401073:	48 89 44 24 18       	mov    %rax,0x18(%rsp)
  401078:	31 c0                	xor    %eax,%eax
  40107a:	e8 9c 02 00 00       	callq  40131b <string_length>
  40107f:	83 f8 06             	cmp    $0x6,%eax
  401082:	74 4e                	je     4010d2 <phase_5+0x70>
  401084:	e8 b1 03 00 00       	callq  40143a <explode_bomb>
  
  
  
  401089:	eb 47                	jmp    4010d2 <phase_5+0x70> 
  
 
  40108b:	0f b6 0c 03          	movzbl (%rbx,%rax,1),%ecx
  40108f:	88 0c 24             	mov    %cl,(%rsp)
  401092:	48 8b 14 24          	mov    (%rsp),%rdx
  401096:	83 e2 0f             	and    $0xf,%edx
  401099:	0f b6 92 b0 24 40 00 	movzbl 0x4024b0(%rdx),%edx
  4010a0:	88 54 04 10          	mov    %dl,0x10(%rsp,%rax,1)
  4010a4:	48 83 c0 01          	add    $0x1,%rax
  4010a8:	48 83 f8 06          	cmp    $0x6,%rax
  4010ac:	75 dd                	jne    40108b <phase_5+0x29>
  4010ae:	c6 44 24 16 00       	movb   $0x0,0x16(%rsp)
  4010b3:	be 5e 24 40 00       	mov    $0x40245e,%esi
  4010b8:	48 8d 7c 24 10       	lea    0x10(%rsp),%rdi
  4010bd:	e8 76 02 00 00       	callq  401338 <strings_not_equal>
  4010c2:	85 c0                	test   %eax,%eax
  4010c4:	74 13                	je     4010d9 <phase_5+0x77>
  4010c6:	e8 6f 03 00 00       	callq  40143a <explode_bomb>
  4010cb:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
  4010d0:	eb 07                	jmp    4010d9 <phase_5+0x77>
  
  
  4010d2:	b8 00 00 00 00       	mov    $0x0,%eax
  4010d7:	eb b2                	jmp    40108b <phase_5+0x29>
  4010d9:	48 8b 44 24 18       	mov    0x18(%rsp),%rax
  4010de:	64 48 33 04 25 28 00 	xor    %fs:0x28,%rax
  4010e5:	00 00 
  4010e7:	74 05                	je     4010ee <phase_5+0x8c>
  4010e9:	e8 42 fa ff ff       	callq  400b30 <__stack_chk_fail@plt>
  4010ee:	48 83 c4 20          	add    $0x20,%rsp
  4010f2:	5b                   	pop    %rbx
  4010f3:	c3                   	retq   
```



```assembly
40107a:	e8 9c 02 00 00       	callq  40131b <string_length>
40107f:	83 f8 06             	cmp    $0x6,%eax
401082:	74 4e                	je     4010d2 <phase_5+0x70>

# 跳转

4010d2:	b8 00 00 00 00       	mov    $0x0,%eax
4010d7:	eb b2                	jmp    40108b <phase_5+0x29>

# 跳转到400108b
```

结合函数名和对返回值的比较，输入为长度为6的字符串（不知道怎么输入的）,跳转到4010d2



```assembly
40108b:	0f b6 0c 03          	movzbl (%rbx,%rax,1),%ecx
```

任意输入一段长度为6的字符串，debug到本步

```shell
p/s (char*)$rbx
<为你输入的字符串>
```

猜测得出

```c
char* rbx = "abcdef";
int rax;
char ecx = rbx[rax];
```

```assembly
4010a4:	48 83 c0 01          	add    $0x1,%rax
4010a8:	48 83 f8 06          	cmp    $0x6,%rax
4010ac:	75 dd                	jne    40108b <phase_5+0x29>
```

可以看出在进行循环，对字符串的每个数字

```assembly
40108b:	0f b6 0c 03          	movzbl (%rbx,%rax,1),%ecx
40108f:	88 0c 24             	mov    %cl,(%rsp)
401092:	48 8b 14 24          	mov    (%rsp),%rdx
```

```shell
p/c $rdx
```

rdx内保存rbx[rax]

```assembly
401096:	83 e2 0f             	and    $0xf,%edx
401099:	0f b6 92 b0 24 40 00 	movzbl 0x4024b0(%rdx),%edx
4010a0:	88 54 04 10          	mov    %dl,0x10(%rsp,%rax,1)
```

和0xf做&运算，得到值小于等于0xf，0-15

```shell
x/s 0x4024b0
# 只保留前16位
"maduiersnfotvbyl"
```

```c
rsp[rax + 0x10] = dl;
```





```assembly
4010ae:	c6 44 24 16 00       	movb   $0x0,0x16(%rsp)
```

字符串给最后一位添加'\0'





```assembly
4010b3:	be 5e 24 40 00       	mov    $0x40245e,%esi
4010b8:	48 8d 7c 24 10       	lea    0x10(%rsp),%rdi
4010bd:	e8 76 02 00 00       	callq  401338 <strings_not_equal>
```

```shell
x/s 0x40245e
"flyers"
```



根据位运算知识，可以反推出输入的字符是多少

·（可打印范围为33-126）



c语言实现

```c
#include <stdio.h>
#include <stdlib.h>
void func5(char rbx[])
{
    char temp[] = "maduiersnfotvbyl";
    char rsp[20];
    for (int rax = 0; rax < 6; rax++)
    {
        int rdx = rbx[rax];
        rdx &= 0xf;
        rdx = temp[rdx];
        rsp[rax] = rdx;
    }
    rsp[6] = 0;
    printf("%s\n",rsp);
}
int main()
{
    char rbx[20];
    rbx[0] = 41;
    rbx[1] = 63;
    rbx[2] = 62;
    rbx[3] = 37;
    rbx[4] = 38;
    rbx[5] = 39;
    rbx[6] = 0;
    printf("%s\n", rbx);
    func5(rbx);
    return 0;
}
```



##### phase_5

（多种答案）

)?>%&'



#### 炸弹六

（一万个循环）

```assembly
00000000004010f4 <phase_6>:
  4010f4:	41 56                	push   %r14
  4010f6:	41 55                	push   %r13
  4010f8:	41 54                	push   %r12
  4010fa:	55                   	push   %rbp
  4010fb:	53                   	push   %rbx
  4010fc:	48 83 ec 50          	sub    $0x50,%rsp
  401100:	49 89 e5             	mov    %rsp,%r13
  401103:	48 89 e6             	mov    %rsp,%rsi
  401106:	e8 51 03 00 00       	callq  40145c <read_six_numbers>
  40110b:	49 89 e6             	mov    %rsp,%r14
  40110e:	41 bc 00 00 00 00    	mov    $0x0,%r12d
  401114:	4c 89 ed             	mov    %r13,%rbp
  401117:	41 8b 45 00          	mov    0x0(%r13),%eax
  40111b:	83 e8 01             	sub    $0x1,%eax
  40111e:	83 f8 05             	cmp    $0x5,%eax
  401121:	76 05                	jbe    401128 <phase_6+0x34>
  401123:	e8 12 03 00 00       	callq  40143a <explode_bomb>
  401128:	41 83 c4 01          	add    $0x1,%r12d
  40112c:	41 83 fc 06          	cmp    $0x6,%r12d
  401130:	74 21                	je     401153 <phase_6+0x5f>
  401132:	44 89 e3             	mov    %r12d,%ebx
  401135:	48 63 c3             	movslq %ebx,%rax
  401138:	8b 04 84             	mov    (%rsp,%rax,4),%eax
  40113b:	39 45 00             	cmp    %eax,0x0(%rbp)
  40113e:	75 05                	jne    401145 <phase_6+0x51>
  401140:	e8 f5 02 00 00       	callq  40143a <explode_bomb>
  401145:	83 c3 01             	add    $0x1,%ebx
  401148:	83 fb 05             	cmp    $0x5,%ebx
  40114b:	7e e8                	jle    401135 <phase_6+0x41>
  40114d:	49 83 c5 04          	add    $0x4,%r13
  401151:	eb c1                	jmp    401114 <phase_6+0x20>
  401153:	48 8d 74 24 18       	lea    0x18(%rsp),%rsi
  401158:	4c 89 f0             	mov    %r14,%rax
  40115b:	b9 07 00 00 00       	mov    $0x7,%ecx
  401160:	89 ca                	mov    %ecx,%edx
  401162:	2b 10                	sub    (%rax),%edx
  401164:	89 10                	mov    %edx,(%rax)
  401166:	48 83 c0 04          	add    $0x4,%rax
  40116a:	48 39 f0             	cmp    %rsi,%rax
  40116d:	75 f1                	jne    401160 <phase_6+0x6c>
  40116f:	be 00 00 00 00       	mov    $0x0,%esi
  401174:	eb 21                	jmp    401197 <phase_6+0xa3>
  401176:	48 8b 52 08          	mov    0x8(%rdx),%rdx
  40117a:	83 c0 01             	add    $0x1,%eax
  40117d:	39 c8                	cmp    %ecx,%eax
  40117f:	75 f5                	jne    401176 <phase_6+0x82>
  401181:	eb 05                	jmp    401188 <phase_6+0x94>
  401183:	ba d0 32 60 00       	mov    $0x6032d0,%edx
  401188:	48 89 54 74 20       	mov    %rdx,0x20(%rsp,%rsi,2)
  40118d:	48 83 c6 04          	add    $0x4,%rsi
  401191:	48 83 fe 18          	cmp    $0x18,%rsi
  401195:	74 14                	je     4011ab <phase_6+0xb7>
  401197:	8b 0c 34             	mov    (%rsp,%rsi,1),%ecx
  40119a:	83 f9 01             	cmp    $0x1,%ecx
  40119d:	7e e4                	jle    401183 <phase_6+0x8f>
  40119f:	b8 01 00 00 00       	mov    $0x1,%eax
  4011a4:	ba d0 32 60 00       	mov    $0x6032d0,%edx
  4011a9:	eb cb                	jmp    401176 <phase_6+0x82>
  4011ab:	48 8b 5c 24 20       	mov    0x20(%rsp),%rbx
  4011b0:	48 8d 44 24 28       	lea    0x28(%rsp),%rax
  4011b5:	48 8d 74 24 50       	lea    0x50(%rsp),%rsi
  4011ba:	48 89 d9             	mov    %rbx,%rcx
  4011bd:	48 8b 10             	mov    (%rax),%rdx
  4011c0:	48 89 51 08          	mov    %rdx,0x8(%rcx)
  4011c4:	48 83 c0 08          	add    $0x8,%rax
  4011c8:	48 39 f0             	cmp    %rsi,%rax
  4011cb:	74 05                	je     4011d2 <phase_6+0xde>
  4011cd:	48 89 d1             	mov    %rdx,%rcx
  4011d0:	eb eb                	jmp    4011bd <phase_6+0xc9>
  4011d2:	48 c7 42 08 00 00 00 	movq   $0x0,0x8(%rdx)
  4011d9:	00 
  4011da:	bd 05 00 00 00       	mov    $0x5,%ebp
  4011df:	48 8b 43 08          	mov    0x8(%rbx),%rax
  4011e3:	8b 00                	mov    (%rax),%eax
  4011e5:	39 03                	cmp    %eax,(%rbx)
  4011e7:	7d 05                	jge    4011ee <phase_6+0xfa>
  4011e9:	e8 4c 02 00 00       	callq  40143a <explode_bomb>
  4011ee:	48 8b 5b 08          	mov    0x8(%rbx),%rbx
  4011f2:	83 ed 01             	sub    $0x1,%ebp
  4011f5:	75 e8                	jne    4011df <phase_6+0xeb>
  4011f7:	48 83 c4 50          	add    $0x50,%rsp
  4011fb:	5b                   	pop    %rbx
  4011fc:	5d                   	pop    %rbp
  4011fd:	41 5c                	pop    %r12
  4011ff:	41 5d                	pop    %r13
  401201:	41 5e                	pop    %r14
  401203:	c3                   	retq   
```

输入为六个数字，$rsp指向数组开头

```assembly
401100: 49 89 e5                     	movq	%rsp, %r13

40110b: 49 89 e6                     	movq	%rsp, %r14
```

r13 r14同样指向数组开头



```assembly
  401117:	41 8b 45 00          	mov    0x0(%r13),%eax
  40111b:	83 e8 01             	sub    $0x1,%eax
  40111e:	83 f8 05             	cmp    $0x5,%eax
  401121:	76 05                	jbe    401128 <phase_6+0x34>
  401123:	e8 12 03 00 00       	callq  40143a <explode_bomb>
```

第一个数减1小于等于5，即小于等于6





```assembly
  401114:	4c 89 ed             	mov    %r13,%rbp
  401117:	41 8b 45 00          	mov    0x0(%r13),%eax
  40111b:	83 e8 01             	sub    $0x1,%eax
  40111e:	83 f8 05             	cmp    $0x5,%eax
  401121:	76 05                	jbe    401128 <phase_6+0x34>
  401123:	e8 12 03 00 00       	callq  40143a <explode_bomb>
  401128:	41 83 c4 01          	add    $0x1,%r12d
  40112c:	41 83 fc 06          	cmp    $0x6,%r12d
  401130:	74 21                	je     401153 <phase_6+0x5f>
  
  
  
  401132:	44 89 e3             	mov    %r12d,%ebx
  401135:	48 63 c3             	movslq %ebx,%rax
  401138:	8b 04 84             	mov    (%rsp,%rax,4),%eax
  40113b:	39 45 00             	cmp    %eax,0x0(%rbp)
  40113e:	75 05                	jne    401145 <phase_6+0x51>
  401140:	e8 f5 02 00 00       	callq  40143a <explode_bomb>
  401145:	83 c3 01             	add    $0x1,%ebx
  401148:	83 fb 05             	cmp    $0x5,%ebx
  40114b:	7e e8                	jle    401135 <phase_6+0x41>
  
  
  
  40114d:	49 83 c5 04          	add    $0x4,%r13
  401151:	eb c1                	jmp    401114 <phase_6+0x20>
```

大循环加小循环，大致意思是每个数都不等于前面的数，六个数各不相同，同时每个数都要小于等于6

最外层循环判断是否小于6，第二层循环判断时候和前面的数相同



```assembly
  401153:	48 8d 74 24 18       	lea    0x18(%rsp),%rsi
  401158:	4c 89 f0             	mov    %r14,%rax
  40115b:	b9 07 00 00 00       	mov    $0x7,%ecx
  401160:	89 ca                	mov    %ecx,%edx
  401162:	2b 10                	sub    (%rax),%edx
  401164:	89 10                	mov    %edx,(%rax)
  401166:	48 83 c0 04          	add    $0x4,%rax
  40116a:	48 39 f0             	cmp    %rsi,%rax
  40116d:	75 f1                	jne    401160 <phase_6+0x6c>
```

利用地址进行循环结束判断，rsi指向第六个数尾部

更新为每个数等于7减这个数本身



```assembly
  40116f:	be 00 00 00 00       	mov    $0x0,%esi
  401174:	eb 21                	jmp    401197 <phase_6+0xa3>
  
  
  401176:	48 8b 52 08          	mov    0x8(%rdx),%rdx  # rdx = *(rdx + 8)
  40117a:	83 c0 01             	add    $0x1,%eax       # eax += 1
  40117d:	39 c8                	cmp    %ecx,%eax		
  40117f:	75 f5                	jne    401176 <phase_6+0x82> # if(eax != ecx)
  401181:	eb 05                	jmp    401188 <phase_6+0x94>
  # 因为ecx一定大于1，rdx跳转（ecx - 1）次
  
  
  401183:	ba d0 32 60 00       	mov    $0x6032d0,%edx
  401188:	48 89 54 74 20       	mov    %rdx,0x20(%rsp,%rsi,2)
  40118d:	48 83 c6 04          	add    $0x4,%rsi
  401191:	48 83 fe 18          	cmp    $0x18,%rsi
  401195:	74 14                	je     4011ab <phase_6+0xb7>
  # 当ecx等于1时，rdx等于跳转了（1 - 1）次，于是可以统一起来看
  # 可以看到每次将rdx内的某项数据放入栈中
  # rsi每次递增为4，mov    %rdx,0x20(%rsp,%rsi,2)，可以得出每次存入的数据是8字节，大概率为地址
  # 六次循环结束后，跳出循环
  
  
  
  401197:	8b 0c 34             	mov    (%rsp,%rsi,1),%ecx  # ecx = rsp[rsi]
  40119a:	83 f9 01             	cmp    $0x1,%ecx
  40119d:	7e e4                	jle    401183 <phase_6+0x8f> # if ecx <= 1
  40119f:	b8 01 00 00 00       	mov    $0x1,%eax             # eax = 1
  4011a4:	ba d0 32 60 00       	mov    $0x6032d0,%edx        # edx = 0x6032d0
  4011a9:	eb cb                	jmp    401176 <phase_6+0x82>
```

可以看到出现的0x6032d0，不知道是什么，但是指向某项我们需要的数据，前rdx出现过跳转，具有链表的性质

我们知道将某项数据插入到了栈内



```assembly
x $rsp + 0x20 + <跳转次数> * 0x8
# 能够发现每次跳转后的地址跟之前相差0x10，因此数据都是贴在一起的，可以直接打印出来观察


x/24xw 0x6032d0
0x6032d0 <node1>:       0x0000014c      0x00000001      0x006032e0      0x00000000
0x6032e0 <node2>:       0x000000a8      0x00000002      0x006032f0      0x00000000
0x6032f0 <node3>:       0x0000039c      0x00000003      0x00603300      0x00000000
0x603300 <node4>:       0x000002b3      0x00000004      0x00603310      0x00000000
0x603310 <node5>:       0x000001dd      0x00000005      0x00603320      0x00000000
0x603320 <node6>:       0x000001bb      0x00000006      0x00000000      0x00000000
```

使用map模拟内存的情况

```c++
#include <map>
typedef long long address;
struct Node
{
    int para1;
    int para2;
    address next;
};

std::map<address, Node> nodes;

address root = 0x6032d0;
nodes[0x6032d0] = {0x14c, 0x1, 0x6032e0};
nodes[0x6032e0] = {0xa8, 0x2, 0x6032f0};
nodes[0x6032f0] = {0x39c, 0x3, 0x603300};
nodes[0x603300] = {0x2b3, 0x4, 0x603310};
nodes[0x603310] = {0x1dd, 0x5, 0x603320};
nodes[0x603320] = {0x1bb, 0x6, 0x0};
```





```assembly
  4011ab:	48 8b 5c 24 20       	mov    0x20(%rsp),%rbx  # rbx = addresses[0]
  4011b0:	48 8d 44 24 28       	lea    0x28(%rsp),%rax  # rax = &addresses[1]
  4011b5:	48 8d 74 24 50       	lea    0x50(%rsp),%rsi	# rsi = &addresses[6]
  4011ba:	48 89 d9             	mov    %rbx,%rcx		# rcx = rbx
  
  
  4011bd:	48 8b 10             	mov    (%rax),%rdx     # rdx = (address)*rax
  4011c0:	48 89 51 08          	mov    %rdx,0x8(%rcx)	# (Node)rcx.next = rdx
  4011c4:	48 83 c0 08          	add    $0x8,%rax		# rax 指向下一个address
  4011c8:	48 39 f0             	cmp    %rsi,%rax		# 判断是否结束循环
  4011cb:	74 05                	je     4011d2 <phase_6+0xde>
  4011cd:	48 89 d1             	mov    %rdx,%rcx		# rcx = rdx
  4011d0:	eb eb                	jmp    4011bd <phase_6+0xc9>
  # 此循环从新连接链表
  # 顺序为栈内的顺序
  
  
  
  4011d2:	48 c7 42 08 00 00 00 	movq   $0x0,0x8(%rdx) # 最后一个结点的next指针置零
  4011da:	bd 05 00 00 00       	mov    $0x5,%ebp	# ebp = 5
  
  
  4011df:	48 8b 43 08          	mov    0x8(%rbx),%rax # rax = *rbx.next
  4011e3:	8b 00                	mov    (%rax),%eax		# eax = *rax;
  4011e5:	39 03                	cmp    %eax,(%rbx)		# 比较node的第一个8字节的值
  4011e7:	7d 05                	jge    4011ee <phase_6+0xfa>
  4011e9:	e8 4c 02 00 00       	callq  40143a <explode_bomb>
  
  4011ee:	48 8b 5b 08          	mov    0x8(%rbx),%rbx
  4011f2:	83 ed 01             	sub    $0x1,%ebp
  4011f5:	75 e8                	jne    4011df <phase_6+0xeb>
  # 循环判断 要求 node内的变量 降序
  # 按照序号排序为 3 4 5 6 1 2
  # 输入的值需要翻转 4 3 2 1 6 5
```







c++模拟，没有完全按照寄存器的逻辑来写

```c++
#include <cstdio>
#include <cstdlib>
#include <map>
#define debug(node) printf("para1: %x ,para2: %x ,next: %llx\n", node.para1, node.para2, node.next)
typedef long long address;
struct Node
{
    int para1;
    int para2;
    address next;
};
std::map<address, Node> nodes;
void bomb()
{
    printf("bomb!!\n");
    exit(0);
}
int main()
{
    address root = 0x6032d0;
    nodes[0x6032d0] = {0x14c, 0x1, 0x6032e0};
    nodes[0x6032e0] = {0xa8, 0x2, 0x6032f0};
    nodes[0x6032f0] = {0x39c, 0x3, 0x603300};
    nodes[0x603300] = {0x2b3, 0x4, 0x603310};
    nodes[0x603310] = {0x1dd, 0x5, 0x603320};
    nodes[0x603320] = {0x1bb, 0x6, 0x0};

    int nums[] = {4, 3, 2, 1, 6, 5};

    // 第一个大循环套小循环
    for (int i = 0; i < 6; i++)
    {
        if (nums[i] > 6)
            bomb();
        for (int j = i + 1; j < 6; j++)
            if (nums[i] == nums[j])
                bomb();
    }

    // 第二个循环
    for (int i = 0; i < 6; i++)
        nums[i] = 7 - nums[i];

    //第三个循环，将node入栈

    address addresses[6];
    int idx = 0;
    for (int i = 0; i < 6; i++)
    {
        int eax = 1;
        int ecx = nums[i];
        address edx = root;
        while (eax < ecx)
        {
            edx = nodes[edx].next;
            eax++;
        }
        addresses[idx++] = edx;
    }

    // 第四个循环，从新连接链表
    address newRoot = addresses[0];
    for (int rax = 1; rax < idx; rax++)
    {
        address rdx = addresses[rax];
        Node &rcx = nodes[newRoot];
        rcx.next = rdx;
        newRoot = rdx;
    }
    nodes[newRoot].next = 0;

    // 检查
    address debugRoot = addresses[0];
    debug(nodes[root]);
    for (int i = 1; i < 6; i++)
    {
        debugRoot = nodes[debugRoot].next;
        debug(nodes[debugRoot]);
    }

    // 第五个模拟，检查是否降序
    address rbx = addresses[0];
    for (int ebp = 5; ebp >= 0; ebp--)
    {
        Node rax = nodes[nodes[rbx].next];
        if (rax.para1 >= nodes[rbx].para1)
            bomb();
        else
        {
            rbx = nodes[rbx].next;
        }
    }

    // 完成
    printf("well done!\n");

    return 0;
}
```



##### phase_6

4 3 2 1 6 5

#### 彩蛋

```assembly
00000000004015c4 <phase_defused>:
  4015c4:	48 83 ec 78          	sub    $0x78,%rsp
  4015c8:	64 48 8b 04 25 28 00 	mov    %fs:0x28,%rax
  4015cf:	00 00 
  4015d1:	48 89 44 24 68       	mov    %rax,0x68(%rsp)
  4015d6:	31 c0                	xor    %eax,%eax
  4015d8:	83 3d 81 21 20 00 06 	cmpl   $0x6,0x202181(%rip)        # 603760 <num_input_strings>
  4015df:	75 5e                	jne    40163f <phase_defused+0x7b>
  4015e1:	4c 8d 44 24 10       	lea    0x10(%rsp),%r8
  4015e6:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx
  4015eb:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx
  4015f0:	be 19 26 40 00       	mov    $0x402619,%esi
  4015f5:	bf 70 38 60 00       	mov    $0x603870,%edi
  4015fa:	e8 f1 f5 ff ff       	callq  400bf0 <__isoc99_sscanf@plt>
  4015ff:	83 f8 03             	cmp    $0x3,%eax
  401602:	75 31                	jne    401635 <phase_defused+0x71>
  401604:	be 22 26 40 00       	mov    $0x402622,%esi
  401609:	48 8d 7c 24 10       	lea    0x10(%rsp),%rdi
  40160e:	e8 25 fd ff ff       	callq  401338 <strings_not_equal>
  401613:	85 c0                	test   %eax,%eax
  401615:	75 1e                	jne    401635 <phase_defused+0x71>
  401617:	bf f8 24 40 00       	mov    $0x4024f8,%edi
  40161c:	e8 ef f4 ff ff       	callq  400b10 <puts@plt>
  401621:	bf 20 25 40 00       	mov    $0x402520,%edi
  401626:	e8 e5 f4 ff ff       	callq  400b10 <puts@plt>
  40162b:	b8 00 00 00 00       	mov    $0x0,%eax
  401630:	e8 0d fc ff ff       	callq  401242 <secret_phase>
  401635:	bf 58 25 40 00       	mov    $0x402558,%edi
  40163a:	e8 d1 f4 ff ff       	callq  400b10 <puts@plt>
  40163f:	48 8b 44 24 68       	mov    0x68(%rsp),%rax
  401644:	64 48 33 04 25 28 00 	xor    %fs:0x28,%rax
  40164b:	00 00 
  40164d:	74 05                	je     401654 <phase_defused+0x90>
  40164f:	e8 dc f4 ff ff       	callq  400b30 <__stack_chk_fail@plt>
  401654:	48 83 c4 78          	add    $0x78,%rsp
  401658:	c3                   	retq  
```

```assembly
0000000000401242 <secret_phase>:
  401242:	53                   	push   %rbx
  401243:	e8 56 02 00 00       	callq  40149e <read_line>
  401248:	ba 0a 00 00 00       	mov    $0xa,%edx
  40124d:	be 00 00 00 00       	mov    $0x0,%esi
  401252:	48 89 c7             	mov    %rax,%rdi
  401255:	e8 76 f9 ff ff       	callq  400bd0 <strtol@plt>
  40125a:	48 89 c3             	mov    %rax,%rbx
  40125d:	8d 40 ff             	lea    -0x1(%rax),%eax
  401260:	3d e8 03 00 00       	cmp    $0x3e8,%eax
  401265:	76 05                	jbe    40126c <secret_phase+0x2a>
  401267:	e8 ce 01 00 00       	callq  40143a <explode_bomb>
  40126c:	89 de                	mov    %ebx,%esi
  40126e:	bf f0 30 60 00       	mov    $0x6030f0,%edi
  401273:	e8 8c ff ff ff       	callq  401204 <fun7>
  401278:	83 f8 02             	cmp    $0x2,%eax
  40127b:	74 05                	je     401282 <secret_phase+0x40>
  40127d:	e8 b8 01 00 00       	callq  40143a <explode_bomb>
  401282:	bf 38 24 40 00       	mov    $0x402438,%edi
  401287:	e8 84 f8 ff ff       	callq  400b10 <puts@plt>
  40128c:	e8 33 03 00 00       	callq  4015c4 <phase_defused>
  401291:	5b                   	pop    %rbx
  401292:	c3                   	retq   
```





##### secret_phase
