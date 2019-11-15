#include "Blocks.hpp"

// Direct2D interfaces
ID2D1Factory *pd2Factory;
ID2D1HwndRenderTarget *pTarget;
ID2D1SolidColorBrush *pBlackBrush, *pWhiteBrush, *pLightGrayBrush, *pDarkGrayBrush,
	*pBlueColorBrush, *pRedColorBrush, *pGreenColorBrush, *pYellowColorBrush;

// Direct2D variables
D2D1_RENDER_TARGET_PROPERTIES rtProp;
D2D1_HWND_RENDER_TARGET_PROPERTIES hwndrtProp;
D2D1_COLOR_F blackColor = D2D1::ColorF(D2D1::ColorF::Black, 1),
	whiteColor = D2D1::ColorF(D2D1::ColorF::White),
	lightGrayColor = D2D1::ColorF(D2D1::ColorF::LightGray),
	darkGrayColor = D2D1::ColorF(D2D1::ColorF::DarkGray),
	blueColor = D2D1::ColorF(D2D1::ColorF::Blue),
	redColor = D2D1::ColorF(D2D1::ColorF::Red),
	greenColor = D2D1::ColorF(D2D1::ColorF::Green),
	yellowColor = D2D1::ColorF(D2D1::ColorF::Yellow);

// DirectWrite interfaces
IDWriteFactory *pdwFactory;

// DirectWrite variables
IDWriteTextFormat *pBlockHeaderTextFormat;

// Drawing variables
DrawType drawType;
InitProc initProc;
DrawProc Draw;
void *drawArg;

// Common constants
const DWORD mainWindowStyle = WS_VISIBLE | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU;
const wchar_t *const mainWindowClassName = L"BlocksClass";
const wchar_t *const mainWindowLoadingTitle = L"Blocks! (loading...)";
const wchar_t *const mainWindowTitle = L"Blocks!";

// Common variables
RECT clientRect;
HWND hMainWindow;
PAINTSTRUCT ps;
int blockHeaderHeight = 20, closeButtonSize = blockHeaderHeight, portSize = 3, _x, _y;
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

WNDCLASSEXW windowClass = {sizeof(WNDCLASSEXW), 0, (WNDPROC)WindowProc, 0, 0, 0, 0, 0, 0, 0, mainWindowClassName, 0};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	drawType = CURRENT_DRAW_TYPE;

	switch (drawType) {
	case DT_GDI:
		Draw = GDIDraw;
		initProc = GDIInit;
		break;
	case DT_DIB:
		Draw = DIBDraw;
		initProc = DIBInit;
		break;
	case DT_DX:
		Draw = DXDraw;
		initProc = DXInit;
		break;
	}

	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursorW(0, IDC_ARROW);
	RegisterClassExW(&windowClass);
	hMainWindow = CreateWindowExW(0, mainWindowClassName, mainWindowLoadingTitle, mainWindowStyle, 0, 0, 800, 600, 0, 0, 0, 0);
	GetClientRect(hMainWindow, &clientRect);

	initProc();

	space->blocks = (Block *)calloc(2, sizeof(Block));
#ifndef USE_CLASSES
	Block b1 = {10, 180, 120, 120, L"Block 1!", false, false, {0, 2}};
	Block b2 = {180, 10, 120, 120, L"Block 2!", false, false, {3, 3}};
#else
	Block b1;
	b1.x = 10;
	b1.y = 180;
	b1.width = 120;
	b1.height = 120;
	b1.headerString = L"Block 1!";
	b1.ports.inPortsCount = 0;
	b1.ports.outPortsCount = 2;
	Block b2;
	b2.x = 180;
	b2.y = 10;
	b2.width = 120;
	b2.height = 120;
	b2.headerString = L"Block 2!";
	b2.ports.inPortsCount = 3;
	b2.ports.outPortsCount = 3;
#endif
	space->blocks[0] = b1;
	space->blocks[1] = b2;
	space->blockCount = 2;

	SetWindowTextW(hMainWindow, mainWindowTitle);
	
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
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		for (int i = 0; i < space->blockCount; i++) {
			Block *block = &space->blocks[i];
			if ((x > block->x) && (x < block->x + block->width) && (y > block->y) && (y < block->y + block->height)) {
				if ((y > block->y) && (y < block->y + blockHeaderHeight)) {
					if (x > block->x + block->width - closeButtonSize) {
						block->x = -1000;
						block->y = -1000;
						return 0;
					}
					draggingBlock = block;
					block->isDragging = true;
					_x = block->x - x;
					_y = block->y - y;
				}
				if (selectedBlock) {
					selectedBlock->isSelected = false;
				}
				selectedBlock = block;
				block->isSelected = true;
				return 0;
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
		Draw(space, drawArg);
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
		pTarget->CreateSolidColorBrush(blueColor, &pBlueColorBrush);
		pTarget->CreateSolidColorBrush(redColor, &pRedColorBrush);
		pTarget->CreateSolidColorBrush(greenColor, &pGreenColorBrush);
		pTarget->CreateSolidColorBrush(yellowColor, &pYellowColorBrush);
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
		if (Block *block = &space->blocks[i]) {
			GDIDrawBlock(block);
		}
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
		if (Block *block = &space->blocks[i]) {
			DIBDrawBlock(block);
		}
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

void DIBDrawBlock(Block *block)
{

}

void DXDrawBlock(Block *block)
{

	D2D1_RECT_F fullBlock = BlockRect(block);
	D2D1_RECT_F header = HeaderRect(block);
	D2D1_RECT_F body = BodyRect(block);
	D2D1_RECT_F close = CloseRect(block);

	pTarget->FillRectangle(&fullBlock, pWhiteBrush);
	pTarget->DrawRectangle(&body, pBlackBrush, 1.0f);
	if (block->isDragging) {
		pTarget->FillRectangle(&header, pDarkGrayBrush);
	} else if (block->isSelected) {
		pTarget->FillRectangle(&header, pLightGrayBrush);
	}
	pTarget->DrawRectangle(&close, pBlackBrush, 1.0f);
	pTarget->DrawLine(IntPoint(close.left, close.top), IntPoint(close.right, close.bottom), pBlackBrush, 0.5f);
	pTarget->DrawLine(IntPoint(close.left, close.bottom), IntPoint(close.right, close.top), pBlackBrush, 0.5f);
	pTarget->DrawRectangle(&header, pBlackBrush, 1.0f);
	pTarget->DrawTextW(block->headerString, wcslen(block->headerString), pBlockHeaderTextFormat, &header, pBlackBrush);

	if (block->ports.inPortsCount) {
		float inPortsStep = (float)(block->height) / (block->ports.inPortsCount + 1);
		float portHeight = (float)(block->y + blockHeaderHeight + inPortsStep);
		for (int i = 0; i < block->ports.inPortsCount; i++, portHeight += inPortsStep) {
			D2D1_ELLIPSE port = IntEllipse(IntPoint(block->x, portHeight), portSize, portSize);
			pTarget->FillEllipse(&port, pBlackBrush);
		}
	}
		
	if (block->ports.outPortsCount) {
		float outPortsStep = (float)(block->height) / (block->ports.outPortsCount + 1);
		float portHeight = (float)(block->y + blockHeaderHeight + outPortsStep);
		for (int i = 0; i < block->ports.outPortsCount; i++, portHeight += outPortsStep) {
			D2D1_ELLIPSE port = IntEllipse(IntPoint(block->x + block->width, portHeight), portSize, portSize);
			pTarget->FillEllipse(&port, pBlackBrush);
		}
	}

}
