
#include "ucode.c"

int main(int argc, char *argv[]) /* l2u [f1 f2] */
{
	int fd1 = 0, fd2 = 1, n;
	int rr = 0, lr = 0; 
	char c;
	char* test = "test";

	int redirection = 0;

	//sometimes printf doesn't work unless varible is formatted with it
	char *me = "** Sheree's";
	printf("\n%s L2U** \n\n", me); 

	//just l2u, likely involved with pipes ex | l2u |
	if(argc == 1)
		redirection = 0;
	else
	{
		if(argv[1]){fd1 = open(argv[1], O_RDONLY);}
		if(argv[2]){fd2 = open(argv[2], O_WRONLY|O_CREAT);}
	}
	

	while( n = read(fd1, &c, 1))
	{

		if(c >= 'a' && c <= 'z') //lowercase encountered
			c -= 32; //capitalize it!

		if (redirection)
			mputc(c); //out
		else{
			if(fd2 == 1 && c == '\n'){write(2, "\r", 1);}

			write(fd2, &c, 1);
		}
	}

	close(fd1);
	close(fd2);
}
