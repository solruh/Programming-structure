#ifndef DFA_INCLUDED

#define DFA_INCLUDED

#include "dynarray_gere.h"


void freeToken(void *pvItem, void *pvExtra);

int lexLine(const char *pcLine, DynArray_T oTokens,char* executable);

int syntaticLine(DynArray_T output_lexical, DynArray_T command_,char *executable,char* pipe);

void freeinfo(void *ptr,void *pvExtra);

typedef struct command_set{

    char* pipe;
    char* redirect_in;
    char* redirect_out;
    char* command;
    DynArray_T arg;

}Command_set;


#define PIPE '|'
#define STDIN_REDIRECT '<'
#define STDOUT_REDIRECT '>'
#define NULL_CHAR '\0'
#define NEWLINE '\n'
#define FLAG 2
#define DOUBLE_QUOTATION '"'

enum TokenType {WORD_TOKEN, PIPE_TOKEN,IN_TOKEN,OUT_TOKEN};

enum LexState {START_STATE,IN_WORD,START_QUOT,END_QUOT,REDIR_STATE,PIPE_STATE};

/*--------------------------------------------------------------------*/

/* A Token is either a number or a word, expressed as a string. */

struct Token
{
   enum TokenType eType;
   /* The type of the token. */

   char *pcValue;
   /* The string which is the token's value. */
};

/*--------------------------------------------------------------------*/



/* Free token pvItem.  pvExtra is unused. */



/*--------------------------------------------------------------------*/
void printanyToken(void *pvItem, void *pvExtra);



/*--------------------------------------------------------------------*/


/*--------------------------------------------------------------------*/



/*--------------------------------------------------------------------*/

enum {MAX_LINE_SIZE = 1024};

enum {FALSE, TRUE};


#endif
