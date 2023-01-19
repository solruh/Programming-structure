/***************************************
Name-Reda Geberemedhin Derbew   sgrep.c
Assignment number- 2        ID -20180756
****************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for skeleton code */
#include <unistd.h> /* for getopt */
#include "str.h"

#define FIND_STR        "-f"
#define REPLACE_STR     "-r"
#define DIFF_STR        "-d"

#define MAX_STR_LEN 1023

#define FALSE 0
#define TRUE  1

typedef enum {
  INVALID,
  FIND,
  REPLACE,
  DIFF
} CommandType;

/*
 * Fill out your functions here (If you need) 
 */

/*--------------------------------------------------------------------*/
/* PrintUsage()
   print out the usage of the Simple Grep Program                     */
/*--------------------------------------------------------------------*/
void 
PrintUsage(const char* argv0) 
{
  const static char *fmt = 
    "Simple Grep (sgrep) Usage:\n"
    "%s [COMMAND] [OPTIONS]...\n"
    "\nCOMMNAD\n"
    "\tFind: -f [search-string]\n"
    "\tReplace: -r [string1] [string2]\n"
    "\tDiff: -d [file1] [file2]\n";

  printf(fmt, argv0);
}
/*-------------------------------------------------------------------*/
/* DoFind()
   Your task:
   1. Do argument validation 
   - String or file argument length is no more than 1023  //what do we mean by this one??
   - If you encounter a command-line argument that's too long, 
   print out "Error: argument is too long"
   
   2. Read the each line from standard input (stdin)
   - If you encounter a line larger than 1023 bytes, 
   print out "Error: input line is too long" 
   - Error message should be printed out to standard error (stderr)
   
   3. Check & print out the line contains a given string (search-string)
      
   Tips:
   - fgets() is an useful function to read characters from file. Note 
   that the fget() reads until newline or the end-of-file is reached. 
   - fprintf(sderr, ...) should be useful for printing out error
   message to standard error

   NOTE: If there is any problem, return FALSE; if not, return TRUE  */
/*-------------------------------------------------------------------*/
/**********************************************************************
DoFind takes pcSearch as its parameter and it gets its inputs from the 
stdin. it prints an error to the stderr and returns FALSE if either the 
parameter's length or length of the buf is more than 1023bytes. if that 
is not the case, it searches pcSearch in buf and if it fails to find 
pcSearch in buf it will do nothing but if pcSearch is successfully
found in buf, it prints the whole lines containing the string pointed by 
pcSearch and returns TRUE. in this function I used fgets() function to 
read from standard input. It reads a line from the stdin and stores it 
into the string pointed to by buf. It stops when either ((sizeof(buf))-1)
characters are read, the newline character is read, or the end-of-file 
is reached, whichever comes first.
**********************************************************************/
int
DoFind(const char *pcSearch)
{
  char buf[MAX_STR_LEN + 2]; 
  int len; 
  
  if(StrGetLength(pcSearch)>MAX_STR_LEN){
    fprintf(stderr,"Error: argument is too long\n");
    return FALSE;
  }
    
  while ((fgets(buf, sizeof(buf), stdin))!=NULL) {
      if ((len = StrGetLength(buf)) > MAX_STR_LEN) {
      fprintf(stderr, "Error: input line is too long\n");
      return FALSE;
    }

      if(StrSearch(buf, pcSearch)) {
          if(StrSearch(buf,"\n")) printf("%s",buf);
          else printf("%s\n",buf);
      
      }
  }
   
  return TRUE;
}
/*-------------------------------------------------------------------*/
/* DoReplace()
   Your task:
   1. Do argument validation 
      - String length is no more than 1023
      - If you encounter a command-line argument that's too long, 
        print out "Error: argument is too long"
      - If word1 is an empty string,
        print out "Error: Can't replace an empty substring"
      
   2. Read the each line from standard input (stdin)
      - If you encounter a line larger than 1023 bytes, 
        print out "Error: input line is too long" 
      - Error message should be printed out to standard error (stderr)

   3. Replace the string and print out the replaced string

   NOTE: If there is any problem, return FALSE; if not, return TRUE  */
/*-------------------------------------------------------------------*/
/***********************************************************************
DoReplace takes pcString1 and pcString2 as its parameters and if the 
length of either of the parameters is larger than 1023 bytes, it prints
an error to stderr and returns false. besides if pcString1 points to 
an empty string, it prints an error to stderr and returns false. in 
addition, if the length of the buffer is larger than 1023 bytes it 
prints an error to stderr and returns false. if we did not find any 
error and if string pointed by pcString1 is not found in buf, then 
it prints buf wihtout changing it and returns true. but if 
string pointed by pcString1 occurrs in the buf every string pointed 
by pcString1 in buf is replaced by string pointed by pcString2 and 
the buf with replaced string is printed out and true is returned. 
************************************************************************/
int
DoReplace(const char *pcString1, const char *pcString2)
{

  char buf[MAX_STR_LEN + 2];
  int len;

    if(pcString1==NULL) {
        fprintf(stderr,"Error: can't replace an empty substring\n");
        return FALSE;
          }
    if(StrGetLength(pcString1)>MAX_STR_LEN || StrGetLength(pcString2)>MAX_STR_LEN){
      fprintf(stderr,"Error: argument is too long\n");
      return FALSE;
    }

    
  while ((fgets(buf, sizeof(buf), stdin))!=NULL) {
  
      if ((len = StrGetLength(buf)) > MAX_STR_LEN) {
      fprintf(stderr, "Error: input line is too long\n");
      return FALSE;
    }


    char *pc = buf;
    char *pcRem = StrSearch(buf,pcString1); // to point to string pointed by buf 

    while(pcRem!=NULL){
      while(pc!=pcRem){
        printf("%c",*pc);
        pc ++;
      }
      printf("%s",pcString2);
      pc = pc + StrGetLength(pcString1);
      pcRem = StrSearch(pc,pcString1);
    }

    while(*pc!='\0'){
      printf("%c", *pc);
      pc++;
    }

  }

  return TRUE;
}
/*-------------------------------------------------------------------*/
/* DoDiff()
   Your task:
   1. Do argument validation 
     - file name length is no more than 1023
     - If a command-line argument is too long,  //DOES THIS MEAN EACH LINE OR THE WHOLE FILE ARGUMENT??
       print out "Error: argument is too long" to stderr

   2. Open the two files
      - The name of files are given by two parameters
      - If you fail to open either file, print out error messsage
      - Error message: "Error: failed to open file [filename]\n"
      - Error message should be printed out to stderr

   3. Read the each line from each file
      - If you encounter a line larger than 1023 bytes, 
        print out "Error: input line [filename] is too long" 
      - Error message should be printed out to stderr

   4. Compare the two files (file1, file2) line by line 

   5. Print out any different line with the following format
      file1@linenumber:file1's line
      file2@linenumber:file2's line

   6. If one of the files ends earlier than the other, print out an
      error message "Error: [filename] ends early at line XX", where
      XX is the final line number of [filename].

   NOTE: If there is any problem, return FALSE; if not, return TRUE  */
/*-------------------------------------------------------------------*/
/**********************************************************************
DoDiff takes file1 and file2 as its parameters and if eithr file's 
length is larger than 1023 bytes it prints an error to stderr and 
returns false. besides if either of the file pointed by each parameter
is an empty file it prints an error to stderr and returns false. in 
addition for each line if the length of eithr of buffers length is 
greater than 1023 bytes, it prints an error to stderr and returns 
false.and if one of the files ends earlier than the other, 
it prints an error message to stderr and returns false. 
while comparing each buffer line from file1 and file2, if 
difference is found, both lines are printed along with the name of 
the file and line number. if no difference is found between the two
buffers, then do nothing. and it return true if no problem was found.
**********************************************************************/
int
DoDiff(const char *file1, const char *file2)
{
  /* TODO: fill out this function */  
  char buf1[MAX_STR_LEN + 2];
  char buf2[MAX_STR_LEN + 2];
  int len;
  // to track the line number when the two files have different line contents.
  //and to track line number if one of the files ends first
  int lineTracker=0;   
  char *tempbuf1;
  char *tempbuf2;
  FILE *filetext1;
  FILE *filetext2;

  filetext1=fopen(file1,"r");
  filetext2=fopen(file2,"r");
  if(filetext1==NULL) { 
    fprintf(stderr, "Error: failed to open file[%s] \n",file1);     // i followed the format similar to in your task
    return FALSE;
                  }
  if(filetext2==NULL){
    fprintf(stderr, "Error: failed to open file[%s] \n",file2);     //i followed the format similar to in your task
    return FALSE;
  }
  if(StrGetLength(file1)>MAX_STR_LEN || StrGetLength(file2)>MAX_STR_LEN){
      fprintf(stderr,"Error: argument is too long");
      return FALSE;
    }
    tempbuf1=fgets(buf1, sizeof(buf1), filetext1);
    tempbuf2=fgets(buf2, sizeof(buf2), filetext2);
  while (!(tempbuf1==NULL && tempbuf2==NULL)) {
      
          lineTracker++;
          const char *buffer1=(const char *)buf1; //casting buf1 to const char pointer and saving it to buffer1
          const char *buffer2=(const char *)buf2; //casting buf2 to const char pointer and saving it to buffer2

        if ((len = StrGetLength(buffer1)) > MAX_STR_LEN)  //buffer1 is const char to prevent from non const argument
            {
            fprintf(stderr, "Error: input line [%s] is too long\n",file1);
            return FALSE;
            }
   
        if ((len = StrGetLength(buffer2)) > MAX_STR_LEN) 
              {
              fprintf(stderr, "Error: input line [%s] is too long\n",file2);
              return FALSE;
              }
      
        if(tempbuf1==NULL && tempbuf2!=NULL){  
            printf("Error: [%s] ends early at line %d",file1,lineTracker-1);
            fclose(filetext1);
            fclose(filetext2);
            return FALSE;
          }
    
        else if(tempbuf1!=NULL && tempbuf2==NULL) {
            printf("Error: [%s] ends early at line %d",file2,lineTracker-1);
            fclose(filetext1);
            fclose(filetext2);
            return FALSE;
            }
          //taking buffer1 & buffer2 incase compiler gives non constant char * argument
        if(StrCompare(buffer1,buffer2)!=0) 
              {
                printf("%s@%d:%s",file1,lineTracker,buf1);
                printf("%s@%d:%s",file2,lineTracker,buf2);
              }
        tempbuf1=fgets(buf1, sizeof(buf1), filetext1);
        tempbuf2=fgets(buf2, sizeof(buf2), filetext2);

  }
  fclose(filetext1);
  fclose(filetext2);
  return TRUE;
}
/*-------------------------------------------------------------------*/
/* CommandCheck() 
   - Parse the command and check number of argument. 
   - It returns the command type number
   - This function only checks number of argument. 
   - If the unknown function is given or the number of argument is 
   different from required number, this function returns FALSE.
   
   Note: You SHOULD check the argument rule later                    */
/*-------------------------------------------------------------------*/ 
int
CommandCheck(const int argc, const char *argv1)
{
  int cmdtype = INVALID;
   
  /* check minimum number of argument */
  if (argc < 3)
    return cmdtype;
   
  /* check command type */ 
  if (strcmp(argv1, FIND_STR) == 0) {
    if (argc != 3)
      return FALSE;    
    cmdtype = FIND;       
  }
  else if (strcmp(argv1, REPLACE_STR) == 0) {
    if (argc != 4)
      return FALSE;
    cmdtype = REPLACE;
  }
  else if (strcmp(argv1, DIFF_STR) == 0) {
    if (argc != 4)
      return FALSE;
    cmdtype = DIFF;
  }
   
  return cmdtype;
}
/*-------------------------------------------------------------------*/
int 
main(const int argc, const char *argv[]) 
{
  int type, ret;
   
  /* Do argument check and parsing */
  if (!(type = CommandCheck(argc, argv[1]))) {
    fprintf(stderr, "Error: argument parsing error\n");
    PrintUsage(argv[0]);
    return (EXIT_FAILURE);
  }
   
  /* Do appropriate job */
  switch (type) {
  case FIND:
    ret = DoFind(argv[2]);
    break;
  case REPLACE:
    ret = DoReplace(argv[2], argv[3]);
    break;
  case DIFF:
    ret = DoDiff(argv[2], argv[3]);
    break;
  } 

  return (ret)? EXIT_SUCCESS : EXIT_FAILURE;
}