#include "globals.h"

const char errortype[50][256]=
{
	"带括号的算术表达式右边没有括号与左括号匹配",
	"从因子推出除整数，标识符，算术表达式之外的情况",
	"标识符与标识符之间没有逗号隔开",
	"逗号后不是合法的标示符",
	"变量定义没有类型说明",
	"定义同种类型的变量时没有以分号结束",
	"至少有一个变量定义",
	"没有赋值符号",
	"赋值表达式没有以分号结束",
	"关系符后面不是正常的操作数",
	"不是合法的布尔表达式",
	"括号内无任何运算",
	"多出了右括号",
	"多出了左括号",
	"while语句缺少do关键字",
	"缺少then关键字",
	"无end",
	"没有程序名或者程序名不合法",
	"程序头以分号结束",
	"不合法的程序入口",
	"没有程序头",
	"非法的整数，出现非数字字符",
	"缺少右边的注释符",
	"缺少左边的注释符",
	"字符串定义不完整，少了右边的“ '”",    
	"出现非法符号",
	"repeat语句缺少until关键字"
};



void error(int i,int line)
{
	printf("\nerror at line %d: %s\n", line, errortype[i]);
   system("pause");
	exit(0);
}

int check(Variable_s* head,char* content)
{
	while(head != NULL)
	{
		if(strcmp(head->goal->content, content))
			return head->goal->num;
		head = head->next;
	}
	return -1;
}

Node_s* moveforward(Node_s* p)
{
	if(p->next!=NULL)
	{
		return p->next;
	}
	else
	{
		if (p->type != 12)
		{
			error(16, p->line);
		}
		exit(0);
	}
}

int movebackward(Node_s* p)
{
	if(p->prev!=NULL)
	{
		p = p->prev;
		return 1;
	}
	else
		return 0;
}

