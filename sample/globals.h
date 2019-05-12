

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 35
#define MAXSIGNS 22

typedef enum
{
	ERROR,
	/* reserved words */
	AND, ARRAY, BEGIN, BOOL, CALL, CASE, CHAR, CONSTANT,
	DIM, DO, ELSE, END, CUS_FALSE, FOR, IF, INPUT, INTEGER,
	NOT, OF, OR, OUTPUT, PROCEDURE, PROGRAM, READ, REAL,
	REPEAT, SET, STOP, THEN, TO, CUS_TRUE, UNTIL, VAR, WHILE, WRITE,
	//IF,THEN,ELSE,END,REPEAT,UNTIL,READ,WRITE,
	/* multicharacter tokens */
	ID, NUM, STRING,
	/* special symbols */
	LPAREN, RPAREN, TIMES, RNOTE, PLUS, COMMA, MINUS, DOT, DOTDOT,
	OVER, LNOTE, COLON, ASSIGN, SEMI, LT, LTEQ, ANGLE, EQ, GT, GTEQ,
	LSQUARE, RSQUARE,
	SIGNS,
	/* book-keeping tokens */
	ENDFILE,
} TokenType;

extern FILE* source; /* source code text file */
extern FILE* listing; /* listing output text file */
extern FILE* code; /* code text file for TM simulator */

extern int lineno; /* source line number for listing */

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

typedef enum { StmtK, ExpK } NodeKind;
typedef enum { ProgramK, VarK, AssignK, IfK, RepeatK, WhileK, BeginK, VarDefK, IDK, IntK, BoolK, CharK, ReadK, WriteK } StmtKind;
typedef enum { OpK, ConstK, IdK, ConstBoolK } ExpKind;

/* ExpType is used for type checking */
typedef enum { Void, Integer, Boolean } ExpType;

#define MAXCHILDREN 3

typedef struct node
{
	struct node* prev;
	char content[2000];
	int type;
	struct node* next;
	int num;
	int line;
}Node_s;

typedef struct variable
{
	struct node* goal;    //ָ�����
	struct variable* next;
	struct variable* prev;
}Variable_s;

typedef struct E
{
	int codebegin;
	int true_exit;
	int false_exit;
}E_s;

typedef struct bool_four_Elem
{
	char op[512];
	char arg1[512];
	char arg2[512];
	int transfer; //���������ʽ�д���ָ��������±꣬�������ʽ��ʾ��ת��Ŀ��
	int type;    //1��ʾ�������ʽ��2��ʾ�������ʽ
}bool_four_Elem_s;



typedef struct CodeListRec  //�м����ģ�operation , arg1 , arg2 ,des��
{
	char * op, *c;//��ʱ����֪�� *c����;
	int arg1, arg2, des;
	struct CodeListRec * next;
} *CodeListList;
extern CodeListList codelist;

typedef struct ChainRec   //���Ļ���
{
	int codeid;
	struct ChainRec * next;
}*ChainList;

typedef struct treeNode  //���Ľڵ㶨��
{
	struct treeNode * child[MAXCHILDREN];  //����ڵ� ��������ʽ����
	struct treeNode * sibling;  //�ֵܽڵ�
	int lineno;
	NodeKind nodekind;  //�ڵ������������
	union { StmtKind stmt; ExpKind exp; } kind;  //statement ���� expression ���
	struct {
		TokenType op;//Token������
		int val;  //Token��ֵ
		char * name; //Token������
		int place;  //λ��
		ChainList ExpTRUE;//��ȷ��
		ChainList ExpFALSE;//������
		ChainList Chain; //��
		int EcodeBegin;  //
	} attr;
	ExpType type; /* for type checking of exps ��bool ���� integer ����void */ 
} TreeNode;

/**************************************************/
/***********   Flags for tracing       ************/
/**************************************************/

/* EchoSource = TRUE causes the source program to
* be echoed to the listing file with line numbers
* during parsing
*/
extern int EchoSource;

/* TraceScan = TRUE causes token information to be
* printed to the listing file as each token is
* recognized by the scanner
*/
extern int TraceScan;

/* TraceParse = TRUE causes the syntax tree to be
* printed to the listing file in linearized form
* (using indents for children)
*/
extern int TraceParse;

/* TraceAnalyze = TRUE causes symbol table inserts
* and lookups to be reported to the listing file
*/
extern int TraceAnalyze;

/* TraceCode = TRUE causes comments to be written
* to the TM code file as code is generated
*/
extern int TraceCode;

extern int Error;
#endif