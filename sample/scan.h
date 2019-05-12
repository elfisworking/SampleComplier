

#ifndef _SCAN_H_
#define _SCAN_H_

/* MAXTOKENLEN is the maximum size of a token */
#define MAXTOKENLEN 40   //token����Ϊ������СΪ10

/* tokenString array stores the lexeme of each token */
extern char tokenString[MAXTOKENLEN + 1];  //tokenString���鱣��ÿ��token�Ĵʻ�

/* function getToken returns the
 * next token in source file
 */
TokenType getToken(void);//������һ��token����Դ�ļ�����
void setTokenTest(int flg,  TokenType tokentype, char * tokenString);
typedef struct TokenRec  //����ṹ�Ķ���
{
	TokenType tokentype;  //ö������tokentype ������globals.h�ļ�
	char * tokenString;
	struct TokenRec * next;
}*TokenList;
TokenList getTokenList();  //�������
int getFlg();      
void setTokenList(TokenList ttokenlist);
void mergeTokenList(TokenList ttokenlist);
void setFlg(int tisTestToken_flag);
void setRestoreTokenList();
void ClearTokenList();  //�������
#endif
