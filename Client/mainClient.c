/*Template de c em que suporta multi-byte e unicode*/
/*			ATUALMENTE NESTE TEMPLATE EST� ATIVO A
		OP�OA UNICODE LOGO ESTA A CORRER COM UTF-16*/
#include <windows.h>
#include <winuser.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>

#include "ClientStructures.h"
#include "ClientThreads.h"
#include "resource1.h"
#include "Canvas.h"
#include "..\Communicate\MessageProtocol.h"
#include "..\Communicate\DllSetup.h"

LPVOID lpgcSharedMemGame = NULL;
LPVOID lpgcSharedMemMessage = NULL;

HWND gWnd = NULL;
Game* gameObj = NULL;

RECT rectWindowProp;
HDC memDC = NULL;  //double buffering matrix
HBITMAP hBit = NULL;
HBRUSH hBrush = NULL;
HDC tempDC = NULL;
HBITMAP hBmp = NULL;
BITMAP bmp;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK manageDialogEvents(HWND, UINT, WPARAM, LPARAM);

extern BOOL bKeepRunning;
ClientStructure gClientInfo;

// ============================================================================
// FUN��O DE IN�CIO DO PROGRAMA: WinMain()
// ============================================================================
// Em Windows, o programa come�a sempre a sua execu��o na fun��o WinMain()que desempenha // o papel da fun��o main() do C em modo consola WINAPI indica o "tipo da fun��o" (WINAPI // para todas as declaradas nos headers do Windows e CALLBACK para as fun��es de // processamento da janela)
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInst);
	UNREFERENCED_PARAMETER(lpCmdLine);

	HANDLE hThreads[NUMBER_OF_CLIENT_THREADS];
	DWORD dwThreadsIds[NUMBER_OF_CLIENT_THREADS];

	ZeroMemory(&gClientInfo, sizeof(ClientStructure));

	HWND hWnd; // hWnd � o handler da janela, gerado mais abaixo por CreateWindow()
	MSG lpMsg; // MSG � uma estrutura definida no Windows para as mensagens
	WNDCLASSEX wcApp; // WNDCLASSEX � uma estrutura cujos membros servem para
	// definir as caracter�sticas da classe da janela
	// ============================================================================
	// 1. Defini��o das caracter�sticas da janela "wcApp"
	// (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)
	// ============================================================================
	wcApp.cbSize = sizeof(WNDCLASSEX); // Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst; // Inst�ncia da janela actualmente exibida
	// ("hInst" � par�metro de WinMain e vem
	// inicializada da�)
	wcApp.lpszClassName = NAME_OF_WINDOWS_PROGRAM; // Nome da janela (neste caso = nome do programa)
	wcApp.lpfnWndProc = WndProc; // Endere�o da fun��o de processamento da janela // ("TrataEventos" foi declarada no in�cio e // encontra-se mais abaixo)
	wcApp.style = CS_HREDRAW | CS_VREDRAW;// Estilo da janela: Fazer o redraw se for // modificada horizontal ou verticalmente

	// "hIcon" = handler do �con normal
	wcApp.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	// "hIconSm" = handler do �con pequeno
	wcApp.hIconSm = LoadIcon(NULL, IDI_INFORMATION);

	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW); // "hCursor" = handler do cursor (rato)
	// "NULL" = Forma definida no Windows
	// "IDC_ARROW" Aspecto "seta"
	wcApp.lpszMenuName = NULL; // Classe do menu que a janela pode ter
	// (NULL = n�o tem menu)
	wcApp.cbClsExtra = 0; // Livre, para uso particular
	wcApp.cbWndExtra = 0; // Livre, para uso particular
	wcApp.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	// "hbrBackground" = handler para "brush" de pintura do fundo da janela. Devolvido por // "GetStockObject".Neste caso o fundo ser� branco
	// ============================================================================
	// 2. Registar a classe "wcApp" no Windows
	// ============================================================================
	if (!RegisterClassEx(&wcApp))
		return(0);
	// ============================================================================
	// 3. Criar a janela
	// ============================================================================
	hWnd = CreateWindow(
		NAME_OF_WINDOWS_PROGRAM, // Nome da janela (programa) definido acima
		TEXT("Arkanoid - Client"),// Texto que figura na barra do t�tulo
		WS_OVERLAPPEDWINDOW, // Estilo da janela (WS_OVERLAPPED= normal)
		CW_USEDEFAULT, // Posi��o x pixels (default=� direita da �ltima)
		CW_USEDEFAULT, // Posi��o y pixels (default=abaixo da �ltima)
		CW_USEDEFAULT, // Largura da janela (em pixels)
		CW_USEDEFAULT, // Altura da janela (em pixels)
		(HWND)HWND_DESKTOP, // handle da janela pai (se se criar uma a partir de
		// outra) ou HWND_DESKTOP se a janela for a primeira,
		// criada a partir do "desktop"
		(HMENU)NULL, // handle do menu da janela (se tiver menu)
		(HINSTANCE)hInst, // handle da inst�ncia do programa actual ("hInst" �
		// passado num dos par�metros de WinMain()
		0); // N�o h� par�metros adicionais para a janela
		// ============================================================================
		// 4. Mostrar a janela
		// ============================================================================
	ShowWindow(hWnd, nCmdShow); // "hWnd"= handler da janela, devolvido por
	// "CreateWindow"; "nCmdShow"= modo de exibi��o (p.e.
	// normal/modal); � passado como par�metro de WinMain()


	UpdateWindow(hWnd); // Refrescar a janela (Windows envia � janela uma
	// mensagem para pintar, mostrar dados, (refrescar)�

	gClientInfo.hWndWindow = hWnd;
	DialogBox(NULL,
		MAKEINTRESOURCE(IDD_DIALOGLOGIN),
		hWnd,
		manageDialogEvents);

	hThreads[0] = CreateThread(
		NULL,
		0,
		readInputThread,
		NULL,
		0,
		&dwThreadsIds[0]
	);

	hThreads[1] = CreateThread(
		NULL,
		0,
		readMessageThread,
		(LPVOID)&gClientInfo,
		0,
		&dwThreadsIds[1]
	);

	hThreads[2] = CreateThread(
		NULL,
		0,
		readGameDataThread,
		NULL,
		0,
		&dwThreadsIds[2]
	);
	PlaySound(TEXT("background_music.wav"), NULL, SND_ASYNC | SND_NODEFAULT);

	// ============================================================================
	// 5. Loop de Mensagens
	// ============================================================================
	while (GetMessage(&lpMsg, NULL, 0, 0))
	{
		TranslateMessage(&lpMsg); // Pr�-processamento da mensagem (p.e. obter c�digo
		// ASCII da tecla premida)
		DispatchMessage(&lpMsg); // Enviar a mensagem traduzida de volta ao Windows, que
		// aguarda at� que a possa reenviar � fun��o de
		// tratamento da janela, CALLBACK TrataEventos (abaixo)
	}

	WaitForMultipleObjects(NUMBER_OF_CLIENT_THREADS, hThreads, TRUE, INFINITE);

	freeThreads(hThreads);
	// ============================================================================
	// 6. Fim do programa
	// ============================================================================
	return((int)lpMsg.wParam); // Retorna sempre o par�metro wParam da estrutura lpMsg
}
// ============================================================================
// FUN��O DE PROCESSAMENTO DA JANELA
// Esta fun��o pode ter um nome qualquer: Apenas � neces�rio que na inicializa��o da // estrutura "wcApp", feita no in�cio de WinMain(), se identifique essa fun��o. Neste // caso "wcApp.lpfnWndProc = WndProc"
//
// A fun��o EndProc � sempre do tipo "switch..." com "cases" que descriminam a mensagem // recebida e a tratar. Estas mensagens s�o identificadas por constantes (p.e.
// WM_DESTROY, WM_CHAR, WM_KEYDOWN, WM_PAINT...) definidas em windows.h ============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;
	//HBRUSH hBrush = NULL;

	gWnd = hWnd;
	switch (messg)
	{
	case WM_CREATE:
		GetClientRect(hWnd, &rectWindowProp);

		hDC = GetDC(hWnd);

		memDC = CreateCompatibleDC(hDC);

		hBit = CreateCompatibleBitmap(hDC, rectWindowProp.right, rectWindowProp.bottom);

		SelectObject(memDC, hBit);

		DeleteObject(hBit);

		gClientInfo.doubleBufferingDC = memDC;

		hBrush = CreateSolidBrush(RGB(170, 15, 70));

		SelectObject(memDC, hBrush);

		PatBlt(memDC, 0, 0, rectWindowProp.right, rectWindowProp.bottom, PATCOPY);

		ReleaseDC(hWnd, hDC);
		break;
	case  WM_PAINT:

		PatBlt(memDC, 0, 0, rectWindowProp.right, rectWindowProp.bottom, PATCOPY);

		drawGame(gameObj, memDC);

		hDC = BeginPaint(hWnd, &ps);

		BitBlt(hDC, 0, 0, rectWindowProp.right, rectWindowProp.bottom, memDC, 0, 0, SRCCOPY);

		EndPaint(hWnd, &ps);
		break;
	case WM_ERASEBKGND:
		return(1); // Prevent erasing the background to reduce flickering
		break;
	case WM_KEYDOWN:

		switch (wParam)
		{
		case VK_LEFT:
			//xBitMap = xBitMap > 0 ? xBitMap - 10 : 0;
			SendMessageDll(&bKeepRunning, KeyPressedLeftMessage);
			break;
		case VK_RIGHT:
			SendMessageDll(&bKeepRunning, KeyPressedRigthMessage);
			break;
		case VK_DOWN:
			//yBitMap = yBitMap < rect.top - bmp.bmWidth ? yBitMap + 10 : rect.bottom - bmp.bmHeight;
			break;
		}
		break;

	case WM_CLOSE:
		switch (MessageBox(hWnd,
			TEXT("Tens certeza que quer mesmo fechar?"),
			TEXT("Sair - Janela"),
			MB_YESNO | MB_ICONINFORMATION))
		{
		case IDYES:
			SendMessageDll(&bKeepRunning, QuitGameMessage);
			DestroyWindow(hWnd);
			break;
		case IDNO:
			//n�o faz nada
			return (1);
			break;
		}
		break;
	case WM_DESTROY: // Destruir a janela e terminar o programa
	// "PostQuitMessage(Exit Status)"
		free(gameObj);
		PostQuitMessage(0);
		break;
	default:
		// Neste exemplo, para qualquer outra mensagem (p.e. "minimizar","maximizar","restaurar") // n�o � efectuado nenhum processamento, apenas se segue o "default" do Windows
		return DefWindowProc(hWnd, messg, wParam, lParam);
		break;
	}
	return(0);
}

BOOL CALLBACK manageDialogEvents(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (messg)
	{
	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_SENDLOGIN)
		{
			if (GetDlgItemText(hWnd, IDC_USERNAME, gClientInfo.tcUserName, _countof(gClientInfo.tcUserName)) &&
				(IsDlgButtonChecked(hWnd, IDC_RADIOSHAREDMEMORY) == BST_CHECKED || IsDlgButtonChecked(hWnd, IDC_RADIONAMEDPIPE) == BST_CHECKED))
			{
				if (IsDlgButtonChecked(hWnd, IDC_RADIOSHAREDMEMORY) == BST_CHECKED)
				{
					Login(gClientInfo.tcUserName, gClientInfo.hWndWindow, gClientInfo.doubleBufferingDC, clientSharedMemoryConnection);
				}
				else if (IsDlgButtonChecked(hWnd, IDC_RADIONAMEDPIPE) == BST_CHECKED)
				{
					gameObj = malloc(sizeof(Game));
					Login(gClientInfo.tcUserName, gClientInfo.hWndWindow, gClientInfo.doubleBufferingDC, clientNamedPipeLocalConnection);
				}
				EndDialog(hWnd, 0);
			}
		}
		break;
	}
	return(0);
}