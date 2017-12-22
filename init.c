// 3. Operation of the Wanix system:
//    The sdimage is an EXT2 FS containing

//             /---|
//                 |----bin/ : All binary executables are in /bin
//                 |
//                 |---dev/ : special files tty0  c 4 0 
//                 |                        ttyS0 c 5 0 
//                 |                        ttyS1 c 5 1   
//                 |                        lp0   c 6 0
//                 |---etc/ : passwd file 
//                 |           
//                 |---boot/: bootable Wanix kernels
//                 |
//                 |---user/: users HOME directories

  // After mounting the root file system, P0 creats P1, whose Umode image is 
  // the /bin/init program. P1 will go Umode directly to execute /bin/init, 
  // in which it forks children P2 on /dev/tty0. P1 may also fork P3 /dev/ttyS0 
  // and P4 on /dev/ttyS1, etc. Then P1 waits for any child to die. Henceforth, P1
  // plays the role of the INIT process (P1) in Unix.

#include "ucode.c"
int console;
int s0, s1;

int parent()
// P1's code
{
    int pid, status;
    while(1){
        printf("INIT : wait for ZOMBIE child\n");
        pid = wait(&status);
        if (pid==console){ // if console login process tty0 died, reopen it
        
            printf("INIT: forks a new console login\n");
            console = fork(); // fork another one
            if (console)
                continue;
            else
                exec("login /dev/tty0"); // new console login process
        }
        else if(pid == s0) //if ttyS0 died fork another
        {
            s0 = fork();
            if(s0)
                continue;
            else
                exec("login /dev/ttyS0");
        }
        else if(pid == s1) //if ttyS1 died fork another
        {
            s1 = fork();
            if(s1)
                continue;
            else
                exec("login /dev/ttyS1");
        }
        printf("INIT: I just buried an orphan child proc %d\n", pid);
    }
}

main(int argc, char *argv[])
{
    int in, out; // file descriptors for terminal I/O


    in = open("/dev/tty0", O_RDONLY); // file descriptor 0
    out = open("/dev/tty0", O_WRONLY); // for display to console
    
    printf("INIT : fork a login proc on console\n");
    
    console = fork();
    
    if (console) //parent of fork
    {
        s0 = fork();
        if(s0) //then fork s1
        {
            s1 = fork();
            if(s1)
                parent(); //s0 will keep running parent P1
            else
                //printf("\nChild of init: S1 starting login: %d\n", in);
                exec("login /dev/ttyS1");
        }
        else
            //printf("\nChild of init: S0 starting login: %d\n", in);
            exec("login /dev/ttyS0");
    }
    else // child: exec to login on tty0
        //printf("\nConsole starting login: %d\n", in);
        exec("login /dev/tty0");

}