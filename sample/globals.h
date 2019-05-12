

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
	struct node* goal;    //指向变量
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
	int transfer; //在算术表达式中代表指向变量的下标，布尔表达式表示跳转的目标
	int type;    //1表示算术表达式，2表示布尔表达式
}bool_four_Elem_s;



typedef struct CodeListRec  //中间代码的（operation , arg1 , arg2 ,des）
{
	char * op, *c;//暂时还不知道 *c的用途
	int arg1, arg2, des;
	struct CodeListRec * next;
} *CodeListList;
extern CodeListList codelist;

typedef struct ChainRec   //链的回溯
{
	int codeid;
	struct ChainRec * next;
}*ChainList;

typedef struct treeNode  //树的节点定义
{
	struct treeNode * child[MAXCHILDREN];  //子类节点 用数组形式表现
	struct treeNode * sibling;  //兄弟节点
	int lineno;
	NodeKind nodekind;  //节点属于哪种类别
	union { StmtKind stmt; ExpKind exp; } kind;  //statement 或者 expression 类别
	struct {
		TokenType op;//Token的属性
		int val;  //Token的值
		char * name; //Token的名字
		int place;  //位置
		ChainList ExpTRUE;//正确链
		ChainList ExpFALSE;//错误链
		ChainList Chain; //链
		int EcodeBegin;  //
	} attr;
	ExpType type; /* for type checking of exps 是bool 还是 integer 或者void */ 
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