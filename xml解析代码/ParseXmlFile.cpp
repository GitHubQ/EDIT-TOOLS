/********************************************************************
	created:	2007/11/15
	created:	15:11:2007   11:47
	filename:	ParseXmlFile.cpp
	author:		Wang xuebin 
	depend:		libxml2.lib
	build:		nmake TARGET_NAME=ParseXmlFile
	
	purpose:	����xml�ļ�
*********************************************************************/

#include <libxml/parser.h>
#include <iostream.h>

int main(int argc, char* argv[])
{
	xmlDocPtr doc;			//��������ĵ�ָ�� 
	xmlNodePtr curNode;		//������ָ��(����Ҫ��Ϊ���ڸ��������ƶ�) 
	xmlChar *szKey;			//��ʱ�ַ�������

	char *szDocName;
	if (argc <= 1)  
	{
		printf("Usage: %s docname\n", argv[0]);
		return(0);
	}
	szDocName = argv[1];

	doc = xmlReadFile(szDocName,"GB2312",XML_PARSE_RECOVER);  //�����ļ� 
	
	//�������ĵ��Ƿ�ɹ���������ɹ���libxml��ָһ��ע��Ĵ���ֹͣ��
	//һ�����������ǲ��ʵ��ı��롣XML��׼�ĵ�������UTF-8��UTF-16�⻹�����������뱣�档
	//����ĵ���������libxml���Զ���Ϊ��ת����UTF-8���������XML������Ϣ������XML��׼��.
	if (NULL == doc) 
	{ 	
		fprintf(stderr,"Document not parsed successfully. \n"); 	
		return -1; 
	} 
	
	curNode = xmlDocGetRootElement(doc);  //ȷ���ĵ���Ԫ��
	
	/*���ȷ�ϵ�ǰ�ĵ��а�������*/ 
	if (NULL == curNode)
	{ 
		fprintf(stderr,"empty document\n"); 
		xmlFreeDoc(doc); 
		return -1; 
	} 
	
	/*����������У�������Ҫȷ���ĵ�����ȷ�����͡���root���������ʾ����ʹ���ĵ��ĸ����͡�*/
	if (xmlStrcmp(curNode->name, BAD_CAST "root")) 
	{
		fprintf(stderr,"document of the wrong type, root node != root"); 
		xmlFreeDoc(doc); 
		return -1; 
	} 

	curNode = curNode->xmlChildrenNode;
	xmlNodePtr propNodePtr = curNode;
	while(curNode != NULL) 
	{
		//ȡ���ڵ��е�����
		if ((!xmlStrcmp(curNode->name, (const xmlChar *)"newNode1"))) 
		{
			szKey = xmlNodeGetContent(curNode);
			printf("newNode1: %s\n", szKey); 
			xmlFree(szKey); 
		} 

		//���Ҵ�������attribute�Ľڵ�
		if (xmlHasProp(curNode,BAD_CAST "attribute"))
		{
			propNodePtr = curNode;
		}
		curNode = curNode->next; 
	} 

	//��������
	xmlAttrPtr attrPtr = propNodePtr->properties;
	while (attrPtr != NULL)
	{
		if (!xmlStrcmp(attrPtr->name, BAD_CAST "attribute"))
		{
			xmlChar* szAttr = xmlGetProp(propNodePtr,BAD_CAST "attribute");
			cout<<"get attribute = "<<szAttr<<endl;
			xmlFree(szAttr);
		}
		attrPtr = attrPtr->next;
	}

	xmlFreeDoc(doc);
	return 0;
}
