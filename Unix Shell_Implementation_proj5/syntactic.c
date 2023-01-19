#include "dynarray_gere.h"
#include "dfa_gere.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <strings.h>

enum States{Start_state,command_state,arg_state,redir_in_state,redir_out_state,pipe_state};

void prints(void *pvItem, void *pvExtra){
    
    char* ptr = (char*)pvItem;
    printf("%s\n",ptr);

}


void printCommand(void *pvItem, void *pvExtra)
{
   Command_set *new = (Command_set*)pvItem;

   printf("Args\n");
   DynArray_map(new->arg,prints,NULL);
   printf("\n");

   printf("name\n");
   printf("%s",new->command);
   printf("\n");

   printf("in\n");
   printf("%s",new->redirect_in);
   printf("\n");

   printf("out\n");
   printf("%s",new->redirect_out);
   printf("\n");

}


Command_set* store_command_information(char* command,char*redir_in,char*redir_out,char*pipe,DynArray_T arg){
    
    Command_set* store_information = calloc(1,sizeof(struct command_set));

    if (store_information == NULL){
        return NULL;
    }
    if (command){
        store_information->command = calloc(1,strlen(command) + 1);
        strcpy(store_information->command,command);
        free(command);
    }
    else
        store_information->command = NULL;
    

    if (redir_in){
        store_information->redirect_in = calloc(1,strlen(redir_in) + 1);
        strcpy(store_information->redirect_in,redir_in);
        free(redir_in);
    }
    else
        store_information->redirect_in = NULL;
    
    if (redir_out){
        store_information->redirect_out = calloc(1,strlen(redir_out) + 1);
        strcpy(store_information->redirect_out,redir_out);
        free(redir_out);
    }
    else
        store_information->redirect_out = NULL;
    

    store_information->pipe =  NULL;

    store_information->arg = arg;

    return store_information;

}

int handle_synt_start(DynArray_T output_lex,char** command_name_dptr,enum States* cur_state,int* is_pipe,int index,char* executable){
    
    int last_index = (output_lex->iLength) - 1;

    struct Token* psToken = (struct Token *)(output_lex->ppvArray[index]);
    enum TokenType etype = psToken->eType;

    struct Token* psToken2 = (struct Token *)(output_lex->ppvArray[last_index]);
    enum TokenType etype2 = psToken2->eType;


    if (etype == PIPE_TOKEN || etype == IN_TOKEN || etype == OUT_TOKEN){
        if (*is_pipe){
            fprintf(stderr,"%s: Pipe or redirection destination is not specified\n",executable);	
			return FALSE;
        }
        else{
            fprintf(stderr,"%s: Missing command name\n",executable);
			return FALSE;
        }
    }

    if (etype == WORD_TOKEN){
        *command_name_dptr = calloc(1,strlen(psToken->pcValue) + 1);
        if (*command_name_dptr == NULL){
            fprintf(stderr,"Cannot allocate memory\n");
   			return FALSE;
        }
        strcpy(*command_name_dptr,psToken->pcValue);
        *cur_state = command_state;
    }

    if (etype2 == PIPE_TOKEN || etype2 == IN_TOKEN || etype2 == OUT_TOKEN){
        fprintf(stderr,"%s: Pipe or redirection destination is not specified\n",executable);
		return FALSE;
    }

    return FLAG;


}

int handle_special_command_arg(DynArray_T lex,enum States* state,int * is_pipe,int * is_redir_in,
  int * is_redir_out,int index,char* executable){
    
    struct Token* psToken = (struct Token *)(lex->ppvArray[index]);
    enum TokenType etype = psToken->eType;

    if (etype == PIPE_TOKEN){
        if (*is_redir_out== TRUE){
			fprintf(stderr,"%s: Multiple redirection of standard out\n",executable);
			return FALSE;
		}
		*state = pipe_state;
	    *is_pipe = TRUE;
    }

    else if (etype == OUT_TOKEN){
        if (*is_redir_out == TRUE){
		    fprintf(stderr,"%s: Multiple redirection of standard out\n",executable);
			return FALSE;
		}
		*state = redir_out_state;
		*is_redir_out= TRUE;

    }
    else if (etype == IN_TOKEN){
        if (*is_redir_in == TRUE || *is_pipe == TRUE){
		    fprintf(stderr,"%s: Multiple redirection of standard input\n",executable);
			return FALSE;
		}
		*state = redir_in_state;
		*is_redir_in= TRUE;

    }

    return FLAG;

    

}

int handle_synt_command(DynArray_T lex,enum States* state,int * is_pipe,int * is_redir_in,
  int * is_redir_out,int index,char* executable,DynArray_T* token_ptr){

    struct Token* psToken = (struct Token *)(lex->ppvArray[index]);
    enum TokenType etype = psToken->eType;

    if (etype == WORD_TOKEN){
        *token_ptr = DynArray_new(0);
        if (!DynArray_add(*token_ptr,psToken->pcValue)){
            fprintf(stderr,"Cannot allocate memory\n");
			return FALSE;
        }
    
        *state = arg_state;
        return FLAG;
    }

    int outcome = handle_special_command_arg(lex,state,is_pipe,is_redir_in,is_redir_out,index,executable);

    if (outcome == FALSE)
        return FALSE;
    else
        return FLAG;
    

}

int handle_synt_arg(DynArray_T lex,enum States* state,int * is_pipe,int * is_redir_in,
  int * is_redir_out,int index,char* executable,DynArray_T* token_ptr){

    struct Token* psToken = (struct Token *)(lex->ppvArray[index]);
    enum TokenType etype = psToken->eType;

    if (etype == WORD_TOKEN){
        if (!DynArray_add(*token_ptr,psToken->pcValue)){
            fprintf(stderr,"Cannot allocate memory\n");
			return FALSE;
        }
        *state = arg_state;
        return FLAG;
    }

    int outcome = handle_special_command_arg(lex,state,is_pipe,is_redir_in,is_redir_out,index,executable);

    if (outcome == FALSE)
        return FALSE;
    else
        return FLAG;


}

int handle_synt_pipe(enum States* state,int* index_p,DynArray_T* token_ptr,char** command_dp,
char** redir_in_dp, char** redir_out_dp, DynArray_T command_array){

    Command_set* new = store_command_information(*command_dp,*redir_in_dp,*redir_out_dp,NULL,*token_ptr);

    if (! DynArray_add(command_array,new)){
        fprintf(stderr,"Cannot allocate memory\n");
        return FALSE;
    }
    *token_ptr = DynArray_new(0);
    *command_dp = NULL;
    *redir_in_dp = NULL;
    *redir_out_dp = NULL;

    *state = Start_state;
    *index_p = *index_p - 1;

    return FLAG;

}

int handle_synt_redirect(enum States* state, DynArray_T lex, char** redir_in_or_out_ptr,int index,char* executable){

    struct Token* psToken = (struct Token *)(lex->ppvArray[index]);
    enum TokenType etype = psToken->eType;

    if (etype == PIPE_TOKEN || etype == IN_TOKEN || etype == OUT_TOKEN ){
        fprintf(stderr,"%s: Pipe or redirection destination is not specified\n",executable);
		return FALSE;
    }

    else if (etype == WORD_TOKEN){
        *redir_in_or_out_ptr = calloc(1,strlen(psToken->pcValue) + 1);
        if (*redir_in_or_out_ptr == NULL){
            fprintf(stderr,"Cannot allocate memory\n");
			return FALSE;
        }
        strcpy(*redir_in_or_out_ptr,psToken->pcValue);
        *state = arg_state;
    }

    return FLAG;



}

int handle_synatctic_analyzer(DynArray_T output_lex, DynArray_T command_array,char* executable, enum States* current_state,
char** command_dptr,char** redir_in_dptr,char** redir_out_dptr,int * is_pipe,int* is_redir_in,int* is_redir_out){

    int i = 0,outcome;
    DynArray_T token = DynArray_new(0);

    //printf("%d\n",output_lex->iLength);

    while (i < output_lex->iLength){

        //printf("\nindex %d  : command %s : redir in %s: redir out %s\n",i,*command_dptr,*redir_in_dptr,*redir_out_dptr);
        //printf("_______(((((((\n");
        //DynArray_map(token,printanyToken,NULL);
        //fflush(stdout);

        if (*current_state == Start_state){

            outcome = handle_synt_start(output_lex,command_dptr,current_state,is_pipe,i,executable);

            if (outcome == FALSE)
                return FALSE;

        }
        else if (*current_state == command_state){

            outcome = handle_synt_command(output_lex,current_state,is_pipe,is_redir_in,is_redir_out,i,executable,&token);

            if (outcome == FALSE)
                return FALSE;

        }
        else if (*current_state == redir_in_state){

            outcome = handle_synt_redirect(current_state,output_lex,redir_in_dptr,i,executable);

            if (outcome == FALSE)
                return FALSE;


        }
        else if (*current_state == redir_out_state){

            outcome = handle_synt_redirect(current_state,output_lex,redir_out_dptr,i,executable);

            if (outcome == FALSE)
                return FALSE;


        }

        else if (*current_state == arg_state){

            outcome = handle_synt_arg(output_lex,current_state,is_pipe,is_redir_in,is_redir_out,i,executable,&token);

            if (outcome == FALSE)

                return FALSE;

        }
        else if (*current_state == pipe_state){

            outcome = handle_synt_pipe(current_state,&i,&token,command_dptr,redir_in_dptr,redir_out_dptr,command_array);

            if (outcome == FALSE)

                return FALSE;


        }




        i++;
    }

    Command_set* new = store_command_information(*command_dptr,*redir_in_dptr,*redir_out_dptr,NULL,token);

    if (!DynArray_add(command_array,new)){
        fprintf(stderr,"Cannot allocate memory\n");
		return FALSE;
    }

    return TRUE;

}


int syntaticLine(DynArray_T output_lexical, DynArray_T command_,char *executable,char* pipe){
    
    int is_there_pipe = FALSE,is_there_redir_in = FALSE,is_there_redir_out = FALSE;

    char* command = NULL, *redir_in = NULL,*redir_out = NULL;


    if (!output_lexical->iLength){
        return TRUE;
    }


    enum States state = Start_state;

    // call the function and return appropriately
    int outcome = handle_synatctic_analyzer(output_lexical,command_,executable,&state,&command,&redir_in,&redir_out,&is_there_pipe,
    &is_there_redir_in,&is_there_redir_out);

    if (outcome == TRUE)
        return TRUE;
    else
        return FALSE;
    


}

/*
int main()
{
   char acLine[MAX_LINE_SIZE];
   DynArray_T oTokens,oTokens2;
   int iSuccessful,i2;
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

          oTokens2 = DynArray_new(0);
          if (oTokens == NULL){
              exit(0);
          }
          i2 = syntaticLine(oTokens,oTokens2,b,NULL);
          
          DynArray_map(oTokens2, printCommand, NULL);
          printf("\n");
          //DynArray_map(oTokens2, freeToken, NULL);
          DynArray_free(oTokens2);

      }
      printf("------------------------------------\n");

      DynArray_map(oTokens, freeToken, NULL);
      DynArray_free(oTokens);


   }

}*/



