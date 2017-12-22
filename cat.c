#include "ucode.c"

int main(int argc, char *argv[])
{
	int fd = 0, n = 0, lr = 0, rr = 0, i = 0;

	char buf[1024], c;
	char port[64];

	STAT st_tty, st0, st1;

  //from page 367 KC's MTX book****
  //this is to determine if stdin/stdout has been redirected or not

	gettty(port);
	stat(port, &st_tty);

	fstat(0, &st0); 
	fstat(1, &st1);
	
	
	if (st_tty.st_dev != st0.st_dev || st_tty.st_ino != st0.st_ino) { lr = 1; } //left redirection
	if (st_tty.st_dev != st1.st_dev || st_tty.st_ino != st1.st_ino) { rr = 1; } //right redirection

  //******


	if (argv[1]) { fd = open(argv[1], O_RDONLY); }
	
	if (fd < 0) { return; }

/*
if fd = 0

  (1) if no left redirection

    (edge cases)
    cat       write char to fd2, on enter write to fd1 
    cat > f   write char to fd2, on enter write to fd1 
    cat |     write char to fd2, on enter write to fd1 

  (2) has left redirection

    (2.1) just left redirection

      | cat   write to fd1 & fd2 on newline
      cat < f1  write to fd1 & fd2 on newline

    (2.2) both right and left

      | cat |   write to fd1

if fd = n

  just file 
  
*/

	//CASE FD == 0
	if (!fd)
	{
    //get inputs from stdin, fd = 0
		while (n = read(fd, &c, 1))
		{
			if (c == '\r') { c = '\n'; }
			//if no left redirection
			if (!lr)
			{
        		//printf("pipes should be working!\n");
        		//getc();
				//(store data for later for fd1)
				buf[i++] = c;

		        //if stdout has been redirected, ex "cat filename > outfile",
		        //it must not add \r to each line so display the \r to fd = 2
				//if (c == '\n') 
        		//  write(2, "\r", 1); 

				//write a char to fd2
				write(2, &c, 1);
        		write(1, &c, 1);
				//on newline, write data
				if (c == '\n')
				{
					//write(1, buf, i - 1);
					write(2, "\r", 1);
					//write(1, "\n", 1);
					i = 0;
				}
			}
			else //lr
			{
        		//print \r for each \n for right visual effects
       			//because files end with just \n in UNIX
				if (c == '\n' && !rr) { write(2, "\r", 1); } 
				write(1, &c, 1);
			}
		}
	}
	else //file
	{
    	//print \r for each \n for right visual effects
		while (n = read(fd, &c, 1))
		{
			if (c == '\n' && !rr) { write(2, "\r", 1); } 
			write(1, &c, 1);
		}
	}
	close(fd);
}