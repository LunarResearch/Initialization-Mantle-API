#include "grApi.h"


#ifdef _UNICODE
#define tWinMain wWinMain
#else
#define tWinMain WinMain
#endif // _UNICODE


int APIENTRY tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PTCHAR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	grApi gr;

	UpdateWindow(gr.hWnd);

	MSG msg{};
	while (msg.message != WM_QUIT)
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	return static_cast<GR_INT>(msg.wParam);
}
