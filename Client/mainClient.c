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
#include "..\Communicate\MessageProtocol.h"
#include "..\Communicate\DllSetup.h"

LPVOID lpgcSharedMemGame = NULL;
LPVOID lpgcSharedMemMessage = NULL;

LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);

//Modo de Inicio
int getLoginMethod()
{
	int Input;

	_tprintf(TEXT("\nInsira Opção de comunicação:"));
	_tprintf(TEXT("\n\t 1 - Memoria Partilhada"));
	_tprintf(TEXT("\n\t 2 - Pipe"));
	_tprintf(TEXT("\n\t 3 - Remoto"));
	_tprintf(TEXT("\n\t 0 - Sair"));
	_tprintf(TEXT("\n\t > "));
	_tscanf_s(TEXT(" %d"), &Input);

	return Input;
}

void getUserName(ClientStructure* dest)
{
	_tprintf(TEXT("\n Insira username(ate 20) :"));
	_tscanf_s(TEXT("%19s"), dest->tcUserName, (unsigned)_countof(dest->tcUserName));
}
//FIM - Modo de Inicio

int _tmain(int argc, LPTSTR argv[])
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);
	ClientStructure ClientInfo;

	HANDLE hThreads[NUMBER_OF_CLIENT_THREADS];
	DWORD dwThreadsIds[NUMBER_OF_CLIENT_THREADS];

	ZeroMemory(&ClientInfo, sizeof(ClientStructure));

	//UNICODE: Por defeito, a consola Windows não processa caracteres wide.
	//A maneira mais fácil para ter esta funcionalidade é chamar _setmode:
	#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	#endif

	getUserName(&ClientInfo);
	switch (getLoginMethod())
	{
	case 1:
		Login(ClientInfo.tcUserName, clientSharedMemoryConnection);
		break;
	case 2:
		Login(ClientInfo.tcUserName, clientNamedPipeLocalConnection);
		break;
	case 3:
		break;
	case 0:
	default:
		exit(EXIT_SUCCESS);
	}

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
	system("PAUSE");
	return 0;
}

// ============================================================================
// FUNÇÃO DE INÍCIO DO PROGRAMA: WinMain()
// ============================================================================
// Em Windows, o programa começa sempre a sua execução na função WinMain()que desempenha // o papel da função main() do C em modo consola WINAPI indica o "tipo da função" (WINAPI // para todas as declaradas nos headers do Windows e CALLBACK para as funções de // processamento da janela)
// Parâmetros:
// hInst: Gerado pelo Windows, é o handle (número) da instância deste programa
// hPrevInst: Gerado pelo Windows, é sempre NULL para o NT (era usado no Windows 3.1)
// lpCmdLine: Gerado pelo Windows, é um ponteiro para uma string terminada por 0
// destinada a conter parâmetros para o programa
// nCmdShow: Parâmetro que especifica o modo de exibição da janela (usado em
// ShowWindow()
//int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
//{
//	UNREFERENCED_PARAMETER(hPrevInst);
//	UNREFERENCED_PARAMETER(lpCmdLine);
//
//	HWND hWnd; // hWnd é o handler da janela, gerado mais abaixo por CreateWindow()
//	MSG lpMsg; // MSG é uma estrutura definida no Windows para as mensagens
//	WNDCLASSEX wcApp; // WNDCLASSEX é uma estrutura cujos membros servem para
//	// definir as características da classe da janela
//	// ============================================================================
//	// 1. Definição das características da janela "wcApp"
//	// (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)
//	// ============================================================================
//	wcApp.cbSize = sizeof(WNDCLASSEX); // Tamanho da estrutura WNDCLASSEX
//	wcApp.hInstance = hInst; // Instância da janela actualmente exibida
//	// ("hInst" é parâmetro de WinMain e vem
//	// inicializada daí)
//	wcApp.lpszClassName = NAME_OF_WINDOWS_PROGRAM; // Nome da janela (neste caso = nome do programa)
//	wcApp.lpfnWndProc = TrataEventos; // Endereço da função de processamento da janela // ("TrataEventos" foi declarada no início e // encontra-se mais abaixo)
//	wcApp.style = CS_HREDRAW | CS_VREDRAW;// Estilo da janela: Fazer o redraw se for // modificada horizontal ou verticalmente
//	wcApp.hIcon = LoadIcon(NULL, IDI_APPLICATION);// "hIcon" = handler do ícon normal
//	//"NULL" = Icon definido no Windows
//	// "IDI_AP..." Ícone "aplicação"
//	wcApp.hIconSm = LoadIcon(NULL, IDI_INFORMATION);// "hIconSm" = handler do ícon pequeno
//	//"NULL" = Icon definido no Windows
//	// "IDI_INF..." Ícon de informação
//	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW); // "hCursor" = handler do cursor (rato)
//	// "NULL" = Forma definida no Windows
//	// "IDC_ARROW" Aspecto "seta"
//	wcApp.lpszMenuName = NULL; // Classe do menu que a janela pode ter
//	// (NULL = não tem menu)
//	wcApp.cbClsExtra = 0; // Livre, para uso particular
//	wcApp.cbWndExtra = 0; // Livre, para uso particular
//	wcApp.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
//	// "hbrBackground" = handler para "brush" de pintura do fundo da janela. Devolvido por // "GetStockObject".Neste caso o fundo será branco
//	// ============================================================================
//	// 2. Registar a classe "wcApp" no Windows
//	// ============================================================================
//	if (!RegisterClassEx(&wcApp))
//		return(0);
//	// ============================================================================
//	// 3. Criar a janela
//	// ============================================================================
//	hWnd = CreateWindow(
//		NAME_OF_WINDOWS_PROGRAM, // Nome da janela (programa) definido acima
//		TEXT("Exemplo de Janela Principal em C"),// Texto que figura na barra do título
//		WS_OVERLAPPEDWINDOW, // Estilo da janela (WS_OVERLAPPED= normal)
//		CW_USEDEFAULT, // Posição x pixels (default=à direita da última)
//		CW_USEDEFAULT, // Posição y pixels (default=abaixo da última)
//		CW_USEDEFAULT, // Largura da janela (em pixels)
//		CW_USEDEFAULT, // Altura da janela (em pixels)
//		(HWND)HWND_DESKTOP, // handle da janela pai (se se criar uma a partir de
//		// outra) ou HWND_DESKTOP se a janela for a primeira,
//		// criada a partir do "desktop"
//		(HMENU)NULL, // handle do menu da janela (se tiver menu)
//		(HINSTANCE)hInst, // handle da instância do programa actual ("hInst" é
//		// passado num dos parâmetros de WinMain()
//		0); // Não há parâmetros adicionais para a janela
//		// ============================================================================
//		// 4. Mostrar a janela
//		// ============================================================================
//	ShowWindow(hWnd, nCmdShow); // "hWnd"= handler da janela, devolvido por
//	// "CreateWindow"; "nCmdShow"= modo de exibição (p.e.
//	// normal/modal); é passado como parâmetro de WinMain()
//	UpdateWindow(hWnd); // Refrescar a janela (Windows envia à janela uma
//	// mensagem para pintar, mostrar dados, (refrescar)…
//	// ============================================================================
//	// 5. Loop de Mensagens
//	// ============================================================================
//	// O Windows envia mensagens às janelas (programas). Estas mensagens ficam numa fila de
//	// espera até que GetMessage(...) possa ler "a mensagem seguinte"
//	// Parâmetros de "getMessage":
//	// 1)"&lpMsg"=Endereço de uma estrutura do tipo MSG ("MSG lpMsg" ja foi declarada no
//	// início de WinMain()):
//	// HWND hwnd handler da janela a que se destina a mensagem
//	// UINT message Identificador da mensagem
//	// WPARAM wParam Parâmetro, p.e. código da tecla premida
//	// LPARAM lParam Parâmetro, p.e. se ALT também estava premida
//	// DWORD time Hora a que a mensagem foi enviada pelo Windows
//	// POINT pt Localização do mouse (x, y)
//	// 2)handle da window para a qual se pretendem receber mensagens (=NULL se se pretendem // receber as mensagens para todas as janelas pertencentes à thread actual)
//	// 3)Código limite inferior das mensagens que se pretendem receber
//	// 4)Código limite superior das mensagens que se pretendem receber
//	// NOTA: GetMessage() devolve 0 quando for recebida a mensagem de fecho da janela,
//	// terminando então o loop de recepção de mensagens, e o programa
//	while (GetMessage(&lpMsg, NULL, 0, 0))
//	{
//		TranslateMessage(&lpMsg); // Pré-processamento da mensagem (p.e. obter código
//		// ASCII da tecla premida)
//		DispatchMessage(&lpMsg); // Enviar a mensagem traduzida de volta ao Windows, que
//		// aguarda até que a possa reenviar à função de
//		// tratamento da janela, CALLBACK TrataEventos (abaixo)
//	}
//	// ============================================================================
//	// 6. Fim do programa
//	// ============================================================================
//	return((int)lpMsg.wParam); // Retorna sempre o parâmetro wParam da estrutura lpMsg
//}
//// ============================================================================
//// FUNÇÃO DE PROCESSAMENTO DA JANELA
//// Esta função pode ter um nome qualquer: Apenas é necesário que na inicialização da // estrutura "wcApp", feita no início de WinMain(), se identifique essa função. Neste // caso "wcApp.lpfnWndProc = WndProc"
////
//// WndProc recebe as mensagens enviadas pelo Windows (depois de lidas e pré-processadas // no loop "while" da função WinMain()
////
//// Parâmetros:
//// hWnd O handler da janela, obtido no CreateWindow()
//// messg Ponteiro para a estrutura mensagem (ver estrutura em 5. Loop...
//// wParam O parâmetro wParam da estrutura messg (a mensagem)
//// lParam O parâmetro lParam desta mesma estrutura
////
//// NOTA:Estes parâmetros estão aqui acessíveis o que simplifica o acesso aos seus valores
////
//// A função EndProc é sempre do tipo "switch..." com "cases" que descriminam a mensagem // recebida e a tratar. Estas mensagens são identificadas por constantes (p.e.
//// WM_DESTROY, WM_CHAR, WM_KEYDOWN, WM_PAINT...) definidas em windows.h ============================================================================
//LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
//{
//	switch (messg)
//	{
//	case WM_DESTROY: // Destruir a janela e terminar o programa
//	// "PostQuitMessage(Exit Status)"
//		PostQuitMessage(0);
//		break;
//	default:
//		// Neste exemplo, para qualquer outra mensagem (p.e. "minimizar","maximizar","restaurar") // não é efectuado nenhum processamento, apenas se segue o "default" do Windows
//		return DefWindowProc(hWnd, messg, wParam, lParam);
//		break;
//	}
//	return(0);
//}