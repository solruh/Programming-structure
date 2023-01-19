
#include "dynarray_gere.h"
#include "dfa_gere.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#define _D_DEFAULT_SOURCE
#define ALARM 5

#define NUM_BUILTIN 4

char * executable;

int j = 2;

#define pop_cmd(ptr,index) (Command_set *)(ptr->ppvArray[index])

#define ADD(value1,value2,value3) value1 + value2 + value3

int isValid( const char * command)
{
    assert(command != NULL);

    
    char * builtins[NUM_BUILTIN] = {"cd","exit","setenv","unsetenv"}; 

    for(int i = 0;i < NUM_BUILTIN;i++)
    {
        if(strcmp(command,builtins[i]) == 0 )
        {
            return 1;
        }
    }

    return 0;
}

static int execute_builtins(const char ** arg_list,int num_args)
{
    assert(executable != NULL && arg_list != NULL);


    if(isValid(arg_list[0]) == 0)
    {
        return 1;
    }

    if((strcmp(arg_list[0],"setenv")) == 0)
    {
        if(num_args != 2 && num_args != 3)
        {
            fprintf(stderr,"%s: setenv takes one or two parameters\n",executable);
	   		return 0;
        }

        if(num_args == 2)
        {
            if (setenv(arg_list[1],"",1) == -1)
            {
	  				perror(executable);
	  				return 0;
	  	    }
        }
        else
        {
             if (setenv(arg_list[1],arg_list[2],1)  == -1)
             {
	   			perror(executable);
	   			return 0;
	   		 }
        }
        
    }
    else if((strcmp(arg_list[0],"unsetenv")) == 0)
    {

        if(num_args != 2)
        {
            fprintf(stderr,"%s: unsetenv takes one parameter\n",executable);
	  	    return 0;
        }

         if(unsetenv(arg_list[1]) == -1){
	  			perror(executable);
	  			return 0;
         }

    }
    else if((strcmp(arg_list[0],"cd")) == 0)
    {
        if(num_args != 1 && num_args != 2)
        {
            fprintf(stderr,"%s: cd takes one parameter\n",executable);
            return 0;
        }

        if(num_args == 1)
        {
            if ( chdir(getenv("HOME")) == -1 ){
	  				perror(executable);
	  				return 0;
	  	    }
        }
        else
        {
            if ( chdir(arg_list[1]) == -1 ) {
	  				perror(executable);
	  				return 0;
	  		}
        }
        
    }
    else{

        if(num_args != 1)
        {
            fprintf(stderr,"%s: exit does not take any parameters\n",executable);
	  	    return 0;
        }

        printf("\n");
		exit(0);
    }

    return 1;
}


static int redirect_stdin(const char * filename)
{
    assert(filename != NULL && executable != NULL);

    int fd;

     if((fd = open(filename,O_RDONLY)) < 0)
     {
         perror(executable);
		 return 0;
     }

	if (dup2(fd,0) < 0){
		perror(executable);
		if (close(fd) < 0){
			perror(executable);
		}
		return 0;
    }

    if (close(fd) < 0)
    {
        perror(executable);
        return 0;
    }

    return 1;


}

static int redirect_stdout(const char * filename)
{
     assert(filename != NULL && executable != NULL);
     
     int fd;

     /* if not created */
     if ((fd = creat(filename,0600)) < 0)
     {
		perror(executable);
		return 0;
	 }

     if (dup2(fd,1) < 0)
     {
		perror(executable);
		if (close(fd) < 0){
		   perror(executable);
		}
		return 0;
                
	 }

     return 1;

}

DynArray_T setup_args(DynArray_T cmd_arr,int index)
{
    Command_set * cmd_set = pop_cmd(cmd_arr,index);


    DynArray_T args = cmd_set->arg;


    /* append NULL */
    if (DynArray_add(cmd_set->arg,NULL) == 0){
		fprintf(stderr,"Cannot allocate memory\n");
		return 0;
	}

    /* append command name in the front */
    if (DynArray_addAt(args,0,(void *)cmd_set->command) == 0){
		fprintf(stderr,"Cannot allocate memory\n");
		return NULL;
	}

    return args;
}


void do_redirect (Command_set * cmd_set)
{
    const char * in,*out;

    in = cmd_set->redirect_in;
    out = cmd_set->redirect_out;


    if(in)
    {
         redirect_stdin(in);
    }
    if(out)
    {
        redirect_stdout(out);
    }

    return;
}

static int  single_execute(DynArray_T cmd_arr)
{
    //const char * in,*out;
    pid_t pid;

    /* set up the arguments in some data structure */
    DynArray_T arg = setup_args(cmd_arr,0);
    Command_set * cmd_set = pop_cmd(cmd_arr,0);


    if(arg == NULL)
    {
        return 0;
    }

    /* make sure only the child calls this function */
    fflush(NULL);
    pid = fork();

    if(pid < 0)
    {
        perror(cmd_set->command);
        return 0;
    }

    if(pid != 0)
        wait(NULL);

    else
    {
        signal(SIGINT,SIG_DFL);

        /* redirect the file based on command set */
        do_redirect(cmd_set);

        //execute
        fflush(stdout);
        execvp(((char * const *)arg->ppvArray)[0],(char * const *)arg->ppvArray);
        /* should not be here */
        exit(EXIT_FAILURE); 
    }
    
    return 1;
}


void close_fds(int * fd,int num_pipes)
{
    for(int tmp = 0;tmp < 2 *num_pipes;tmp++)
    {
        if ( close(fd[tmp]) < 0)
        {
            perror(executable);
            exit(EXIT_FAILURE);
        }
    }
}


static void do_pipe(DynArray_T cmd_list, int * fd,int cmd_no,int fd_index)
{
     Command_set * cmd_set = pop_cmd(cmd_list,cmd_no);
     //char * const * array;
     pid_t pid;

     DynArray_T args = setup_args(cmd_list,cmd_no);
    int num_pipes = (cmd_list->iLength)-1;

     if(args == NULL)
     {
        return;
     }

     //array = (char * const *)args->ppvArray;

     fflush(NULL);

     if((pid = fork()) < 0)
     {
         perror(executable);
         exit(EXIT_FAILURE);
     }

    if(pid == 0)
    {
        signal(SIGINT,SIG_DFL);
		signal(SIGQUIT,SIG_DFL);

        if(cmd_no  != 0 && cmd_no != num_pipes)
        {
            /* redirect  stdin*/
            if (dup2(fd[cmd_no * 2-2],0) < 0)
            {
                perror(executable);
                exit(EXIT_FAILURE);
	     	}
            /* redirect stdout */
            if (dup2(fd[cmd_no * 2+1],1) < 0)
            {
                perror(executable);
                exit(EXIT_FAILURE);
            }            
        }

        // corner commands
        else
        {
            /* code */
            do_redirect(cmd_set);

            if(cmd_no == 0)
            {
                if (dup2(fd[cmd_no * 2+1],1) < 0){
	     			perror(executable);
	     			exit(EXIT_FAILURE);
	     		}	
            }

            else
            {
                if(dup2(fd[cmd_no * 2 - 2], 0) < 0 ){
	     			perror(executable);
	     			exit(EXIT_FAILURE);
	     		}
            }
            
        }

        close_fds(fd,num_pipes);
        execvp(((char * const *)args->ppvArray)[0],(char * const *)args->ppvArray);
    }

}

static int piped_executions(DynArray_T command)
{

    assert(command != NULL);

    // command length is the number of commands
    int num_pipes = (command->iLength)-1;
    int fd[2 * num_pipes],fd_index = 1;

    /* set up the fd */
    for(int tmp = 0;tmp < num_pipes;tmp++)
    {
        if((pipe((2 * tmp) + fd )) < 0)
        {
            perror(executable);
            return 0;
        }
    }

    for(int cmd_no = 0;cmd_no < num_pipes + 1;)
    {

        do_pipe(command,fd,cmd_no,fd_index);

        /* wait for childrens to execute the pipes */
        /*for(int tmp = 0;tmp < num_pipes + 1;tmp++)
        {
            wait(NULL);
        }*/

        /* skip the receiver and sender fds */
        fd_index += 2;
        cmd_no++;
    }

    close_fds(fd,num_pipes);
    /* wait for childrens to execute the pipes */
    for(int tmp = 0;tmp < num_pipes + 1;tmp++)
    {
        wait(NULL);
    }


    return 1;
}

FILE* open_ishrc(){

    char* ptr = getenv("HOME"),*addr,*ishrc;
    //int addr_len;
    ishrc = "/.ishrc";

    if (ptr == NULL){
        fprintf(stderr,"Couldn't find the home directory\n");
        return NULL;
    }

    int len = ADD(strlen(ishrc),strlen(ptr),1);

    //addr_len = strlen(ptr);
    addr = calloc(len,sizeof(char));

    if (addr == NULL){
        fprintf(stderr,"memory allocation failed\n");
        return NULL;
    }

    strcpy(addr,getenv("HOME"));

    strcat(addr,ishrc);

    FILE* open_file = fopen(addr,"r");

    if (open_file == NULL){
        fprintf(stderr,"Couldn't open the file\n");
        return NULL;
    }

    return open_file;

}



void flushed_printf(const char *ptr, int code){
    printf("%s",ptr);
    if (code)
        fflush(stdout);
    else
        fflush(NULL);
    
}



void handle_the_filepointers(FILE ** fp_db,char* buffer,char* file_stream){
    if (file_stream == NULL){
        if (*fp_db){
            fclose(*fp_db);
            *fp_db = NULL;
        }
    }
    if (*fp_db)
        flushed_printf(buffer,0);
}



int handle_lex_and_syn(DynArray_T cmd_arr,DynArray_T token_arr,char* buffer,char* argv0){
    if (!token_arr)
        return 0;

    int outcome_of_lex = lexLine(buffer,token_arr,argv0);

    if (outcome_of_lex == 0 || !token_arr->iLength){
        return 0;
    }
    if (!cmd_arr)
        return 0;

    int outcome_of_syn = syntaticLine(token_arr,cmd_arr,argv0,NULL);

    if(outcome_of_syn == 0)
        return 0;
    
    return 1;



}

int  wrapper_for_built_in(Command_set* cmd,DynArray_T token_array){

    if (DynArray_add(cmd->arg,NULL) == 0){
        return 0;
    }

    if (DynArray_addAt(cmd->arg,0,(void *)cmd->command) == 0)
        return 0;
    
    DynArray_T array_of_string = cmd->arg;
    const char** b = (const char **)array_of_string->ppvArray;

    execute_builtins(b,token_array->iLength);



    return 1;


}

void initialize_signals(sigset_t* set_ptr){
    sigemptyset(set_ptr);
    sigaddset(set_ptr,SIGALRM);
    sigaddset(set_ptr,SIGQUIT);
    sigaddset(set_ptr,SIGINT);
}

void quit_after_5(int sig){

    j = j - 1;

    if (j < 1){
        exit(0);
    }
    else if (j == 1){
        flushed_printf("\nType Ctrl-\\ again within 5 seconds to exit.\n",1);
        alarm(ALARM);
    }
    
}


void reset(int sig){
    j = 2;
}

void install_signal(){
    signal(SIGINT,SIG_IGN);
    signal(SIGQUIT,quit_after_5);
    signal(SIGALRM,reset);
}





int main(int argc , char ** argv)
{
    // handle signals properly

    sigset_t s;
    

    initialize_signals(&s);
    int sig = sigprocmask(SIG_UNBLOCK,&s,NULL);
    assert(sig == 0);

    //IGNORE SIGINT
    install_signal();


    FILE* fp = open_ishrc();

    int outcome;

    executable = calloc(strlen(argv[0]) + 1,sizeof(char));
    strcpy(executable,argv[0]);


    DynArray_T Array_of_tokens,Array_of_commands;

    Command_set* cmd_arr;

    flushed_printf("% ",1);

    if (fp == NULL)
        return EXIT_SUCCESS;
    
    char* input_stream,*file_stream,buffer[MAX_LINE_SIZE];
    //buffer2[MAX_LINE_SIZE];

    while (  (fp && (file_stream = fgets(buffer,MAX_LINE_SIZE,fp))) || 
    (input_stream = fgets(buffer,MAX_LINE_SIZE,stdin)) ){

        Array_of_commands = DynArray_new(0);
        Array_of_tokens = DynArray_new(0);

        handle_the_filepointers(&fp,buffer,file_stream);

        outcome =  handle_lex_and_syn(Array_of_commands,Array_of_tokens,buffer,argv[0]);

        if (outcome == 0){
            flushed_printf("% ",0);
            continue;
        }

        cmd_arr = (Command_set *)(Array_of_commands->ppvArray[0]);

        if (cmd_arr->command == NULL){
            continue;
        }

        if (isValid(cmd_arr->command)){
           outcome =  wrapper_for_built_in(cmd_arr,Array_of_tokens);
            if (outcome == 0){
                flushed_printf("% ",0);
                continue;
            }
            
        }
        else{

            if (Array_of_commands->iLength == 1){
                single_execute(Array_of_commands);
            }
            else{
                piped_executions(Array_of_commands);
            }
        }

        flushed_printf("% ",0);

    }

    free(executable);

    return 0;

    


}