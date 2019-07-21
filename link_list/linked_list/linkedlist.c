/**
 *  Source file for a linked list in C
 *
 * @authors:   		Michael Denzel
 * @creation_date:	2016-09-05
 * @contact:		m.denzel@cs.bham.ac.uk
 */

//standard includes

// TODO: Add the includes you need

//own includes
#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"


// TODO: Implement those methods!
node* get_node(list * l ,unsigned int index ){
	node * current  = l->head ;
	while(current){
		if(index==0){
			return current;
		}
		current = current->next ;
		index -- ; 
		
	}
	return NULL;
}
int get(list * l, unsigned int index){
	node * temp = get_node(l,index) ; 
	if(temp==NULL){
		return -1 ; 
	}
	return temp->value;
}
int prepend(list * l, int data){
	node* new_head = (node*)malloc(sizeof(node)); 
	if(new_head==NULL){
		return -1 ; 
	}
	new_head->value = data ; 
	new_head->next = l->head ;
	if(l->head==NULL){
		l->tail = new_head;
	} 
	l->head = new_head ; 
	l->length ++ ; 
	return 0 ;
}

int append(list * l, int data){
	
	node* new_tail = (node*)malloc(sizeof(node));
	if(new_tail==NULL){
		return -1 ; 
	}
	new_tail->value = data ; 
	new_tail->next = NULL ;  
	if(l->tail==NULL){
		l->tail = new_tail ; 
		l->head = l->tail ; 
	}else{
		l->tail->next = new_tail;
	}
	l->length ++ ; 
	return 0 ; 
}

int remove_element(list * l, unsigned int index){
	if(index==0&&l->head!=NULL){
		node * temp = l->head ; 
		l->head = temp->next ; 
		free(temp);
	}
	node * previous = get_node(l,index-1);
	if(previous==NULL){
		return -1 ; 
	}
	node * temp  = previous->next ;
	previous->next = temp->next ; 
	temp->next=NULL;
	l->length--;
	free(temp) ;  
	return 0 ; 
}

int insert(list * l, unsigned int index, int data){
	node * current = get_node(l,index);
	if(current==NULL){
		return-1;
	}
	node * new_node  = (node*)malloc(sizeof(node));
	if(new_node==NULL){
		return -1 ;
	}
	new_node->value = data ; 
	new_node->next = current->next ; 
	current->next = new_node ; 
	l->length++;
	return 0 ; 
}

void print_list(list * l){
	node* current = l->head ;
	if(current==NULL){
		printf("empty list" );
	} 
	while(current){
		printf("%d ",current->value );
		current = current->next ; 
	}
	printf("\n");
}

void init(list * l){
	l->head = NULL ;
	l->tail=NULL ; 
}

void destroy(list *l){
	node* current = l->head ; 
	while(current){
		free(current) ; 
		current=current->next ; 
	}
}


