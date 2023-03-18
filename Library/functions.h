#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H


list *createlist(void);

bool delet(list *list, listobj *match);

listobj *extract(list *list,listobj *obj);

void insert_lastM(msg *obj, mailbox *node);

void insert_last(listobj *obj, list *node);

void insert(listobj *obj,
            list *new_list);

listobj *create_listobj(TCB *task);

  exception remove_mailbox (mailbox* mBox);
  
exception init_kernel(void);


exception create_task(void (*task_body)(), uint deadline);

exception receive_wait ( mailbox* mBox, void*
                        pData);




extern list *ReadyList;
extern list *WaitingList;
extern list *TimerList;

void idle(); 

#endif