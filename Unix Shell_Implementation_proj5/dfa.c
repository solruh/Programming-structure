/*--------------------------------------------------------------------*/
/* dfa.c                                                              */
/* Original Author: Bob Dondero                                       */
/* Illustrate lexical analysis using a deterministic finite state     */
/* automaton (DFA)                                                    */
/*--------------------------------------------------------------------*/

#include "dynarray_gere.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "dfa_gere.h"

#define PIPE '|'
#define STDIN_REDIRECT '<'
#define STDOUT_REDIRECT '>'
#define NULL_CHAR '\0'
#define NEWLINE '\n'
#define FLAG 2
#define DOUBLE_QUOTATION '"'



/*--------------------------------------------------------------------*/



//four tokens
void printanyToken(void *pvItem, void *pvExtra)
{
   struct Token *psToken = (struct Token*)pvItem;
    if (psToken->eType == WORD_TOKEN)
        printf(" WORD TOKEN----->%s \n", psToken->pcValue);
    else if (psToken->eType == PIPE_TOKEN)
        printf(" PIPE TOKEN----->%s \n", psToken->pcValue);
    else if (psToken->eType == IN_TOKEN)
        printf("STANDARD IN REDIRECT TOKEN----->%s \n", psToken->pcValue);
    else
        printf("STANDARD OUT REDIRECT TOKEN----->%s \n", psToken->pcValue);
}

 void freeToken(void *pvItem, void *pvExtra)

/* Free token pvItem.  pvExtra is unused. */

{
   struct Token *psToken = (struct Token*)pvItem;
   free(psToken->pcValue);
   free(psToken);
}

struct Token *makeToken(enum TokenType eTokenType,
   char *pcValue)

/* Create and return a Token whose type is eTokenType and whose
   value consists of string pcValue.  Return NULL if insufficient
   memory is available.  The caller owns the Token. */

{
   struct Token *psToken;

   psToken = (struct Token*)malloc(sizeof(struct Token));
   if (psToken == NULL)
      return NULL;

   psToken->eType = eTokenType;

   psToken->pcValue = (char*)malloc(strlen(pcValue) + 1);
   if (psToken->pcValue == NULL)
   {
      free(psToken);
      return NULL;
   }

   strcpy(psToken->pcValue, pcValue);

   return psToken;
}

 int handle_special_tokens(char c,char* value,DynArray_T oTokens, enum LexState* state){
    
    struct Token* psToken;

    if (c == PIPE)
        psToken = makeToken(PIPE_TOKEN,value);
    else if(c == STDIN_REDIRECT)
        psToken = makeToken(IN_TOKEN,value);
    else
        psToken = makeToken(OUT_TOKEN,value);
    
    if (psToken == NULL){
        fprintf(stderr,"Cannot allocate memory\n");
        return FALSE;
    }
    if (! DynArray_add(oTokens,psToken)){
        fprintf(stderr,"Cannot allocate memory\n");
        return FALSE;
    }

    memset(value,0,MAX_LINE_SIZE); 

    //change state
    if(c == PIPE)
        *state = PIPE_STATE;
    else
        *state = REDIR_STATE;
    
    return FLAG;
    
    
}

int handle_word_tokens(char c,char* value,DynArray_T oTokens, enum LexState* state){

    struct Token* psToken;

    //printf("strlen is %lu",strlen(value));

    if (strlen(value) == 0)
        return FLAG;


    psToken = makeToken(WORD_TOKEN,value);
    if (psToken == NULL){
        fprintf(stderr,"Cannot allocate memory\n");
        return FALSE;
    }
    if (! DynArray_add(oTokens,psToken)){
        fprintf(stderr,"Cannot allocate memory\n");
        return FALSE;
    }
    memset(value,0,MAX_LINE_SIZE);
    //printf("strlen is %lu",strlen(value));

    return FLAG;

}



int handle_start_state(char c,int* quotation_p,enum LexState* inner_state,char* value,char* binary,DynArray_T oToken){

    int index = strlen(value),outcome;
    

    if (c == NEWLINE || c == NULL_CHAR){
        if (*quotation_p == FALSE){ // the quotation has no pair
            fprintf(stderr,"%s: Could not find quote pair\n",binary);
            return FALSE;
        }
        else
            return TRUE;
        
    }
    else if (isspace(c)){
        *inner_state = START_STATE;
    }

    else if (c == DOUBLE_QUOTATION){
        *inner_state = START_QUOT;
    }
    else if (c == PIPE || c == STDIN_REDIRECT || c == STDOUT_REDIRECT){
        value[index] = c;
        outcome = handle_special_tokens(c,value,oToken,inner_state);
        if (outcome == FALSE)
            return FALSE;
    }

    else{
        value[index] = c;
        *inner_state = IN_WORD;
    }

    return FLAG;

}




int handle_quote_state(char c, int * quotation_p, enum LexState * state, char *value, char *binary, DynArray_T oToken){
    
    *quotation_p = FALSE;
    int index = strlen(value);

    if (c == NEWLINE || c == NULL_CHAR){
        if (*quotation_p == FALSE){ // the quotation has no pair
            fprintf(stderr,"%s: Could not find quote pair\n",binary);
            return FALSE;
        }
    }

    else if (c == DOUBLE_QUOTATION){
        *state = END_QUOT;
        *quotation_p = TRUE;
    }
    else
    {
        *state = START_QUOT;
        value[index] = c;

    }
    return FLAG;
    
}

int handle_inword_state(char c, int * quotation_p, enum LexState * state, char *value, char *binary, DynArray_T oToken){

    int outcome,index = strlen(value);


    if (c == NEWLINE || c == NULL_CHAR){
        outcome = handle_word_tokens(c,value,oToken,state);
        if (outcome == FALSE)
            return FALSE;
        else
            return TRUE;
        
    }
    else if (isspace(c)){
        outcome = handle_word_tokens(c,value,oToken,state);
        
        if (outcome == FALSE)
            return FALSE;
        else
            *state = START_STATE;
        
        
    }

    else if(c == PIPE || c == STDIN_REDIRECT || c == STDOUT_REDIRECT){

        outcome = handle_word_tokens(c,value,oToken,state);
        
        if (outcome == FALSE){
            return FALSE;
        }
        value[index] = c;

        outcome = handle_special_tokens(c,value,oToken,state);

        if (outcome == FALSE)
            return FALSE;

    }

    else if (c == DOUBLE_QUOTATION)
        *state = START_QUOT;

    else{
        value[index] = c;
        *state = IN_WORD;
    }

    return FLAG;

}

int handle_endquote_state(char c, int * quotation_p, enum LexState * state, char *value, char *binary, DynArray_T oToken){
    
    int outcome,index = strlen(value);
    //printf("00000    %c",c);
    
    if (c == NEWLINE || c == NULL_CHAR){
        if (*quotation_p == FALSE){ // the quotation has no pair
            fprintf(stderr,"%s: Could not find quote pair\n",binary);
            return FALSE;
        }
        else{
            outcome = handle_word_tokens(c,value,oToken,state);
            //printf("***%d((((\n",outcome);
            if (outcome == FALSE)
                return FALSE;
            else
                return TRUE;
        
        }
    }

    else if(c == DOUBLE_QUOTATION)
        *state = START_QUOT;

    else if(c == PIPE || c == STDIN_REDIRECT || c == STDOUT_REDIRECT){

        outcome = handle_word_tokens(c,value,oToken,state);
        
        if (outcome == FALSE){
            return FALSE;
        }
        value[index] = c;

        outcome = handle_special_tokens(c,value,oToken,state);

        if (outcome == FALSE)
            return FALSE;

    }

    else if(isspace(c)){
        
        value[index] = c;

        outcome = handle_word_tokens(c,value,oToken,state);

        if (outcome == FALSE)
            return FALSE;
        
        *state = START_STATE;
        
    }

    else{

        value[index] = c;
        *state = IN_WORD;
    }

    return FLAG;
    

}


int handle_pipeorredir_state(char c, int * quotation_p, enum LexState * state, char *value, char *binary, DynArray_T oToken){
    
    int outcome,index = strlen(value);

    if (c == NEWLINE || c == NULL_CHAR){
        if (*quotation_p == FALSE){ // the quotation has no pair
            fprintf(stderr,"%s: Could not find quote pair\n",binary);
            return FALSE;
        }
        else
            return TRUE;
        
    }

    else if(isspace(c))
        *state = START_STATE;

    else if (c == PIPE || c == STDIN_REDIRECT || c == STDOUT_REDIRECT){
        value[index] = c;
        outcome = handle_special_tokens(c,value,oToken,state);
        if (outcome == FALSE)
            return FALSE;
    }

    else if(c == DOUBLE_QUOTATION)
        *state = START_QUOT;
    else
    {
        value[index] = c;
        *state = IN_WORD;

    }

    return FLAG;
    
    
}



 
 int handle_tokens(const char*pcLine,DynArray_T oTokens,char* value,enum LexState* state,char* binary){

    int i_line = 0;
    char c;
    //char* ptr;

    int outcome;
    int quotation = 1;

    while (1){

        c = pcLine[i_line];
        i_line++;
        //printf("________value is %s lenof string is %lu , we are in state %d______ char is %c\n",value,strlen(value),*state,c);

        if (*state == START_STATE){

            outcome = handle_start_state(c,&quotation,state,value,binary,oTokens);
            if (outcome == FALSE)
                return FALSE;
            else if (outcome == TRUE)
                return TRUE;
            
        }
        else if(*state == START_QUOT){

            outcome = handle_quote_state(c,&quotation,state,value,binary,oTokens);
            if (outcome == FALSE)
                return FALSE;

        }
        else if(*state == IN_WORD){

            outcome = handle_inword_state(c,&quotation,state,value,binary,oTokens);

            if (outcome == TRUE)
                return TRUE;

            else if(outcome == FALSE)
                return FALSE;
            
        }
        else if(*state == END_QUOT){

            outcome = handle_endquote_state(c,&quotation,state,value,binary,oTokens);

            if (outcome == TRUE)
                return TRUE;

            else if(outcome == FALSE)
                return FALSE;

        }
        else if(*state == PIPE_STATE || *state == REDIR_STATE ){

            outcome = handle_pipeorredir_state(c,&quotation,state,value,binary,oTokens);

            if (outcome == TRUE)
                return TRUE;

            else if(outcome == FALSE)
                return FALSE;


            
        }
        




    }

    return 100;



}

/*--------------------------------------------------------------------*/

int lexLine(const char *pcLine, DynArray_T oTokens,char* executable)

/* Lexically analyze string pcLine.  Populate oTokens with the
   tokens that pcLine contains.  Return 1 (TRUE) if successful, or
   0 (FALSE) otherwise.  In the latter case, oTokens may contain
   tokens that were discovered before the error. The caller owns the
   tokens placed in oTokens. */

/* lexLine() uses a DFA approach.  It "reads" its characters from
   pcLine. */

{


   enum LexState eState = START_STATE;

   //int iLineIndex = 0;

   //char c;
   char* value = calloc(MAX_LINE_SIZE,sizeof(char));
   //struct Token *psToken;

   assert(pcLine != NULL);
   assert(oTokens != NULL);

   int outcome = handle_tokens(pcLine,oTokens,value,&eState,executable);
   free(value);

    if (outcome)
        return TRUE;
    else
        return FALSE;
    
}

/*--------------------------------------------------------------------*/

//int main(void)

/* Read a line from stdin, and write to stdout each number and word
   that it contains.  Repeat until EOF.  Return 0 iff successful. */

/*
{
   char acLine[MAX_LINE_SIZE];
   DynArray_T oTokens;
   int iSuccessful;
   char *b = "ish";

   printf("------------------------------------\n");
   while (fgets(acLine, MAX_LINE_SIZE, stdin) != NULL)
   {
      oTokens = DynArray_new(0);
      if (oTokens == NULL)
      {
         fprintf(stderr, "Cannot allocate memory\n");
         exit(EXIT_FAILURE);
      }

      iSuccessful = lexLine(acLine, oTokens,b);
      if (iSuccessful)
      {
          //printf("_____");
          DynArray_map(oTokens, printanyToken, NULL);
          printf("\n");

      }
      printf("------------------------------------\n");

      DynArray_map(oTokens, freeToken, NULL);
      DynArray_free(oTokens);
   }

   return 0;
}*/