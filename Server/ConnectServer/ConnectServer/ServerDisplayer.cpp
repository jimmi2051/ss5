// Rev - 2023

#include "stdafx.h"
#include "ServerDisplayer.h"
#include "Log.h"
#include "resource.h"
#include "ServerList.h"
#include "SocketManager.h"
#include "Util.h"
#include "Protect.h"

CServerDisplayer gServerDisplayer;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServerDisplayer::CServerDisplayer() // OK
{
	memset(&this->m_log, 0, sizeof(this->m_log));
	this->m_fonttitle = CreateFont(50, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"));

	this->m_brush = CreateSolidBrush(RGB(116, 170, 156));
}

CServerDisplayer::~CServerDisplayer() // OK
{
	DeleteObject(static_cast<HGDIOBJ>(this->m_fonttitle));
	DeleteObject(static_cast<HGDIOBJ>(this->m_brush));
}

void CServerDisplayer::Init(HWND hWnd) // OK
{
PROTECT_START
this->m_hwnd = hWnd;
PROTECT_FINAL
	gLog.AddLog(1, "Log");
}

void CServerDisplayer::Run() // OK
{
	this->LogTextPaint();
	this->PaintAllInfo();
	this->PaintStatusBar();
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

	SetTextColor(hdc, RGB(255, 255, 255));
	FillRect(hdc, &rect, this->m_brush);
	strcpy_s(this->m_DisplayerText, "ConnectServer");


	SetTextColor(hdcw, RGB(255, 255, 255));
	FillRect(hdcw, &rect, this->m_brush);
	strcpy_s(this->m_DisplayerText1, "| XML Ver:");

	SetTextColor(hdcq, RGB(255, 255, 255));
	FillRect(hdcq, &rect, this->m_brush);
	strcpy_s(this->m_DisplayerText2, "1.0.0.1");

	TextOut(hdc, 55, 15, this->m_DisplayerText, strlen(this->m_DisplayerText));
	TextOut(hdcw, 360, 15, this->m_DisplayerText1, strlen(this->m_DisplayerText1));
	TextOut(hdcq, 570, 15, this->m_DisplayerText2, strlen(this->m_DisplayerText2));

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

void CServerDisplayer::LogTextPaint()
{
	RECT rect;
	GetClientRect(m_hwnd, &rect);
	rect.top = -10;
	rect.bottom = 500;
	HDC hdc = GetDC(m_hwnd);
	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
	FillRect(hdc, &rect, brush);
	DeleteObject(brush);

	int line = MAX_LOG_TEXT_LINE;
	int count = m_count - 1;

	if (count < 0)
		count = MAX_LOG_TEXT_LINE - 1;

	for (int n = 0; n < MAX_LOG_TEXT_LINE; ++n)
	{
		switch (m_log[count].color)
		{
		case LOG_BLACK:
			SetTextColor(hdc, RGB(192, 192, 192));
			break;
		case LOG_RED:
			SetTextColor(hdc, RGB(255, 0, 0));
			break;
		case LOG_GREEN:
			SetTextColor(hdc, RGB(110, 255, 0));
			break;
		case LOG_BLUE:
			SetTextColor(hdc, RGB(0, 110, 255));
			break;
		case LOG_ORANGE:
			SetTextColor(hdc, RGB(255, 110, 0));
			break;
		case LOG_PURPLE:
			SetTextColor(hdc, RGB(160, 70, 160));
			break;
		case LOG_PINK:
			SetTextColor(hdc, RGB(255, 0, 128));
			break;
		case LOG_YELLOW:
			SetTextColor(hdc, RGB(255, 240, 0));
			break;
		default:
			break;
		}
		SetBkMode(hdc, TRANSPARENT);

		const int size = lstrlenA(m_log[count].text);
		if (size > 0)
		{
			TextOutA(hdc, 0, (65 + ((line - 1) * 15)), m_log[count].text, size);
			--line;
		}

		--count;
		if (count < 0)
			count = MAX_LOG_TEXT_LINE - 1;
	}

	ReleaseDC(m_hwnd, hdc);
}

void CServerDisplayer::PaintStatusBar()
{
	char buff[256];
	sprintf_s(buff, "[%s] ConnectServer | XML V1.0", VERSION);

	SetWindowText(m_hwnd, buff);

	HWND hWndStatusBar = GetDlgItem(m_hwnd, IDC_STATUSBAR);
	DWORD dwStyle = GetWindowLong(hWndStatusBar, GWL_STYLE);
	dwStyle |= SBARS_SIZEGRIP | CCS_BOTTOM | WS_BORDER | WS_CLIPSIBLINGS;
	SetWindowLong(hWndStatusBar, GWL_STYLE, dwStyle);

	char szTempText[256];

	sprintf_s(szTempText, "QueueSize: %d", gSocketManager.GetQueueSize());
	SendMessage(hWndStatusBar, SB_SETTEXT, 0, (LPARAM)szTempText);

	sprintf_s(szTempText, "Connected: %d", GetUserCount());
	SendMessage(hWndStatusBar, SB_SETTEXT, 1, (LPARAM)szTempText);

	sprintf_s(szTempText, "GameServers: %d/%d", gServerList.m_GameServersList, gServerList.m_GameServersCount);
	SendMessage(hWndStatusBar, SB_SETTEXT, 2, (LPARAM)szTempText);

	sprintf_s(szTempText, "JoinServer: %s", (gServerList.CheckJoinServerState() == 1) ? "ON" : "OFF");
	SendMessage(hWndStatusBar, SB_SETTEXT, 3, (LPARAM)szTempText);

	sprintf_s(szTempText, "Versão: %s", VERSION);
	SendMessage(hWndStatusBar, SB_SETTEXT, 4, (LPARAM)szTempText);

	sprintf_s(szTempText, "Modo: %s", (gServerList.CheckJoinServerState() == 0) ? "Standby" : "Active");
	SendMessage(hWndStatusBar, SB_SETTEXT, 5, (LPARAM)szTempText);

	sprintf_s(szTempText, "Licença: Premium");
	SendMessage(hWndStatusBar, SB_SETTEXT, 6, (LPARAM)szTempText);

	SendMessage(hWndStatusBar, SB_SETTEXT, 7, 0);

	ShowWindow(hWndStatusBar, SW_SHOW);
}

void CServerDisplayer::LogAddText(eLogColor color, const char* text, int size)
{
	PROTECT_START
		size = min(size, MAX_LOG_TEXT_SIZE - 1);

	memset(this->m_log[this->m_count].text, 0, sizeof(this->m_log[this->m_count].text));

	memcpy(this->m_log[this->m_count].text, text, size);

	this->m_log[this->m_count].color = color;

	this->m_count = (++this->m_count >= MAX_LOG_TEXT_LINE) ? 0 : this->m_count;

	PROTECT_FINAL

		gLog.Output(LOG_GENERAL, "%s", &text[9]);
}
