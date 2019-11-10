#include "Blocks.hpp"

// Direct2D interfaces
ID2D1Factory *pd2Factory;
ID2D1HwndRenderTarget *pTarget;
ID2D1SolidColorBrush *pBlackBrush, *pWhiteBrush, *pLightGrayBrush, *pDarkGrayBrush;

// Direct2D variables
D2D1_RENDER_TARGET_PROPERTIES rtProp;
D2D1_HWND_RENDER_TARGET_PROPERTIES hwndrtProp;
D2D1_COLOR_F blackColor = D2D1::ColorF(D2D1::ColorF::Black, 1.0f),
	whiteColor = D2D1::ColorF(D2D1::ColorF::White, 1.0f),
	lightGrayColor = D2D1::ColorF(D2D1::ColorF::LightGray, 1.0f),
	darkGrayColor = D2D1::ColorF(D2D1::ColorF::DarkGray, 1.0f);

// DirectWrite interfaces
IDWriteFactory *pdwFactory;

// DirectWrite variables
IDWriteTextFormat *pBlockHeaderTextFormat;

// Drawing variables
DrawType drawType;
InitProc initProc;
DrawProc drawProc;
void *drawArg;

// Common constants
const DWORD mainWindowStyle = WS_VISIBLE | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU;

// Common variables
RECT clientRect;
HWND hMainWindow;
PAINTSTRUCT ps;
int blockHeaderHeight = 20, _x, _y;
Block b1 = {10, 180, 120, 120, L"Block 1!", false, false, null};
Block b2 = {180, 10, 120, 120, L"Block 2!", false, false, null};
Block *selectedBlock, *draggingBlock;
Space _space;
Space *space = &_space;

// GDI variables
HDC hdcMainWindow, hdcBuffer;

// DIB variables
POINT screen;
HBITMAP hbmBuffer;
PixelStruct *image;
BITMAPINFO *pbmBufferInfo;

WNDCLASSEXW windowClass = {sizeof(WNDCLASSEXW), 0, (WNDPROC)WindowProc, 0, 0, 0, 0, 0, 0, 0, L"BlocksClass", 0};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	drawType = DT_DX;

	switch (drawType) {
	case DT_GDI:
		drawProc = GDIDraw;
		initProc = GDIInit;
		break;
	case DT_DIB:
		drawProc = DIBDraw;
		initProc = DIBInit;
		break;
	case DT_DX:
		drawProc = DXDraw;
		initProc = DXInit;
		break;
	}

	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursorW(0, IDC_ARROW);
	RegisterClassExW(&windowClass);
	hMainWindow = CreateWindowExW(0, L"BlocksClass", L"Blocks! (loading...)", mainWindowStyle, 0, 0, 800, 600, 0, 0, 0, 0);
	GetClientRect(hMainWindow, &clientRect);

	initProc();

	space->blocks = (Block *)calloc(2, sizeof(Block));
	space->blocks[0] = b1;
	space->blocks[1] = b2;
	space->blockCount = 2;

	SetWindowTextW(hMainWindow, L"Blocks!");
	
	MSG msg;
	while(GetMessageW(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	ExitProcess(0);

}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	int x, y;

	switch (uMsg) {
	case WM_CREATE:
		ShowWindow(hMainWindow, SW_NORMAL);
		UpdateWindow(hMainWindow);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_SIZE:
		if (hMainWindow) {
			if (!GetClientRect(hMainWindow, &clientRect)) {
				GetLastError();
			}
			if (pTarget->Resize(D2D1::SizeU(LOWORD(lParam), HIWORD(lParam))) != 0) {
				GetLastError();
			}
		}
		return 0;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
			return 0;
		}
		InvalidateRect(hMainWindow, &clientRect, 0);
		break;
	case WM_LBUTTONDOWN:
		if (selectedBlock) {
			selectedBlock->isSelected = false;
		}
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		for (int i = 0; i < space->blockCount; i++) {
			Block *block = &space->blocks[i];
			if (x > block->x && x < block->x + block->width && y > block->y && y < block->y + blockHeaderHeight) {
				selectedBlock = block;
				draggingBlock = block;
				block->isSelected = true;
				block->isDragging = true;
				_x = block->x - x;
				_y = block->y - y;
				break;
			}
		}
		return 0;
	case WM_LBUTTONUP:
		if (draggingBlock) {
			draggingBlock->isDragging = false;
			draggingBlock = null;
		}
		return 0;
	case WM_MOUSEMOVE:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		if (draggingBlock) {
			draggingBlock->x = x + _x;
			draggingBlock->y = y + _y;
		}
		InvalidateRect(hMainWindow, &clientRect, 0);
		return 0;
	case WM_PAINT:
		drawProc(space, (void *)drawArg);
		return 0;
	}
	return DefWindowProcW(hWnd, uMsg, wParam, lParam);

}

void GDIInit(void)
{

	hdcMainWindow = GetDC(hMainWindow);
	HDC hdc = GetDC(0);
	hdcBuffer = CreateCompatibleDC(hdc);
	ReleaseDC(0, hdc);
	SelectObject(hdcBuffer, GetStockObject(WHITE_BRUSH));
	SelectObject(hdcBuffer, GetStockObject(BLACK_PEN));

}

void DIBInit(void)
{
	
	screen.x = GetSystemMetrics(SM_CXSCREEN);
	screen.y = GetSystemMetrics(SM_CYSCREEN);
	pbmBufferInfo = (BITMAPINFO *)calloc(1, sizeof(BITMAPINFO));
	pbmBufferInfo->bmiHeader.biPlanes = 1;
	pbmBufferInfo->bmiHeader.biBitCount = pixelBits;
	pbmBufferInfo->bmiHeader.biCompression = BI_RGB;
	pbmBufferInfo->bmiHeader.biWidth = screen.x;
	pbmBufferInfo->bmiHeader.biHeight = screen.y;
	image = (PixelStruct *)malloc(screen.x * screen.y * sizeof(PixelStruct));
	hbmBuffer = CreateDIBSection(hdcBuffer, pbmBufferInfo, DIB_RGB_COLORS, (void **)image, 0, 0);
	hbmBuffer = CreateCompatibleBitmap(hdcBuffer, screen.x, screen.y);
	SelectObject(hdcBuffer, hbmBuffer);

}

void DXInit(void)
{

	if (D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pd2Factory) != S_OK) {
		MessageBoxW(0, L"Could not create Direct2D factory!", null, MB_ICONERROR);
		ExitProcess(-1);
	}
	if (DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown **)&pdwFactory) != S_OK) {
		MessageBoxW(0, L"Could not create DirectWrite factory!", null, MB_ICONERROR);
		ExitProcess(-1);
	}
	rtProp = D2D1::RenderTargetProperties();
	hwndrtProp = D2D1::HwndRenderTargetProperties(hMainWindow, D2D1::SizeU(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top));
	pd2Factory->CreateHwndRenderTarget(rtProp, hwndrtProp, &pTarget);
	if (pTarget) {
		blackColor.r = blackColor.g = blackColor.b = 0;
		pTarget->CreateSolidColorBrush(blackColor, &pBlackBrush);
		pTarget->CreateSolidColorBrush(whiteColor, &pWhiteBrush);
		pTarget->CreateSolidColorBrush(lightGrayColor, &pLightGrayBrush);
		pTarget->CreateSolidColorBrush(darkGrayColor, &pDarkGrayBrush);
	}
	pdwFactory->CreateTextFormat(L"Arial", null, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 18.0f, L"en-us", &pBlockHeaderTextFormat);

}

void GDIDraw(Space *workingSpace, void *arg)
{

	if (!workingSpace) {
		return;
	}
	
	Space *space = (Space *)workingSpace;
	BeginPaint(hMainWindow, &ps);
	FillRect(hdcBuffer, &clientRect, 0);
	for (int i = 0; i < space->blockCount; i++) {
		//GDIDrawBlock(&space->blocks[i]);
	}
	BitBlt(ps.hdc, 0, 0, clientRect.right, clientRect.bottom, hdcBuffer, 0, 0, SRCCOPY);
	EndPaint(hMainWindow, &ps);

}

void DIBDraw(Space *workingSpace, void *arg)
{

	if (!workingSpace) {
		return;
	}
	
	Space *space = (Space *)workingSpace;
	BeginPaint(hMainWindow, &ps);
	for (int i = 0; i < space->blockCount; i++) {
		DIBDrawBlock(&space->blocks[i]);
	}
	StretchDIBits(ps.hdc, 0, 0, clientRect.right, clientRect.bottom, 0, 0, clientRect.right, clientRect.bottom, (const void *)image, pbmBufferInfo, DIB_RGB_COLORS, SRCCOPY);
	EndPaint(hMainWindow, &ps);

}

void DXDraw(Space *workingSpace, void *arg)
{

	if (!pTarget || !workingSpace) {
		return;
	}
	
	Space *space = (Space *)workingSpace;
	pTarget->BeginDraw();
	pTarget->Clear(&whiteColor);
	for (int i = 0; i < space->blockCount; i++) {
		if (Block *block = &space->blocks[i]) {
			DXDrawBlock(block);
		}
	}
	pTarget->EndDraw();

}

void GDIDrawBlock(Block *block)
{

	Rectangle(hdcBuffer, block->x, block->y, block->x + block->width, block->y + blockHeaderHeight);
	Rectangle(hdcBuffer, block->x, (block->y + blockHeaderHeight), block->x + block->width, block->y + block->height + blockHeaderHeight);

}

void DIBDrawBlock(Block *)
{

}

void DXDrawBlock(Block *block)
{

	D2D1_RECT_F header = D2D1::RectF((FLOAT)(block->x), (FLOAT)(block->y), (FLOAT)(block->x + block->width), (FLOAT)(block->y + blockHeaderHeight));
	D2D1_RECT_F body = D2D1::RectF((FLOAT)block->x, (FLOAT)(block->y + blockHeaderHeight), (FLOAT)(block->x + block->width), (FLOAT)(block->y + block->height + blockHeaderHeight));

	pTarget->DrawRectangle(body, pBlackBrush, 1.0f, null);
	if (block->isDragging) {
		pTarget->FillRectangle(header, pDarkGrayBrush);
	} else if (block->isSelected) {
		pTarget->FillRectangle(header, pLightGrayBrush);
	}
	pTarget->DrawRectangle(header, pBlackBrush, 1.0f, null);
	pTarget->DrawTextW(block->headerString, wcslen(block->headerString), pBlockHeaderTextFormat, &header, pBlackBrush);

}