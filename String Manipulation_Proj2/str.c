/* ********************************************************
Name - Reda 
*********************************************************/
#include<stdio.h>
#include<assert.h>
#include "str.h"
/*********************************************************
StrGetLength takes pcSrc as a parameter and it assersts 
pcSrc is not null. it returns the length of the string.
*********************************************************/
size_t StrGetLength(const char* pcSrc){
	const char *pcEnd;
	pcEnd=pcSrc;
	assert(pcSrc!=NULL);
	while(*pcEnd!='\0'){
		pcEnd++;
	}
	return pcEnd-pcSrc;
}
/**********************************************************
StryCopy takes pcDest and pcSrc as its parameters and 
asserts they are not null. it then creates temppcDest equal
to pcDest. it copies each characters of pcSrc to temppcDest 
and it returns pcDest.
**********************************************************/
char *StrCopy(char *pcDest, const char* pcSrc){
	assert(pcSrc!=NULL && pcSrc!=NULL);
	char *temppcDest=pcDest;
	while(*pcSrc!='\0'){
		*temppcDest=*pcSrc;
		 pcSrc++;
		 temppcDest++;
		}
	*temppcDest='\0';      // this will include the null character
	return pcDest;
}
/*********************************************************
StrCompare takes pcS1 and pcS2 as its pointer parameters 
and asserts they are not null. it then compares each char
characters of the two arguments and returns an integer less 
than zero if pcS1 is less than pcS2, equal to zero if 
pcS1 and pcS2 are equal or greater than zero if pcS1 is 
greater than pcS2 based on the ASCII number of characters.
***********************************************************/
int StrCompare(const char* pcS1, const char* pcS2){
	assert(pcS1!=NULL && pcS2!=NULL);
	while(*pcS2 || *pcS1){
		if(*pcS1!=*pcS2) break;
		else{
		pcS1++;
		pcS2++;
		}
	}

	return (int)(*pcS1 -*pcS2);

}
/**********************************************************
StrSearch takes pcHaystack and pcNeedle as its parameters 
and asserst both are not null. it then searches pcNeedle
in pcHaystack and it pcNeedle is successfully found on
pcHaystack, it returns startOfpcNeedleInpcHay(which points
to the pcHaystak starting from the first occurruence of 
pcNeedle in pcHaystack) but if it fails to find pcNeedle 
in pcHaystack, it returns null.
***********************************************************/
char *StrSearch(const char* pcHaystack, const char *pcNeedle){
	assert(pcHaystack!=NULL && pcNeedle!=NULL);
	const char *temp_pcNeedle = pcNeedle;
	while(*pcHaystack!='\0'){
		char *startOfPcNeedleInpcHay=(char *)pcHaystack;  //casting to char type(not const char)
		while(*temp_pcNeedle!='\0' && *pcHaystack!='\0'){
			if((*temp_pcNeedle==*pcHaystack)){
			pcHaystack++;
			temp_pcNeedle++;
			}
			else break;
			}
		if(*temp_pcNeedle=='\0') return startOfPcNeedleInpcHay;
		pcHaystack =startOfPcNeedleInpcHay+1;
		temp_pcNeedle = pcNeedle;
	}
	
	return NULL;
}




/*********************************************************
StrConcat takes pcDest and pcSrc as its parameters and it 
asserts both parameters are not null. it adds each 
characters of pcSrc to pcDest(without overwriting pcDest)
it then returns pcDest. 
**********************************************************/
char *StrConcat(char *pcDest, const char* pcSrc){
	assert(pcSrc!=NULL && pcDest!=NULL);
	char *temppcDest=pcDest;  // points to the same content as pcDest but pointer location can differ
	while(*temppcDest!='\0') temppcDest++;
	while(*pcSrc!='\0'){
		*temppcDest=*pcSrc;
		pcSrc++;
		temppcDest++;
	}
	*temppcDest='\0';  //this will include a null character
	return pcDest;
}
