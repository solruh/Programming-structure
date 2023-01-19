/************************************************
Name- Reda 
assignment no -3              
*************************************************/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

#define UNIT_ARRAY_SIZE 64

/*************************************************************
the struct UserInfo has fields character pointer name,id 
which point to name and id respectively and integer purchase.
**************************************************************/

struct UserInfo {
  char *name;                // customer name
  char *id;                  // customer id
  int purchase;              // purchase amount (> 0)
};
typedef struct UserInfo U_info;
/*************************************************************
the struct DB has fields integer curArrSize and numItems.
and a struct UserInfo pointer pArray.
**************************************************************/
struct DB {
  U_info *pArray;   // pointer to the array
  int curArrSize;   // current array size (max # of elements)
  int numItems;     // # of stored items, needed to determine
			     // # whether the array should be expanded
			     // # or not
};

typedef struct DB* DB_T;

/***********************************************************************
The fucntion Createcustomer takes no parameter and retunrs a pointer to
the memory block for the new DB_T object if there is enough memory to 
be allocated. if there is no enough memeory space to allocate to either
pointer to the array pArray or to the pointer that points to the struct 
DB, it returns null.
************************************************************************/
DB_T
CreateCustomerDB(void)
{
  DB_T d;

  d = (DB_T) calloc(1, sizeof(struct DB));
  if (d == NULL) {
    fprintf(stderr, "Can't allocate a memory for DB_T\n");
    return NULL;
  }
  d->curArrSize = UNIT_ARRAY_SIZE; // start with 64 elements
  d->pArray = (U_info *)calloc(d->curArrSize,
               sizeof(U_info));
  if (d->pArray == NULL) {
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	    d->curArrSize);
    free(d);
    return NULL;
  }
  return d;
  return NULL;
}
/**************************************************************************
the function DestroyCustomerDB takes a pointer to the struct DB as a 
parameter. it does not have a return value rather it frees all the 
memory occupied by the DB_T object and all the underlying objects.
**************************************************************************/
void
DestroyCustomerDB(DB_T d)
{
  /* fill out this function */
  assert(d!=NULL);
  for(int i=0;i<d->curArrSize;i++)
  {
    if(d->pArray[i].name!=NULL) free(d->pArray[i].name);
    if(d->pArray[i].id!=NULL) free(d->pArray[i].id);
}
  free(d->pArray);
  free(d);
}
/***************************************************************************
the function RegisterCustomer takes DB_T d, id, name and purchase as a
parameter. if either an element with the same name or id already exists
or if the purhcase amount is non-positive it is a failure and the function 
returns -1. also if the d or id or name is null, it is a failure and the 
function returns -1. if any of the above doesn't happen(i.e. on success)
the function returns 0.
***************************************************************************/
int
RegisterCustomer(DB_T d, const char *id,
		 const char *name, const int purchase)
{
  /* fill out this function */
  assert(purchase>0 && id!=NULL && name!=NULL);
  assert(d!=NULL);
  if(d==NULL || name==NULL || id==NULL || purchase<=0) return -1;
  for(int i=0; i<d->curArrSize;i++){
    if(d->pArray[i].name!=NULL){
        if(strcmp(d->pArray[i].name,name)==0) return -1;}
          if(d->pArray[i].id!=NULL){
          if(strcmp(d->pArray[i].id,id)==0) return -1;}
  
  }
  if(d->numItems==d->curArrSize)  {
    d->pArray=(U_info *)realloc(d->pArray,sizeof(U_info)*(d->curArrSize + UNIT_ARRAY_SIZE));
    if(d->pArray==NULL) return -1;
   
    U_info *p=d->pArray;
    int max_index;
     max_index=d->curArrSize + UNIT_ARRAY_SIZE-1;
     p=p+   (d->curArrSize);
     printf("i am here1111111111\n");
     while(p < (d->pArray + max_index )){
      p->name=NULL; 
      p->id=NULL;
      p->purchase=0;
      p++;
     }
    p->name = NULL;
    p->id = NULL;
    p->purchase = 0;
     printf("i am here22222222\n");
     d->curArrSize=d->curArrSize + UNIT_ARRAY_SIZE;
    
    }
    printf("i am here\n");
  
    for(int i=0;i< d->curArrSize;i++){
        //
        if(d->pArray[i].name==NULL && d->pArray[i].id==NULL){
                    printf("here is it\n");
                    printf("%d\n", i);
                   (d->pArray[i].name)=strdup(name);
                   (d->pArray[i].id)=strdup(id);
                   (d->pArray[i].purchase)=purchase;
                   printf("fuck\n");
                   if(d->pArray[i].name==NULL && d->pArray[i].id!=NULL) {free(d->pArray[i].id); return -1;} 
                   else if(d->pArray[i].name!=NULL && d->pArray[i].id==NULL) {free(d->pArray[i].name); return -1;}
                   break;

              }
              
           }
          d->numItems+=1;
          return 0;
      }

/************************************************************************
the function UnregisterCustomerByID takes the a pointer to the struct DB
and a pointer to a constant char id as a parameter. if either the id is 
not found in the array or d is null it's a failure and the function 
returns -1. if d is not null and we found the id in the array, then all
the memory allocated for that item is freed and the functions returns 0.
************************************************************************/
int
UnregisterCustomerByID(DB_T d, const char *id)
{
  /* fill out this function */
  assert(d!=NULL);
  assert(id!=NULL);
  if(d==NULL || id==NULL) return -1;
  int x=0;  // to check if id exists in the array of ids
  int id_index=0; //to track the index where id occurs
  for(int i=0;i<d->curArrSize;i++){
    //how do i access each pointer to the id's of the struct user info.
    //both as a pointer and as a string??
    if(d->pArray[i].id!=NULL && strcmp(d->pArray[i].id,id)==0) /*or (*(d->pArray[d->numItems]->id))*/ 
      {x=1;
      break;}
      id_index++;
  }
  if(x) {
    //not sure if we need to do this?
    free(d->pArray[id_index].name);
    free(d->pArray[id_index].id);
    //free(d->pArray[name_index].purchase);
    d->pArray[id_index].id = NULL;
    d->pArray[id_index].name = NULL;
    d->pArray[id_index].purchase = 0;
    //free(d->pArray[id_index]); // is this enough to replace all the three free's??
    d->numItems--;
    return 0;
  }
  else return -1;
}
/************************************************************************
the function UnregisterCustomerByName takes the a pointer to the struct DB
and a pointer to a constant char name as a parameter. if either the name
is not found in the array or d is null or name is null it's a failure and 
the function returns -1. if d is not null and we found the name in the 
array, then all the memory allocated for that item is freed and the 
functions returns 0.
************************************************************************/
int
UnregisterCustomerByName(DB_T d, const char *name)
{
  /* fill out this function */
  assert(d!=NULL);
  assert(name!=NULL);
  if(d==NULL || name==NULL) return -1;
  int x=0;  // to check if name exists in the array of names
  int name_index=0;//to track the index of the name
  for(int i=0;i<d->curArrSize;i++){
    if(d->pArray[i].name!=NULL && strcmp(d->pArray[i].name,name)==0) {
      x=1;
      break;
    }
    name_index++;
  }
  if(x) {
    free(d->pArray[name_index].name);
    free(d->pArray[name_index].id);
    d->pArray[name_index].id = NULL;
    d->pArray[name_index].name = NULL;
    d->pArray[name_index].purchase = 0;
    //free(d->pArray[name_index].purchase);
    //free(d->pArray[id_index]); // is this enough to clear the memory??
    d->numItems--;
    return 0;
  }
  else return -1;
}
/************************************************************************
the function GetPurchaseByID takes the a pointer to the struct DB
and a pointer to a constant char id as a parameter.If either there is no 
customer in the array whose ID matches the given one or if the pointer to 
the struct DB or the pointer to the id is null, then it is a failure and 
the function returns -1. else it returns the purchase amount.
************************************************************************/
int
GetPurchaseByID(DB_T d, const char* id)
{
  /* fill out this function */
  assert(d!=NULL);
  assert(id!=NULL);
  if(d==NULL || id==NULL) return -1;
  int x=0;  // to check if id exists in the array of ids
  int id_index=0; //to track the index where id occurs
  for(int i=0;i<d->curArrSize;i++){
    if(d->pArray[i].id!=NULL && strcmp(d->pArray[i].id,id)==0) {
      x=1; break; }
      id_index++;
  }
  if(x) return (d->pArray[id_index].purchase);
  else return -1;
}
/************************************************************************
the function GetPurchaseByName takes the a pointer to the struct DB
and a pointer to a constant char name as a parameter.If either there is no 
customer in the array whose Name matches the given one or if the pointer 
to the struct DB or the pointer to the name is null, then it is a failure 
and the function returns -1. else it returns the purchase amount.
************************************************************************/
int
GetPurchaseByName(DB_T d, const char* name)
{
  /* fill out this function */
  assert(d!=NULL);
  assert(name!=NULL);
  if(d==NULL || name==NULL) return -1;
  int x=0;  // to check if id exists in the array of ids
  int name_index=0; //to track the index where id occurs
  for(int i=0;i<d->curArrSize;i++){
    if(d->pArray[i].name!=NULL && strcmp(d->pArray[i].name,name)==0) {
      x=1; break; }
      name_index++;
  }
  if(x) return (d->pArray[name_index].purchase);
  else return -1;
}
/***************************************************************************
the function GetSumCustomerPurchase takes  the a pointer to the struct DB 
and FUNCPTR_T fp as parameters. if either d or fp is null, then it is a 
failure and the function returns -1. if that is not the case, the function 
iterates every user item in d, calls fp once for each user item, and returns 
the sum of the numbers returned by fp.
***************************************************************************/
int
GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp)
{
  /* fill out this function */

  assert(d!=NULL);
  assert(fp!=NULL);
  if(d==NULL || fp==NULL) return -1;
  int total_Sum=0;  // to indicate the total purchase amount
  for(int i=0;i<d->curArrSize;i++){
    if(d->pArray[i].name!=NULL && d->pArray[i].id!=NULL){
      total_Sum+=fp(d->pArray[i].id,d->pArray[i].name,d->pArray[i].purchase);
    }
  }
  return total_Sum;
}
