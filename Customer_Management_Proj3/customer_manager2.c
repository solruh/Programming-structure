/************************************************
Name- Reda Geberemedhin DErbew Reda  ID- 20180756
assignment no -3              
*************************************************/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

#define UNIT_BUCKET_SIZE 1024
/*************************************************************
the struct UserInfo has fields character pointer name,id 
which point to name and id respectively and struct user info
pointers nextId and nextName which point to next struct user
info. and it also contains an integer member purchase.
**************************************************************/
struct UserInfo {
  char *name;                // customer name
  char *id;                  // customer id
  int purchase;              // purchase amount (> 0)
  struct UserInfo *nextId;
  struct UserInfo *nextName;   // Is this really important??
};
typedef struct UserInfo U_info;

/*************************************************************
the struct DB has fields HashTable pointers ht_id and ht_name
and an integer member numItems.
**************************************************************/
struct DB{
  U_info **ht_id;
  U_info **ht_name;
  int numItems;
  int curArrSize;
};

typedef struct DB *DB_T;


enum {HASH_MULTIPLIER = 65599};

static int hash_function(const char *pcKey, int iBucketCount)

/* Return a hash code for pcKey that is between 0 and iBucketCount-1,
   inclusive. Adapted from the EE209 lecture notes. */
{
  
   int i;
   unsigned int uiHash = 0U;
   for (i = 0; pcKey[i] != '\0'; i++)
      uiHash = uiHash * (unsigned int)HASH_MULTIPLIER
               + (unsigned int)pcKey[i];
   return (int)(uiHash % (unsigned int)iBucketCount);
}





/***********************************************************************
The fucntion Createcustomer takes no parameter and retunrs a pointer to
the memory block for the new DB_T object if there is enough memory to 
be allocated.Besides it allocates a block of memmory for two hash table
pointers, where each elements of the hash table point to a struct user info.
pointers if there is no enough memeory space to allocate to either of
pointers. if there is no enough memory to be allocated in any of the three 
cases, it prints an error message to standard output and returns NULL.
if memory allocation is successful,then it returns the pointer
to the struct DB object.                            
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
  d->curArrSize = UNIT_BUCKET_SIZE; // start with 64 elements
  d->ht_id = (U_info **)calloc(d->curArrSize,
               sizeof(U_info *));
  if (d->ht_id == NULL) {
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	    d->curArrSize);
    free(d->ht_id);
    free(d);
    return NULL;
  }
  d->ht_name=(U_info **)calloc(d->curArrSize, sizeof(U_info *));
  if (d->ht_name == NULL) {
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
      d->curArrSize);  // i think i might need to review this.
    free(d->ht_name);
    free(d);
    return NULL;
  }
  return d;
  //return NULL;
}
/**********************************************************************
the function DestroyCustomerDB takes a pointer to the struct DB as 
a parameter.it does not have a return value rather it frees all the 
memory occupied by the DB_T object and all the underlying objects 
pointed by it and all the memory occupied by the two arrays of 
hashtable pointers and the two hastable and all theier underlying
objects.
***********************************************************************/
void
DestroyCustomerDB(DB_T d)
{
  assert(d!=NULL);
  if(d==NULL) return;
  U_info *p;
  for(int i=0;i<d->curArrSize;i++){
    for(p=d->ht_id[i]; p!=NULL;p=p->nextId){
  free(p->name);
  free(p->id);
  free(p);
      }
    }
    free(d->ht_id);
    free(d->ht_name);
    free(d);

}
/************************************************************************
the function RegisterCustomer takes DB_T d, id, name and purchase as a
parameter. if either an element with the same name or id already exists
or if the purhcase amount is non-positive it is a failure and the 
function returns -1. also if the d or id or name is null, it is a 
failure and the function returns -1. if any of the above doesn't 
happen(i.e. on success) the function doubles the bucket size if 0.75 
of the already existing bucket is used and it registers the new user 
info and returns 0.
***************************************************************************/
int
RegisterCustomer(DB_T d, const char *id,
		 const char *name, const int purchase)
{
  /* fill out this function */
  assert(purchase>0 && id!=NULL && name!=NULL);
  assert(d!=NULL);
  if(d==NULL || name==NULL || id==NULL || purchase<=0) return -1;
  int id_index=hash_function(id,d->curArrSize);
  int name_index=hash_function(name,d->curArrSize);

  printf("seg is here\n");
  U_info *p;
  for(p=d->ht_id[id_index]; p!=NULL;p=p->nextId){
        if((p->id!=NULL && strcmp(p->id,id)==0) || (p->name!=NULL && strcmp(p->name,name)==0)) 
                return -1;
  }
  for(p=d->ht_name[name_index]; p!=NULL;p=p->nextName){
          if(p->name!=NULL && strcmp(p->id,id)==0 || p->id!=NULL && strcmp(p->name,name)==0) 
                  return -1;
  } 

  printf("seg is here toooooooo\n");
  if(d->numItems==0.75*d->curArrSize){
    U_info **new_ht_id = calloc(2*d->curArrSize,sizeof(U_info *));
    U_info **new_ht_name = calloc(2*d->curArrSize,sizeof(U_info *));
    if(new_ht_id==NULL && new_ht_name==NULL) return -1;
    if(new_ht_name!=NULL && new_ht_id==NULL){
      free(new_ht_name);
      return -1;
    }
    if(new_ht_name==NULL && new_ht_id!=NULL){
      free(new_ht_id);
      return -1;
    }
    int old_arraysize=d->curArrSize;
    U_info ** old_ht_id= d->ht_id;
    U_info ** old_ht_name=d->ht_name;
    U_info * ptrId;
    U_info * ptrName;
    U_info * temp_ptr;
    d->curArrSize=d->curArrSize*2;
    d->ht_name=new_ht_name;
    d->ht_id=new_ht_id;
    U_info *start;
    int new_name_index,new_id_index; 

    for(int i = 0;i< old_arraysize;i++ ){
      if(d->ht_name[i]!=NULL){
        start = d->ht_name[i];
        while(start!=NULL){
          
          d->ht_name[i] = start->nextName;
          new_name_index = hash_function(start->name,d->curArrSize);
          new_id_index  = hash_function(start->id,d->curArrSize);
          //adds in the name table
          temp_ptr = new_ht_name[new_name_index];
          new_ht_name[new_name_index] =  start;
          start->nextName = temp_ptr;
          //add in the id table
          temp_ptr = new_ht_id[new_id_index];
          new_ht_id[new_id_index] = start;
          start->nextId = temp_ptr;
          start = d->ht_name[i];        

        }

      }
    }

    free(d->ht_name);
    free(d->ht_id);
    d->ht_name = new_ht_name;
    d->ht_id = new_ht_id; 

    printf("I am done doubling\n");
}
    
  printf("I am here\n");
    U_info *ptr = calloc(1,sizeof(struct UserInfo));
    if (ptr == NULL)
        return (-1);
    ptr->id = strdup(id);
    ptr->name = strdup(name);
    ptr->purchase = purchase;

    if(ptr->name==NULL || ptr->id==NULL){
      free(ptr->id);
      free(ptr->name);
      free(ptr);
      return -1;
    }
    

    int id_index2 = hash_function(id,d->curArrSize);
    int name_index2 = hash_function(name,d->curArrSize);
    ptr->nextId = d->ht_id[id_index2];
    d->ht_id[id_index2] = ptr;
    ptr->nextName = d->ht_name[name_index2];
    d->ht_name[name_index2] = ptr;
    d->numItems++;
      return 0; 
}

/************************************************************************
the function UnregisterCustomerByID takes the a pointer to the struct DB
and a pointer to a constant char id as a parameter. if either the id or
d is null or if the id is not found in the ******* of user info it's a 
failure and the function returns -1. if that is not the 
case,  the user info that matches the id is found and unlinked from the 
linked list and the memory allocated for name and id pointers are freed 
and the function returns 0.
************************************************************************/
int
UnregisterCustomerByID(DB_T d, const char *id)
{
  /* fill out this function */
  assert(d!=NULL);
  assert(id!=NULL);
  if(d==NULL || id==NULL) return -1;
  int id_index=hash_function(id,d->curArrSize); //to track the index where id occurs
  U_info *p=d->ht_id[id_index];
  char * name=NULL;

  for(p=d->ht_id[id_index]; p!=NULL;p=p->nextId){ 
      if(p==NULL) {
        return -1; 
        break; 
        }
      else{
        if( p->id!=NULL && strcmp(p->id,id)==0){
          name=p->name;
          break;
            }
        }
      }
    if(name == NULL){
      return -1;
    }
    assert(name!=NULL);
    int name_index= hash_function(name, d->curArrSize);
    U_info *ptr;

      for(ptr=d->ht_name[name_index]; ptr!=NULL; ptr=ptr->nextName){
        if(ptr == d->ht_name[name_index]){
          printf("i am here\n");
          if(strcmp(ptr->name,name)==0){
            free(ptr->name);
            d->ht_name[name_index] = ptr->nextName;
            break;
          }
        }
        if(strcmp(ptr->nextName->name,name)==0){
          free(ptr->nextName->name);
          ptr->nextName = ptr->nextName->nextName;
          break;
        }          


    }

    printf("I have freed the id\n" );



    U_info *save_ptr;    
    for(ptr=d->ht_id[id_index]; ptr!=NULL ; ptr=ptr->nextId){
      if(ptr==NULL){
        return -1;
        break;
      }
        if(ptr == d->ht_id[id_index]){
          if(strcmp(ptr->id,id)==0){
            save_ptr = ptr->nextId;
            free(ptr->id);
            free(ptr);
            d->ht_id[id_index] = save_ptr;
            d->numItems--;
            printf("here ia amdfasdfads\n");
            return 0;
          }
        }
        if(ptr->nextId==NULL){
          break;
        }
        if(strcmp(ptr->nextId->id,id)==0){
          save_ptr = ptr->nextId->nextId;
          free(ptr->nextId->id);
          free(ptr->nextId);
          ptr->nextId = save_ptr;
          d->numItems--;
          return 0;

        }    
}
  return -1;
     
}
/************************************************************************
the function UnregisterCustomerByName takes the a pointer to the struct DB
and a pointer to a constant char name as a parameter.if either the name or
d is null or if the name is not found in the ******* of user info it's a 
failure and the function returns -1. if that is not the case,  the user 
info that matches the name is found and unlinked from the 
linked list and the memory allocated for name and id pointers are freed 
and the function returns 0.
************************************************************************/
int
UnregisterCustomerByName(DB_T d, const char *name)
{
  /* fill out this function */
  assert(d!=NULL);
  assert(name!=NULL);
  if(d==NULL || name==NULL) return -1;
  int name_index=hash_function(name,d->curArrSize); //to track the index where id occurs
  U_info *p;
  char * id=NULL;

  for(p=d->ht_name[name_index]; p!=NULL;p=p->nextName){ 
      if(p==NULL) {
        return -1; 
        break; 
        }
    
        if(p->name!=NULL && strcmp(p->name,name)==0){
          id=p->id;
          break;
            }
        
      }

    printf("%s\n",id );
    if(id== NULL){

      return -1;
    }
    int id_index= hash_function(id, d->curArrSize);
    U_info *ptr;
    

    for(ptr=d->ht_id[id_index]; ptr!=NULL; ptr=ptr->nextId){
        if(ptr == d->ht_id[id_index]){     // is this really important??
          printf("i am here\n");
          if(strcmp(ptr->id,id)==0){
            free(ptr->id);
            d->ht_id[id_index] = ptr->nextId;
            break;
          }
        }
        if(strcmp(ptr->nextId->id,id)==0){    // what if it ptr->nextID is null??
          free(ptr->nextId->id);
          ptr->nextId = ptr->nextId->nextId;
          break;
        }          


    }

    printf("I have freed the id\n" );



    U_info *save_ptr;    
    for(ptr=d->ht_name[name_index]; ptr!=NULL ; ptr=ptr->nextName){
      if(ptr==NULL){
        return -1;
        break;
      }
        if(ptr == d->ht_name[name_index]){
          if(strcmp(ptr->name,name)==0){
            save_ptr = ptr->nextName;
            free(ptr->name);
            free(ptr);
            d->ht_name[name_index] = save_ptr;
            d->numItems--;
            return 0;
          }
        }
        if(ptr->nextName==NULL){
          break;
        }
        if(strcmp(ptr->nextName->name,name)==0){
          save_ptr = ptr->nextName->nextName;
          free(ptr->nextName->name);
          free(ptr->nextName);
          ptr->nextName = save_ptr;
          d->numItems--;
          return 0;

        }    
}
  return -1;

}
/************************************************************************
the function GetPurchaseByID takes the a pointer to the struct DB
and a pointer to a constant char id as a parameter.If either there is no 
customer in the linked list of user info for each array of user info 
pointers whose ID matches the given one or if the pointer to the struct 
DB or the pointer to the id is null, then it is a failure and the function 
returns -1. else it returns the purchase amount.
************************************************************************/
int
GetPurchaseByID(DB_T d, const char* id)
{
  /* fill out this function */
  assert(d!=NULL);
  assert(id!=NULL);
  if(d==NULL || id==NULL) return -1;
  int id_index=hash_function(id,d->curArrSize); //to track the index where id occurs
  U_info *p;
  for(p=d->ht_id[id_index]; p!=NULL;p=p->nextId){
      if(p->id!=NULL && strcmp(p->id,id)==0){
      return (p->purchase);
      }
    }
  return (-1);
}
/************************************************************************
the function GetPurchaseByName takes the a pointer to the struct DB
and a pointer to a constant char name as a parameter.If either there is no 
customer in the linked list of user info for each array of user info 
pointers  whose Name matches the given one or if the pointer to the struct
 DB or the pointer to the id is null, then it is a failure and the function 
returns -1. else it returns the purchase amount.
************************************************************************/
int
GetPurchaseByName(DB_T d, const char* name)
{
  /* fill out this function */
  assert(d!=NULL);
  assert(name!=NULL);
  if(d==NULL || name==NULL) return -1;
  int name_index=hash_function(name,d->curArrSize); //to track the index where id occurs
  U_info *p;
  for(p=d->ht_name[name_index]; p!=NULL;p=p->nextName){
      if(p->name!=NULL && strcmp(p->name,name)==0){
          return (p->purchase);
        }
      }
  
  return (-1);
}
/*--------------------------------------------------------------------*/
int
GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp)
{
  /* fill out this function */
  assert(d!=NULL);
  assert(fp!=NULL);
  if(d==NULL || fp==NULL) return -1;
  int TotalSumOfPurchase=0;
  U_info *p;
  for(int i=0;i<UNIT_BUCKET_SIZE;i++){
      for(p=d->ht_name[i]; p!=NULL;p=p->nextName){
          if(p->id!=NULL && p->name!=NULL) TotalSumOfPurchase += fp(p->id,p->name,p->purchase);
      }
  }
  return TotalSumOfPurchase;
}