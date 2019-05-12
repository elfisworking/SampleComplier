#include "globals.h"

const char errortype[50][256]=
{
	"�����ŵ��������ʽ�ұ�û��������������ƥ��",
	"�������Ƴ�����������ʶ�����������ʽ֮������",
	"��ʶ�����ʶ��֮��û�ж��Ÿ���",
	"���ź��ǺϷ��ı�ʾ��",
	"��������û������˵��",
	"����ͬ�����͵ı���ʱû���ԷֺŽ���",
	"������һ����������",
	"û�и�ֵ����",
	"��ֵ���ʽû���ԷֺŽ���",
	"��ϵ�����治�������Ĳ�����",
	"���ǺϷ��Ĳ������ʽ",
	"���������κ�����",
	"�����������",
	"�����������",
	"while���ȱ��do�ؼ���",
	"ȱ��then�ؼ���",
	"��end",
	"û�г��������߳��������Ϸ�",
	"����ͷ�ԷֺŽ���",
	"���Ϸ��ĳ������",
	"û�г���ͷ",
	"�Ƿ������������ַ������ַ�",
	"ȱ���ұߵ�ע�ͷ�",
	"ȱ����ߵ�ע�ͷ�",
	"�ַ������岻�����������ұߵġ� '��",    
	"���ַǷ�����",
	"repeat���ȱ��until�ؼ���"
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

