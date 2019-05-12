/****************************************************/
/* File: scan.c                                     */
/* The scanner implementation for the TINY compiler */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "assit.h"

int isTestToken_flag = FALSE;

TokenList tokenlist = NULL , currenttokenlist = NULL;//定义两个链表
//函数说明：创建一个TokenRec链表的新节点
void newTokenRec(TokenType tokentype, char *tokenString) {	
	TokenList p = tokenlist;
	while (p->next)
	{
		p = p->next;
	}
	p->next = (TokenList)malloc(sizeof(struct TokenRec));
	p = p->next;
	p->tokentype = tokentype;
	p->tokenString = copyString(tokenString);
	p->next = NULL;
}
//清空Token链表
void ClearTokenList() {
	TokenList p = tokenlist->next, tmp;
	tokenlist->next = NULL;
	while (p)
	{		
		free(p->tokenString);		
		tmp = p;
		p = p->next;
		free(tmp);
	}
}

/* states in scanner DFA */
typedef enum
{
	START, INASSIGN, INCOMMENT, INNUM, INID, INSTR, INSIGNS, INDOTDOT, DONE
}
StateType;

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN + 1];

/* BUFLEN = length of the input buffer for
source code lines */
#define BUFLEN 256

#define MAX_ID 1000  //定义标识符和常量出现的最大个数

static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0; /* current position in LineBuf */
static int bufsize = 0; /* current size of buffer string */
static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

int left_singal_qoute = FALSE;  //出现左单引号的标志
int left_note = FALSE;  //出现左注释/*的标志

int ID_NUM_STR[3][MAX_ID] = { 0 };

/* getNextChar fetches the next non-blank character
from lineBuf, reading in a new line if lineBuf is
exhausted */
static int getNextChar(void)
{
	if (!(linepos < bufsize))
	{
		lineno++;
		if (left_singal_qoute)
		{
			fprintf(listing, "\nerror: line%d:未找到匹配的右单引号", lineno - 1);
			left_singal_qoute = FALSE;
			//return EOF;
		}
		if (left_note)
		{
			fprintf(listing, "\nerror: line%d:未找到匹配的注释右界符*/", lineno - 1);
			left_note = FALSE;
			//return EOF;
		}

		if (fgets(lineBuf, BUFLEN - 1, source))
		{
			if (EchoSource) fprintf(listing, "%4d: %s", lineno, lineBuf);
			bufsize = strlen(lineBuf);
			linepos = 0;
			return lineBuf[linepos++];
		}
		else
		{
			EOF_flag = TRUE;
			return EOF;
		}
	}
	else return lineBuf[linepos++];
}

/* ungetNextChar backtracks one character
in lineBuf */
static void ungetNextChar(void)
{
	if (!EOF_flag) linepos--;
}

/* lookup table of reserved words */
static struct
{
	char* str;
	TokenType tok;
} reservedWords[MAXRESERVED] = {
	{ "and", AND }, { "array", ARRAY }, { "begin", BEGIN }, { "bool", BOOL }, { "call", CALL }, { "case", CASE }, { "char", CHAR }, { "constant", CONSTANT }, { "dim", DIM }, { "do", DO }, { "else", ELSE }, { "end", END },
	{ "false", CUS_FALSE }, { "for", FOR }, { "if", IF }, { "input", INPUT }, { "integer", INTEGER }, { "not", NOT }, { "of", OF }, { "or", OR }, { "output", OUTPUT }, { "procedure", PROCEDURE }, { "program", PROGRAM },
	{ "read", READ }, { "real", REAL }, { "repeat", REPEAT }, { "set", SET }, { "stop", STOP }, { "then", THEN }, { "to", TO }, { "true", CUS_TRUE }, { "until", UNTIL }, { "var", VAR },
	{ "while", WHILE }, { "write", WRITE }
};

static struct
{
	char* str;
	TokenType tok;
} signs[MAXSIGNS] = {
	{ "(", LPAREN }, { ")", RPAREN }, { "*", TIMES }, { "*/", RNOTE }, { "+", PLUS }, { ",", COMMA }, { "-", MINUS },
	{ ".", ENDFILE }, { "..", DOTDOT }, { "/", OVER }, { "/*", LNOTE }, { ":", COLON }, { ":=", ASSIGN }, { ";", SEMI },
	{ "<", LT }, { "<=", LTEQ }, { "<>", ANGLE }, { "=", EQ }, { ">", GT }, { ">=", GTEQ }, { "[", LSQUARE }, { "]", RSQUARE }
};

char signstr[] = "()*/+,-.:=;<=>[]";

/* lookup an identifier to see if it is a reserved word */
/* uses linear search */
static TokenType reservedLookup(char * s)
{
	int i;
	for (i = 0; i<MAXRESERVED; i++)
	if (!strcmp(s, reservedWords[i].str))
		return reservedWords[i].tok;
	return ID;
}

static TokenType signsLookup(char * s)
{
	int i;
	for (i = 0; i<MAXRESERVED; i++)
	if (!strcmp(s, signs[i].str))
		return signs[i].tok;
	return ERROR;
}

/****************************************/
/* the primary function of the scanner  */
/****************************************/
/* function getToken returns the
* next token in source file
*/

int global_tknum = 0;
//,,, RNOTE, PLUS,COMMA, MINUS, DOT, DOTDOT, 
//	OVER, LNOTE, COLON, ASSIGN, SEMI, LT, LTEQ, ANGLE, EQ, GT, GTEQ, SQU_BK,
TokenType getToken(Node_s *nn)
{  /* index for storing into tokenString */
	int tokenStringIndex = 0;
	/* holds current token to be returned */
	TokenType currentToken;
	/* current state - always begins at START */
	StateType state = START;
	/* flag to indicate save to tokenString */
	int save;
	char c1 = 0, c2 = 0;

	int rn = 0;
	TraceScan = TRUE;
	while (state != DONE)
	{
		int c = getNextChar();
		save = TRUE;
		switch (state)
		{
		case START:
			//找到'则找与之相匹配的另一个',该行内未找到,则报错
			if (c == '\'')
			{
				state = INSTR;
				left_singal_qoute = TRUE;
				while (((c2 = getNextChar()) != "\n") && (c2 != EOF))
				{
					if (c2 == '\'')
					{
						left_singal_qoute = FALSE;
						break;
					}
				}
				if (left_singal_qoute)
				{
					fprintf(listing, "\nerror at line %d: 未找到匹配的右单引号", lineno);
				}
			}
			else if (isdigit(c))
				state = INNUM;
			else if (isalpha(c))
				state = INID;
			else if (c == '.')
				state = INDOTDOT; //在.开头上的状态集中
			else if ((c == ' ') || (c == '\t') || (c == '\n'))
				save = FALSE;
			else if (c == '{')
			{
				save = FALSE;
				state = INCOMMENT;
			}
			else if (strchr(signstr, c) != NULL)
			{
				state = INSIGNS;
			}
			else
			{
				state = DONE;
				switch (c)
				{
				case EOF:
					save = FALSE;
					currentToken = ENDFILE;
					break;
				default:
					currentToken = ERROR;
					break;
				}
			}
			break;
		case INCOMMENT:
			save = FALSE;
			if (c == EOF)
			{
				state = DONE;
				currentToken = ENDFILE;
			}
			else if (c == '}') state = START;
			break;
		case INASSIGN:
			state = DONE;
			if (c == '=')
				currentToken = ASSIGN;
			else
			{ /* backup in the input */
				ungetNextChar();
				save = FALSE;
				currentToken = ERROR;
			}
			break;
		case INNUM:
			if (!isdigit(c)) 
			{ 
				if (isspace(c) || c == '+' || c == '-' || c == '*' || c == '/' || c == '='
					|| c == '<' || c == '>' || c == '(' || c == ')' || c == ':'
					|| c == '.' || c == ';' || c == ',') {
						ungetNextChar();
						save = FALSE;
						state = DONE;
						currentToken = NUM;
				}
				else {
					fprintf(listing, "\nerror at line %d: 错误的标识符格式", lineno);
					currentToken = ERROR;
				}
			}
			break;
		case INID:
			if (!(isalpha(c) || isdigit(c)))
			{ /* backup in the input */
				ungetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = ID;
			}
			break;
		case INSTR:
			ungetNextChar();
			save = FALSE;
			state = DONE;
			currentToken = STRING;
			break;
		case INSIGNS:
			if (strchr(signstr, c) == NULL)
			{ /* backup in the input */
				ungetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = SIGNS;
			}
			break;
		case INDOTDOT:  
			state = DONE;
			if (c == '.') {
				currentToken = DOTDOT;
			}
			else {
				ungetNextChar();
				save = FALSE;
				currentToken = DOT;  //.后面的不是 . 说明只有一个点 跳转到 DOT（.）的状态
			}
			break;
		case DONE:
		default: /* should never happen */
			fprintf(listing, "Scanner Bug: state= %d\n", state);
			state = DONE;
			currentToken = ERROR;
			break;
		}
		if ((save) && (tokenStringIndex <= MAXTOKENLEN))
			tokenString[tokenStringIndex++] = (char)c;
		if (state == DONE)
		{
			tokenString[tokenStringIndex] = '\0';
			if (currentToken == ID)
			{
				currentToken = reservedLookup(tokenString);
			}

			if (currentToken == SIGNS)
			{
				currentToken = signsLookup(tokenString);
				//若找到/*,则继续读文件,直到找到*/,在一行内找不到则报错
				if (currentToken == LNOTE)
				{
					left_note = TRUE;
					while (((c2 = getNextChar()) != "\n") && (c2 != EOF))
					{
						if ((c1 == '*') && (c2 == '/'))
						{
							left_note = FALSE;
							break;
						}
						c1 = c2;
					}
					if (left_note)
					{
						fprintf(listing, "\nerror at line %d: 未找到匹配的注释右界符*/", lineno);
						return EOF;
					}
					TraceScan = FALSE;
				}
			}
		}
	}
	
	if (currentToken != ENDFILE)
	{
		nn->num = printToken(currentToken, tokenString);
		nn->next = NULL;
		nn->line = lineno;
		strcpy(nn->content, tokenString);
		nn->type = currentToken;
		global_tknum++;
	}
	
	//if (TraceScan) {
	//	//fprintf(listing,"\t%d: ",lineno);
	//	printToken(currentToken, tokenString);
	//}
	return currentToken;
} /* end getToken */


void setTokenTest(int flg, TokenType tokentype , char * tokenString) {
	isTestToken_flag = flg;
	if (flg == TRUE) {
		isTestToken_flag = TRUE;
		tokenlist = (TokenList)malloc(sizeof(struct TokenRec));
		tokenlist->next = NULL;
		newTokenRec(tokentype, tokenString);
	}
}

TokenList getTokenList() {
	return tokenlist;
}

int getFlg() {
	return isTestToken_flag;
}

void setTokenList(TokenList ttokenlist) {
	tokenlist = ttokenlist;
}

void setFlg(int tisTestToken_flag) {
	isTestToken_flag = tisTestToken_flag;
}

void setRestoreTokenList() {
	TokenList p = tokenlist;
	while (tokenlist->next)
	{
		tokenlist = tokenlist->next;
	}
	tokenlist->next = currenttokenlist->next;

	currenttokenlist = p;
	tokenlist = (TokenList)malloc(sizeof(struct TokenRec));
	tokenlist->next = NULL;
}

void mergeTokenList(TokenList ttokenlist) {
	TokenList p = ttokenlist;
	while (p->next) {
		p = p->next;
	}
	p->next = tokenlist->next;
	tokenlist = ttokenlist;
}