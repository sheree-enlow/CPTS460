/****************** Algorithm of login *******************/
// login.c : Upon entry, argv[0]=login, argv[1]=/dev/ttyX
// #include "ucode.c"
// #include "string.c"

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


#include "ucode.c"

char *tty, buf[1024], buf2[1024], *ln, *params[64], uname[64], pass[64], temp[64], *executable, *home;
int in, out, err, pw, uid, gid;

int chkUser();
int flush();

int main(int argc, char *argv[])
{

	int i = 0; 

	tty = argv[1];
	
	//close these in case INIT opened them before
	close(0); 
	close(1);
	close(2); 

	//argv[1] -> tty
	in = open(tty, O_RDONLY);
	out = open(tty, O_WRONLY);
	err = open(tty, O_WRONLY);

	settty(tty); //store tty string in PROC.tty[] for putc()

	//causing no login input...
	//signal(2,1); //ignore Control-C interrupts so that
				 //control-C KILLS other procs on this tty but not the main sh

	pw = open("/etc/passwd", O_RDONLY);

	//tty -> /dev/tty0 (likely)
	printf("Sheree's login version: open %s as stdin, stdout, stderr...\n\n", tty);

	while (1)
	{	
		lseek(pw, 0, 0); //make sure fp is set to beginning


		printf("login: ");  gets(uname); 
		printf("password: "); gets(pass);

		//kcw:abcde
		//buster:123
		//guest:mercy

		i = read(pw, buf, sizeof(buf)); 
		buf[i] = 0; //remember null terminater


		//printf("\nbuf: %s\n", buf);
		ln = strtok(buf, "\r\n");

		i = 0;

		while(ln)
		{	
			params[i++] = ln;
			ln = strtok(NULL, "\r\n");
		}




		if (isValid())
		{	
			//set uid to user uid
			chuid(uid, gid);
			
			chdir(home); //go to home dir

			close(pw); //close the open file
			
			//exec to prograom in user's account
			//most likely is sh
			strcpy(buf2, executable);
			strcat(buf2, " ");
			strcat(buf2, home);

			printf("user's program: %s\n", buf2);
			exec(buf2);	
		}
		printf("INCORRECT INPUT, login failed\n");
		i = 0;
	}
}

int isValid()
{
	int i = 0; 
/*
          username:password:gid:uid:fullname:HOMEDIR:program
  e.g.    root:xxxxxxx:1000:0:superuser:/root:sh
*/
	while (params[i])
	{

		if (strcmp(strtok(params[i], ":"), uname) == 0 && strcmp(strtok(NULL, ":"), pass) == 0)
		{
			gid = atoi(strtok(NULL, ":"));
			uid = atoi(strtok(NULL, ":"));

			strtok(NULL, ":");

			home = strtok(NULL, ":");
			executable = strtok(NULL, ":");

			return 1;
		}
		i++;
	}
	printf("failed: %s\n", params[0]);
	return 0; //invalid
}


