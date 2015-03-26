#include "xmlParse.h"

static iconv_t cd;
static xmlDocPtr doc;
xmlNodePtr root; 
PALETTERESETDATA g_ResetData;
LanguageGroup m_LanguageGroup;
int nCurIndex;
PALETTE  g_OnePalette;
int nLoadMode;

char m_szAppPath[MAX_PATH];

GSTATUS GetVersionName(char *pczPath, GVERSION *pVersion)
{
	xmlDocPtr doc;
	xmlNodePtr root,curNode;
	int i,n;
	XmlLoadPathName(pczPath);

	openLibXml();
	doc = xmlReadFile(pczPath, "UTF-8", XML_PARSE_RECOVER);   

	if (NULL == doc)
		return GD_ERR_INVALID_PARAM;

	root = xmlDocGetRootElement(doc);
	curNode = root->children;

	while(curNode != NULL)
	{
		if(!xmlStrcmp(curNode->name, (const xmlChar*)"Version"))
		{
			pVersion->szVersion = (char*)xmlNodeGetContent(curNode);
			break;
		}
		curNode = curNode->next;
	}	
	closeLibXml(pczPath);

	return GD_ERR_OK;
}


GSTATUS xmlLoadPalette(char *pczPath, Gint32 nDayNightMode, Gint32 nIndex)
{
	xmlDocPtr doc;
	xmlNodePtr root,curNode;
	int paletNum =0;

	XmlLoadPathName(pczPath);

	//更改载入模式：单个载入
	nLoadMode = 1;
	openLibXml();
	doc = xmlReadFile(pczPath, "UTF-8", XML_PARSE_RECOVER);   

	g_OnePalette = malloc(sizeof(PALETTE));

	while(curNode != NULL)
	{
		if(!xmlStrcmp(curNode->name, (const xmlChar*)"PaletteNum"))
		{
			char* cstr = (char*)xmlNodeGetContent(curNode);
			paletNum = atoi(cstr);
			break;
		}
		curNode = curNode->next;
	}

	if(paletNum <= 0)
		return GD_ERR_NO_DATA;

	if (nIndex > paletNum || (nDayNightMode != 0 && nDayNightMode != 1))
	{
		return GD_ERR_INVALID_PARAM;
	}

	while(curNode != NULL)
	{
		if(!xmlStrcmp(curNode->name, (const xmlChar*)"Palette"))
		{
			if (xmlHasProp(curNode, BAD_CAST "No"))	
			{
				int tmpNum;
				char* tmpStr = (char*)xmlGetProp(curNode, BAD_CAST "No");
				tmpNum = atoi(tmpStr);

				if (nIndex == tmpNum)
				{				
					if (xmlHasProp(curNode, BAD_CAST "Type"))			
					{
						if(0 == nDayNightMode)
						{
							if (!strcmp((char*)xmlGetProp(curNode, BAD_CAST "Type"), "Day"))
							{
								xmlNodePtr CtgNode = curNode->children;
								loadCategory(curNode, true);
							}
							break;
						}
						else if (1 == nDayNightMode)
						{
							if (!strcmp((char*)xmlGetProp(curNode, BAD_CAST "Type"), "Night"))
							{
								xmlNodePtr CtgNode = curNode->children;
								loadCategory(curNode, false);
							}
							break;
						}
					}
				}
			}
		}
		curNode = curNode->next;
	}

	closeLibXml(pczPath);	
}

void Dat2Xml()
{
	int i,j,k,l;
	FILE *fp;
	char szFileName[MAX_PATH];

	g_ResetData.g_bNight = Gfalse;
	nCurIndex = 0;

	//载入类名字典
	sprintf(szFileName, "%s%s", m_szAppPath, "\\PaletteHelp.dat");
	fp = fopen(szFileName, "rb");
	if(fp)
	{
		size_t st=fread(&m_LanguageGroup,sizeof(LanguageGroup),1,fp);
		if(st!=1){
			memset(&m_LanguageGroup,0,sizeof(LanguageGroup));}
		fclose(fp);fp=NULL;
	}

	for (l=0; l<g_palette.nTotalNum*2; ++l)
	{
		xmlNodePtr scaleNode[9],node;
		char* cs;
		char numTrans[3] = {0};

		nCurIndex = l*0.5;
		_itoa(l*0.5, numTrans, 10);

		node = xmlNewNode(NULL, BAD_CAST "Palette");
		if (l%2 == 0)
		{
			cs = g_palette.m_Ppalette[nCurIndex].czDayName;
			xmlNewProp(node, BAD_CAST "Type", BAD_CAST "Day");
			g_ResetData.g_bNight = Gfalse;
		}
		else if (l%2 == 1)
		{
			cs = g_palette.m_Ppalette[nCurIndex].czNightName;
			xmlNewProp(node, BAD_CAST "Type", BAD_CAST "Night");
			g_ResetData.g_bNight = Gtrue;
		}

		xmlNewProp(node, BAD_CAST "Name", BAD_CAST cs);
		xmlNewProp(node, BAD_CAST "No", BAD_CAST numTrans);
		xmlAddChild(root, node);

		//		addCategory("场景",node,0, PALETTEUNIT_NUM);
		addCategory("区域", 0, node, CategItemCount(0)/*类型个数*/);
		addCategory("道路", 1, node, CategItemCount(1));
		addCategory("导航线", 2, node, CategItemCount(2));
		addCategory("文本", 3, node, CategItemCount(3));
		addCategory("OPERA", 4, node, CategItemCount(4));
		addCategory("放大路口", 5, node, CategItemCount(5));
		addCategory("点", 6, node, CategItemCount(6));
		addCategory("街区图", 7, node, CategItemCount(7));
		addCategory("实时交通", 8, node, CategItemCount(8));
	}
}

void ExportXml(char* name)
{
	char numTrans[3] ={0};

	if (0 == g_palette.nTotalNum)
	{
		return;
	}
	//打开libxml库

	openLibXml();

	_itoa(g_palette.nTotalNum, numTrans, 10); //////色盘个数
	xmlNewTextChild(root, NULL,  BAD_CAST "PaletteNum", BAD_CAST numTrans);	
	xmlNewTextChild(root, NULL, BAD_CAST "Version", BAD_CAST g_palette.crVersion);

	Dat2Xml();

	closeLibXml(name);

	//加载应用程序路径, windows方法
// 	CString strFullName = AfxGetApp()->m_pszHelpFilePath;
// 	if("" == strFullName)
// 	{
// 		char crPath[20] = {0};
// 		::GetModuleFileNameA(NULL, crPath, MAX_PATH);
// 		//		strcat(crPath, "mine.xml");	
// 	}
// 	::ShellExecute(NULL, _T("open"), _T("explorer.exe"), _T("/select,.\\config.xml"), NULL, SW_SHOWNORMAL );
}

void loadCategory(xmlNodePtr CtgNode, bool bDay)
{
	while (CtgNode != NULL)
	{
		int nCtgID;
		int offSet;

		if (!xmlStrcmp(CtgNode->name, BAD_CAST "Category"))
		{
			xmlNodePtr typeNode;
			int i;
			int nScale ;
			int ctgCount;

			if (xmlHasProp(CtgNode, BAD_CAST "CategID"))
			{
				char* strCtg = (char*)xmlGetProp(CtgNode, BAD_CAST "CategID");
				nCtgID = atoi(strCtg);
				TypeOffSet(nCtgID, &offSet);
			}

			typeNode = CtgNode->children;
			i = 0;
			ctgCount = CategItemCount(nCtgID);

			//解析二级节点
			while (typeNode != NULL)
			{
				if (!xmlStrcmp(typeNode->name, BAD_CAST "Type"))
				{
					xmlNodePtr scaleNode = typeNode->children;
					while(scaleNode != NULL)
					{
						if(!xmlStrcmp(scaleNode->name, BAD_CAST "Scale"))
						{
							xmlNodePtr ItemNode;
							char* strScale = (char*)xmlGetProp(scaleNode, BAD_CAST "no");
							nScale = atoi(strScale);

							//颜色节点
							ItemNode = scaleNode->children;
							while (ItemNode != NULL)
							{
								LoadColrLineInfo(ItemNode, nScale, nCtgID, offSet, i, bDay);
								ItemNode = ItemNode->next;
							}
						}

						scaleNode = scaleNode->next;
					}
				}
				typeNode = typeNode->next;
				++i;
			}

		}
		CtgNode = CtgNode->next;
	}
}

void loadPalettes(xmlNodePtr curNode)
{
	int nCount = -2;
	bool bDay = true;

	while(curNode != NULL)
	{
		if (nCount%2 == 0)
		{
			bDay = true;
		}
		else
		{
			bDay = false;
		}
		if(!xmlStrcmp(curNode->name, (const xmlChar*)"Palette"))
		{
			xmlNodePtr CtgNode;
			if (xmlHasProp(curNode, BAD_CAST "No"))
			{
				char* cstrNum = (char*)xmlGetProp(curNode, BAD_CAST "No");
				nCurIndex = atoi(cstrNum);
			}

			if (xmlHasProp(curNode, BAD_CAST "Type"))
			{
				if (xmlHasProp(curNode, BAD_CAST "Name"))
				{	
					char* xmlStr = (char*)xmlGetProp(curNode, BAD_CAST "Name");

					if (!strcmp((char*)xmlGetProp(curNode, BAD_CAST "Type"), "Day"))
					{
						sprintf(g_palette.m_Ppalette[nCurIndex].czDayName,"%s", xmlStr);
					}
					else if (!strcmp((char*)xmlGetProp(curNode, BAD_CAST "Type"), "Night"))
					{	
						sprintf(g_palette.m_Ppalette[nCurIndex].czNightName,"%s", xmlStr);
					}
				}
			}	

			loadCategory(curNode->children, bDay);
		}
		curNode = curNode->next;
		++nCount;
	}
}

void loadXml()
{
	xmlDocPtr doc;
	xmlNodePtr root, curNode;

	//更改载入模式：全部载入
	nLoadMode = 0;

	xmlKeepBlanksDefault(0);  
	doc = xmlReadFile("mine.xml", "UTF-8", XML_PARSE_RECOVER);   

	if (NULL == doc)
		return;

	root = xmlDocGetRootElement(doc);

	curNode = root->children;

	//加载 色盘个数，版本信息
	while(curNode != NULL)
	{
		if(!xmlStrcmp(curNode->name, (const xmlChar*)"PaletteNum"))
		{
			char* cstr = (char*)xmlNodeGetContent(curNode);
			int nNum = atoi(cstr);
			g_palette.nTotalNum = (char)atoi(cstr);
		}
		if (!xmlStrcmp(curNode->name, (const xmlChar*)"Version"))
		{
			char* cstr = (char*)xmlNodeGetContent(curNode);
			int m,n;
			n = strlen(cstr);
			for (m=0; m<n; ++m)
			{
				g_palette.crVersion[m]= *cstr++;
			}
			g_palette.crVersion[m] = '\0';
			break;
		}
		curNode = curNode->next;
	}

	if (NULL == g_palette.m_Ppalette)
	{
		//g_palette.m_Ppalette = (PPALETTE)malloc(sizeof(PALETTE)*g_palette.nTotalNum);
		g_palette.m_Ppalette = new PALETTE[g_palette.nTotalNum];
	}

	//加载全部色盘信息
	loadPalettes(root->children);
}

void LoadColrLineInfo(xmlNodePtr ItemNode, int nScale, int nCountType, int offSet, int i, bool bDay)
{		
	char* subStr;

	char* strVals = (char*)xmlNodeGetContent(ItemNode);
	::CharLowerA(strVals);

	if(!xmlStrcmp(ItemNode->name, BAD_CAST "Color"))
	{
		//颜色信息
		int j=0;
		subStr = strVals;
		for (int j=0; j<GetColorTypeCount(nCountType); ++j)
		{
			subStr=strstr(strVals,",");
			if (strlen(strVals) <= 0 )
			{
				continue;
			}

			char strItem[9] = {0};
			if (strlen(strVals) > 0 && subStr == NULL)
			{
				subStr = strVals;
				memcpy(strItem, strVals, strlen(strVals));
				strVals = "";
			}
			else
			{
				memcpy(strItem, strVals, subStr-strVals);
				strVals = strVals+(subStr-strVals);
				strVals = strVals++;
			}

			if (strcmp("", strItem) && strcmp(",", strItem))
			{
				long nVal = 0;
				sscanf(strItem, "%x", &nVal);

				if (0 == nLoadMode)
				{
					//载入全部色盘时存储在g_palette的第nCurIndex个色盘上
					if (bDay)
					{
						g_palette.m_Ppalette[nCurIndex].stColorUnit[offSet+i].stDayColor[nScale][j].rgbRed = nVal>>24;
						g_palette.m_Ppalette[nCurIndex].stColorUnit[offSet+i].stDayColor[nScale][j].rgbGreen =  (nVal>>16)&0xff;
						g_palette.m_Ppalette[nCurIndex].stColorUnit[offSet+i].stDayColor[nScale][j].rgbBlue = (nVal>>8)&0xff;
						g_palette.m_Ppalette[nCurIndex].stColorUnit[offSet+i].stDayColor[nScale][j].rgbReserved = nVal&0xff;
					}
					else
					{
						g_palette.m_Ppalette[nCurIndex].stColorUnit[offSet+i].stNightColor[nScale][j].rgbRed = nVal>>24;
						g_palette.m_Ppalette[nCurIndex].stColorUnit[offSet+i].stNightColor[nScale][j].rgbGreen = (nVal>>16)&0xff;
						g_palette.m_Ppalette[nCurIndex].stColorUnit[offSet+i].stNightColor[nScale][j].rgbBlue = (nVal>>8)&0xff;
						g_palette.m_Ppalette[nCurIndex].stColorUnit[offSet+i].stNightColor[nScale][j].rgbReserved = nVal&0xff;
					}
				}
				else if (1 == nLoadMode)
				{
					//载入单个色盘
					if (bDay)
					{
						g_OnePalette.stColorUnit[offSet+i].stDayColor[nScale][j].rgbRed = nVal>>24;
						g_OnePalette.stColorUnit[offSet+i].stDayColor[nScale][j].rgbGreen =  (nVal>>16)&0xff;
						g_OnePalette.stColorUnit[offSet+i].stDayColor[nScale][j].rgbBlue = (nVal>>8)&0xff;
						g_OnePalette.stColorUnit[offSet+i].stDayColor[nScale][j].rgbReserved = nVal&0xff;
					}
					else
					{
						g_OnePalette.stColorUnit[offSet+i].stNightColor[nScale][j].rgbRed = nVal>>24;
						g_OnePalette.stColorUnit[offSet+i].stNightColor[nScale][j].rgbGreen = (nVal>>16)&0xff;
						g_OnePalette.stColorUnit[offSet+i].stNightColor[nScale][j].rgbBlue = (nVal>>8)&0xff;
						g_OnePalette.stColorUnit[offSet+i].stNightColor[nScale][j].rgbReserved = nVal&0xff;
					}

				}

			}
		}

	}
	else if(!xmlStrcmp(ItemNode->name, BAD_CAST "Line"))
	{
		//线宽信息：
		int m=0;
		subStr = strVals;

		for (m=0; m<GetLineTypeCount(nCountType); ++m)
		{
			subStr=strstr(strVals,",");
			if (strlen(strVals) <= 0 )
			{
				continue;
			}

			char strItem[9] = {0};
			if (strlen(strVals) > 0 && subStr == NULL)
			{
				subStr = strVals;
				memcpy(strItem, strVals, strlen(strVals));
				strVals = "";
			}
			else
			{
				memcpy(strItem, strVals, subStr-strVals);
				strVals = strVals+(subStr-strVals);
				strVals = strVals++;		
			}

			if (strcmp("", strItem) && strcmp(",", strItem))
			{
				int lineVal = atoi(strItem);

				if (0 == nLoadMode)
				{
					//载入全部色盘的线宽
					g_palette.m_Ppalette[nCurIndex].stLineWidth[i+offSet].LW[nScale][m] = (char)lineVal;	
				}
				else if (1 == nLoadMode)
				{
					//载入单个色盘线宽信息
					g_OnePalette.stLineWidth[i+offSet].LW[nScale][m] = (char)lineVal;	
				}
			}
		}
	}
}

void Conv2Utf8(const char* src, char* dest)
{
	unsigned int nconv;
	size_t nSize,nOutSize;
	char* crOut;
	const char* crIn = src;
	memset(dest,0,sizeof(dest));
	crOut = dest;

	if (crIn == NULL || 0 == *crIn)
	{
		return;
	}

	crIn = src;
	nSize = strlen(crIn);
	nOutSize = strlen(crIn)*2;
	nconv = iconv(cd,(const char**)&crIn,&nSize,&crOut,&nOutSize);

	memcpy(dest, crOut, strlen(crOut));
}

void openLibXml()
{
	doc = xmlNewDoc (BAD_CAST "1.0");

	//将字符串编码由utf-8转换为gbk
	cd=iconv_open("utf-8","gb2312"); 

	root = xmlNewNode (NULL, BAD_CAST "root");

	//	xmlNewProp (root, BAD_CAST "version", BAD_CAST "1.0");
	xmlDocSetRootElement (doc, root);

}

void closeLibXml(char* name)
{
	xmlNodePtr countNode;

	xmlSaveFormatFileEnc (name, doc, "utf-8", 1);
	xmlFreeDoc (doc);
	xmlCleanupParser ();
	xmlMemoryDump ();

	iconv_close(cd);
}



long GetColorItem(int i, int j, int k, char* inStr)
{
	RGBQUAD rgbTemp;
	long nRgb;
	int n;
	int strLen;
	char addStr[9] = {0};
	COLORREF rgb;

	if (!g_ResetData.g_bNight)
	{
		rgbTemp = g_palette.m_Ppalette[nCurIndex].stColorUnit[i].stDayColor[j][k];	
		rgb = rgbTemp.rgbRed<<24|rgbTemp.rgbGreen<<16|rgbTemp.rgbBlue<<8|rgbTemp.rgbReserved;
		nRgb = rgb;
	}
	else
	{
		rgbTemp = g_palette.m_Ppalette[nCurIndex].stColorUnit[i].stNightColor[j][k];	
		rgb = rgbTemp.rgbRed<<24|rgbTemp.rgbGreen<<16|rgbTemp.rgbBlue<<8|rgbTemp.rgbReserved;
		nRgb = rgb;
	}
	for (n = 0; n < 8; ++n)
	{
		addStr[n] = '0';
	}
	addStr[n] = '\0';
	_ltoa(nRgb, inStr, 16);
	::CharUpperA(inStr);
	strLen = strlen(inStr);
	if (strLen>0 && strLen<8)
	{
		strLen = 8-strLen;
		memcpy(addStr+strLen, inStr, strlen(inStr));
		strcpy(inStr, addStr);
	}
	return nRgb;
}


//二级节点
void addTypeNode(const int nCtgID, const int nTypeIDNums, xmlNodePtr parent)
{
	int i,j;

	//nTypeIDNums ：二级节点中包含的三级节点个数，同时也是第三级节点的ID个数
	for(i=0;i<nTypeIDNums;i++)
	{
		int offSet;
		char crTrans[DIRCNAME_LEN] = {0};
		xmlNodePtr typeNode;

		TypeOffSet(nCtgID, &offSet);

		if (m_LanguageGroup.ndict > 0)
		{
			Conv2Utf8(m_LanguageGroup.Dict_Text[i+offSet].English, crTrans);
		}

		//遇到"//"或""表示是预留的空间
		if(!strcmp("//", crTrans) || !strcmp("", crTrans))
			break;	

		typeNode = xmlNewNode(NULL, BAD_CAST "Type");
		xmlNewProp(typeNode, BAD_CAST "Name", BAD_CAST crTrans);

		// 		int m=0;
		// 		char LineNames[100] = {0};
		// 		char strNames[40]={0}; 
		// 		for (m=0; m < GetLineTypeCount(nCtgID); ++m)
		// 		{
		// 			strcat(LineNames, GetLineString(nCtgID, m));
		// 			strcat(LineNames, ",");
		// 
		// 			Conv2Utf8(LineNames, strNames);
		// 		}
		// 		xmlNewProp(typeNode, BAD_CAST "line", BAD_CAST strNames);

		//添加三级节点：放大倍数
		for(j=0;j<SCALE_NUM;j++)
		{
			char numTrans[40] = {0};
			xmlNodePtr scaleNode;
			int nCount;
			char crNames[100] = {0};
			char crValues[100] = {0};
			char colrCopy[100] = {0};

			_itoa(j, numTrans, 10);
			scaleNode = xmlNewNode(NULL, BAD_CAST "Scale");
			xmlNewProp(scaleNode, BAD_CAST "no", BAD_CAST numTrans );

			//			xmlNewProp(typeNode, BAD_CAST "ID", BAD_CAST m_LanguageGroup.Dict_Text[i+offSet].China);
			//第四级节点：颜色，线宽，字体大小，共有二级节点个数个不同组合。

			//添加颜色属性
			nCount = GetColorTypeCount(nCtgID);
			for(int k=0;k<nCount;++k)
			{
				// 				strcat(crNames, GetNameString(nCtgID, k));
				// 				strcat(crNames, ',');
				char strVals[40] = {0};

				GetColorItem(i+offSet, j, k, strVals);
				strcat(crValues, strVals);
				strcat(crValues, ",");
			}

			// 			char crColorName[100]={0};
			// 			Conv2Utf8(crNames, crColorName);

			//			xmlNewProp(ColorNode, BAD_CAST "content", BAD_CAST crColorName);
			if (strlen(crValues) > 0 && crValues != NULL)
			{
				xmlNodePtr ColorNode;
				xmlNodePtr colrValNode;
				ColorNode = xmlNewNode(NULL, BAD_CAST "Color");
				memcpy(colrCopy, crValues, strlen(crValues)-1);
				colrValNode = xmlNewText(BAD_CAST colrCopy);
				xmlAddChild(ColorNode, colrValNode);
				xmlAddChild(scaleNode, ColorNode);
			}


			// 添加线宽属性
			int LineNum = GetLineTypeCount(nCtgID);;
			char LineValues[100] = {0};
			char lineCopy[100] = {0};
			for(int k=0;k<LineNum;++k)
			{
				char cLine = g_palette.m_Ppalette[nCurIndex].stLineWidth[i+offSet].LW[j][k];
				int nLine=0;
				char strTranslate[6] = {0};
				memcpy(&nLine,&cLine,sizeof(char));
				_itoa(nLine, strTranslate, 10);
				strcat(LineValues, strTranslate);
				strcat(LineValues, ",");
			}

			if (strlen(LineValues) > 0)
			{
				xmlNodePtr LineNode = xmlNewNode(NULL, BAD_CAST "Line");
				//				xmlNewProp(LineNode, BAD_CAST "content", BAD_CAST strNames);
				xmlNodePtr lineValNode;
				memcpy(lineCopy, LineValues, strlen(LineValues)-1);
				lineValNode = xmlNewText(BAD_CAST lineCopy);
				xmlAddChild(LineNode, lineValNode);
				xmlAddChild(scaleNode, LineNode);
			}

			xmlAddChild(typeNode, scaleNode);
			xmlAddChild(parent, typeNode);
		}		
	}	
}

void addCategory(const char* strCateg, const int nCtgID, xmlNodePtr parent, const int nTypeNums)
{
	xmlNodePtr CategNode;
	char trans[40] = {0};
	char ctgID[3] = {0};

	//一级节点的标签属性
	Conv2Utf8(strCateg, trans);
	CategNode = xmlNewNode(NULL, BAD_CAST "Category");
	xmlNewProp(CategNode, BAD_CAST "Type", BAD_CAST trans);

	_itoa(nCtgID, ctgID, 10);
	xmlNewProp(CategNode, BAD_CAST "CategID", BAD_CAST ctgID);
	xmlAddChild(parent, CategNode);

	//添加二级节点
	addTypeNode(nCtgID, nTypeNums, CategNode);
}

void XmlLoadPathName(char* pathName)
{
	int i,n;
	n = strlen(pathName);
	for (i=0; i<n; ++i)
	{
		m_szAppPath[i] = *pathName++;
	}
	m_szAppPath[i] = '\0';
}

char* GetLineString(int nCtgID, int nLineID)
{	
	switch (nCtgID)
	{
	case E_AREA: 
		return "";
		break;
	case E_ROAD:
		switch (nLineID)
		{
		case 0:
			return "主宽度";
			break;
		case 1:
			return "边线";
			break;
		case 2:
			return "3D宽度";
			break;
		case 3:
			return "3D边线";
			break;
		default:
			return "";
			break;
		}
		break;
	case E_NAVI_LINE:
		switch (nLineID)
		{
		case 0:
			return "主宽度";
			break;
		case 1:
			return "边线";
			break;
		case 2:
			return "3D宽度";
			break;
		case 3:
			return "3D边线";
			break;
		default:
			return "";
			break;
		}
		break;
	case E_TEXT:
		return "大小";
		break;
	case E_OPERA:
		switch (nLineID)
		{
		case 0:
			return "主宽度";
			break;
		case 1:
			return "边线";
			break;
		case 2:
			return "3D宽度";
			break;
		case 3:
			return "3D边线";
			break;
		default:
			return "";
			break;
		}
		break;
	case E_ZOOM_OUT:
		switch (nLineID)
		{
		case 0:
			return "主宽度";
			break;
		case 1:
			return "边线";
			break;
		default:
			return "";
			break;
		}
		break;
	case E_POINT:
		switch (nLineID)
		{
		case 0:
			return "中字体";
			break;
		case 1:
			return "大字体";
			break;
		case 2:
			return "";

			break;
		case 3:
			return "小字体";
			break;
		default:
			return "";
			break;
		}
		break;
	case E_STREETS:
		switch (nLineID)
		{
		case 0:
			return "主宽度";
			break;
		case 1:
			return "边线";
			break;
		case 2:
			return "3D宽度";
			break;
		case 3:
			return "3D边线";
			break;
		default:
			break;
		}
		break;
	case E_TRAFFIC:
		return "大小";
		break;		
	default:
		return "";
		break;
	}	
}

int GetLineTypeCount(int nTypeID)
{	
	switch (nTypeID)
	{
	case E_AREA: 
		return 0;
		break;
	case E_ROAD:
		return 4;
		break;
	case E_NAVI_LINE:
		return 4;
		break;
	case E_TEXT:
		return 1;
		break;
	case E_OPERA:
		return 4;
		break;
	case E_ZOOM_OUT:
		return 2;
		break;
	case E_POINT:
		return 4;
		break;
	case E_STREETS:
		return 4;
		break;
	case E_TRAFFIC:
		return 1;
		break;		
	default:
		return -1;
		break;
	}

}
int GetColorTypeCount(int nTypeID)
{	
	switch (nTypeID)
	{
	case E_AREA: 
		return 1;
		break;
	case E_ROAD:
		return 4;
		break;
	case E_NAVI_LINE:
		return 4;
		break;
	case E_TEXT:
		return 3;
		break;
	case E_OPERA:
		return 4;
		break;
	case E_ZOOM_OUT:
		return 3;
		break;
	case E_POINT:
		return 4;
		break;
	case E_STREETS:
		return 6;
		break;
	case E_TRAFFIC:
		return 4;
		break;		
	default:
		return -1;
		break;
	}
}

void TypeOffSet(int nCtgID, int* nOffSet)
{
	switch (nCtgID)
	{
	case E_AREA:
		*nOffSet = 0;
		break;
	case E_ROAD:
		*nOffSet = 10;
		break;
	case E_NAVI_LINE:
		*nOffSet = 41;
		break;
	case E_TEXT:
		*nOffSet = 61;
		break;
	case E_OPERA:
		*nOffSet = 81;
		break;
	case E_ZOOM_OUT:
		*nOffSet = 91;
		break;
	case E_POINT:
		*nOffSet = 98;
		break;
	case E_STREETS:
		*nOffSet = 151;
		break;
	case E_TRAFFIC:
		*nOffSet = 401;
		break;
	default:
		*nOffSet = -1;
		break;		
	}

}

int CategItemCount(int nCtgID)
{
	switch (nCtgID)
	{
	case E_AREA:
		return 9;
		break;
	case E_ROAD:
		return 30;
		break;
	case E_NAVI_LINE:
		return 19;
		break;
	case E_TEXT:
		return 19;
		break;
	case E_OPERA:
		return 9;
		break;
	case E_ZOOM_OUT:
		return 7;
		break;
	case E_POINT:
		return 52;
		break;
	case E_STREETS:
		return 249;
		break;
	case E_TRAFFIC:
		return 19;
		break;
	default:
		return -1;
		break;		
	}
}


void GetColorNameByID(int nID, char* inStr)
{
	switch (nID)
	{
	case 0:
		Conv2Utf8("主颜色", inStr);
		break;
	case 1:
		Conv2Utf8("2.5D主颜色", inStr);
		break;
	case 2:
		Conv2Utf8("边线颜色", inStr);
		break;
	case 3:
		Conv2Utf8("2.5D边线", inStr);
		break;
	case 4:
		Conv2Utf8("线状道路", inStr);
		break;
	case 5:
		Conv2Utf8("线状道路边线", inStr);
		break;
	case 6:
		Conv2Utf8("侧面", inStr);
		break;
	case 10:
		Conv2Utf8("顶点", inStr);
		break;
	case 11:
		Conv2Utf8("交界线", inStr);
		break;
	case 12:
		Conv2Utf8("主宽度", inStr);
		break;
	case 13:
		Conv2Utf8("3D宽度", inStr);
		break;
	case 14:
		Conv2Utf8("边线宽度", inStr);
		break;
	case 15:
		Conv2Utf8("3D边线", inStr);
		break;
	case 16:
		Conv2Utf8("区域", inStr);
		break;
	case 17:
		Conv2Utf8("阴影颜色", inStr);
		break;
	case 18:
		Conv2Utf8("大小", inStr);
		break;
	case 19:
		Conv2Utf8("小字体", inStr);
		break;
	case 20:
		Conv2Utf8("中字体", inStr);
		break;
	case 21:
		Conv2Utf8("大字体", inStr);
		break;
	default:
		break;
	}
}

cvector vct_new(int nSize)
{
	cvector aVec;
	aVec.data = malloc(nSize);
	aVec.nLen = 0;
	aVec.elemSize = nSize;
	return aVec;
}

void vct_pushBack(cvector* vec, void* elem, int nSize)
{
	int aSize = vec->nLen;

	vec->elemSize = nSize;

	if (aSize == 0)
	{
		vec->data = malloc(nSize);	
		memcpy((unsigned char*)vec->data, (unsigned char*)elem, nSize);
		++vec->nLen;
	}
	else if (aSize > 0)
	{
		vec->data = realloc(vec->data, (aSize+1)*nSize);
		memcpy(((unsigned char*)vec->data)+nSize, (unsigned char*)elem, nSize);
		++vec->nLen;
	}
}

void vct_getAt(cvector* vec, int i, void* elem)
{
	if (vec->nLen >0 && vec->data != GNULL)
	{
		elem = malloc(vec->elemSize);
		memcpy((unsigned char*)elem,((unsigned char*)vec->data)+i, vec->elemSize);
	}
	// 	if ((cvector*)(vec->data) != GNULL)
	// 	{
	// 		cvector* temp = &vct_new(sizeof(cvector));
	// 		void* voidData = (void*)temp;
	// 		memcpy((unsigned char*)voidData,(unsigned char*)vec->data, sizeof(cvector));
	// 		temp = (cvector*)voidData;
	// 		int nCount = temp->nLen;
	// 		for (int j=0; j<nCount; ++j)
	// 		{
	// 			cvector* vctTemp;
	// 			vctTemp = &vct_new(temp->elemSize);
	// 
	// 			void* voidVec = (void*)vctTemp;
	// 			memcpy((unsigned char*)voidVec,((unsigned char*)temp->data)+j, temp->elemSize);
	// 			vctTemp = (cvector*)voidVec;
	// 			vct_pushBack((cvector*)&elem, vctTemp, sizeof(cvector));
	// 		}
	// 	}
}

void vct_free(cvector* vec)
{
	if (vec != NULL)
	{
		delete[] vec;
	}
}