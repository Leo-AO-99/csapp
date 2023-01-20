# shellLab

***先阅读preview/shlab.pdf 和 README***

实验涉及到的信号量的一些细节（书本上有更多实际细节）

- fork时，子进程会继承父进程的signal mask

- 注意异步信号安全

- jobs为全局变量，多进程存在并发问题，进行操作时应该进行上锁等操作

- 使用man命令行，查看各个函数的使用细节

```shell
man waitpid
```

- 认真阅读tsh.c的注释

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

---

###### test01

```shell
#
# trace01.txt - Properly terminate on EOF.
#
```

直接通过

---

###### test02

```shell
#
# trace02.txt - Process builtin quit command.
#
quit
```

实现内置quit命令

```c
void eval(char *cmdline) 
{
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */

    strcpy(buf, cmdline);
    bg = parseline(buf, argv);
    if (argv[0] == NULL)
        return;  /* Ignore empty lines*/
    
    if (!builtin_cmd(argv)) {
        if ((pid = fork()) == 0) {
            if (execve(argv[0], argv, environ) < 0) {
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
        }

        /* Parent waits for foreground job to terminate */
        if (!bg) {
            int status;
            if (waitpid(pid, &status, 0) < 0)
                unix_error("waitfg: waitpid error");
        }
        else
            printf("%d %s", pid, cmdline);
    }
    return;
}
```

```c
/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.  
 */
int builtin_cmd(char **argv) 
{
    if (!strcmp(argv[0], "quit"))      /* quit command */
        exit(0);

    if (!strcmp(argv[0], "jobs")) {    /* jobs command list all background jobs */
        listjobs(jobs);
    }
    else if(!strcmp(argv[0], "bg")) {  /* bg <job> command change a stopped background job to a running background job */
        do_bgfg(argv);
    }
    else if(!strcmp(argv[0], "fg")) {  /* fg <job> command change a stopped or running background job to a running in the foreground */
        do_bgfg(argv);
    }
    else if(!strcmp(argv[0], "kill")) { /* kill <job> terminate a job */
        
    }
    else if(!strcmp(argv[0], "&")) {   /* ignore */
        
    }
    else
        return 0;                       /* not a builtin command */

    return 1;
}
```

---

###### test03

```shell
#
# trace03.txt - Run a foreground job.
#
/bin/echo tsh> quit
quit
```

测试/bin/echo，实现fg

eval内调用execve实现

---

###### test04

```shell
#
# trace04.txt - Run a background job.
#
/bin/echo -e tsh> ./myspin 1 \046
./myspin 1 &
```

实现bg

```shell
tldr echo
 - Enable interpretation of backslash escapes (special characters):
   echo -e "{{Column 1\tColumn 2}}"
```

-e可是输出转义字符

myspin.c	# Takes argument <n> and spins for <n> seconds

```c
// 修改打印格式，要求打印job id
printf("[%d] (%d) %s", pid2jid(pid), pid, cmdline);
```

---

###### test05

```shell
#
# trace05.txt - Process jobs builtin command.
#
/bin/echo -e tsh> ./myspin 2 \046
./myspin 2 &

/bin/echo -e tsh> ./myspin 3 \046
./myspin 3 &

/bin/echo tsh> jobs
jobs
```

编写builtin_cmd 实现 jobs，调用提供的listjobs

打印全局变量jobs的内容

要在eval中实现对jobs操作

运行前台任务是，shell应该等待进程，实现waitfg


```c
/* 
 * eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.  
*/
void eval(char *cmdline) 
{
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */
    sigset_t mask;

    strcpy(buf, cmdline);
    bg = parseline(buf, argv);
    if (argv[0] == NULL)
        return;  /* Ignore empty lines*/


    if(!builtin_cmd(argv)) {
        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);
        sigprocmask(SIG_BLOCK, &mask, NULL); // block SIGCHLD

        if((pid = fork()) < 0) {
            unix_error("forking error");
        }
        else if(pid == 0) {
            // child process
            sigprocmask(SIG_UNBLOCK, &mask, NULL); // unblock SIGCHLD
            setpgid(0, 0);
            /*
            setpgid(pid, pgid)
            setpgid() sets the PGID of the process specified by pid to pgid.  If pid is zero, then the process ID of the calling process is used.
            If pgid is zero, then the PGID of the process specified by pid is made the same as its process ID.
            */


            if(execvp(argv[0], argv) < 0) {
                // check command validation
                printf("%s: Command not found\n", argv[0]);
                exit(0);
            }
        }
        else {
            // parent process
            if(!bg)
                addjob(jobs, pid, FG, cmdline);
            else
                addjob(jobs, pid, BG, cmdline);
            
            sigprocmask(SIG_UNBLOCK, &mask, NULL); // unblock SIGCHLD

            if(!bg)
                waitfg(pid);
            else
                printf("[%d] (%d) %s", pid2jid(pid), pid, cmdline);
        }
    }
    return;
}

/* 
 * waitfg - Block until process pid is no longer the foreground process
 */
void waitfg(pid_t pid)
{
    struct job_t* job = getjobpid(jobs, pid);
    if(job == NULL)
        return;
    while(fgpid(jobs) == pid) {
        // fg pid equal pid , then block
    }
    return;
}
```

对SIGCHLD的block，相当于mutex，因为要修改全局变量jobs，而父进程与子进程存在并发访问的问题

而对mutex的解锁，（只考虑fg即可）要放在执行wait之前，否则进行等待时，ctrl+c结束子进程时，父进程无法收到，继续等待

setgpid(0, 0) 将子进程的进程组修改成子进程的进程号，即是子进程单独一个进程组。通过shell创建的前台进程，默认是和shell一个进程组。假设没有修改进程组，kill前台进程时，会一起kill掉shell，这是不合理的

---

###### test06

```shell
#
# trace06.txt - Forward SIGINT to foreground job.
#
/bin/echo -e tsh> ./myspin 4
./myspin 4 

SLEEP 2
INT
```

实现sigint_handler，sigchld_handler

sigchld_handler 当父进程（shell）收到子进程的信号，应该如何打印子进程状态信息

```c
/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */
void sigint_handler(int sig) 
{
    pid_t pid = fgpid(jobs);
    if(pid == 0)
        return;

    // printf("pid: %d\n", pid);
    
    kill(-pid, sig);
    return;
}

/* 
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.  
 */
void sigchld_handler(int sig) 
{
    int old_errno = errno;
    pid_t pid;
    int status;
    
    while((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        /*
        pid = -1 meaning wait for any child process.
        WNOHANG     return immediately if no child has exited.
        WUNTRACED   also return if a child has stopped (but not traced via ptrace(2)).  Status for traced children which have stopped is provided even if this option is not specified.
        */
        if(WIFEXITED(status)) {
            // exit normally
            deletejob(jobs, pid);
        }
        else if(WIFSIGNALED(status)) {
            // terminated by signal
            int jid = pid2jid(pid);
            printf("Job [%d] (%d) terminated by signal %d\n", jid, pid, WTERMSIG(status));
            deletejob(jobs, pid);
        }
        else if(WIFSTOPPED(status)) {
            struct job_t* job = getjobpid(jobs, pid);
            job->state = ST;  // change job state to STOP

            int jid = pid2jid(pid);
            printf("Job [%d] (%d) Stopped by signal %d\n", jid, pid, WTERMSIG(status));
        }
    }

    errno = old_errno;
    return;
}
```

---

###### test07

```shell
#
# trace07.txt - Forward SIGINT only to foreground job.
#
/bin/echo -e tsh> ./myspin 4 \046
./myspin 4 &

/bin/echo -e tsh> ./myspin 5
./myspin 5 

SLEEP 2
INT

/bin/echo tsh> jobs
jobs
```

INT应该只中断前台进程

直接完成

---

###### test08

```shell
#
# trace08.txt - Forward SIGTSTP only to foreground job.
#
/bin/echo -e tsh> ./myspin 4 \046
./myspin 4 &

/bin/echo -e tsh> ./myspin 5
./myspin 5 

SLEEP 2
TSTP

/bin/echo tsh> jobs
jobs
```

ctrl+z 停止前台进程

实现sisstp_handler

```c
/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */
void sigtstp_handler(int sig) 
{
    pid_t pid = fgpid(jobs);
    if(pid == 0)
        return;
    
    kill(-pid, sig);
    return;
}
```

---

###### test09

```shell
#
# trace09.txt - Process bg builtin command
#
/bin/echo -e tsh> ./myspin 4 \046
./myspin 4 &

/bin/echo -e tsh> ./myspin 5
./myspin 5 

SLEEP 2
TSTP

/bin/echo tsh> jobs
jobs

/bin/echo tsh> bg %2
bg %2

/bin/echo tsh> jobs
jobs
```

将stopped状态的job，继续在后台执行

实现do_bgfg

```c
/* 
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv) 
{
    struct job_t *job;
    char *tmp;
    int jid;
    pid_t pid;

    tmp = argv[1];

    if(tmp == NULL) {
        printf("%s command requires PID or %%<jobid> argument\n", argv[0]);
        return;
    }

    if(tmp[0] == '%') { // if it is a jid
        jid = atoi(&tmp[1]);
        job = getjobjid(jobs, jid);
        if(job == NULL) {
            printf("(%d): No such job\n", jid);
            return;
        }
        pid = job->pid;
    }
    else if(isdigit(tmp[0])) // if it is a pid
    {
        pid = atoi(&tmp[0]);
        job = getjobpid(jobs, pid);
        if(job == NULL) {
            printf("(%d): No such process\n", pid);
            return;
        }
    }
    else {
        printf("%s: argument must be a PID or %%jobid\n", argv[0]);
        return;
    }

    kill(-pid, SIGCONT);

    if(!strcmp(argv[0], "fg")) {
        job->state = FG;
        waitfg(job->pid); // block for fg job
    }
    else
    {
        printf("[%d] (%d) %s", job->jid, job->pid, job->cmdline);
        job->state = BG;
    }
    return;
}
```

---

###### test10

```shell
#
# trace10.txt - Process fg builtin command. 
#
/bin/echo -e tsh> ./myspin 4 \046
./myspin 4 &

SLEEP 1
/bin/echo tsh> fg %1
fg %1

SLEEP 1
TSTP

/bin/echo tsh> jobs
jobs

/bin/echo tsh> fg %1
fg %1

/bin/echo tsh> jobs
jobs
```

fg command，将stopped的job在前台继续执行

---

###### test11

```shell
#
# trace11.txt - Forward SIGINT to every process in foreground process group
#
/bin/echo -e tsh> ./mysplit 4
./mysplit 4 

SLEEP 2
INT

/bin/echo tsh> /bin/ps a
/bin/ps a
```

查看ctrl+c是否结束进程

---

###### test12

```shell
#
# trace12.txt - Forward SIGTSTP to every process in foreground process group
#
/bin/echo -e tsh> ./mysplit 4
./mysplit 4 

SLEEP 2
TSTP

/bin/echo tsh> jobs
jobs

/bin/echo tsh> /bin/ps a
/bin/ps a
```

查看ctrl+z是暂停任务

---

###### test13

```shell
#
# trace13.txt - Restart every stopped process in process group
#
/bin/echo -e tsh> ./mysplit 4
./mysplit 4 

SLEEP 2
TSTP

/bin/echo tsh> jobs
jobs

/bin/echo tsh> /bin/ps a
/bin/ps a

/bin/echo tsh> fg %1
fg %1

/bin/echo tsh> /bin/ps a
/bin/ps a
```

先stop再恢复到前台继续执行，并查看后台状态

---

###### test14

```shell
#
# trace14.txt - Simple error handling
#
/bin/echo tsh> ./bogus
./bogus

/bin/echo -e tsh> ./myspin 4 \046
./myspin 4 &

/bin/echo tsh> fg
fg

/bin/echo tsh> bg
bg

/bin/echo tsh> fg a
fg a

/bin/echo tsh> bg a
bg a

/bin/echo tsh> fg 9999999
fg 9999999

/bin/echo tsh> bg 9999999
bg 9999999

/bin/echo tsh> fg %2
fg %2

/bin/echo tsh> fg %1
fg %1

SLEEP 2
TSTP

/bin/echo tsh> bg %2
bg %2

/bin/echo tsh> bg %1
bg %1

/bin/echo tsh> jobs
jobs
```

强壮性测试

---

###### test15

```shell
#
# trace15.txt - Putting it all together
#

/bin/echo tsh> ./bogus
./bogus

/bin/echo tsh> ./myspin 10
./myspin 10

SLEEP 2
INT

/bin/echo -e tsh> ./myspin 3 \046
./myspin 3 &

/bin/echo -e tsh> ./myspin 4 \046
./myspin 4 &

/bin/echo tsh> jobs
jobs

/bin/echo tsh> fg %1
fg %1

SLEEP 2
TSTP

/bin/echo tsh> jobs
jobs

/bin/echo tsh> bg %3
bg %3

/bin/echo tsh> bg %1
bg %1

/bin/echo tsh> jobs
jobs

/bin/echo tsh> fg %1
fg %1

/bin/echo tsh> quit
quit
```

缝合怪！！！

---

###### test16

```shell
#
# trace16.txt - Tests whether the shell can handle SIGTSTP and SIGINT
#     signals that come from other processes instead of the terminal.
#

/bin/echo tsh> ./mystop 2 
./mystop 2

SLEEP 3

/bin/echo tsh> jobs
jobs

/bin/echo tsh> ./myint 2 
./myint 2
```

myint发送INT信号