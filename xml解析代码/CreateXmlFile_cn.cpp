/********************************************************************
	created:	2007/11/17
	created:	9:11:2007   15:34
	filename:	CreateXmlFile.cpp
	author:		Wang xuebin 
	depend:		libxml2.lib iconv.lib
	build:		nmake TARGET_NAME=CreateXmlFile_cn
	
	purpose:	创建一个xml文件，其中包含中文
*********************************************************************/

#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <iostream.h>
#include "wxb_codeConv.c"  //自己写的编码转换函数

int main(int argc, char **argv)
{
	//定义文档和节点指针
	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"root");
	
	//设置根节点
	xmlDocSetRootElement(doc,root_node);
	
	//一个中文字符串转换为UTF-8字符串，然后写入
	char* szOut = g2u("节点1的内容");

	//在根节点中直接创建节点
	xmlNewTextChild(root_node, NULL, BAD_CAST "newNode1", BAD_CAST "newNode1 content");
	xmlNewTextChild(root_node, NULL, BAD_CAST "newNode2", BAD_CAST "newNode2 content");
	xmlNewTextChild(root_node, NULL, BAD_CAST "newNode3", BAD_CAST "newNode3 content");

	xmlNewChild(root_node, NULL, BAD_CAST "node1",BAD_CAST szOut);
	free(szOut);

	//创建一个节点，设置其内容和属性，然后加入根结点
	xmlNodePtr node = xmlNewNode(NULL,BAD_CAST"node2");
	xmlNodePtr content = xmlNewText(BAD_CAST"NODE CONTENT");
	xmlAddChild(root_node,node);
	xmlAddChild(node,content);
	szOut = g2u("属性值");
	xmlNewProp(node,BAD_CAST"attribute",BAD_CAST szOut);
	free(szOut);

	//创建一个中文节点
	szOut = g2u("中文节点");
	xmlNewChild(root_node, NULL, BAD_CAST szOut,BAD_CAST "content of chinese node");
	free(szOut);

	//存储xml文档
	int nRel = xmlSaveFormatFileEnc("CreatedXml_cn.xml",doc,"GB2312",1);
	if (nRel != -1)
	{
		cout<<"一个xml文档被创建,写入"<<nRel<<"个字节"<<endl;
	}

	xmlFreeDoc(doc);

	return 1;
}
