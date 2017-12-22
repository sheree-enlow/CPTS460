//sh.c
//*** all outlined in KC's book pg 365 "Design and Implementation of MTX"

//example
//cmdLine = "cat < infile | lower2upper | grep print > outfile"
//starting from main sh process P2, sh forks a child P3, waits for termination
//process: infile --> P5(cat) ==> P4 (lower2upper) ==> P3 (grep --> outfile)



#include "ucode.c"

int tokenize(char *cmdLine, char *params[]);
int do_pipe(char *cmdLine, int *pd);
int do_command(char *cmdLine);
int scan(char *cmdLine, char **head, char **tail);

#define NULL '\0'


char* _buffer;

char* strtok(char *str, const char* delim) {
    
    if(str != NULL) _buffer = str;
    if(_buffer[0] == '\0') return NULL;
 
    char *ret = _buffer, *b;
    const char *d;
 
    for(b = _buffer; *b !='\0'; b++) {
        for(d = delim; *d != '\0'; d++) {
            if(*b == *d) {
                *b = '\0';
                _buffer = b+1;
 
                // skip the beginning delimiters
                if(b == ret) { 
                    ret++; 
                    continue; 
                }
                return ret;
            }
        }
    }
 
    return ret;
}


// read in cmd_line and tokenize into args
char cmd_line[64], buf[64], *args[64];
int pid, status;
int ln;

int main(int argc, char *argv[])
{
	char *filler1, *filler2;

	while(1)
	{

		memset(cmd_line, 0, sizeof(cmd_line)); //zero out
		printf("sheree$ ");
		gets(cmd_line); //watch out for '\n'

		strcpy(buf, cmd_line);

		if(buf == "\n" || buf == "\r\n") //input is nothing
			continue;

		if(buf != 0){
			//getting rid of annoying '\n' from gets()
			//might as well see if we're logging out

		    ln = strlen(buf) - 1;
        if (*buf && buf[ln] == '\n') 
		    	buf[ln] = '\0';
		    if(!strcmp(buf, "logout")){
		    	exit(0);
		    }
		    if(!strcmp(buf, "cd")) //cd by itself, go homeDIR
		    {
		    	printf("going to home directory");
		    	chdir(argv[1]);
		    	continue;
		    }
		}


		printf("command line: %s\n", buf);

		printf("before tokenize\n");

		if(scan(buf, &filler1, &filler2)) //contains a pipe
		    tokenize(buf, args); //strictly of pipe tokenizing
		
		else //no pipes encountered, takes in < > redirection but not used..
			token_space(buf);

		printf("args[0]: %s\n", args[0]);
		//printf("args[1]: %s\n", args[1]);


		//if cd other than home
		if (!strcmp(args[0], "cd"))
		{	
			if(args[1])
				chdir(args[1]);
		}
		else
		{
		//neither cd or logout
			
			pid = fork();
			printf("fork here, pid: %d\n", pid);


			//if parent
			if(pid)
			{
				printf("parent sh waits for child to die\n");
				pid = wait(&status);
			}
			else
			{
				do_pipe(cmd_line, 0);
		    	exit(0);
			}
		}
	}
}

void token_space(char *line)
{
  char *cp;
  cp = line;
  int i = 0;
  
  while (*cp != 0){
       while (*cp == ' ') *cp++ = 0;        
       if (*cp != 0)
           args[i++] = cp;         
       while (*cp != ' ' && *cp != 0) cp++;                  
       if (*cp != 0)   
           *cp = 0;                   
       else 
            break; 
       cp++;
  }
  args[i] = 0; //null terminating
}

// use this function to break up cmdline by pipe symbols
int tokenize(char *cmdLine, char *params[])
{
  int i = 0, j = 0;
  char *c;

  c = strtok(cmdLine, " | ");

  while(c)
  {
    params[i++] = c;
    c = strtok(NULL, " | ");
  }
}


// /***************** do_pipe Algorithm **************/
// 	int pid, pd[2];
// 	pipe(pd);
// 	// create a pipe: pd[0]=READ, pd[1]=WRITE
// 	pid = fork(); // fork a child to share the pipe
// 	if (pid){ // parent: as pipe READER
// 		close(pd[1]); // close pipe WRITE end
// 		dup2(pd[0], 0);
// 		exec(cmd2); // redirect stdin to pipe READ end
// 	}
// 	else{
// 		// child : as pipe WRITER
// 		close(pd[0]); // close pipe READ end
// 		dup2(pd[1], 1); // redirect stdout to pipe WRITE end
// 		exec(cmd1);
// }




int do_pipe(char *cmdLine, int *pd)
{
  int hasPipe, pid, lpd[2];
  char *head, *tail;

  //check if cmd is empty
  if(cmdLine[0] == 0) {
  	printf("empty command\n");
  	return 0;
  }

  if (pd)
  { 
  	//printf("THE PD IS: %s\n", pd[0]);
  	//child: as pipe WRITER
    //close(pd[0]);
    dup2(pd[1],1); //closes fd 1 and opens up old fd (pd[1]) as fd 1
    close(pd[1]);
  }

  //separate cmdLine by head, tail starting rightmost pipe
  hasPipe = scan(cmdLine, &head, &tail);

  //printf("Head %s\n", head);
  //printf("Tail %s\n", tail);

  if (hasPipe)
  {
    //create a pipe: pd[0]=READ, pd[1]=WRITE
    if (pipe(lpd) < 0) 
    {
    	printf("pipe cannot be created");
    	exit(1);
    }
    pid = fork();

    if(pid)
    { //parent: as pipe READER on lpd:
      //close(lpd[1]);
      //dup2(oldfd, newfd) copies oldfd to newfd (can be used interchangeably now)
      dup2(lpd[0], 0); //closes fd 0 and opens up old fd (lpd[0]) as fd 0 
      close(lpd[0]);

      printf("do command tail: %s\n", tail);
      do_command(tail); //exec()..
    }
    else // child : as pipe WRITER...
    {
      do_pipe(head, lpd); //go through again
    }

  }
  //could be two things: (1) execute lefmost command from pipe
  //					           (2) no pipes, just handle command possibly with redirection
  else // if there isnt another pipe too add, execute leftmost command
  {
  	printf("do_pipe -> do_command\n");
    do_command(head);
  }

}

int scan(char *cmdLine, char **head, char **tail)
{
 
  // cmdLine = cmd1 | cmd2 | ... | cmdn-1 | cmdn
  //|<------- head ------------>| tail |; return 1;

  
  char * iterate;
  iterate = *head = cmdLine;

  //get to the end of the string
  while(*iterate)
  {
    iterate++;
  }
  
  //now iterate backwards to find the last |
  while(iterate != cmdLine && *iterate != '|')
  {
    iterate--;
  }
  
  //did not find |
  if(iterate == cmdLine)
  {
    *tail = 0;
    return 0;
  }

  //now separate head from the tail
  
  *iterate = 0; //split from head|tail to head(null char)tail
  iterate += 2; //skip over space between | and cmdn

  *tail = iterate;

  return 1;
}

int do_command(char * cmdLine)
{
	int i = 0;
	int fd = 0;
	char * tail, * head, *p, * redir;
	head = p = cmdLine;

	// scan for redirection
	while(*p && (*p != '>' && *p != '<'))
    {p++;}
		
	//did we find redirection?
	// if so next we need to check that is a file
	if( *p !=0)
	{

		redir = p; // get the redirection symbol

		*(p-1) = 0; // "cmd \0 > \0 file"
		while(*p != ' '){p++;}
		*p = 0;

		tail = ++p;// = file

		while(*p){ p++;}

		if(*(p-1) == ' '){*(p-1) = 0;}// get rid of space at the end
		//printf("\n FOUND REDIR: %s  %s",redir,tail);
		// now process the different types of redirection

		if(!strcmp(redir,"<")) //read case
		{
			close(0); // close stdin
			if(fd = open(tail,O_RDONLY)<0){exit(0);}
			// when we open a file it gets the
			//smallest available FD num so 
			//it automatically is opened as 0
		}
		else if(!strcmp(redir,">")) //write case
		{
			close(1);
			if(fd = open(tail,O_WRONLY|O_CREAT)<0){exit(0);}
		}
		else if(!strcmp(redir,">>")) //append case
		{
			close(1);
			if(fd = open(tail,O_WRONLY|O_APPEND|O_CREAT)<0){exit(0);}
		}

	}

	printf("child task %d exec to %s\n", getpid(), head);
	exec(head);
}


