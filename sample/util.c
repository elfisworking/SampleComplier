/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"

#define MAX_IDFS  10000   //标识符最大个数

int tkid = 0;

struct id_num_str
{
	int type;
	char name[41];
}ins[MAX_IDFS];

int ins_cnt = 0;
;
/* Procedure printToken prints a token
* and its lexeme to the listing file
*/
int printToken(TokenType token, const char* tokenString)
{
	int i;
	int rcnt = 1;
	int flag = 0;
	switch (token)
	{
	case AND:
	case ARRAY:
	case BEGIN:
	case BOOL:
	case CALL:
	case CASE:
	case CHAR:
	case CONSTANT:
	case DIM:
	case DO:
	case ELSE:
	case END:
	case CUS_FALSE:
	case FOR:
	case IF:
	case INPUT:
	case INTEGER:
	case NOT:
	case OF:
	case OR:
	case OUTPUT:
	case PROCEDURE:
	case PROGRAM:
	case READ:
	case REAL:
	case REPEAT:
	case SET:
	case STOP:
	case THEN:
	case TO:
	case CUS_TRUE:
	case UNTIL:
	case VAR:
	case WHILE:
	case WRITE:
	case LPAREN:
	case RPAREN:
	case TIMES:
	case RNOTE:
	case PLUS:
	case COMMA:
	case MINUS:
	case DOT:
	case DOTDOT:
	case OVER:
	case LNOTE:
	case COLON:
	case ASSIGN:
	case SEMI:
	case LT:
	case LTEQ:
	case ANGLE:
	case EQ:
	case GT:
	case GTEQ:
	case LSQUARE:
	case RSQUARE:
		tkid++;
		printf("(%2d,  -)  ", token);
		break;
	case NUM:
	case ID:
	case STRING:
		tkid++;
		flag = 1;
		for (i = 0; i < ins_cnt; i++)
		{
			if ((!strcmp(tokenString, ins[i].name)) && (token == ins[i].type))
			{
				printf("(%2d, %2d)  ", ins[i].type, i + 1);
				rcnt = i + 1;
				break;
			}
		}
		if (i == ins_cnt)
		{
			ins[ins_cnt].type = token;
			strcpy(ins[ins_cnt].name, tokenString);
			ins[ins_cnt].name[strlen(tokenString)] = '\0';
			printf("(%2d, %2d)  ", ins[ins_cnt].type, ins_cnt + 1);
			rcnt = ins_cnt + 1;
			ins_cnt++;
			break;
		}
		break;
	case ERROR:
		fprintf(listing, "\nerror at line %d: 非法字符", lineno);
		break;
	case ENDFILE:
		break;
	default: /* should never happen */
		fprintf(listing, "Unknown token: %d\n", token);
	}
	if ((!(tkid % 5)) && tkid)
		printf("\n");
	if (flag)
	{
		return rcnt;
	}
	else
		return -1;
}

/* Function newStmtNode creates a new statement
* node for syntax tree construction
*/
TreeNode * newStmtNode(StmtKind kind)
{
	TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
	int i;
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);
	else {
		for (i = 0; i<MAXCHILDREN; i++) t->child[i] = NULL;
		t->sibling = NULL;
		t->nodekind = StmtK;
		t->kind.stmt = kind;
		t->lineno = lineno;
	}
	return t;
}

/* Function newExpNode creates a new expression
* node for syntax tree construction
*/
TreeNode * newExpNode(ExpKind kind)
{
	TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
	int i;
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);
	else {
		for (i = 0; i<MAXCHILDREN; i++) t->child[i] = NULL;
		t->sibling = NULL;
		t->nodekind = ExpK;
		t->kind.exp = kind;
		t->lineno = lineno;
		t->type = Void;
	}
	return t;
}

/* Function copyString allocates and makes a new
* copy of an existing string
*/
char * copyString(char * s)
{
	int n;
	char * t;
	if (s == NULL) return NULL;
	n = strlen(s) + 1;
	t = malloc(n);
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);
	else strcpy(t, s);
	return t;
}

/* Variable indentno is used by printTree to
* store current number of spaces to indent
*/
static indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{
	int i;
	for (i = 0; i<indentno; i++)
		fprintf(listing, " ");
}

/* procedure printTree prints a syntax tree to the
* listing file using indentation to indicate subtrees
*/
void printTree(TreeNode * tree)
{
	int i;
	INDENT;
	while (tree != NULL) {
		printSpaces();
		if (tree->nodekind == StmtK)
		{
			switch (tree->kind.stmt) {
			case IfK:
				fprintf(listing, "If\n");
				break;
			case RepeatK:
				fprintf(listing, "Repeat\n");
				break;
			case AssignK:
				fprintf(listing, ":=, %s, -, %s\n", tree->child[0], tree->attr.name);
				break;
			case ProgramK:
				fprintf(listing, "program: %s, -, -\n", tree->attr.name);
				break;
			case ReadK:
				fprintf(listing, "Read\n");
				break;
			case WriteK:
				fprintf(listing, "Write\n");
				break;
			default:
				fprintf(listing, "Unknown ExpNode kind\n");
				break;
			}
		}
		else if (tree->nodekind == ExpK)
		{
			switch (tree->kind.exp) {
			case OpK:
				fprintf(listing, "Op: ");
				printToken(tree->attr.op, "\0", 1, 1);
				break;
			case ConstK:
				fprintf(listing, "Const: %d\n", tree->attr.val);
				break;
			case IdK:
				fprintf(listing, "Id: %s\n", tree->attr.name);
				break;
			default:
				fprintf(listing, "Unknown ExpNode kind\n");
				break;
			}
		}
		else fprintf(listing, "Unknown node kind\n");
		for (i = 0; i<MAXCHILDREN; i++)
			printTree(tree->child[i]);
		tree = tree->sibling;
	}
	UNINDENT;
}