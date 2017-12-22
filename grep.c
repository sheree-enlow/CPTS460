#include "ucode.c"

int main(int argc, char *argv[])
{
  int fd = 0, i = 0, n; //by default stdin
  char c, line[1024], *p;


  //from page 367 KC's MTX book*****************
  //this is to determine if stdin/stdout has been redirected or not

  int rr = 0;
  
  char port[64];
  STAT st_tty, st0, st1;

  gettty(port);
  stat(port, &st_tty);
  fstat(0, &st0);
  fstat(1, &st1);

  //right redirection
  if(st_tty.st_dev != st1.st_dev || st_tty.st_ino != st1.st_ino) {rr = 1;} //stdout is redirected

  //***************

  //argv[2] since 2nd arguement is pattern (argv[1])
  if (argv[2])
    fd = open(argv[2], O_RDONLY);

  // if we dont have anything to open
  if(fd < 0){exit(0);}

  // read each line and scan for pattern and write to stdout if ==true
  //(1) writing to a file so include \n but not \r
  //(2) writing to terminal so \r\n for each new line.
  while(n = read(fd,&c,1))
  {
    line[i++] = c;
    if(c == '\n')
    {

      line[i] = 0;//null terminate

      //if pattern has been encountered
      if(strstr(line,argv[1]))
      {
        //(1)
      	if(rr)
      	{

      		write(1,line,i-1);
          write(1,"\n",1);
      	}

        //(2)
      	else
      	{
      			write(1,line,i-1);
      			write(1,"\r\n",2);
      			
      	}
      }
      i = 0;
    }
  }

  close(fd);
}