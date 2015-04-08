/********************************************************************
	created:	2007/11/15
	created:	15:11:2007   16:01
	filename:	XpathForXmlFile.cpp
	author:		Wang xuebin 
	depend:		libxml2.lib
	build:		nmake TARGET_NAME=XPathForXmlFile
	
	purpose:	使用XPATH查找xml文档中的节点
*********************************************************************/

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <iostream.h>

xmlXPathObjectPtr get_nodeset(xmlDocPtr doc, const xmlChar *szXpath) 
{
	xmlXPathContextPtr context;	    //XPATH上下文指针
	xmlXPathObjectPtr result;		//XPATH对象指针，用来存储查询结果
	
	context = xmlXPathNewContext(doc);		//创建一个XPath上下文指针
	if (context == NULL) 
	{	
		printf("context is NULL\n");
		return NULL; 
	}
	
	result = xmlXPathEvalExpression(szXpath, context); //查询XPath表达式，得到一个查询结果
	xmlXPathFreeContext(context);				//释放上下文指针
	if (result == NULL) 
	{
		printf("xmlXPathEvalExpression return NULL\n"); 
		return NULL;  
	}
	
	if (xmlXPathNodeSetIsEmpty(result->nodesetval))   //检查查询结果是否为空
	{
		xmlXPathFreeObject(result);
		printf("nodeset is empty\n");
		return NULL;
	}
	
	return result;	
}

int main(int argc, char* argv[])
{
	xmlDocPtr doc = NULL;			 //定义解析文档指针 
	xmlNodePtr curNode = NULL;		 //定义结点指针(你需要它为了在各个结点间移动) 

	char *szDocName = NULL;
	if (argc <= 1)  
	{
		printf("Usage: %s docname\n", argv[0]);
		return(0);
	}
	szDocName = argv[1];

	doc = xmlReadFile(szDocName,"GB2312",XML_PARSE_RECOVER);  //解析文件 
	
	if (NULL == doc) 
	{ 	
		fprintf(stderr,"Document not parsed successfully. \n"); 	
		return -1; 
	} 
	
	xmlChar *szXpath =BAD_CAST ("/root/node2[@attribute='yes']"); 
	xmlXPathObjectPtr app_result = get_nodeset(doc,szXpath);  //查询并得到结果
	
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

