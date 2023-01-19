/*************************************************
Name: Geberemedhin Derbew Reda   Assignment 1
Student ID : 20180756              wc209.c            
**************************************************/
#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<assert.h>
#include<stdlib.h>
/*enum data types of possible states*/
enum STATE {START_STATE,IS_SPACE_STATE,CHAR_STATE,SLASH_STATE,COMMENT_START_STATE,STATE_NextTO_COMMENTSTART,STATE_END_COMMENT};
/*enum data type functions that accept input characters and change 
states and count number of lines,words and characters*/
enum STATE Start(int c);
enum STATE slash_State(int c);
enum STATE Space(int c);
enum STATE Char(int c);
enum STATE Comment_start(int c);
enum STATE Next_to_comment(int c);
enum STATE End_comment(int c);
enum STATE dfastate=START_STATE;  // initilize the state to start state
enum STATE prior;   /*a state prior to the comment starting slash state which
to avoid confusion if we should increase word_count after we end with comment*/

/*global variables to keep track of each counts*/
int c;
int character_count=0;
int word_count=0;
int line_count=0;
int begin_of_comment=0;  // to indicate the line where comment starts
/*************************************************************************
in the main function, the state is checked every time we get a character 
and state is changed accordingly. besides it checks if error happens and 
takes action accordingly and depending on the state the file finishes its 
state, word and character are incremented
*****************************************************************************/

int main(void) {

	while((c=getchar())!=EOF){
		if(line_count==0) line_count++;
		if(c=='\n' && dfastate==STATE_END_COMMENT) line_count++;
	switch(dfastate){
		case START_STATE: dfastate=Start(c);
			break;
		case SLASH_STATE: dfastate=slash_State(c);
			break;
		case IS_SPACE_STATE: dfastate=Space(c);
			break;
		case CHAR_STATE: dfastate=Char(c);
			break;
		case COMMENT_START_STATE: dfastate=Comment_start(c);
			break;
		case STATE_NextTO_COMMENTSTART: dfastate=Next_to_comment(c);
			break;
		case STATE_END_COMMENT: dfastate=End_comment(c);
			break;
		default:
			assert(0);
			break;
	}}
	
	if(dfastate==COMMENT_START_STATE || dfastate==STATE_NextTO_COMMENTSTART) {
		fprintf(stderr,"\nError: line %d: unterminated comment\n",begin_of_comment);
		exit(EXIT_FAILURE);
	}
	else if(dfastate==STATE_END_COMMENT) {
	 	
	 	if(prior==SLASH_STATE || prior==CHAR_STATE) word_count++;
	 	character_count++;
	 	
	 }
	else if (dfastate==SLASH_STATE || dfastate==CHAR_STATE)  word_count++;
	
	printf("%d %d %d\n",line_count,word_count,character_count);
	return 0;
}
/* **************************************************
the start function gets the first input and returns the appropriate state
along with tracking the previous state and incrementing character count unless 
the input is an empty file.
****************************************************/
enum STATE Start(int c){
	if(c=='/') {
		dfastate=SLASH_STATE;
		prior=START_STATE;
	}
	    
	else if(c=='\v' || c=='\f' || c=='\t' || c=='\r' || c==' ') {
		dfastate=IS_SPACE_STATE;
	}
	else if(c=='\n'){
		dfastate=IS_SPACE_STATE;
		line_count++;
	}
	else dfastate=CHAR_STATE;
		
	character_count++;
	
	return dfastate;
}
/************************************************************************
slash function gets an input character and returns an appropritate state
along with incrementing character_count, words and lines.
************************************************************************/
enum STATE slash_State(int c){
	
	if(c=='*') {
		begin_of_comment=line_count;
		dfastate=COMMENT_START_STATE;
		character_count--;
	}
	else if(c=='\v' || c=='\t' || c=='\r' || c==' ' || c=='\f'){
		character_count++;
		word_count++;
		dfastate=IS_SPACE_STATE;
	}
	
	else if(c=='\n') {
		line_count++;
		character_count++;
		word_count++;
		dfastate=IS_SPACE_STATE;
	}
	else if(c=='/') {
		dfastate=SLASH_STATE;
		prior=SLASH_STATE;
		character_count++;
	}
	else {
		dfastate=CHAR_STATE;
		character_count++;
	}
	return dfastate;
}
/*************************************************************************** 
Space function gets input characters and then changes states, increments 
characters and lines accordingly(not words) and also it keeps track of prior state.
finally returns the state.
***********************************************************************************/
enum STATE Space(int c){
	if(c=='\n'){
		dfastate=IS_SPACE_STATE;
		line_count++;
	}
	else if(c=='\v' || c=='\t' || c=='\r' || c==' ' || c=='\f') dfastate=IS_SPACE_STATE;
	else if(c=='/') {
		dfastate=SLASH_STATE;
		prior=IS_SPACE_STATE;
	}
	else dfastate=CHAR_STATE;
	character_count++;
	return dfastate;
}
/***************************************************************************
gets input characters and increments characters, words and lines accordingly
 it also keeps of the prior state. finally returns the state.
 *************************************************************************/
enum STATE Char(int c){
	if(c=='/') { 
		dfastate=SLASH_STATE;
		prior=CHAR_STATE;
	}
	else if(c=='\n'){
		dfastate=IS_SPACE_STATE;
		line_count++;
		word_count++;
	}
	else if(c=='\v' || c=='\t' || c=='\r' || c==' ' || c=='\f') {
		dfastate=IS_SPACE_STATE;
		word_count++;
	}
	else dfastate=CHAR_STATE;
	character_count++;
	return dfastate;
}
/****************************************************************************
gets input characters and increments lines and characters accordingly. no need to keep 
track of prior state. finally returns the state.
*******************************************************************************/
enum STATE Comment_start(int c){
	if(c=='\n'){
		character_count++;
		line_count++;
		dfastate=COMMENT_START_STATE;
	}
	else if(c=='*') dfastate=STATE_NextTO_COMMENTSTART;
	else dfastate=COMMENT_START_STATE;
	
	return dfastate;
}
/**************************************************************************
gets input characters and increments lines and characters accordingly. 
no need to keep track of prior state. finally returns the state.
****************************************************************************/
enum STATE Next_to_comment(int c){
	if(c=='\n'){
		character_count++;
		line_count++;
		dfastate=COMMENT_START_STATE;
	}
	else if(c=='*') dfastate=STATE_NextTO_COMMENTSTART;
	else if(c=='/') {
		
		dfastate=STATE_END_COMMENT;
	}
	else dfastate=COMMENT_START_STATE; 
	
	return dfastate;
}
/****************************************************************************************
gets input characters and increments words, characters and lines accordingly and it keeps 
track of the prior state. finally returns the state.
*****************************************************************************************/
enum STATE End_comment(int c){
		character_count++;

		if(prior==SLASH_STATE || prior==CHAR_STATE) word_count++;
		
		if(c=='/') {
			character_count++;
			dfastate=SLASH_STATE;
			prior=STATE_END_COMMENT;
		}
		else if(isspace(c)){ 
			character_count++;
			dfastate=IS_SPACE_STATE;
		}
		else{
			character_count++;
			dfastate=CHAR_STATE;
		}

		return dfastate;
}