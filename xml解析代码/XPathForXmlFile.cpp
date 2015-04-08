/********************************************************************
	created:	2007/11/15
	created:	15:11:2007   16:01
	filename:	XpathForXmlFile.cpp
	author:		Wang xuebin 
	depend:		libxml2.lib
	build:		nmake TARGET_NAME=XPathForXmlFile
	
	purpose:	ʹ��XPATH����xml�ĵ��еĽڵ�
*********************************************************************/

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <iostream.h>

xmlXPathObjectPtr get_nodeset(xmlDocPtr doc, const xmlChar *szXpath) 
{
	xmlXPathContextPtr context;	    //XPATH������ָ��
	xmlXPathObjectPtr result;		//XPATH����ָ�룬�����洢��ѯ���
	
	context = xmlXPathNewContext(doc);		//����һ��XPath������ָ��
	if (context == NULL) 
	{	
		printf("context is NULL\n");
		return NULL; 
	}
	
	result = xmlXPathEvalExpression(szXpath, context); //��ѯXPath���ʽ���õ�һ����ѯ���
	xmlXPathFreeContext(context);				//�ͷ�������ָ��
	if (result == NULL) 
	{
		printf("xmlXPathEvalExpression return NULL\n"); 
		return NULL;  
	}
	
	if (xmlXPathNodeSetIsEmpty(result->nodesetval))   //����ѯ����Ƿ�Ϊ��
	{
		xmlXPathFreeObject(result);
		printf("nodeset is empty\n");
		return NULL;
	}
	
	return result;	
}

int main(int argc, char* argv[])
{
	xmlDocPtr doc = NULL;			 //��������ĵ�ָ�� 
	xmlNodePtr curNode = NULL;		 //������ָ��(����Ҫ��Ϊ���ڸ��������ƶ�) 

	char *szDocName = NULL;
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
	
	xmlChar *szXpath =BAD_CAST ("/root/node2[@attribute='yes']"); 
	xmlXPathObjectPtr app_result = get_nodeset(doc,szXpath);  //��ѯ���õ����
	
	if (NULL == app_result) 
	{
		printf("app_result is NULL\n"); 
		return -1;
	}
	xmlChar *szValue = NULL;
	if(app_result) 
	{
		xmlNodeSetPtr nodeset = app_result->nodesetval;
		for (int i = 0; i < nodeset->nodeNr; i++) 
		{
			curNode = nodeset->nodeTab[i];    
			if(curNode != NULL) 
			{
				szValue = xmlGetProp(curNode,BAD_CAST "attribute");
				if (szValue != NULL) 
				{
					printf("attribute = %s\n", szValue);
					xmlFree(szValue);
				}
				
				szValue = xmlNodeGetContent(curNode);
				if (szValue != NULL) 
				{
					printf("content = %s\n", szValue);
					xmlFree(szValue);
				}
			}
		}
		xmlXPathFreeObject (app_result);
	}
	xmlFreeDoc(doc);
	return 0;
}

