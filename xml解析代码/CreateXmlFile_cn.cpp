/********************************************************************
	created:	2007/11/17
	created:	9:11:2007   15:34
	filename:	CreateXmlFile.cpp
	author:		Wang xuebin 
	depend:		libxml2.lib iconv.lib
	build:		nmake TARGET_NAME=CreateXmlFile_cn
	
	purpose:	����һ��xml�ļ������а�������
*********************************************************************/

#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <iostream.h>
#include "wxb_codeConv.c"  //�Լ�д�ı���ת������

int main(int argc, char **argv)
{
	//�����ĵ��ͽڵ�ָ��
	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"root");
	
	//���ø��ڵ�
	xmlDocSetRootElement(doc,root_node);
	
	//һ�������ַ���ת��ΪUTF-8�ַ�����Ȼ��д��
	char* szOut = g2u("�ڵ�1������");

	//�ڸ��ڵ���ֱ�Ӵ����ڵ�
	xmlNewTextChild(root_node, NULL, BAD_CAST "newNode1", BAD_CAST "newNode1 content");
	xmlNewTextChild(root_node, NULL, BAD_CAST "newNode2", BAD_CAST "newNode2 content");
	xmlNewTextChild(root_node, NULL, BAD_CAST "newNode3", BAD_CAST "newNode3 content");

	xmlNewChild(root_node, NULL, BAD_CAST "node1",BAD_CAST szOut);
	free(szOut);

	//����һ���ڵ㣬���������ݺ����ԣ�Ȼ���������
	xmlNodePtr node = xmlNewNode(NULL,BAD_CAST"node2");
	xmlNodePtr content = xmlNewText(BAD_CAST"NODE CONTENT");
	xmlAddChild(root_node,node);
	xmlAddChild(node,content);
	szOut = g2u("����ֵ");
	xmlNewProp(node,BAD_CAST"attribute",BAD_CAST szOut);
	free(szOut);

	//����һ�����Ľڵ�
	szOut = g2u("���Ľڵ�");
	xmlNewChild(root_node, NULL, BAD_CAST szOut,BAD_CAST "content of chinese node");
	free(szOut);

	//�洢xml�ĵ�
	int nRel = xmlSaveFormatFileEnc("CreatedXml_cn.xml",doc,"GB2312",1);
	if (nRel != -1)
	{
		cout<<"һ��xml�ĵ�������,д��"<<nRel<<"���ֽ�"<<endl;
	}

	xmlFreeDoc(doc);

	return 1;
}
