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
Node_s token,sembuf[30];  //sembuf�����Ų��������߽������ʱ�����Ͳ�������ģ���ջ����ջ
int sem = 0;                        //sembuf�±�
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
	printf("����:���ϕF ѧ��:201692455158\n");
	printf("��������ļ���:");
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

	fprintf(listing, "\n�ʷ��������\n");

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
	fprintf(listing, "\n��������\n");
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
	printf("����1����\n");
	

	
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
			error(0,p->line);                //�����ŵ��������ʽ�ұ�û��������������ƥ��
			return 0;
		}
	}
	else if (p->type == 36||p->type == 37)       //F->digit|ID
	{
		token=*p;     //��ջ
		sembuf[sem++]=token;
	}
	else
	{
		error(1,p->line);                 //�������Ƴ�����������ʶ�����������ʽ֮������
		return 0;
	}
	p = moveforward(p);
	return 1;
}

void newt()               //��¼һ����ʱ����
{
	char tmps[20] = {0};
	strcpy(token_array[j-48],"T");
	sprintf(tmps, "%c", jt);
	strcat(token_array[j-48], tmps);
	strcpy(token.content, token_array[j-48]);    //��ʾΪ��ʱ����
}


void quat(Node_s ww)                //��������ģ���ջ����ջ,ֻ���ڼ�¼+-*/�Ľ��ʱ�Ż�����������
{
	newt();
	strcpy(array[nextstat].op, ww.content);                 //����Q[]������¼���ɵ���Ԫʽ
	strcat(array[nextstat].op, ",");
	strcpy(array[nextstat].arg1, sembuf[sem-2].content);
	strcat(array[nextstat].arg1,",");
	strcpy(array[nextstat].arg2, sembuf[sem-1].content);
	strcat(array[nextstat].arg2,  ",");
	array[nextstat].transfer=j-48;
	array[nextstat].type = 1;
	jt++;
	j++;
	sem--;                     //������һ��������,��ջ
	sem--;                     //�����ڶ�������������ջ
	sembuf[sem++]=token;       //����R��ջ��
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
	if(e.type == 1)    //and����
		and();
	else if(e.type == 20)  //or����
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
	array[nextstat].transfer = -1;   //�����
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
	array[nextstat].transfer = -2;   //�ٳ���
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
	array[nextstat].transfer = -1;   //�����
	array[nextstat].type = 2;
	nextstat++;
	strcpy(array[nextstat].arg1, "-,");
	strcpy(array[nextstat].arg2, "-,");
	strcpy(array[nextstat].op, "j,");
	array[nextstat].transfer = -2;   //�ٳ���
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
	while(p->type != 50)   //ð�Ŵ���һ������������
	{
		//������������б�
		if(p->type == 44)  //����
			p = moveforward(p);
		else
			error(2,p->line);       //��ʶ�����ʶ��֮��û�ж��Ÿ���
		if(p->type == 36)  //��һ����ʾ��
		{
			//������������б�
		}
		else
			error(3,p->line);   //���ź��ǺϷ��ı�ʾ��
		p = moveforward(p);
	}
	p = moveforward(p);
	if(p->type == 4||p->type == 7||p->type == 17)   //���ͣ�bool��char��integer
	{
		//��������˵��
	}
	else
		error(4,p->line);            //��������û������˵��
	p = moveforward(p);
	if(p->type == 52)    //�ֺű�ʾһ������Ķ��嵽�˽���
	{
		//��������
	}
	else
		error(5,p->line);         //����ͬ�����͵ı���ʱû���ԷֺŽ���
}

void var()
{
	p = moveforward(p);
	if(p->type != 36)
		error(6,p->line);     //������һ����������
	while(p->type == 36)   //�������µı�ʾ��
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
	//������ֵ�ı���p->content
	p = moveforward(p);
	if(p->type == 51)   //��ֵ����
	{
		p = moveforward(p);
		E1();
	}
	else 
		error(7,p->line);    //û�и�ֵ����
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
		error(8,p->line);    //��ֵ���ʽû���ԷֺŽ���
	}
	p = moveforward(p);

}

void boolcal()     //bool���ʽ�ж�
{
    char *op, *id1, *id2;
	E_s e;
	p = moveforward(p);
	while(p != NULL)  //����do��thenʱ����
	{
		if(p->type == 18)    //not����
		{
			in_nodestack(&symbol_stack, p);
		}
		else if(p->type == 31)    //true
		{
			E_s e;
			e.true_exit = nextstat;
			e.codebegin = nextstat;
			true_gen();      //������Ԫʽ
		}
		else if(p->type == 13)    //false
		{
			E_s e;
			e.false_exit = nextstat;
			e.codebegin = nextstat;
			false_gen();
		}
		else if(p->type == 36)   //��ʾ��
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
			if(p->type >= 53 && p->type<=58)    //��ϵ��
			{
				op = p->content;
				p = moveforward(p);
				if(p->type == 36||p->type ==37)  //��ʾ��
				{
					id2 = p->content;
					e.codebegin = nextstat;
					e.true_exit = nextstat;
					e.false_exit = nextstat+1;
					generate(op,id1,id2);
				}
				else
					error(9,p->line);     //��ϵ�����治�������Ĳ�����
			}
			else
				error(10,p->line);       //���ǺϷ��Ĳ������ʽ
		}

		else if(p->type == 39)    //������
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
			if(top_node(&symbol_stack).type == 20 ||top_node(&symbol_stack).type == 39)  //or����(
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
			if(top_node(&symbol_stack).type == -1||top_node(&symbol_stack).type == 39)  //�޷��ţ�����������
			{
				in_nodestack(&symbol_stack, p);
				p = moveforward(p);
				continue;
			}
			//or���ȼ����
			cal();
			in_nodestack(&symbol_stack, p);
		}
		else if(p->type == 40)  //������
		{
			if(top_node(&symbol_stack).type == -1||top_node(&symbol_stack).type == 39)   //���������ֻ��������
			{
				error(11,p->line);    //���������κ�����
			}
			if(bracket.top == -1)   //û��������
				error(12,p->line);            //û����������ƥ��������ţ������������
			out_nodestack(&bracket);
			while(top_node(&symbol_stack).type != 39)
			{
				cal();
			}
			out_nodestack(&symbol_stack);
		}
		else 
		{   //�����������ţ����������
			while(symbol_stack.top != -1)
			{
				cal();
			}
			if(bracket.top != -1)
				error(13,p->line);      //�����������
			break;    //������������
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
	if(p->type != 10)   //ȱ��do�ؼ���
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
	//�����ж��������
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
	if(p->type != 29)  //ȱ��then�ؼ���
		error(15,p->line);  
	else
	{
		p = moveforward(p);
		sentence();
		array[false_exit].transfer = nextstat + 1;
		nextstat++;
	}
	if(p->type == 11)   //����else�����˵�
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
		assignment();//��ֵ����
	else if(p->type == 15)
		condition();        //��������
	else if(p->type == 34)
		cycle();     //ѭ������
	else if(p->type == 26)
		repeat();    //�ظ�����
	else if(p->type == 11)  //����else
		else_pro();
	//	else if(p->type == 32)  //����until
	//		until_pro();
}

void begin()
{
	p = moveforward(p);
	while(p != NULL && p->type != 12)   //����δ��������end
	{
		sentence();
	}
	if(p == NULL)
		error(16,p->line);      //��end
	else
	{
		strcpy(array[nextstat].op, "sys");
		strcpy(array[nextstat].arg1, "-,");
		strcpy(array[nextstat].arg2, " -,   -");
		array[nextstat].transfer = '-';
		nextstat++;
		printf("\nexit normally!\n");
	}
	//���������������˳�
}

void syntax()
{
	if(p->type == 23)   //����ͷ
	{
		p = moveforward(p);
		if(p->type != 36)
			error(17,p->line);     //û�г��������߳��������Ϸ�
		else
		{
			//����program��Ԫʽ
			strcpy(array[nextstat].op, "program");
			strcpy(array[nextstat].arg1, p->content);
			strcpy(array[nextstat].arg2, "  -,  -");
			array[nextstat].transfer = -3;
			nextstat++;
		}
		p = moveforward(p);
		if(p->type != 52)  //����ͷ�ԷֺŽ���
			error(18,p->line);
		p = moveforward(p);
		if(p->type == 33)
			var();          //��������ģ��
		p = moveforward(p);
		if(p->type == 3)
			begin();        //�������
		else
			error(19,p->line);      //���Ϸ��ĳ������
	}
	else
		error(20,p->line);         //����û�г���ͷ
}



