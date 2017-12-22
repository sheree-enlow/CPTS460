#include "ucode.c"

int main(int argc, char *argv[])
{
  int fd = 0, term_out, n, i = 0, count = 0;
  int lines = 20; // 20 lines at a time to account for small terminal

  char c, input, line[1024];

  //from page 367 KC's MTX book*****************
  //this is to determine if stdin/stdout has been redirected or not

  int lr = 0, rr = 0;
  char port[64];
  STAT st_tty, st0, st1;

  gettty(port);
  stat(port, &st_tty);
  fstat(0, &st0);
  fstat(1, &st1);

  if(st_tty.st_dev != st0.st_dev || st_tty.st_ino != st0.st_ino) {lr = 1;} //stdin is redirected
  if(st_tty.st_dev != st1.st_dev || st_tty.st_ino != st1.st_ino) {rr = 1;} //stdout is redirected
  
  //****************

  //open file which should be 2nd arguement
  if(argv[1])
    fd = open(argv[1], O_RDONLY);
    
  if(!rr)
    term_out = open(port, O_RDONLY);
    

  // read a few lines and display them
  while(n = read(fd,  &c,1))
  {

    line[i++] = c;
    //count is used to count up the total amount of lines for terminal
    //doesnt matter if its just being redirected somewhere else.

    if(c == '\n') //line
    {
      //we're just redirecting, nothing special here
      if(rr)
      {
        line[i] = 0; //null terminate
        write(1, line, i-1);
        write(1, "\n", 1);
      }
      //no redirection
      else
      {
        if(count > lines)// pause if we've reached the limit on lines (20)
        {
          read(term_out, &input, 1);// freeze
          
          //scroll through entire page
          if(input == ' ')
          {
            count = 0; //if count is zero we will write 20 lines
          }
          if(input == 'q')// exit when q is hit
          {
            exit(0);
          }
          
          // hit any key other than space or q for one line at a time

        }//now we write the line since were still processing that
        write(1, line, i-1);
        write(1, "\r\n", 2); //\r\n for terminal
        count++; 
      }
      i=0; // reset our line variable/input accumulator since another line will be encountered
    }

  }
  close(fd);
  close(term_out);
}