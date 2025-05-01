// ServerDisplayer.cpp: implementation of the CServerDisplayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ServerDisplayer.h"
#include "CustomArena.h"
#include "GameMain.h"
#include "Log.h"
#include "resource.h"
#include "ServerInfo.h"
#include "SocketManager.h"
#include "User.h"

CServerDisplayer gServerDisplayer;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServerDisplayer::CServerDisplayer() // OK
{
	for (int n = 0; n < MAX_LOG_TEXT_LINE; n++)
	{
		memset(&this->m_log[n], 0, sizeof(this->m_log[n]));
	}

	this->m_fonttitle = CreateFont(50, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial");

	this->m_brush = CreateSolidBrush(RGB(255, 191, 0));
	this->m_brush2 = CreateSolidBrush(RGB(40, 40, 40));
}

CServerDisplayer::~CServerDisplayer() // OK
{
	DeleteObject(this->m_fonttitle);
	DeleteObject(this->m_brush);
}

void CServerDisplayer::Init(HWND hWnd) // OK
{
	this->m_hwnd = hWnd;

	gLog.AddLog(1,"Log");

	gLog.AddLog(gServerInfo.m_WriteChatLog,"Chat_log");

	gLog.AddLog(gServerInfo.m_WriteCommandLog,"Command_log");

	gLog.AddLog(gServerInfo.m_WriteTradeLog,"LogTrade_log");

	gLog.AddLog(gServerInfo.m_WriteConnectLog,"Connect_log");

	gLog.AddLog(gServerInfo.m_WriteHackLog,"Hack_log");

	gLog.AddLog(gServerInfo.m_WriteCashShopLog,"CashShop_log");

	gLog.AddLog(gServerInfo.m_WriteChaosMixLog,"ChaosMix_log");
}

void CServerDisplayer::Run() // OK
{
	this->LogTextPaint();
	this->SetWindowName();
	this->PaintAllInfo();
	this->PaintOnline();
	this->PaintPremium();
	this->PaintSeason();
	this->PaintEventTime();
	this->PaintInvasionTime();
	this->PaintCustomArenaTime();
	this->LogTextPaintConnect();
	this->LogTextPaintGlobalMessage();
}

void CServerDisplayer::SetWindowName() // OK
{
	char buff[256];

	wsprintf(buff,"[%s] %s (ON: %d) GameServer | DATGM Ver 1.0",GAMESERVER_VERSION,gServerInfo.m_ServerName, gObjTotalUser);

	SetWindowText(this->m_hwnd,buff);

	HWND hWndStatusBar = GetDlgItem(this->m_hwnd, IDC_STATUSBAR);

	RECT rect;

	GetClientRect(this->m_hwnd,&rect);

	RECT rect2;

	GetClientRect(hWndStatusBar,&rect2);

	MoveWindow(hWndStatusBar,0,rect.bottom-rect2.bottom+rect2.top,rect.right,rect2.bottom-rect2.top,true);

            int iStatusWidths[] = {190,270,360,450,580, -1};

            char text[256];

            SendMessage(hWndStatusBar, SB_SETTEXT, 7, (LPARAM)text);

			wsprintf(text, "Connected: %d/%d", gObjTotalUser, gServerInfo.m_ServerMaxUserNumber);

            SendMessage(hWndStatusBar, SB_SETTEXT, 0,(LPARAM)text);

			wsprintf(text, "OffStore: %d", gObjOffStore);

            SendMessage(hWndStatusBar, SB_SETTEXT, 1,(LPARAM)text);

			wsprintf(text, "OffAttack: %d", gObjOffAttack);

            SendMessage(hWndStatusBar, SB_SETTEXT, 2,(LPARAM)text);

			wsprintf(text, "Bots Buffer: %d", gObjTotalBot);

            SendMessage(hWndStatusBar, SB_SETTEXT, 3,(LPARAM)text);

			wsprintf(text, "Monsters: %d/%d", gObjTotalMonster,MAX_OBJECT_MONSTER);

            SendMessage(hWndStatusBar, SB_SETTEXT, 4,(LPARAM)text);

			SendMessage(hWndStatusBar, SB_SETTEXT, 5,(LPARAM)NULL);

            ShowWindow(hWndStatusBar, SW_SHOW);
}

void CServerDisplayer::PaintAllInfo() // OK
{
	RECT rect;

	GetClientRect(this->m_hwnd, &rect);

	rect.top = 0;
	rect.bottom = 80;

	HDC hdc = GetDC(this->m_hwnd);
	HDC hdcw = GetDC(this->m_hwnd);
	HDC hdcq = GetDC(this->m_hwnd);

	int OldBkMode = SetBkMode(hdc, TRANSPARENT);
	int OldBkMode1 = SetBkMode(hdcw, TRANSPARENT);
	int OldBkMode2 = SetBkMode(hdcq, TRANSPARENT);
	HFONT OldFont = (HFONT)SelectObject(hdc, this->m_fonttitle);
	HFONT OldFont1 = (HFONT)SelectObject(hdcw, this->m_fonttitle);
	HFONT OldFont2 = (HFONT)SelectObject(hdcq, this->m_fonttitle);

	SetTextColor(hdc, RGB(120, 55, 0));
	FillRect(hdc, &rect, this->m_brush);
	strcpy_s(this->m_DisplayerText, "GameServer");


	SetTextColor(hdcw, RGB(0, 0, 120));
	FillRect(hdcw, &rect, this->m_brush);
	strcpy_s(this->m_DisplayerText1, "| DAT Ver:");

	SetTextColor(hdcq, RGB(0, 0, 0));
	FillRect(hdcq, &rect, this->m_brush);
	strcpy_s(this->m_DisplayerText2, "1.0.0.0");

	TextOut(hdc, 130, 15, this->m_DisplayerText, strlen(this->m_DisplayerText));
	TextOut(hdcw, 390, 15, this->m_DisplayerText1, strlen(this->m_DisplayerText1));
	TextOut(hdcq, 600, 15, this->m_DisplayerText2, strlen(this->m_DisplayerText2));

	SelectObject(hdc, OldFont1);
	SetBkMode(hdc, OldBkMode1);
	ReleaseDC(this->m_hwnd, hdc);
	SelectObject(hdcw, OldFont1);
	SetBkMode(hdcw, OldBkMode1);
	ReleaseDC(this->m_hwnd, hdcw);
	SelectObject(hdcq, OldFont2);
	SetBkMode(hdcq, OldBkMode2);
	ReleaseDC(this->m_hwnd, hdcq);
}

void CServerDisplayer::PaintOnline() // OK
{}

void CServerDisplayer::PaintSeason() // OK
{}

void CServerDisplayer::PaintPremium() // OK
{}

void CServerDisplayer::PaintEventTime() // OK
{}

void CServerDisplayer::PaintInvasionTime() // OK
{}

void CServerDisplayer::PaintCustomArenaTime() // OK
{}

void CServerDisplayer::LogTextPaint() // OK
{
	RECT rect;

	GetClientRect(this->m_hwnd, &rect);

	rect.top = 100;
	rect.bottom = 530;

	HDC hdc = GetDC(this->m_hwnd);

	FillRect(hdc, &rect, (HBRUSH)GetStockObject(4));

	int line = MAX_LOG_TEXT_LINE;

	int count = (((this->m_count - 1) >= 0) ? (this->m_count - 1) : (MAX_LOG_TEXT_LINE - 1));

	for (int n = 0; n < MAX_LOG_TEXT_LINE; n++)
	{
		switch (this->m_log[count].color)
		{
		case LOG_BLACK:
			SetTextColor(hdc, RGB(192, 192, 192));
			SetBkMode(hdc, TRANSPARENT);
			break;
		case LOG_RED:
			SetTextColor(hdc, RGB(255, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
			break;
		case LOG_GREEN:
			SetTextColor(hdc, RGB(110, 255, 0));
			SetBkMode(hdc, TRANSPARENT);
			break;
		case LOG_BLUE:
			SetTextColor(hdc, RGB(0, 110, 255));
			SetBkMode(hdc, TRANSPARENT);
			break;
		case LOG_ORANGE:
			SetTextColor(hdc, RGB(255, 110, 0));
			SetBkMode(hdc, TRANSPARENT);
			break;
		case LOG_PURPLE:
			SetTextColor(hdc, RGB(160, 70, 160));
			SetBkMode(hdc, TRANSPARENT);
			break;
		case LOG_PINK:
			SetTextColor(hdc, RGB(255, 0, 128));
			SetBkMode(hdc, TRANSPARENT);
			break;
		case LOG_YELLOW:
			SetTextColor(hdc, RGB(255, 240, 0));
			SetBkMode(hdc, TRANSPARENT);
			break;
		}

		int size = strlen(this->m_log[count].text);

		if (size > 1)
		{
			TextOut(hdc, 0, (65 + (line * 15)), this->m_log[count].text, size);
			line--;
		}

		count = (((--count) >= 0) ? count : (MAX_LOG_TEXT_LINE - 1));
	}

	ReleaseDC(this->m_hwnd, hdc);
}

void CServerDisplayer::LogTextPaintConnect() // OK
{}

void CServerDisplayer::LogTextPaintGlobalMessage() // OK
{
	RECT rect;

	GetClientRect(this->m_hwnd,&rect);

	rect.left = 0;
	rect.top = 100;
	rect.bottom = 250;

	HDC hdc = GetDC(this->m_hwnd);

	FillRect(hdc,&rect,(HBRUSH)GetStockObject(0));

	int OldBkMode = SetBkMode(hdc,TRANSPARENT);

	HFONT OldFont = (HFONT)SelectObject(hdc,this->m_font3);

	FillRect(hdc,&rect,this->m_brush2);

	SetTextColor(hdc,RGB(150,150,150));
	TextOut(hdc,rect.left+5,rect.top+2,"Mensagem Global",15);

	int line = MAX_LOGGLOBAL_TEXT_LINE;

	int count = (((this->m_countGlobal-1)>=0)?(this->m_countGlobal-1):(MAX_LOGGLOBAL_TEXT_LINE-1));

	for(int n=0;n < MAX_LOGGLOBAL_TEXT_LINE;n++)
	{

		SetTextColor(hdc,RGB(153, 153, 0));

		int size = strlen(this->m_logGlobal[count].text);

		if(size > 1)
		{
			TextOut(hdc,rect.left+10,(rect.top+5+(line*15)),this->m_logGlobal[count].text,size);
			line--;
		}

		count = (((--count)>=0)?count:(MAX_LOGGLOBAL_TEXT_LINE-1));
	}

	SelectObject(hdc,OldFont);
	SetBkMode(hdc,OldBkMode);
	ReleaseDC(this->m_hwnd,hdc);
}

void CServerDisplayer::LogAddText(eLogColor color,char* text,int size) // OK
{
	size = ((size>=MAX_LOG_TEXT_SIZE)?(MAX_LOG_TEXT_SIZE-1):size);

	memset(&this->m_log[this->m_count].text,0,sizeof(this->m_log[this->m_count].text));

	memcpy(&this->m_log[this->m_count].text,text,size);

	this->m_log[this->m_count].color = color;

	this->m_count = (((++this->m_count)>=MAX_LOG_TEXT_LINE)?0:this->m_count);

	gLog.Output(LOG_GENERAL,"%s",&text[9]);
}

void CServerDisplayer::LogAddTextConnect(eLogColor color,char* text,int size) // OK
{
	size = ((size>=MAX_LOGCONNECT_TEXT_SIZE)?(MAX_LOGCONNECT_TEXT_SIZE-1):size);

	memset(&this->m_logConnect[this->m_countConnect].text,0,sizeof(this->m_logConnect[this->m_countConnect].text));

	memcpy(&this->m_logConnect[this->m_countConnect].text,text,size);

	this->m_logConnect[this->m_countConnect].color = color;

	this->m_countConnect = (((++this->m_countConnect)>=MAX_LOGCONNECT_TEXT_LINE)?0:this->m_countConnect);

	gLog.Output(LOG_GENERAL,"%s",&text[9]);
}

void CServerDisplayer::LogAddTextGlobal(eLogColor color,char* text,int size) // OK
{
	size = ((size>=MAX_LOGGLOBAL_TEXT_SIZE)?(MAX_LOGGLOBAL_TEXT_SIZE-1):size);

	memset(&this->m_logGlobal[this->m_countGlobal].text,0,sizeof(this->m_logGlobal[this->m_countGlobal].text));

	memcpy(&this->m_logGlobal[this->m_countGlobal].text,text,size);

	this->m_logGlobal[this->m_countGlobal].color = color;

	this->m_countGlobal = (((++this->m_countGlobal)>=MAX_LOGGLOBAL_TEXT_LINE)?0:this->m_countGlobal);
}