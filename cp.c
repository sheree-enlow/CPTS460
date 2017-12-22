#include "ucode.c"


#define NULL 0

//prototypes
int file_to_file(char *src, char *dest);
int file_to_dir(char *src, char *dest);
int dir_to_dir(char *src, char *dest);

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

//copy files or directories
int main(int argc, char *argv[])
{  
  //cp source desination (cp f1 copyf1)
  if(argc < 3)
  {
    printf("need 3 arguements, ex: (cp f1 copyf1)\n");
    exit(0);
  }

  //need helper function for recursion later
  cpy(argv[1], argv[2]); //arg[1] = src, arg[2] = dest
}

//decides what type of copying were doing under these cases
  //CASES
  //(1) file1 to file2 <- cp file1 to file2
  //(2) file to dir <- dir/file
  //(3) dir1 to dir2 <- cp all files from dir1 to dir2
  //(4) dir to file <- error

int cpy(char *src, char *dest)
{
  
  char buf1[1024], buf2[1024];
  STAT f1, f2; //struct in type.h
  
  //give struct data to f1 and f2
  stat(src, &f1); 
  stat(dest, &f2);
  
  //first arg is dir
  if(S_ISDIR(f1.st_mode))
  {
    //(3)
    if(S_ISDIR(f2.st_mode)){ //dir to dir case
      dir_to_dir(src, dest);
    }
    else //either arguement 2 is either a file or doesnt exist
    {  
      //(4) error
      if (S_ISREG(f2.st_mode) || S_ISLNK(f2.st_mode)) // dir to file case
        return 0;

      //(3)
      //arguement 2 has to be a DIR since dir -> file is an error
      else //arg2 does not exist
      { 
        mkdir(dest);
        dir_to_dir(src, dest);
      }
    }
  }
  //first arg is likely a file
  else 
  { 
    //double check first if file or not
    if(!S_ISREG(f1.st_mode) && !S_ISLNK(f1.st_mode)) {return 0;}

    //(2) file -> dir
    if(S_ISDIR(f2.st_mode))
      file_to_dir(src, dest);
    
    //(1) file -> file
    else 
      file_to_file(src, dest);
  }
}

//(1) copy file to newfile
int file_to_file(char *src, char *dest)
{
  int fd1, fd2, n;
  char buf[1024];
  STAT f1;

  //why are you trying to copy the same file???
  if(strcmp(src, dest) == 0)
  {
    printf("same files, no copy done\n");
    return -1;
  }

  stat(src, &f1);

  //a link file
  if(S_ISLNK(f1.st_mode))
  {
    printf("%s ==> ", src);
    n = readlink(src, buf);
    buf[n] = 0;
    printf("%s\n", buf);

    symlink(buf, dest); //create the new link
  }
  //is a regular file or need to create one
  else 
  { 
    //open our source for read and dest to write to
    fd1 = open(src, O_RDONLY);
    fd2 = open(dest, O_WRONLY|O_CREAT);

    //read and print till end of file
    while(n = read(fd1, buf, sizeof(buf)))
    {
      write(fd2, buf, n);
    }
  
    close(fd1);
    close(fd2);
  }
  return 0;
}

//src.txt -> Documents/sheree.txt FILE
//src.txt -> Documents/           DIR

//(2) copy file to directory/newfile
int file_to_dir(char *src, char *dest)
{
  int i = 0;
  char buf[1024], temp[1024], *name[64], *p;
  STAT f2;

  //stat returns negative value on failure
  if(stat(dest, &f2) || !S_ISDIR(f2.st_mode)){
    printf("directory %s does not exist\n", dest);
    return -1;
  }


  //parse out filename
  strcpy(temp, src);
  p = strtok(temp, "/");

  while(p){
    name[i++] = p;
    p = strtok(NULL, "/");
  }

  //rebuild outfile by tacking it onto the dest (directory)
  strcpy(buf, dest);
  strcat(buf, "/");
  strcat(buf, name[i-1]);
  
  return file_to_file(src, buf); //now create this new file in the DIR
}

//(3) copy all directory contents into another dir
int dir_to_dir(char *src, char *dest)
{
  int fd, n, flag = 1;
  STAT f1, f2;

  char *cp, buf[1024], srcpath[128], descpath[128], name[64];
  DIR *dp;

  if(strcmp(src, dest) == 0){
    printf("same directories, cannot copy same dirs\n");
    return -1;
  }

  fd = open(src, O_RDONLY);

  while(n = read(fd, buf, sizeof(buf))){
    cp = buf;
    dp = (DIR *)buf; //the DIR

    //skip over the "." and ".." dirs (like 360 project)
    if (flag == 1){
      flag = 0; //we've encountered that . and ..
      cp += dp->rec_len; //"."
      dp = (DIR *)cp;
      cp += dp->rec_len; // ".."
      dp = (DIR *)cp; //first directory to copy
    }

    while(cp < buf + sizeof(buf)){
      //clear buffers
      memset(srcpath, 0, sizeof(srcpath));
      memset(descpath, 0, sizeof(descpath));
      memset(name, 0, sizeof(name));

      //get name from dir entry
      strncpy(name, dp->name, dp->name_len);

      //build new src path since must loop through the entire directory
      //and cp over to the destination directory
      strcpy(srcpath, src); 
      strcat(srcpath, "/");
      strcat(srcpath, name);

      //build new destination path
      strcpy(descpath, dest);
      strcat(descpath, "/");
      strcat(descpath, name);

      //need to make a new dir if it does not exist
      stat(srcpath, &f1);

      if(S_ISDIR(f1.st_mode)){
      	stat(descpath, &f2);
      	if(!S_ISDIR(f2.st_mode)) //check if dir already exists, make it if not
          mkdir(descpath);
      }
      
      //do this recursively since each dir entry can have a dir or file
      cpy(srcpath, descpath); //back to beginning!
      
      cp += dp->rec_len;
      dp = (DIR *)cp;
    }
    //clear buffer
    memset(buf, 0, sizeof(buf));
  }
  close(fd);
}