/*Template de c em que suporta multi-byte e unicode*/
/*			ATUALMENTE NESTE TEMPLATE ESTÁ ATIVO A
		OPÇOA UNICODE LOGO ESTA A CORRER COM UTF-16*/
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#include "ClientStructures.h"
#include "ClientThreads.h"
#include "resource1.h"
#include "..\Communicate\MessageProtocol.h"
#include "..\Communicate\DllSetup.h"
#include <winuser.h>
#include <winuser.h>
#include <stdlib.h>

LPVOID lpgcSharedMemGame = NULL;
LPVOID lpgcSharedMemMessage = NULL;

LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK manageDialogEvents(HWND, UINT, WPARAM, LPARAM);

ClientStructure gClientInfo;

// ============================================================================
// FUNÇÃO DE INÍCIO DO PROGRAMA: WinMain()
// ============================================================================
// Em Windows, o programa começa sempre a sua execução na função WinMain()que desempenha // o papel da função main() do C em modo consola WINAPI indica o "tipo da função" (WINAPI // para todas as declaradas nos headers do Windows e CALLBACK para as funções de // processamento da janela)
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInst);
	UNREFERENCED_PARAMETER(lpCmdLine);

	HANDLE hThreads[NUMBER_OF_CLIENT_THREADS];
	DWORD dwThreadsIds[NUMBER_OF_CLIENT_THREADS];

	ZeroMemory(&ClientInfo, sizeof(ClientStructure));

	HWND hWnd; // hWnd é o handler da janela, gerado mais abaixo por CreateWindow()
	MSG lpMsg; // MSG é uma estrutura definida no Windows para as mensagens
	WNDCLASSEX wcApp; // WNDCLASSEX é uma estrutura cujos membros servem para
	// definir as características da classe da janela
	// ============================================================================
	// 1. Definição das características da janela "wcApp"
	// (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)
	// ============================================================================
	wcApp.cbSize = sizeof(WNDCLASSEX); // Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst; // Instância da janela actualmente exibida
	// ("hInst" é parâmetro de WinMain e vem
	// inicializada daí)
	wcApp.lpszClassName = NAME_OF_WINDOWS_PROGRAM; // Nome da janela (neste caso = nome do programa)
	wcApp.lpfnWndProc = TrataEventos; // Endereço da função de processamento da janela // ("TrataEventos" foi declarada no início e // encontra-se mais abaixo)
	wcApp.style = CS_HREDRAW | CS_VREDRAW;// Estilo da janela: Fazer o redraw se for // modificada horizontal ou verticalmente

	// "hIcon" = handler do ícon normal
	wcApp.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	// "hIconSm" = handler do ícon pequeno
	wcApp.hIconSm = LoadIcon(NULL, IDI_INFORMATION);

	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW); // "hCursor" = handler do cursor (rato)
	// "NULL" = Forma definida no Windows
	// "IDC_ARROW" Aspecto "seta"
	wcApp.lpszMenuName = NULL; // Classe do menu que a janela pode ter
	// (NULL = não tem menu)
	wcApp.cbClsExtra = 0; // Livre, para uso particular
	wcApp.cbWndExtra = 0; // Livre, para uso particular
	wcApp.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	// "hbrBackground" = handler para "brush" de pintura do fundo da janela. Devolvido por // "GetStockObject".Neste caso o fundo será branco
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
		TEXT("Arkanoid - Client"),// Texto que figura na barra do título
		WS_OVERLAPPEDWINDOW, // Estilo da janela (WS_OVERLAPPED= normal)
		CW_USEDEFAULT, // Posição x pixels (default=à direita da última)
		CW_USEDEFAULT, // Posição y pixels (default=abaixo da última)
		CW_USEDEFAULT, // Largura da janela (em pixels)
		CW_USEDEFAULT, // Altura da janela (em pixels)
		(HWND)HWND_DESKTOP, // handle da janela pai (se se criar uma a partir de
		// outra) ou HWND_DESKTOP se a janela for a primeira,
		// criada a partir do "desktop"
		(HMENU)NULL, // handle do menu da janela (se tiver menu)
		(HINSTANCE)hInst, // handle da instância do programa actual ("hInst" é
		// passado num dos parâmetros de WinMain()
		0); // Não há parâmetros adicionais para a janela
		// ============================================================================
		// 4. Mostrar a janela
		// ============================================================================
	ShowWindow(hWnd, nCmdShow); // "hWnd"= handler da janela, devolvido por
	// "CreateWindow"; "nCmdShow"= modo de exibição (p.e.
	// normal/modal); é passado como parâmetro de WinMain()


	UpdateWindow(hWnd); // Refrescar a janela (Windows envia à janela uma
	// mensagem para pintar, mostrar dados, (refrescar)…
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
		(LPVOID)&ClientInfo,
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

	WaitForMultipleObjects(NUMBER_OF_CLIENT_THREADS, hThreads, TRUE, INFINITE);

	freeThreads(hThreads);

	// ============================================================================
	// 5. Loop de Mensagens
	// ============================================================================
	while (GetMessage(&lpMsg, NULL, 0, 0))
	{
		TranslateMessage(&lpMsg); // Pré-processamento da mensagem (p.e. obter código
		// ASCII da tecla premida)
		DispatchMessage(&lpMsg); // Enviar a mensagem traduzida de volta ao Windows, que
		// aguarda até que a possa reenviar à função de
		// tratamento da janela, CALLBACK TrataEventos (abaixo)
	}
	// ============================================================================
	// 6. Fim do programa
	// ============================================================================
	return((int)lpMsg.wParam); // Retorna sempre o parâmetro wParam da estrutura lpMsg
}
// ============================================================================
// FUNÇÃO DE PROCESSAMENTO DA JANELA
// Esta função pode ter um nome qualquer: Apenas é necesário que na inicialização da // estrutura "wcApp", feita no início de WinMain(), se identifique essa função. Neste // caso "wcApp.lpfnWndProc = WndProc"
//
// A função EndProc é sempre do tipo "switch..." com "cases" que descriminam a mensagem // recebida e a tratar. Estas mensagens são identificadas por constantes (p.e.
// WM_DESTROY, WM_CHAR, WM_KEYDOWN, WM_PAINT...) definidas em windows.h ============================================================================
LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	switch (messg)
	{
	case WM_DESTROY: // Destruir a janela e terminar o programa
	// "PostQuitMessage(Exit Status)"
		PostQuitMessage(0);
		break;
	default:
		// Neste exemplo, para qualquer outra mensagem (p.e. "minimizar","maximizar","restaurar") // não é efectuado nenhum processamento, apenas se segue o "default" do Windows
		return DefWindowProc(hWnd, messg, wParam, lParam);
		break;
	}
	return(0);
}

BOOL CALLBACK manageDialogEvents(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
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
					Login(gClientInfo.tcUserName, clientSharedMemoryConnection);
				}
				else if (IsDlgButtonChecked(hWnd, IDC_RADIONAMEDPIPE) == BST_CHECKED)
				{
					Login(gClientInfo.tcUserName, clientNamedPipeLocalConnection);
				}
				EndDialog(hWnd, 0);
			}
		}
		break;
	}
	return(0);
}