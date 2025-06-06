// Rev - 2023

#include "stdafx.h"
#include "resource.h"
#include "ConnectServer.h"
#include "MiniDump.h"
#include "Protect.h"
#include "ServerDisplayer.h"
#include "ServerList.h"
#include "SocketManager.h"
#include "SocketManagerUdp.h"
#include "ThemidaSDK.h"
#include "Util.h"

HINSTANCE hInst;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];
HWND hWnd;
char CustomerName[32];
char CustomerHardwareId[36];
long MaxIpConnection;
char Version[9];

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	VM_START

		CMiniDump::Start();

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CONNECTSERVER, szWindowClass, MAX_LOADSTRING);

	MyRegisterClass(hInstance);

	if (InitInstance(hInstance, nCmdShow) == 0)
	{
		return 0;
	}

	char buff[256];

	sprintf_s(buff, sizeof(buff), "Loading files...");

	SetWindowText(hWnd, buff);

	gServerDisplayer.Init(hWnd);

	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) == 0)
	{
		WORD ConnectServerPortTCP = GetPrivateProfileInt("ConnectServerInfo", "ConnectServerPortTCP", 44405, ".\\ConnectServer.ini");

		WORD ConnectServerPortUDP = GetPrivateProfileInt("ConnectServerInfo", "ConnectServerPortUDP", 55557, ".\\ConnectServer.ini");

		GetPrivateProfileString("ConnectServerInfo", "Version", "", Version, sizeof(Version), ".\\ConnectServer.ini");

		MaxIpConnection = GetPrivateProfileInt("ConnectServerInfo", "MaxIpConnection", 0, ".\\ConnectServer.ini");

		if (gSocketManager.Start(ConnectServerPortTCP) != 0 && gSocketManagerUdp.Start(ConnectServerPortUDP) != 0)
		{
			gServerList.Load("ServerList.xml");

			SetTimer(hWnd, TIMER_1000, 1000, 0);

			SetTimer(hWnd, TIMER_5000, 5000, 0);

			gServerDisplayer.PaintAllInfo();

			SetTimer(hWnd, TIMER_2000, 2000, 0);

			HACCEL hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_CONNECTSERVER);

			MSG msg;

			while (GetMessage(&msg, 0, 0, 0) != 0)
			{
				if (TranslateAccelerator(msg.hwnd, hAccelTable, &msg) == 0)
				{
					TranslateMessage(&msg);
					DispatchMessageA(&msg);
				}
			}

			CMiniDump::Clean();

			VM_END

				return msg.wParam;
		}
	}
	else
	{
		LogAdd(LOG_RED, "WSAStartup() failed with error: %d", WSAGetLastError());
	}

	CMiniDump::Clean();

	VM_END

		return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = { 0 }; // Inicializa a struct com 0 em todos os membros

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CONNECTSERVER));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 4);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_CONNECTSERVER);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, 0, 800, 600, 0, 0, hInstance, 0);

	if (hWnd == NULL)
	{
		return FALSE;
	}

	HWND hWndStatusBar;
	hWndStatusBar = CreateWindowEx(0, STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd, (HMENU)IDC_STATUSBAR, hInstance, NULL);
	ShowWindow(hWndStatusBar, SW_HIDE);

	int iQueueBarWidths[] = { 80,180,300,400,500,600, -1 };

	SendMessage(hWndStatusBar, SB_SETPARTS, 7, (LPARAM)iQueueBarWidths);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			if (MessageBox(hWnd, "Are you sure to terminate ConnectServer?", "Ask terminate server", MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				DestroyWindow(hWnd);
			}
			break;
		case IDM_RELOAD_RELOADSERVERLIST:
			gServerList.Load("ServerList.xml");
			LogAdd(LOG_BLUE, "[ServerList] ServerList loaded successfully");
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case TIMER_1000:
			gServerList.MainProc();
			break;
		case TIMER_2000:
			gServerDisplayer.Run();
			break;
		case TIMER_5000:
			ConnectServerTimeoutProc();
			break;
		default:
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDC_VERSION, VERSION);
		SetDlgItemText(hDlg, IDC_EXPIREDATE, "09/09/2099");
		return TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
