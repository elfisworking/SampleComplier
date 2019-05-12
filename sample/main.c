/****************************************************/

/* File: main.c                                     */

/* Main program for TINY compiler                   */

/* Compiler Construction: Principles and Practice   */

/* Kenneth C. Louden                                */

/****************************************************/



#include "globals.h"

#include "scan.h"

#include "cus_stack.h"



/* allocate global variables */

int lineno = 0;

FILE * source;

FILE * listing;

FILE * code;

/* allocate and set tracing flags */

int EchoSource = FALSE;

int TraceScan = FALSE;


Node_s* p;

EStack item;
NodeStack symbol_stack;
NodeStack bracket;
NodeStack repeat_stack;
NodeStack if_stack;
NodeStack while_stack;

char token_array[50][512] = {0};
int nextstat = 0;
Node_s token,sembuf[30];  //sembuf数组存放操作数或者结果的临时变量和操作符，模拟出栈，入栈
int sem = 0;                        //sembuf下标
int j = 48;
int jt = 49;
bool_four_Elem_s array[200];

int tmpid[200] = {0};

int E1();
int T();
int F();
void newt();      
void quat(Node_s ww);
void backpatch(int p,int t);
int merge(int p1,int p2);
void or();
void and();
void not();
void cal();
void true_gen();
void false_gen();
void con_gen(char* arg);
void generate(char* op,char* arg1,char* arg2);
void vardefine();
void var();
void assignment();
void boolcal();
void cycle();
void repeat();
void until_pro();
void condition();
void else_pro();
void sentence();
void begin();
void syntax();
void true_false(int *t,int *f);

Node_s* head;
Node_s* tail;

Node_s tmpList[10000];
Node_s tmp;


void main(int argc, char * argv[])
{
	int i;
	TokenType atk;

	char rfile[256] = {0};
	printf("姓名:张煜旻 学号:201692455158\n");
	printf("输入代码文件名:");
	scanf("%s", rfile);
	source = fopen(rfile, "r");

	if (source == NULL)

	{
		fprintf(stderr, "File %s not found\n", rfile);
		system("pause");
		exit(1);

	}
	init_nodestack(&symbol_stack);
	init_nodestack(&bracket);
	init_nodestack(&repeat_stack);
	init_nodestack(&if_stack);
	init_nodestack(&while_stack);
	init_estack(&item);

	listing = stdout; /* send listing to screen */

	fprintf(listing, "\n词法分析结果\n");

	i = 0;
	while (getToken(&tmp) != ENDFILE)
	{
		if(head == NULL)
		{
			head = &tmpList[i];
			head->type = tmp.type;
			head->next = NULL;
			head->prev = NULL;
			strcpy(head->content, tmp.content);
			head->num = tmp.num;
			head->line = tmp.line;
			tail = head;
		}
		else
		{
			tail->next = &tmpList[i];
			tail->next->type = tmp.type;
			strcpy(tail->next->content, tmp.content);
			tail->next->next = NULL;
			tail->next->prev = tail;
			if (tmp.num == -1)
			{
				tail->next->num = tail->num;
			}
			else
			{
				tail->next->num = tmp.num;
			}
			tail->next->line = tmp.line;
			tail = tail->next;
		}
		i++;
	};

	p = head;
	syntax();
	fprintf(listing, "\n语分析结果\n");
	for(i = 0;i<nextstat;i++)
	{
		if((i == 0) || (i == (nextstat - 1)))
		{
			printf("(%2d) (%4s %3s %3s)\n", i, array[i].op, array[i].arg1, array[i].arg2);
			continue;
		}
		if(array[i].type == 1)
			printf("(%2d) (%4s %3s %3s %3s)\n", i, array[i].op, array[i].arg1, array[i].arg2, token_array[array[i].transfer]);
		else
			printf("(%2d) (%4s %3s %3s %3d)\n", i, array[i].op, array[i].arg1, array[i].arg2, array[i].transfer);
	}

	fclose(source);
	printf("输入1结束\n");
	

	
	system("pause");
	return 0;

}

void true_false(int *t,int *f)
{
	while(array[*t].transfer!=-1)
	{
		*t = array[*t].transfer;
	}
	while(array[*f].transfer!=-2)
	{
		*f = array[*f].transfer;
	}
}

int E1()
{     
	Node_s w1;
	T();                           //E_s->T
	while ( p->type == 43 || p->type == 45)      //E_s->E_s+T|E_s-T
	{
		w1=*p;
		p = moveforward(p);
		T();
		quat(w1);
	}
	return 1;
}

int T()
{
	Node_s w2;
	F();                                //T->F 
	while ( p->type == 41 || p->type == 48)         //T->T*F|T/F
	{
		w2=*p;
		p = moveforward(p);
		F();
		quat(w2);   

	}
	return 1;
}

int F()
{
	if ( p->type == 39)                  //F->(E_s)
	{
		p = moveforward(p);
		E1();
		if ( p->type != 40)
		{
			error(0,p->line);                //带括号的算术表达式右边没有括号与左括号匹配
			return 0;
		}
	}
	else if (p->type == 36||p->type == 37)       //F->digit|ID
	{
		token=*p;     //入栈
		sembuf[sem++]=token;
	}
	else
	{
		error(1,p->line);                 //从因子推出除整数，标识符，算术表达式之外的情况
		return 0;
	}
	p = moveforward(p);
	return 1;
}

void newt()               //记录一个临时变量
{
	char tmps[20] = {0};
	strcpy(token_array[j-48],"T");
	sprintf(tmps, "%c", jt);
	strcat(token_array[j-48], tmps);
	strcpy(token.content, token_array[j-48]);    //表示为临时变量
}


void quat(Node_s ww)                //用数组来模拟出栈和入栈,只有在记录+-*/的结果时才会进入这个函数
{
	newt();
	strcpy(array[nextstat].op, ww.content);                 //数组Q[]用来记录生成的四元式
	strcat(array[nextstat].op, ",");
	strcpy(array[nextstat].arg1, sembuf[sem-2].content);
	strcat(array[nextstat].arg1,",");
	strcpy(array[nextstat].arg2, sembuf[sem-1].content);
	strcat(array[nextstat].arg2,  ",");
	array[nextstat].transfer=j-48;
	array[nextstat].type = 1;
	jt++;
	j++;
	sem--;                     //弹出第一个操作数,出栈
	sem--;                     //弹出第二个操作数，出栈
	sembuf[sem++]=token;       //结果圧入栈中
	nextstat++;
}


void backpatch(int p,int t)
{
	while(array[p].transfer!=-1&&array[p].transfer!=-2)
	{
		int temp = array[p].transfer;
		array[p].transfer = t;
		p = temp;
	}
	array[p].transfer = t;
}

int merge(int p1,int p2)
{
	int p = p2;
	while(array[p2].transfer!=-1&&array[p2].transfer!=-2)
		p2 = array[p2].transfer;
	array[p2].transfer = p1;
	return p;
}

void or()
{
	E_s e;
	E_s e2 = out_estack(&item);
	E_s e1 = out_estack(&item);
	backpatch(e1.false_exit,e2.codebegin);
	e.codebegin = e1.codebegin;
	e.true_exit = merge(e1.true_exit,e2.true_exit);
	e.false_exit = e2.false_exit;
	in_estack(&item, &e);
}

void and()
{
	E_s e;
	E_s e2 = out_estack(&item);
	E_s e1 = out_estack(&item);
	backpatch(e1.true_exit,e2.codebegin);
	e.codebegin = e1.codebegin;
	e.true_exit = e2.true_exit;
	e.false_exit = merge(e1.false_exit,e2.false_exit);
	in_estack(&item, &e);
}

void not()
{
	E_s e;
	E_s e1 = out_estack(&item);
	e.true_exit = e1.false_exit;
	e.false_exit = e1.false_exit;
}

void cal()
{
	Node_s e = out_nodestack(&symbol_stack);
	if(e.type == 1)    //and操作
		and();
	else if(e.type == 20)  //or操作
		or();         
	else if(e.type == 18)
		not();
	//	else if(e.type == 40)
	//		error();
}

void true_gen()
{
	E_s e;
	e.true_exit = nextstat;
	e.codebegin = nextstat;
	in_estack(&item, &e);
	strcpy(array[nextstat].arg1, "-,");
	strcpy(array[nextstat].arg2, "-,");
	strcpy(array[nextstat].op, "j,");
	array[nextstat].transfer = -1;   //真出口
	array[nextstat].type = 2;
	nextstat = nextstat + 1;
}

void false_gen()
{
	E_s e;
	e.false_exit = nextstat;
	e.codebegin = nextstat;
	in_estack(&item, &e);
	strcpy(array[nextstat].arg1, "-,");
	strcpy(array[nextstat].arg2, "-,");
	strcpy(array[nextstat].op, "j,");
	array[nextstat].transfer = -2;   //假出口
	array[nextstat].type = 2;
	nextstat++;
}

void con_gen(char* arg)
{
	E_s e;
	e.true_exit = nextstat;
	e.codebegin = nextstat;
	e.false_exit = nextstat+1;
	in_estack(&item, &e);
	strcpy(array[nextstat].arg1, arg);
	strcpy(array[nextstat].arg2, "-,");
	strcpy(array[nextstat].op, "jnz,");
	array[nextstat].transfer = -1;   //真出口
	array[nextstat].type = 2;
	nextstat++;
	strcpy(array[nextstat].arg1, "-,");
	strcpy(array[nextstat].arg2, "-,");
	strcpy(array[nextstat].op, "j,");
	array[nextstat].transfer = -2;   //假出口
	array[nextstat].type = 2;
	nextstat++;
}


void generate(char* op,char* arg1,char* arg2)
{
	E_s e;
	e.true_exit = nextstat;
	e.codebegin = nextstat;
	e.false_exit = nextstat+1;
	in_estack(&item, &e);
	strcpy(array[nextstat].arg1, arg1);
	strcat(array[nextstat].arg1, ",");
	strcpy(array[nextstat].arg2, arg2);
	strcat(array[nextstat].arg2, ",");
	sprintf(array[nextstat].op, "j%s,", op);
	array[nextstat].transfer = -1;
	array[nextstat].type = 2;
	nextstat++;
	strcpy(array[nextstat].arg1,"-,");
	strcpy(array[nextstat].arg2,"-,");
	strcpy(array[nextstat].op, "j,");
	array[nextstat].transfer = -2;
	array[nextstat].type = 2;
	nextstat++;
}




void vardefine()
{
	p = moveforward(p);
	while(p->type != 50)   //冒号代表一类变量定义完毕
	{
		//加入变量定义列表
		if(p->type == 44)  //逗号
			p = moveforward(p);
		else
			error(2,p->line);       //标识符与标识符之间没有逗号隔开
		if(p->type == 36)  //下一个标示符
		{
			//加入变量定义列表
		}
		else
			error(3,p->line);   //逗号后不是合法的标示符
		p = moveforward(p);
	}
	p = moveforward(p);
	if(p->type == 4||p->type == 7||p->type == 17)   //类型：bool、char、integer
	{
		//补充类型说明
	}
	else
		error(4,p->line);            //变量定义没有类型说明
	p = moveforward(p);
	if(p->type == 52)    //分号表示一类变量的定义到此结束
	{
		//正常结束
	}
	else
		error(5,p->line);         //定义同种类型的变量时没有以分号结束
}

void var()
{
	p = moveforward(p);
	if(p->type != 36)
		error(6,p->line);     //至少有一个变量定义
	while(p->type == 36)   //当出现新的标示符
	{
		vardefine();
		p = moveforward(p);
	}
	p = p->prev;
}

void assignment()
{
	char* goal_temp;
	char*  goal_item = p->content;
	//处理被赋值的变量p->content
	p = moveforward(p);
	if(p->type == 51)   //赋值符号
	{
		p = moveforward(p);
		E1();
	}
	else 
		error(7,p->line);    //没有赋值符号
	strcpy(token_array[j-48], goal_item);
	goal_temp = sembuf[--sem].content;
	strcpy(array[nextstat].op, ":=,");
	strcpy(array[nextstat].arg1, goal_temp);
	strcat(array[nextstat].arg1, ",");
	strcpy(array[nextstat].arg2, "-,");
	array[nextstat].transfer = j-48;
	array[nextstat].type = 1;
	nextstat++;
	j++;
	if(p->type != 52)
	{
		p = p->prev;
		error(8,p->line);    //赋值表达式没有以分号结束
	}
	p = moveforward(p);

}

void boolcal()     //bool表达式判断
{
    char *op, *id1, *id2;
	E_s e;
	p = moveforward(p);
	while(p != NULL)  //遇到do和then时结束
	{
		if(p->type == 18)    //not符号
		{
			in_nodestack(&symbol_stack, p);
		}
		else if(p->type == 31)    //true
		{
			E_s e;
			e.true_exit = nextstat;
			e.codebegin = nextstat;
			true_gen();      //生成四元式
		}
		else if(p->type == 13)    //false
		{
			E_s e;
			e.false_exit = nextstat;
			e.codebegin = nextstat;
			false_gen();
		}
		else if(p->type == 36)   //标示符
		{
			if(p->next->type < 53||p->next->type > 58)
			{
				e.true_exit = nextstat;
				e.false_exit = nextstat+1;
				con_gen(p->content);
				p = moveforward(p);
				continue;
			}
			id1 = p->content;
			p = moveforward(p);
			if(p->type >= 53 && p->type<=58)    //关系符
			{
				op = p->content;
				p = moveforward(p);
				if(p->type == 36||p->type ==37)  //标示符
				{
					id2 = p->content;
					e.codebegin = nextstat;
					e.true_exit = nextstat;
					e.false_exit = nextstat+1;
					generate(op,id1,id2);
				}
				else
					error(9,p->line);     //关系符后面不是正常的操作数
			}
			else
				error(10,p->line);       //不是合法的布尔表达式
		}

		else if(p->type == 39)    //左括号
		{
			in_nodestack(&bracket, p);
			in_nodestack(&symbol_stack, p);
		}
		else if(p->type == 1)   //and
		{
			if(symbol_stack.top == -1)
			{
				in_nodestack(&symbol_stack, p);
				p = moveforward(p);
				continue;
			}
			if(top_node(&symbol_stack).type == 20 ||top_node(&symbol_stack).type == 39)  //or或者(
			{
				in_nodestack(&symbol_stack, p);
			}
			else
			{
				cal();
				in_nodestack(&symbol_stack, p);
			}
		}
		else if(p->type == 20)   //or
		{
			if(top_node(&symbol_stack).type == -1||top_node(&symbol_stack).type == 39)  //无符号，或者左括号
			{
				in_nodestack(&symbol_stack, p);
				p = moveforward(p);
				continue;
			}
			//or优先级最低
			cal();
			in_nodestack(&symbol_stack, p);
		}
		else if(p->type == 40)  //右括号
		{
			if(top_node(&symbol_stack).type == -1||top_node(&symbol_stack).type == 39)   //无运算符或只有左括号
			{
				error(11,p->line);    //括号内无任何运算
			}
			if(bracket.top == -1)   //没有左括号
				error(12,p->line);            //没有与右括号匹配的左括号，即多出右括号
			out_nodestack(&bracket);
			while(top_node(&symbol_stack).type != 39)
			{
				cal();
			}
			out_nodestack(&symbol_stack);
		}
		else 
		{   //遇到其他符号，先完成运算
			while(symbol_stack.top != -1)
			{
				cal();
			}
			if(bracket.top != -1)
				error(13,p->line);      //多出了左括号
			break;    //遇到其他符号
		}
		p = moveforward(p);
	}

}

void cycle()
{
	E_s e;
	int true_exit, false_exit;
	int back = nextstat;
	boolcal();
	e = out_estack(&item);
	true_exit = e.true_exit;
	false_exit = e.false_exit;
	true_false(&true_exit,&false_exit);
	//	printf("%d  %d  \n",true_exit,false_exit);
	if(p->type != 10)   //缺少do关键字
		error(14,p->line);
	array[true_exit].transfer = nextstat;
	p = moveforward(p);
	sentence();
	strcpy(array[nextstat].op, "j,");
	strcpy(array[nextstat].arg1, "-,");
	strcpy(array[nextstat].arg2, "-,");
	array[nextstat].transfer = back;
	nextstat++;
	array[false_exit].transfer = nextstat;
	//继续判断其他语句
	//	array[nextstat].op = "j,";
	//	array[nextstat].arg1 = "-,";
	//	array[nextstat].arg2 = "-,";
	//	array[nextstat].transfer = back;
}

void repeat()
{
	E_s e;
	int true_exit, false_exit;
	int back = nextstat;
	p = moveforward(p);
	sentence();
	if(p->type != 32)   //until
		error(26,p->line);
	boolcal();
	e = out_estack(&item);
	true_exit = e.true_exit;
	false_exit = e.false_exit;
	true_false(&true_exit,&false_exit);
	//	printf("%d  %d  \n",true_exit,false_exit);
	array[true_exit].transfer = nextstat;
	array[false_exit].transfer = back;
}

void until_pro()
{
	E_s e;
	int true_exit, false_exit;
	boolcal();
	e = out_estack(&item);
	true_exit = e.true_exit;
	false_exit = e.false_exit;
	true_false(&true_exit,&false_exit);
	//	printf("%d  %d  \n",true_exit,false_exit);
	array[true_exit].transfer = nextstat;
	p = p->prev;
}

void condition()
{
	E_s e;
	int true_over, false_over, true_exit, false_exit;
	boolcal();
    e = out_estack(&item);
	true_exit = e.true_exit;
	false_exit = e.false_exit;
	true_false(&true_exit,&false_exit);
	//	printf("%d  %d  \n",true_exit,false_exit);
	array[true_exit].transfer = nextstat;
	if(p->type != 29)  //缺少then关键字
		error(15,p->line);  
	else
	{
		p = moveforward(p);
		sentence();
		array[false_exit].transfer = nextstat + 1;
		nextstat++;
	}
	if(p->type == 11)   //如有else，过滤掉
	{
		p = moveforward(p);
		sentence();
		strcpy(array[array[false_exit].transfer - 1].op, "j,");
		strcpy(array[array[false_exit].transfer - 1].arg1, "-,");
		strcpy(array[array[false_exit].transfer - 1].arg2, "-,");
		array[array[false_exit].transfer - 1].transfer = nextstat;
	}
}

void else_pro()
{
	sentence();

}

void sentence()
{
	if(p->type == 36)
		assignment();//赋值语句块
	else if(p->type == 15)
		condition();        //条件语句块
	else if(p->type == 34)
		cycle();     //循环语句块
	else if(p->type == 26)
		repeat();    //重复语句块
	else if(p->type == 11)  //遇到else
		else_pro();
	//	else if(p->type == 32)  //遇到until
	//		until_pro();
}

void begin()
{
	p = moveforward(p);
	while(p != NULL && p->type != 12)   //程序未结束，非end
	{
		sentence();
	}
	if(p == NULL)
		error(16,p->line);      //无end
	else
	{
		strcpy(array[nextstat].op, "sys");
		strcpy(array[nextstat].arg1, "-,");
		strcpy(array[nextstat].arg2, " -,   -");
		array[nextstat].transfer = '-';
		nextstat++;
		printf("\nexit normally!\n");
	}
	//程序完整，正常退出
}

void syntax()
{
	if(p->type == 23)   //程序头
	{
		p = moveforward(p);
		if(p->type != 36)
			error(17,p->line);     //没有程序名或者程序名不合法
		else
		{
			//生成program四元式
			strcpy(array[nextstat].op, "program");
			strcpy(array[nextstat].arg1, p->content);
			strcpy(array[nextstat].arg2, "  -,  -");
			array[nextstat].transfer = -3;
			nextstat++;
		}
		p = moveforward(p);
		if(p->type != 52)  //程序头以分号结束
			error(18,p->line);
		p = moveforward(p);
		if(p->type == 33)
			var();          //变量定义模块
		p = moveforward(p);
		if(p->type == 3)
			begin();        //程序入口
		else
			error(19,p->line);      //不合法的程序入口
	}
	else
		error(20,p->line);         //出错，没有程序头
}



