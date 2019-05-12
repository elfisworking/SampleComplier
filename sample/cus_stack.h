#include "globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//栈里元素个数
#define	SIZE	1000	
//栈的声明
typedef struct nodestack{
	Node_s sta[SIZE];
	int top;
}NodeStack;

typedef struct estack{
	E_s sta[SIZE];
	int top;
}EStack;
//栈的初始化
void init_nodestack(NodeStack* s)
{
	int i;
	for(i = 0; i < SIZE; i++)
	{
		s->sta[i].line = 0;
		s->sta[i].prev = NULL;
		s->sta[i].next = NULL;
		s->sta[i].num = 0;
		memset(s->sta[i].content, 0, 2000);
	}
	s->top = -1;
}
//压栈
int in_nodestack(NodeStack* s, Node_s* data)
{
	if(s->top == SIZE-1){
		printf("the stack is full. \n");
		return 1;
	}	

	(s->top)++;
	s->sta[s->top].prev = data->prev;
	s->sta[s->top].next = data->next;
	s->sta[s->top].num = data->num;
	s->sta[s->top].line = data->line;
	s->sta[s->top].type = data->type;
	strcpy(s->sta[s->top].content, data->content);
	return 0;
}

//出栈
Node_s out_nodestack(NodeStack* s)
{
	Node_s data;
	if(s->top < 0 ){
		printf("the stack is empty. \n");
		return data;
	} else {
		data.prev = s->sta[s->top].prev;
		data.next = s->sta[s->top].next;
		data.num = s->sta[s->top].num;
		data.line = s->sta[s->top].line;
		data.type = s->sta[s->top].type;
		strcpy(data.content, s->sta[s->top].content);
		(s->top)--;
		return data;	
	}
}

Node_s top_node(NodeStack* s)
{
	Node_s data;
	if(s->top < 0 ){
		printf("the stack is empty. \n");
		return data;
	} else {
		data.prev = s->sta[s->top].prev;
		data.next = s->sta[s->top].next;
		data.num = s->sta[s->top].num;
		data.line = s->sta[s->top].line;
		strcpy(data.content, s->sta[s->top].content);
		return data;	
	} 
}

void destory_node(NodeStack *s)
{
	int i;
	s->top = -1;
	for(i = 0; i < SIZE; i++)
	{
		s->sta[i].line = 0;
		s->sta[i].prev = NULL;
		s->sta[i].next = NULL;
		s->sta[i].num = 0;
		memset(s->sta[i].content, 0, 2000);
	}
}

//栈的初始化
void init_estack(EStack* s)
{
	int i;
	for(i = 0; i < SIZE; i++)
	{
		s->sta[i].codebegin = 0;
		s->sta[i].true_exit = 0;
		s->sta[i].false_exit = 0;
	}
	s->top = -1;
}
//压栈
int in_estack(EStack* s, E_s* data)
{
	if(s->top == SIZE-1){
		printf("the stack is full. \n");
		return 1;
	}	

	(s->top)++;
	s->sta[s->top].codebegin = data->codebegin;
	s->sta[s->top].true_exit = data->true_exit;
	s->sta[s->top].false_exit = data->false_exit;
	return 0;
}

//出栈
E_s out_estack(EStack* s)
{
	E_s data;
	if(s->top < 0 ){
		printf("the stack is empty. \n");
		return data;
	} else {
		data.codebegin = s->sta[s->top].codebegin;
		data.true_exit = s->sta[s->top].true_exit;
		data.false_exit = s->sta[s->top].false_exit;
		(s->top)--;
		return data;	
	}
}

void destory_e(EStack *s)
{
	int i;
	s->top = -1;
	for(i = 0; i < SIZE; i++)
	{
		s->sta[i].codebegin = 0;
		s->sta[i].true_exit = 0;
		s->sta[i].false_exit = 0;
	}
}