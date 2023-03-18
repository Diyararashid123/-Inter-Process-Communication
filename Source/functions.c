#include "kernel_functions.h"
#include "functions.h"
#include "at91sam3x8.h"
#include "system_sam3x.h"
#include "exceptions.h"
#include "core_cmFunc.h"
#include "core_cm3.h"
#include "core_cmInstr.h"

int Ticks=0;
exception kernelmode=INIT;
TCB * NextTask=NULL;
TCB * PreviousTask=NULL;
 list *ReadyList;
 list *WaitingList;
 list *TimerList;

 
 
 void TimerInt() {
  Ticks++;
 
  while(TimerList->pHead->nTCnt<=Ticks&&TimerList->pHead!=NULL){
  PreviousTask=NextTask;
  insert(extract(TimerList,TimerList->pHead),ReadyList);
    NextTask=ReadyList->pHead->pTask;

  }
  
    while(WaitingList->pHead->pTask->Deadline<=Ticks&&WaitingList->pHead!=NULL){
  PreviousTask=NextTask;
  insert(extract(WaitingList,WaitingList->pHead),ReadyList);
    NextTask=ReadyList->pHead->pTask;

  }
  
  
  
  }
list *createlist(void) { // creating list

  list *new_list;
  new_list = (list *)calloc(1, sizeof(list));

  if (new_list == NULL) {

    free(new_list);
    free(new_list->pHead);
    free(new_list->pTail);
    return NULL;
  }

  else {
    new_list->pHead->pNext = new_list->pTail;
    new_list->pTail->pPrevious = new_list->pHead;
    return (list *)new_list;
  }
}

bool delet(list *list, listobj *match) { // måste vara en pointer
  // flera än en rad kod
  if (list == NULL || match == NULL)
    return FALSE;
  if (match->pTask->PC==idle)
    return FALSE;
  listobj *current = list->pHead;
  listobj *temp = NULL;

  if (current->pNext == NULL && current == match) {
    
    free(current->pTask);
    free(current);

    // sätta min tail och head till NULL för att efter jag har tagit bortt den
    // så måste jag veta att jag har NULL
    list->pHead = NULL;
    list->pTail = NULL;
    
    return TRUE;
  }
  if (list->pHead ==  match) { // if my current previous is NULL im at the first node
    list->pHead = current->pNext; // when i delet my first node witch is my
      list->pHead->pPrevious=NULL;                            // list->HEAD then i will move it to next node
    
    free(current->pTask);
    free(current);
    return TRUE;
  }

  while (current->pNext != NULL) {
    // if my current is not NULL then i go to the next node untill i find the
    // node that i want to delet
    if (current == match) // To find the node that i want to delet

      break; // if i find it i will leave the while loop

    current = current->pNext; // if i dont have a match i move my current to the
                              // next node
  }

  if (current !=
      match) // if i dont have dont delet any node then it will return fail
    return FALSE;

  // if i delet the node in the middle then i need to connect the node before it
  // with the node after it
  if (current->pNext != NULL && current->pPrevious != NULL) {
    temp = current->pPrevious;    // sett temp to my previous node
    temp->pNext = current->pNext; // my temp next it equal to my previous next
    temp = current->pNext;        // temp is equal to my current's next node
    temp->pPrevious =
        current->pPrevious; // temp previous is equal to my current's previous
    
    free(current->pTask);
         
    free(current);
    return TRUE; // if everything is right return TRUE
  }

  // the last node, if i delet the last node then the node before it next must
  // be null
  temp = current->pPrevious; // sett temp to my previous node
  temp->pNext =
      NULL; // and when i delet the last node my previous next must be NULL
  free(current->pTask);
  free(current);
  return TRUE;
}


void deleteMail(mailbox *box, msg *match) { // måste vara en pointer
   // flera än en rad kod
  if (box == NULL || match == NULL)
    return ;
 
  msg *current = box->pHead;
  msg *temp = NULL;
/*
  if (box->nMessages>0) {
    box->nMessages--;
    free(current->pData);
    free(current);

    // sätta min tail och head till NULL för att efter jag har tagit bortt den
    // så måste jag veta att jag har NULL
    box->pHead = NULL;
    box->pTail = NULL;
    
    return ;
  }
  */
  if (box->pHead ==  match) { // if my current previous is NULL im at the first node
    box->pHead = current->pNext; // when i delet my first node witch is my
      box->pHead->pPrevious=NULL;  
                          // list->HEAD then i will move it to next node
    box->nMessages--;
    free(current->pData);
    free(current);
    return ;
  }

  while (current->pNext != NULL) {
    // if my current is not NULL then i go to the next node untill i find the
    // node that i want to delet
    if (current == match) // To find the node that i want to delet

      break; // if i find it i will leave the while loop

    current = current->pNext; // if i dont have a match i move my current to the
                              // next node
  }

  if (current !=
      match) // if i dont have dont delet any node then it will return fail
    return ;

  // if i delet the node in the middle then i need to connect the node before it
  // with the node after it
  if (current->pNext != NULL && current->pPrevious != NULL) {
    temp = current->pPrevious;    // sett temp to my previous node
    temp->pNext = current->pNext; // my temp next it equal to my previous next
    temp = current->pNext;        // temp is equal to my current's next node
    temp->pPrevious =
        current->pPrevious; // temp previous is equal to my current's previous
    box->nMessages--;
    free(current->pData);
    free(current);
    return ; // if everything is right return TRUE
  }


  box->nMessages--;
  temp = current->pPrevious; // sett temp to my previous node
  temp->pNext =
      NULL; // and when i delet the last node my previous next must be NULL
    free(current->pData);
    free(current);
  return ;
}

void insertTL(listobj *obj,
            list *new_list) { // to make it easier to add stuff to the list
  listobj *current = NULL;
  current = new_list->pHead;
  
  if(obj->nTCnt>obj->pTask->Deadline)
  obj->nTCnt=obj->pTask->Deadline;
  
  if(current==NULL){
  new_list->pHead=obj;
  new_list->pTail=obj;
  obj->pNext=NULL;
  obj->pPrevious=NULL;
  }

  else if(new_list->pHead==new_list->pTail){
    if (obj->nTCnt < current->nTCnt){
      new_list->pHead=obj;
      obj->pNext=current;
      current->pPrevious=obj;
      }

    else{
     new_list->pTail=obj;
     current->pNext=obj;
     obj->pPrevious=current;
     }
      
  }
      else if(current->nTCnt >obj->nTCnt){
      obj->pNext=current;
      new_list->pHead=obj;
      current->pPrevious=obj;
      
      }
  else if(obj->nTCnt> new_list->pTail->nTCnt){
  new_list->pTail->pNext=obj;
  obj->pPrevious=new_list->pTail;
  new_list->pTail=obj;
  }
  else{

  while ((current->pNext != NULL) &&
         (obj->nTCnt > current->nTCnt))
    current = current->pNext;

  obj->pNext = current;
  obj->pPrevious = current->pPrevious;
  current->pPrevious=obj;
  obj->pPrevious->pNext = obj;
    }
            }

void insert(listobj *obj,
            list *new_list) { // to make it easier to add stuff to the list
  listobj *current = NULL;
  current = new_list->pHead;
  if(current==NULL){
  new_list->pHead=obj;
  new_list->pTail=obj;
  obj->pNext=NULL;
  obj->pPrevious=NULL;
  }

  else if(new_list->pHead==new_list->pTail){
    if (obj->pTask->Deadline < current->pTask->Deadline){
      new_list->pHead=obj;
      obj->pNext=current;
      current->pPrevious=obj;
      }

    else{
     new_list->pTail=obj;
     current->pNext=obj;
     obj->pPrevious=current;
     }
      
  }
      else if(current->pTask->Deadline >obj->pTask->Deadline){
      obj->pNext=current;
      new_list->pHead=obj;
      current->pPrevious=obj;
      
      }
  else if(obj->pTask->Deadline > new_list->pTail->pTask->Deadline){
  new_list->pTail->pNext=obj;
  obj->pPrevious=new_list->pTail;
  new_list->pTail=obj;
  }
  else{

  while ((current->pNext != NULL) &&
         (obj->pTask->Deadline > current->pTask->Deadline))
    current = current->pNext;

  obj->pNext = current;
  obj->pPrevious = current->pPrevious;
  current->pPrevious=obj;
  obj->pPrevious->pNext = obj;
    }
            }
listobj *create_listobj(TCB *task) { // list for TCB

  listobj *new_listobj;

  new_listobj = (listobj *)calloc(1, sizeof(listobj));
  new_listobj->pTask = task;
  new_listobj->nTCnt = 0;
  return new_listobj;
}

exception init_kernel(void) {
  Ticks = 0;
  ReadyList = createlist();
  if (ReadyList == NULL) {

    free(ReadyList);
    return FAIL;
  }

  TimerList = createlist();
  if (TimerList == NULL) {

    free(ReadyList);
    free(TimerList);
    return FAIL;
  }

  WaitingList = createlist();
  if (WaitingList == NULL) {

    free(ReadyList);
    free(TimerList);
    free(WaitingList);
    return FAIL;
  }
  exception status = create_task(idle, UINT_MAX);
  if (status == OK) {
    return status;
  } else {
    if (status != OK) {
      free(ReadyList);
      free(TimerList);
      free(WaitingList);

      return status;
    }
  }

  kernelmode = 1;// INIT and one are the same thing both enable the kernelmode
  return OK;
}
exception create_task(void (*task_body)(), uint deadline) {
  
  isr_off();
  TCB *new_tcb = (TCB *)calloc(1, sizeof(TCB));

  if (new_tcb == NULL) {
    free(new_tcb);
    return FAIL;
  }

  new_tcb->PC = task_body;
  new_tcb->SPSR = 0x21000000;
  new_tcb->Deadline = deadline;

  new_tcb->StackSeg[STACK_SIZE - 2] = 0x21000000;
  new_tcb->StackSeg[STACK_SIZE - 3] = (uint)task_body;
  new_tcb->SP = &(new_tcb->StackSeg[STACK_SIZE - 9]);
  
  if (kernelmode == INIT) {
    
      listobj*ins=create_listobj(new_tcb);
      if(ins==NULL){
      free(new_tcb);
      free(ins);
      free(new_tcb);
      return FAIL;
      
      }
    insert(ins, ReadyList);
    return OK;
  }

  else {
    isr_off();
     PreviousTask = NextTask;
    insert(create_listobj(new_tcb), ReadyList);
      NextTask = ReadyList->pHead->pTask;
    SwitchContext();
  }
  return OK;
}



listobj *extract(list *list,listobj *obj){
  
  listobj *current = list->pHead;
  listobj *temp = NULL;

  if (current->pNext == NULL && current == obj) {
    list->pHead = NULL;
    list->pTail = NULL;
  }
  else if(current==obj){
    list->pHead->pNext->pPrevious=NULL;
  list->pHead=current->pNext;
  
  }
  else if(list->pTail==obj){
  list->pTail->pPrevious->pNext=NULL;
  list->pTail=list->pTail->pPrevious;
  }
  else {
    while (current->pNext != NULL) {
    // if my current is not NULL then i go to the next node untill i find the
    // node that i want to delet
    if (current == obj) // To find the node that i want to delet

      break; // if i find it i will leave the while loop

    current = current->pNext; // if i dont have a match i move my current to the
                              // next node
  }
  
      temp = current->pPrevious;    // sett temp to my previous node
    temp->pNext = current->pNext; // my temp next it equal to my previous next
    temp = current->pNext;        // temp is equal to my current's next node
    temp->pPrevious =
    current->pPrevious; // temp previous is equal to my current's previous
   

    
  }
  obj->pNext=NULL;
  obj->pPrevious=NULL;
return obj;
}





mailbox* create_mailbox (uint nMessages, uint nDataSize){
  mailbox *new_mailbox = (mailbox *)calloc(1, sizeof(mailbox));
  
  if(new_mailbox==NULL){
    free(new_mailbox);
    return NULL; 
  }
  new_mailbox->nDataSize=nDataSize;
  new_mailbox->nMaxMessages=nMessages;
  new_mailbox->pHead=NULL;
  new_mailbox->pTail=NULL;
  return new_mailbox;
}


exception remove_mailbox (mailbox* mBox){
  if(mBox->pHead==NULL){
  free(mBox->pHead);
  free(mBox->pTail);
  free(mBox);
  return OK;
  }
  return NOT_EMPTY;
}
void DeleteM(mailbox* mBox){
  if(mBox->pHead!=NULL){
    free(mBox->pHead->pData);
    mBox->nMessages--;
    if(mBox->nMessages>0&&mBox->pHead!=NULL){
    mBox->pHead=mBox->pHead->pNext;
    
   
    mBox->pHead->pPrevious=NULL;
    if(mBox->pHead==mBox->pTail)
      mBox->pHead->pNext=NULL;
      
    }
    else {
      mBox->pHead->pNext=NULL;
       mBox->pHead->pPrevious=NULL;
      mBox->pHead=NULL;
      
    mBox->pTail=NULL;
      free(mBox->pHead);
    }
}
}

exception send_wait( mailbox* mBox, void* pData){
      isr_off();
      msg* receive = mBox ->pHead;
      int x=0;
  if(receive->Status==RECEIVER){
     memcpy(receive ->pData,pData, mBox ->nDataSize);
        
        DeleteM(mBox);
     mBox->nBlockedMsg--;

    PreviousTask = NextTask;
  
    insert( extract(WaitingList,receive->pBlock),ReadyList);
    NextTask = ReadyList->pHead->pTask;

  }else {
    
  
          msg *new_msg = (msg *)malloc(sizeof(msg));
   if(new_msg == NULL)
      return FAIL;
   
    x=1;
   
    new_msg->pData = (char*)malloc(mBox->nDataSize);
    if(new_msg->pData == NULL){
      free(new_msg);
      return FAIL;
    }
     memcpy(new_msg ->pData,pData, mBox ->nDataSize);
    if(mBox->nMaxMessages==mBox->nMessages){
      mBox->nBlockedMsg--;
        DeleteM(mBox); 
    }
     
     new_msg->pBlock=ReadyList->pHead;

       mBox->nBlockedMsg++;
     new_msg->Status=SENDER;
     insert_lastM(new_msg,mBox);
     ReadyList->pHead->pMessage=new_msg;
    PreviousTask = ReadyList ->pHead ->pTask;
    insert( extract(ReadyList,ReadyList->pHead),WaitingList);
    NextTask = ReadyList->pHead->pTask;
     
  }
   SwitchContext();
  
  if(Ticks>=NextTask->Deadline){
    if(x==1){
      int i=mBox->nMessages;
    deleteMail(mBox,ReadyList->pHead->pMessage);
    if(mBox->nMessages<i)
    mBox->nBlockedMsg--;
    }
   return DEADLINE_REACHED;
  }
else
return OK;
}

exception receive_wait ( mailbox* mBox, void*
                        pData){
      isr_off();
      msg* sent = mBox ->pHead;
      int x=0;
  if(sent->Status==SENDER){                  
     
   memcpy(pData,sent->pData, mBox ->nDataSize);
    mBox->nBlockedMsg--;
      DeleteM(mBox);
      
      if(sent->pBlock!=NULL){
    PreviousTask = NextTask;
    insert( extract(WaitingList,sent->pBlock),ReadyList);
    NextTask = ReadyList->pHead->pTask; 
        SwitchContext();
           return OK;               
      }
      else {
        free(sent->pData);
      } 
     }
  
  else{
     msg *new_msg = (msg *)calloc(1, sizeof(msg));
     if(new_msg==NULL)
       return FAIL;    
     
     if(mBox->nMaxMessages==mBox->nMessages){
        mBox->nBlockedMsg--;
        DeleteM(mBox);
     }
        x=1;
     new_msg->pBlock=ReadyList->pHead;
     new_msg->pData=NULL;
     new_msg->Status=RECEIVER;
     insert_lastM(new_msg,mBox);
       mBox->nBlockedMsg++;
      ReadyList->pHead->pMessage=new_msg;
    PreviousTask = NextTask;
    insert( extract(ReadyList,ReadyList->pHead),WaitingList);
    NextTask = ReadyList->pHead->pTask;
  
  } 
    SwitchContext();
  if(Ticks>=NextTask->Deadline){
    if(x==1){
      int i=mBox->nMessages;
    deleteMail(mBox,ReadyList->pHead->pMessage);
    if(mBox->nMessages<i)
    mBox->nBlockedMsg--;
    }
   return DEADLINE_REACHED;
  }
else
return OK;
}

void insert_lastM(msg *obj, mailbox *node) {
  if (obj == NULL || node == NULL)
    return;

  msg *newnode = obj;
    node->nMessages++;
  // INGEN NOD
  if (node->pHead == NULL) {
    node->pHead = newnode;
    node->pTail = newnode;
    newnode->pNext=NULL;
    newnode->pPrevious=NULL;
    return;
  }

  // EN NOD
  else if (node->pHead->pNext == NULL) {
    node->pTail = newnode;
    newnode->pPrevious = node->pHead;
    node->pHead->pNext=newnode;
    
    return;
  }

  // FLERA NODER
  node->pTail->pNext = newnode;
  newnode->pPrevious = node->pTail;
  node->pTail = newnode;
}


exception send_no_wait( mailbox *mBox, void
                       *pData){
                       
 isr_off();   
   msg* receive = mBox ->pHead;
   if(receive->Status==RECEIVER){
     memcpy(receive ->pData,pData, mBox ->nDataSize);
    
   
    PreviousTask = NextTask;
    if(receive->pBlock!= NULL){
    insert( extract(WaitingList,mBox->pHead->pBlock),ReadyList);
    }
    NextTask = ReadyList->pHead->pTask;
    
    DeleteM(mBox);
    
    SwitchContext();  
                       }
   else{
          msg *new_msg = (msg *)calloc(1, sizeof(msg));
   if(new_msg == NULL)
      return FAIL;
   
    new_msg->pData = (char*)malloc(mBox->nDataSize);
    if(new_msg->pData == NULL){
      
      return FAIL;
    }
        if(mBox->nMaxMessages==mBox->nMessages)
        DeleteM(mBox);      
     
     memcpy(new_msg ->pData, pData, mBox ->nDataSize);
     new_msg->Status=SENDER;
     insert_lastM(new_msg,mBox);
   
   
   }

  if(Ticks>NextTask->Deadline){
   insert( extract(WaitingList,mBox->pHead->pBlock),ReadyList);
   return DEADLINE_REACHED;
  }
else
return OK;
                       }

exception receive_no_wait( mailbox* mBox, void*
                          pData){
                          
      isr_off();
      msg* sent = mBox ->pHead;
  if(sent->Status==SENDER){                  
      memcpy(pData,sent->pData, mBox ->nDataSize);
      DeleteM(mBox);                       

      
   if(sent->pBlock!=NULL){
    PreviousTask = NextTask;
    insert( extract(WaitingList,sent->pBlock),ReadyList);
    NextTask = ReadyList->pHead->pTask;   
    SwitchContext();
           return OK;               
      }
      else {
        free(sent->pData);
        return OK;
      }
}
  else {
    return FAIL;
  }
}


void insert_last(listobj *obj, list *node) {
  if (obj == NULL || node == NULL)
    return;

  listobj *newnode = obj;

  if (node->pHead == NULL) {
    node->pHead = newnode;
    node->pTail = newnode;
    return;
  }

  // EN NOD
  else if (node->pHead->pNext == NULL) {
    node->pTail = newnode;
    newnode->pPrevious = node->pHead;
    node->pHead->pNext=newnode;
    
    return;
  }

  // FLERA NODER
  node->pTail->pNext = newnode;
  newnode->pPrevious = node->pTail;
  node->pTail = newnode;
}

void run(void) {
  kernelmode=RUNNING;
  NextTask = ReadyList->pHead->pTask;
  LoadContext_In_Run();
  /* supplied to you in the assembly file
   * does not save any of the registers
   * but simply restores registers from saved values
   * from the TCB of NextTask
   */
}

void terminate(void) {
  isr_off();
  delet(ReadyList,ReadyList->pHead);
  /* extract() detaches the head node from the ReadyList and
   * returns the list object of the running task */
  NextTask = ReadyList->pHead->pTask;
  switch_to_stack_of_next_task();


  LoadContext_In_Terminate();
  /* supplied to you in the assembly file
   * does not save any of the registers. Specifically, does not save the
   * process stack pointer (psp), but
   * simply restores registers from saved values from the TCB of NextTask
   * note: the stack pointer is restored from NextTask->SP */
}

void idle() {
  while (1)
    ;
}

exception wait(uint nTicks) {

  isr_off();
  ReadyList->pHead->nTCnt=Ticks+nTicks;
  PreviousTask = NextTask;
  insertTL(extract(ReadyList,ReadyList->pHead), TimerList);
  NextTask = ReadyList->pHead->pTask;
  SwitchContext();
  if (Ticks >= NextTask->Deadline) {
    return DEADLINE_REACHED;

  
  }
    else return OK;
}
void set_ticks(uint nTicks) { Ticks = nTicks; }

uint ticks(void) { return Ticks; }

uint deadline(void) { return NextTask->Deadline; }

void set_deadline(uint deadline) {
  isr_off(); 
    NextTask->Deadline = deadline;
  PreviousTask = NextTask;
  insert(extract(ReadyList,ReadyList->pHead),ReadyList);
  NextTask = ReadyList->pHead->pTask;
  SwitchContext();
}

