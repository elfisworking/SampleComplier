

#ifndef _SCAN_H_
#define _SCAN_H_

/* MAXTOKENLEN is the maximum size of a token */
#define MAXTOKENLEN 40   //token长度为的最大大小为10

/* tokenString array stores the lexeme of each token */
extern char tokenString[MAXTOKENLEN + 1];  //tokenString数组保存每个token的词汇

/* function getToken returns the
 * next token in source file
 */
TokenType getToken(void);//返回下一个token在资源文件夹中
void setTokenTest(int flg,  TokenType tokentype, char * tokenString);
typedef struct TokenRec  //链表结构的定义
{
	TokenType tokentype;  //枚举类型tokentype 定义于globals.h文件
	char * tokenString;
	struct TokenRec * next;
}*TokenList;
TokenList getTokenList();  //获得链表
int getFlg();      
void setTokenList(TokenList ttokenlist);
void mergeTokenList(TokenList ttokenlist);
void setFlg(int tisTestToken_flag);
void setRestoreTokenList();
void ClearTokenList();  //清空链表
#endif
