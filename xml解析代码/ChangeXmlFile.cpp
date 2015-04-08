/********************************************************************
	created:	2007/11/15
	created:	15:11:2007   15:20
	filename:	ChangeXmlFile.cpp
	author:		Wang xuebin 
	depend:		libxml2.lib
	build:		nmake TARGET_NAME=ChangeXmlFile
	
	purpose:	�޸�XMLԪ�ؼ�����
*********************************************************************/

#include <libxml/parser.h>
#include <iostream.h>

int main(int argc, char* argv[])
{
	xmlDocPtr doc;   //��������ĵ�ָ�� 
	xmlNodePtr curNode;  //������ָ��(����Ҫ��Ϊ���ڸ��������ƶ�) 

	char *szDocName;
	if (argc <= 1)  
	{
		printf("Usage: %s docname\n", argv[0]);
		return(0);
	}
	szDocName = argv[1];

	doc = xmlReadFile(szDocName,"GB2312",XML_PARSE_RECOVER);  //�����ļ� 
	
	if (NULL == doc) 
	{ 	
		fprintf(stderr,"Document not parsed successfully. \n"); 	
		return -1; 
	} 
	
	curNode = xmlDocGetRootElement(doc); 
	/*���ȷ�ϵ�ǰ�ĵ��а�������*/ 
	if (NULL == curNode) 
	{ 
		fprintf(stderr,"empty document\n"); 
		xmlFreeDoc(doc); 
		return -1; 
	} 

	curNode = curNode->children;
	while (NULL != curNode)
	{
		//ɾ��newNode1
		if (!xmlStrcmp(curNode->name, BAD_CAST "newNode1"))
		{
			xmlNodePtr tempNode;
			tempNode = curNode->next;
			xmlUnlinkNode(curNode);
			xmlFreeNode(curNode);
			curNode = tempNode;
			continue;
		}
		
		//�޸�node2������ֵ
		if (!xmlStrcmp(curNode->name, BAD_CAST "node2"))
		{
			xmlSetProp(curNode,BAD_CAST "attribute", BAD_CAST "no");
		}
		//�޸�newNode2������
		if (!xmlStrcmp(curNode->name, BAD_CAST "newNode2"))
		{
			xmlNodeSetContent(curNode, BAD_CAST "content changed");
		}

		//����һ������
		if (!xmlStrcmp(curNode->name, BAD_CAST "newNode3"))
		{
			xmlNewProp(curNode, BAD_CAST "newAttr", BAD_CAST "YES");
		}

		//����һ���ӽڵ�
		if (!xmlStrcmp(curNode->name, BAD_CAST "son"))
		{
			xmlNewTextChild(curNode, NULL, BAD_CAST "newGrandSon", BAD_CAST "new content");
		}
		
		curNode = curNode->next;
	}

	//�洢xml�ĵ�
	int nRel = xmlSaveFile("ChangedXml.xml",doc);
	if (nRel != -1)
	{
		cout<<"һ��xml�ĵ�������,д��"<<nRel<<"���ֽ�"<<endl;
	}
	xmlFreeDoc(doc); 
	return 0;
}
