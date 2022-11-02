#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include"tldlist.h"


struct tldlist{
    Date *begin;
    Date *end;
    TLDNode* head;
};
struct tldnode{
    char domain[32];
    long count;
    TLDNode* next;
};
struct tlditerator{
    TLDNode* head;
    TLDNode* curr;
};

/*
 * tldlist_create generates a list structure for storing counts against
 * top level domains (TLDs)
 *
 * creates a TLDList that is constrained to the `begin' and `end' Date's
 * returns a pointer to the list if successful, NULL if not
 */
TLDList *tldlist_create(Date *begin, Date *end){
    TLDList * tldlist=(TLDList *)malloc(sizeof(TLDList));
    tldlist->begin=date_duplicate(begin);
    tldlist->end=date_duplicate(end);
    tldlist->head=NULL;
    return tldlist;
}

/*
 * tldlist_destroy destroys the list structure in `tld'
 *
 * all heap allocated storage associated with the list is returned to the heap
 */
void tldlist_destroy(TLDList *tld){
    date_destroy(tld->begin);
    date_destroy(tld->end);
    TLDNode* curr=tld->head;
    TLDNode* to_free;
    while(curr!=NULL){
        to_free=curr;
        curr=curr->next;
        free(to_free);
    }
    free(tld);
}

/*
 * tldlist_add adds the TLD contained in `hostname' to the tldlist if
 * `d' falls in the begin and end dates associated with the list;
 * returns 1 if the entry was counted, 0 if not
 */
int tldlist_add(TLDList *tld, char *hostname, Date *d){
    char *p;
    p=hostname;

    while(strchr(p,'.')!=NULL){
        p=strchr(p,'.')+1;
    }
    for(int i=0;i<strlen(p);i++){
        if(*(p+i)>='A'&&*(p+i)<='Z'){
            *(p+i)=*(p+i)+32;
        }
    }
    if(date_compare(d,tld->begin)>=0&&date_compare(d,tld->end)<=0){
        TLDNode* to_add=(TLDNode*)malloc(sizeof(TLDNode));
        strcpy(to_add->domain,p);
        to_add->count=1;
        to_add->next=NULL;
        if(tld->head==NULL){
            tld->head=to_add;
        }
        else{
            TLDNode* curr=tld->head;
            while(curr->next!=NULL){
                curr=curr->next;
            }
            curr->next=to_add;
        }
        return 1;
    }
    else{
        printf("no add date\n");
        return 0;
    }
    return 0;
}

/*
 * tldlist_count returns the number of successful tldlist_add() calls since
 * the creation of the TLDList
 */
long tldlist_count(TLDList *tld){
    long sum=0;
    TLDNode* curr=tld->head;
    while(curr!=NULL){
        sum+=curr->count;
        curr=curr->next;
    }
    return sum;
}

/*
 * tldlist_iter_create creates an iterator over the TLDList; returns a pointer
 * to the iterator if successful, NULL if not
 */
TLDIterator *tldlist_iter_create(TLDList *tld){
    TLDIterator * iterator=(TLDIterator*)malloc(sizeof(TLDIterator));
    iterator->head=NULL;
    TLDNode* curr_in_tld=tld->head;
    while(curr_in_tld!=NULL){
        if(iterator->head==NULL){
            TLDNode* to_add=(TLDNode*)malloc(sizeof(TLDNode));
            to_add->count=curr_in_tld->count;
            strcpy(to_add->domain,curr_in_tld->domain);
            to_add->next=NULL;
            iterator->head=to_add;
            iterator->curr=iterator->head;
        }
        else{
            TLDNode* curr_in_ite=iterator->head;
            TLDNode* prev;
            int in=0;
            while(curr_in_ite!=NULL){
                if(strcmp(curr_in_ite->domain,curr_in_tld->domain)==0){
                    in=1;
                    curr_in_ite->count++;
                    break;
                }
                prev=curr_in_ite;
                curr_in_ite=curr_in_ite->next;
            }
            if(!in){
                TLDNode* to_add=(TLDNode*)malloc(sizeof(TLDNode));
                to_add->count=curr_in_tld->count;
                strcpy(to_add->domain,curr_in_tld->domain);
                to_add->next=NULL;
                prev->next=to_add;
            }
        }
        curr_in_tld=curr_in_tld->next;
    }
    return iterator;
}

/*
 * tldlist_iter_next returns the next element in the list; returns a pointer
 * to the TLDNode if successful, NULL if no more elements to return
 */
TLDNode *tldlist_iter_next(TLDIterator *iter){
    TLDNode * ret=iter->curr;
    if(iter->curr!=NULL)
        iter->curr=iter->curr->next;
    return ret;
}

/*
 * tldlist_iter_destroy destroys the iterator specified by `iter'
 */
void tldlist_iter_destroy(TLDIterator *iter){
    TLDNode* curr=iter->head;
    TLDNode* to_free;
    while(curr!=NULL){
        to_free=curr;
        curr=curr->next;
        free(to_free);
    }
    free(iter);
}

/*
 * tldnode_tldname returns the tld associated with the TLDNode
 */
char *tldnode_tldname(TLDNode *node){
    return node->domain;
}

/*
 * tldnode_count returns the number of times that a log entry for the
 * corresponding tld was added to the list
 */
long tldnode_count(TLDNode *node){
    if(node!=NULL){
        return node->count;
    }
    else{
        return 0;
    }
}
