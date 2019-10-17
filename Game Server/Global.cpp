#include "Global.h"
#include "MemoryBitStream.h"

HINSTANCE Global::mHInstance = NULL;
HWND Global::mHwnd = NULL;
LPD3DXSPRITE Global::mSpriteHandler = NULL;
int Global::mWidth = 1300;
int Global::mHeight = 770;
LPDIRECT3DDEVICE9 Global::mDevice = nullptr;
bool Global::isGameRunning = true;
IDirect3DSurface9* Global::backSurface = nullptr;
TCPSocketPtr Global::socket = NULL;
int Global::RTT = 0;
float Global::fps = 0;
LPDIRECT3DTEXTURE9 Global::mMapTexture = NULL;
LPDIRECT3DTEXTURE9 Global::mTankTexture = NULL;
LPDIRECT3DTEXTURE9 Global::mBoxTexture = NULL;

void Global::SendExitGame()
{
	OutputMemoryBitStream os;
	os.Write(Define::ExitGame, Define::bitofTypePacket);
	socket->Send(os.GetBufferPtr(), os.GetByteLength());
}

void Global::SetCurrentDevice(LPDIRECT3DDEVICE9 device)
{
	mDevice = device;
}

LPDIRECT3DDEVICE9 Global::GetCurrentDevice()
{
	return mDevice;
}

HINSTANCE Global::GetCurrentHINSTACE()
{
	return mHInstance;
}

HWND Global::getCurrentHWND()
{
	return mHwnd;
}

void Global::SetCurrentHINSTACE(HINSTANCE hInstance)
{
	mHInstance = hInstance;
}

void Global::SetCurrentHWND(HWND hWnd)
{
	mHwnd = hWnd;
}

void Global::SetCurrentSpriteHandler(LPD3DXSPRITE spriteHandler)
{
	mSpriteHandler = spriteHandler;
}

LPD3DXSPRITE Global::GetCurrentSpriteHandler()
{
	return mSpriteHandler;
}

void Global::SetWidth(int width)
{
	mWidth = width;
}

int Global::GetWidth()
{
	return mWidth;
}

void Global::SetHeight(int height)
{
	mHeight = height;
}

int Global::GetHeight()
{
	return mHeight;
}