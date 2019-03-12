#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <Dwmapi.h> 
#pragma comment(lib,"dwmapi.lib")
#pragma comment(lib,"d3d9.lib") 
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib, "winmm.lib")

#define M_PI	3.14159265358979323846264338327950288419716939937510

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }      //自定义一个SAFE_RELEASE()宏,便于资源的释放

class DxManager
{
public:
	DxManager();
	~DxManager();

	
	HWND game_hWnd;//游戏窗口句柄
	HWND over_hWnd;//overlay窗口句柄
	int s_width;//窗口大小
	int s_height;
	void SetupWindow(LPCTSTR szWndName);

	LPDIRECT3D9 pD3D;
	LPDIRECT3DDEVICE9 pDevice;
	LPD3DXFONT pFont;
	LPD3DXFONT pFontMenu;
	ID3DXLine* pD3DLine;
	void InitD3D();
	void render();
	void DrawString(int x, int y, DWORD D3DColor, LPD3DXFONT pUseFont, const char *fmt, ...);
	void DrawBox(float x, float y, float width, float height, D3DCOLOR color);
	void DrawPoint(int x, int y, int w, int h, DWORD color);
	void DrawCircle(int X, int Y, int radius, int numSides, DWORD Color);
	void FillRGB(int x, int y, int w, int h, D3DCOLOR color);


	void DrawBlood(float x, float y, float h, float w, float fBlood);
	void DrawCrosshair();

	//fps帧数
	void DrawFPS(int x, int y, DWORD D3DColor, LPD3DXFONT pUseFont);
	//顶点缓存测试
	LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer;    //顶点缓冲区对象
	LPDIRECT3DINDEXBUFFER9  g_pIndexBuffer;    // 索引缓存对象
	void TestDD();
	//变换测试
	void Matrix_Set();
	void TestBH();
};
//struct CUSTOMVERTEX
//{
//	FLOAT x, y, z, rhw;
//	DWORD color;
//};
////D3DFVF_XYZRHW，表示包含经过坐标变换的顶点坐标值，这样Direct3D就知道这些顶点坐标不需要再经过顶点坐标变换了，他们的坐标值就是最终显示屏幕上的坐标值了。
//#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)  //FVF灵活顶点格式

struct CUSTOMVERTEX
{
	FLOAT x, y, z;
	DWORD color;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)  //FVF灵活顶点格式