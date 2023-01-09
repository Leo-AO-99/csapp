# shellLab

##### 编译

```shell
make // 编译
make clean // 清除二进制可执行文件
```



##### 实验目的

编写tsh.c，实现tiny shell

实现七个函数

```c
// 解释shell内输入的指令
void eval(char *cmdline);

// 判断执行内置命令
int builtin_cmd(char **argv);

// 实现bg操作和fg操作，命令行是否以 & 结尾
void do_bgfg(char **argv);

// 等待fg进程结束
void waitfg(pid_t pid);

// 处理sigchild信号
void sigchild_handler(int sig);

// 处理sigstp信号
void sigtstp_handler(int sig);

// 处理sigint操作
void sigint_handler(int sig);
```

tsh要支持的内置命令

**–** The quit command terminates the shell.

**–** The jobs command lists all background jobs.

**–** The bg <job> command restarts <job> by sending it a SIGCONT signal, and then runs it in the background. The <job> argument can be either a PID or a JID.

**–** The fg <job> command restarts <job> by sending it a SIGCONT signal, and then runs it in the foreground. The <job> argument can be either a PID or a JID.



##### 实验结果校验

tracexx.txt为测试文件

make testxx 检验自己编写的结果

make rtestxx 正确执行的结果

###### test01

```shell
#
# trace01.txt - Properly terminate on EOF.
#
```

直接通过

###### test02

```shell
#
# trace02.txt - Process builtin quit command.
#
quit
```

实现内置quit命令

```c
int builtin_cmd(char **argv)
{
	if(!strcmp(argv[0], "quit"))
		exit(0);
	return 0;
}
```

###### test03

```shell
#
# trace03.txt - Run a foreground job.
#
/bin/echo tsh> quit
quit
```

测试/bin/echo，实现fg

```c
void eval(char *cmdline)
{
    char *argv[MAXARGS];
    char buf[NAXLINE];
    strcpy(buf, cmdline);
    parseline(buf, argv);
    
    if(argv[0] == NULL)
        exit(0);
    
    pid_t pid;
    
    if(!builtin_cmd(argv))
    {
        if((pid = fork()) == 0)
        {
            if(execve(argv[0], argv, environ) < 0)
            {
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
        }
    }
}
```



###### test04

```shell
#
# trace04.txt - Run a background job.
#
/bin/echo -e tsh> ./myspin 1 \046
./myspin 1 &
```

实现bg

