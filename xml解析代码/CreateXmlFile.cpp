/********************************************************************
	created:	2007/11/09
	created:	9:11:2007   15:34
	filename:	CreateXmlFile.cpp
	author:		Wang xuebin 
	depend:		libxml2.lib 
	build:		nmake TARGET_NAME=CreateXmlFile
	
	purpose:	����һ��xml�ļ�
*********************************************************************/

#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <iostream.h>

int main()
{
	//�����ĵ��ͽڵ�ָ��
	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"root");
	
	//���ø��ڵ�
	xmlDocSetRootElement(doc,root_node);
	

	//�ڸ��ڵ���ֱ�Ӵ����ڵ�
	xmlNewTextChild(root_node, NULL, BAD_CAST "newNode1", BAD_CAST "newNode1 content");
	xmlNewTextChild(root_node, NULL, BAD_CAST "newNode2", BAD_CAST "newNode2 content");
	xmlNewTextChild(root_node, NULL, BAD_CAST "newNode3", BAD_CAST "newNode3 content");

	//����һ���ڵ㣬���������ݺ����ԣ�Ȼ���������
	xmlNodePtr node = xmlNewNode(NULL,BAD_CAST"node2");
	xmlNodePtr content = xmlNewText(BAD_CAST"NODE CONTENT");
	xmlAddChild(root_node,node);
	xmlAddChild(node,content);
	xmlNewProp(node,BAD_CAST"attribute",BAD_CAST "yes");

	//����һ�����Ӻ����ӽڵ�
	node = xmlNewNode(NULL, BAD_CAST "son");
	xmlAddChild(root_node,node);
	xmlNodePtr grandson = xmlNewNode(NULL, BAD_CAST "grandson");
	xmlAddChild(node,grandson);
	xmlAddChild(grandson, xmlNewText(BAD_CAST "This is a grandson node"));

	//�洢xml�ĵ�
	int nRel = xmlSaveFile("CreatedXml.xml",doc);
	if (nRel != -1)
	{
		cout<<"һ��xml�ĵ�������,д��"<<nRel<<"���ֽ�"<<endl;
	}

	//�ͷ��ĵ��ڽڵ㶯̬������ڴ�
	xmlFreeDoc(doc);
	return 1;
}
