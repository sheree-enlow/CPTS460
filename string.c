/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
#ifndef STRING_C
#define STRING_C



#define NULL '\0'

// char* strtok(char* s, char* delm)
// {
//     printf("in strtokkk");
//     static int currIndex = 0;
//     if(!s || !delm || s[currIndex] == '\0')
//     return NULL;
//     char *W;
//     int i = currIndex, k = 0, j = 0;
//     //char *ptr;
//     //static char *Iterator = s;
//     //ptr = s;

//     /*if (s == NULL){
//     s = Iterator;
//     }*/
//     while (s[i] != '\0'){
//         j = 0;
//         while (delm[j] != '\0'){
//             if (s[i] != delm[j])
//                 W[k] = s[i];
//             else goto It;
//             j++;
//         }
//         //ptr++;
//         i++;
//         k++;
//         return "hi";
//     }
// It:
//     printf("in it");
//     W[i] = 0;
//     currIndex = i+1;
//     //Iterator = ++ptr;
//     return W;
// }

// char* strtok(char* teststr, char *ch) {
//   char *dummystr=NULL;
//   char *start=NULL;
//   char *end=NULL;
//   char nullch = '\0';
//   char* address_of_null=&nullch;
//   static char* nexttok;

//   if(teststr!=NULL) {
//     dummystr = teststr;
//   } else {
//     if(*nexttok=='\0') return NULL;
//     dummystr = nexttok;
//   }


//   while(dummystr!=NULL) {
//     //empty string
//     if(*dummystr=='\0') break;

//     if(*dummystr!=ch)
//     {
//       if(!start) 
//         start = dummystr;

//       dummystr++;
//       //to handle the case where the delimiter is not at the end of the string.
//       if(*dummystr=='\0') 
//       {
//         nexttok = dummystr;
//         break;
//       }
//     }
//     else
//     {
//       if(start) {
//         end = dummystr;
//         nexttok = dummystr+1;
//         *end = *address_of_null;
//         break;
//       } else {
//           dummystr++;
//       }
//     }
//   }
//   return start;
// }


// char* strtok(char *str, const char* delim) {
//     static char* _buffer;
//     if(str != NULL) _buffer = str;
//     if(_buffer[0] == '\0') return NULL;
 
//     char *ret = _buffer, *b;
//     const char *d;
 
//     for(b = _buffer; *b !='\0'; b++) {
//         for(d = delim; *d != '\0'; d++) {
//             if(*b == *d) {
//                 *b = '\0';
//                 _buffer = b+1;
 
//                 // skip the beginning delimiters
//                 if(b == ret) { 
//                     ret++; 
//                     continue; 
//                 }
//                 return ret;
//             }
//         }
//     }
 
//     return ret;
// }

#endif
