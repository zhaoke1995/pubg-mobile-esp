#include "GameESP.h"
#include "Misc.h"
#include "aobscan.h"

DxManager *pDxm = NULL;
D3DMenu* pMenu = NULL;
MemoryManager *pMM = NULL;



//------全局实时更新的地址-------
DWORD dwJuzhenAddr = 0;
DWORD dwPeople[1000];
DWORD dwPeopleCount;

DWORD dwGoods[2000];
DWORD dwGoodsCount;

DWORD dwCars[1000];
DWORD dwCarsCount;

DWORD MyTeamId;
DWORD dwLocalPlayerAddr;
//------全局实时更新的地址-------




CRITICAL_SECTION cs;//定义临界区对象
DWORD dwGamePid;


//fcd7????00002000 对象
//98??????08000400 本人
//1676（血量偏移）
//3440（坐标偏移）

//国服：
//对象地址：C8 ?? ?? 2A 00 00 20 00
//本人数据：98 ?? ?? 2A 00 00 20 00
//矩阵地址：AB AA AA 3E 00 00 00 80 00 00 00 80 00 00 80 3F 00 00 00 80 61 8B 98 3F
//人机偏移：一级18 二级 10进制684
//血量偏移：一级18 二级68C
//编号偏移：一级18 二级2CC 三级14
//激光偏移：一级18 二级10D0  三级偏移650 后坐偏移574  上下偏移610  左右偏移628
//瞬间偏移：一级18 二级10D0  三级偏移650 直接2EC


//对象特征：FC D7 ? ? ? ? 00 00 20 00
//队友编号： + 24 + 716 + 20
//血量： + 24 + 1676
//无后： + 24 + 4272 + 1616 + 1732
//激光： + 24 + 4272 + 1616 + 1756
//顺击： + 24 + 4272 + 1616 + 748
//零秒开镜： + 24 + 4272 + 1616 + 1480
//人物状态： + 24 + 4432
//敌人动态：4004
//上帝视角： + 24 + 2704 + 288
//上帝视角： + 24 + 3024 + 288
//人机判断： + 24 + 684
//本人特征：98 ? ? ? ? ? ? 08 00 04 00
//物品特征：9C ? ? ? ? ? ? 00 00 20 00
//载具特征：EC ? ? ? ? ? ? 00 00 20 00

//9C ?? ?? 2A 00 00 20 00


namespace GameData {
	BYTE bJuzhenTag[] = { 0xAB,0xAA,0xAA,0x3E,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x80,0x00,0x00,0x80,0x3F,0x00,0x00,0x00,0x80,0x61,0x8B,0x98,0x3F };
	BYTE bPeopleTag[] = { 0xc8,'?', '?',0x2a,0x00,0x00,0x20,0x00 };
	BYTE bLocalPlayerTag[] = { 0x98,'?','?',0x2A ,0x00 ,0x00 ,0x20 ,0x00 };
	//BYTE bGoodsTag[] = { 0x9C,'?','?','?', 0x00 ,0x00 ,0x20 ,0x00 };
	//BYTE bGoodsTag[] = { 0x9C,0xdf,'?','?', 0x00 ,0x00 ,0x20 ,0x00 };
	BYTE bGoodsTag[] = { 0x9C,'?','?',0x2a, 0x00 ,0x00 ,0x20 ,0x00 };
	BYTE bCarsTag[] = { 0xEC,'?', '?', '?',0x00,0x00,0x20,0x00 };
	DWORD OFFSET_1 = 0x18; //24
	DWORD OFFSET_2 = 0x120; //288
	DWORD BLOOD_OFFSET = 0x68C;
	DWORD POS_OFFSET = 3440;
	DWORD RENJI_OFFSET = 684;
	DWORD STATUS_OFFSET = 2544;

	DWORD B_GOODS_OFFSET = 132;//判断是否为物品
	DWORD GOODS_ID_OFFSET = 272;//物品id偏移
	DWORD GOODS_POS_OFFSET = 336;//物品坐标偏移
}


namespace global
{
	int bEsp = false;
	int bPlayer = false;
	int bVehicle = false;
	int bGoods = false;

	int bAim = false;
	int bAimOpen = false;
	int nAimPos = 0;

	int bAbout = 0;
	int nDebug = 0;

	int nOther = 0;
	int bSpeed = false;
	int bWeapon = false;
	int bXiGoods = false;
	int bMiniMap = false;
}

const char *opt_Grp[] = { "+", "-" };
const char *opt_OFFON[] = { "关", "开" };
const char *sRadar[] = { "关", "敌人", "所有人" };
const char *sAimSlot[] = { "头部", "颈部", "胸部", "裆部" };
const char *sSpeedHax[] = { "关", "1.5倍", "2倍", "2.5倍" ,"5倍" };

void ShowMenu()
{
	
	if (pMenu == 0) {
		pMenu = new D3DMenu((char*)"菜单选项", 390, 134);
		pMenu->dxm = pDxm;
		pMenu->visible = 1;
		pMenu->col_title = MCOLOR_TITLE;
	}
	else
	{
		pMenu->dxm = pDxm;
		if (pMenu->noitems == 0)
		{
			pMenu->AddGroup((char*)"[透視]", &global::bEsp, (char**)opt_Grp);
			if (global::bEsp)
			{
				pMenu->AddItem((char*)"人物透視", &global::bPlayer, (char**)opt_OFFON);
				pMenu->AddItem((char*)"物品透視", &global::bGoods, (char**)opt_OFFON);
				pMenu->AddItem((char*)"车辆透視", &global::bVehicle, (char**)opt_OFFON);
			}
			pMenu->AddGroup((char*)"[自瞄]", &global::bAim, (char**)opt_Grp);
			if (global::bAim)
			{
				pMenu->AddItem((char*)"自瞄开关", &global::bAimOpen, (char**)opt_OFFON);
				pMenu->AddItem((char*)"自瞄位置", &global::nAimPos, (char**)sAimSlot, 4);
			}
			//其他
			pMenu->AddGroup((char*)"[其他]", &global::nOther, (char**)opt_Grp);
			if (global::nOther)
			{
				pMenu->AddItem((char*)"绘小地图", &global::bMiniMap, (char**)opt_OFFON);
				pMenu->AddItem((char*)"无后坐力", &global::bWeapon, (char**)opt_OFFON);
				pMenu->AddItem((char*)"吸引空投", &global::bXiGoods, (char**)opt_OFFON);
				pMenu->AddItem((char*)"全局加速", &global::bSpeed, (char**)sSpeedHax, 4);
			}
			pMenu->AddGroup((char*)"[关于]", &global::bAbout, (char**)opt_Grp);
			if (global::bAbout)
			{
				pMenu->AddItem((char*)"打印日志", &global::nDebug, (char**)opt_OFFON);
			}
		}
		pMenu->Show();
		pMenu->Nav();
	}
}


BOOL WorldToScreen2(D3DXVECTOR3 _Enemy, D3DXVECTOR3 &_Screen,int &nLen)
{
	_Screen = D3DXVECTOR3(0, 0, 0);
	float ScreenW = 0;
	D3DMATRIX GameViewMatrix = pMM->RPM<D3DMATRIX>(dwJuzhenAddr, sizeof(D3DMATRIX));
	ScreenW = (GameViewMatrix._14 * _Enemy.x) + (GameViewMatrix._24* _Enemy.y) + (GameViewMatrix._34 * _Enemy.z + GameViewMatrix._44);
	nLen = ScreenW / 100;
	if (ScreenW < 0.0001f)
	{ 
		return FALSE;
	}
		

	float ScreenY = (GameViewMatrix._12 * _Enemy.x) + (GameViewMatrix._22 * _Enemy.y) + (GameViewMatrix._32 * (_Enemy.z+85) + GameViewMatrix._42);
	float ScreenX = (GameViewMatrix._11 * _Enemy.x) + (GameViewMatrix._21 * _Enemy.y) + (GameViewMatrix._31 * _Enemy.z + GameViewMatrix._41);

	_Screen.y = (pDxm->s_height / 2) - (pDxm->s_height / 2) * ScreenY / ScreenW;
	_Screen.x = (pDxm->s_width / 2) + (pDxm->s_width / 2) * ScreenX / ScreenW;
	float y1 = (pDxm->s_height / 2) - (GameViewMatrix._12*_Enemy.x + GameViewMatrix._22 * _Enemy.y +GameViewMatrix._32 *(_Enemy.z -95 ) +GameViewMatrix._42) *(pDxm->s_height / 2)/ ScreenW;
	_Screen.z=y1 - _Screen.y;
	return TRUE;
}

BOOL WorldToScreenGoods(D3DXVECTOR3 _Enemy, D3DXVECTOR3 &_Screen, int &nLen)
{
	_Screen = D3DXVECTOR3(0, 0, 0);
	float ScreenW = 0;
	D3DMATRIX GameViewMatrix = pMM->RPM<D3DMATRIX>(dwJuzhenAddr, sizeof(D3DMATRIX));
	ScreenW = (GameViewMatrix._14 * _Enemy.x) + (GameViewMatrix._24* _Enemy.y) + (GameViewMatrix._34 * _Enemy.z + GameViewMatrix._44);
	nLen = ScreenW / 100;
	if (ScreenW < 0.0001f)
	{
		return FALSE;
	}


	float ScreenY = (GameViewMatrix._12 * _Enemy.x) + (GameViewMatrix._22 * _Enemy.y) + (GameViewMatrix._32 * (_Enemy.z + 5) + GameViewMatrix._42);
	float ScreenX = (GameViewMatrix._11 * _Enemy.x) + (GameViewMatrix._21 * _Enemy.y) + (GameViewMatrix._31 * _Enemy.z + GameViewMatrix._41);

	_Screen.y = (pDxm->s_height / 2) - (pDxm->s_height / 2) * ScreenY / ScreenW;
	_Screen.x = (pDxm->s_width / 2) + (pDxm->s_width / 2) * ScreenX / ScreenW;
	float y1 = (pDxm->s_height / 2) - (GameViewMatrix._12*_Enemy.x + GameViewMatrix._22 * _Enemy.y + GameViewMatrix._32 *(_Enemy.z) + GameViewMatrix._42) *(pDxm->s_height / 2) / ScreenW;
	_Screen.z = y1 - _Screen.y;
	return TRUE;
}

DWORD WINAPI ThreadUpdateData(LPVOID p)
{
	while (1)
	{
		
		if (dwJuzhenAddr == 0 || pMM->RPM<float>(dwJuzhenAddr + 56, 4) != 3)
		{
			std::vector<DWORD_PTR> vAddr;
			pMM->MemSearch(GameData::bJuzhenTag, sizeof(GameData::bJuzhenTag), 0x40000000, 0x7fffffff, FALSE, 1, vAddr);
			//vAddr = AobScan::FindSigX32(dwGamePid, "ABAAAA3E00000080000000800000803F00000080618B983F", 0x40000000, 0x7fffffff);
			std::vector<DWORD_PTR>::iterator it;
		
			for (it = vAddr.begin(); it != vAddr.end(); it++)
			{
				DWORD dwTag = pMM->RPM<DWORD>(*it + 0x3b8, sizeof(DWORD));
				//ViewWorld.m[0][3]
				D3DMATRIX dmTmp = pMM->RPM<D3DMATRIX>(*it + 268 - 56, sizeof(D3DMATRIX));
				if (dwTag == 0x80000000)
				{
					dwJuzhenAddr = *it + 268 - 56;
					break;
				}
			}
		}
		//更新对象
		std::vector<DWORD_PTR> vPeople;
		pMM->MemSearch(GameData::bPeopleTag, sizeof(GameData::bPeopleTag), 0x00327000, 0x7fffffff, FALSE, 0, vPeople);
		//vPeople = AobScan::FindSigX32(dwGamePid, "fcd7????00002000", 0x10000000, 0x7fffffff);
		//LeaveCriticalSection(&cs);//离开临界区
		dwPeopleCount = vPeople.size();
		for (int i=0;i< dwPeopleCount;i++)
		{
			dwPeople[i] = vPeople[i];
		}

		

		Sleep(1000*2);//更新数据的时间
	}
	return 1;
}

DWORD WINAPI ThreadUpdateData2(LPVOID p)
{
	while (1)
	{
		//更新物品
		std::vector<DWORD_PTR> vGoods;
		pMM->MemSearch(GameData::bGoodsTag, sizeof(GameData::bGoodsTag), 0x00327000, 0x7fffffff, FALSE, 0, vGoods);
		//vGoods = AobScan::FindSigX32(dwGamePid, "9CDF????00002000", 0x10000000, 0x7fffffff);

		dwGoodsCount = vGoods.size();
		for (int i = 0; i < dwGoodsCount; i++)
		{
			dwGoods[i] = vGoods[i];
		}
		//printf("物品数目 %d\n", dwGoodsCount);
		Sleep(1000 * 2);
	}
}

DWORD WINAPI ThreadUpdateData3(LPVOID p)
{
	while (1)
	{
		//更新物品
		std::vector<DWORD_PTR> vCars;
		pMM->MemSearch(GameData::bCarsTag, sizeof(GameData::bCarsTag), 0x00327000, 0x7fffffff, FALSE, 0, vCars);
		//vGoods = AobScan::FindSigX32(dwGamePid, "9CDF????00002000", 0x10000000, 0x7fffffff);

		dwCarsCount = vCars.size();
		for (int i = 0; i < dwCarsCount; i++)
		{
			dwCars[i] = vCars[i];
		}
		//printf("车辆数目 %d\n", dwCarsCount);
		Sleep(1000 * 2);
	}
}

FLOAT GetDistance(FLOAT X, FLOAT Y, FLOAT X1, FLOAT Y1)
{
	FLOAT XX, YY;
	XX = X - X1;
	YY = Y - Y1;
	return sqrt(XX * XX + YY * YY);
}
BOOL bLockAim = FALSE;
float fMin = 0.0f;
float g_AimX = 0.0f;
float g_AimY = 0.0f;


void ESPWork()
{
#pragma region 菜单处理
	ShowMenu();
#pragma endregion 

#pragma region FPS准心等杂项
	pDxm->DrawFPS(pDxm->s_width-320,5,D3DCOLOR_XRGB(168, 39, 136),pDxm->pFont);
	//pDxm->DrawCrosshair();
#pragma endregion 

#pragma region 主要功能

	int nRealPeopleCount = 0;
	fMin = 0.0f;
	g_AimX = 0.0f;
	g_AimY = 0.0f;

	for (int i = 0;i<dwPeopleCount;i++)
	{
		DWORD_PTR dw1 = pMM->RPM<DWORD_PTR>(dwPeople[i] + GameData::OFFSET_1, sizeof(DWORD_PTR));
		DWORD_PTR dw2 = pMM->RPM<DWORD_PTR>(dw1 + GameData::OFFSET_2, sizeof(DWORD_PTR));
		float fBlood = pMM->RPM<float>(dw2 + GameData::BLOOD_OFFSET, sizeof(float));
		
		if (fabs(fBlood) > 1e-5 && fBlood >0.0)//大于0
		{

			DWORD dwTeamId = pMM->RPM<DWORD>(pMM->RPM<DWORD>(dw1 + 0x2cc, sizeof(DWORD)) + 0x14, sizeof(DWORD));
	
			if(dwTeamId<0x10)
			{
				continue;
			}

			nRealPeopleCount++;
			D3DXVECTOR3 vPos = pMM->RPM<D3DXVECTOR3>(dw2 + GameData::POS_OFFSET, sizeof(D3DXVECTOR3));
			D3DXVECTOR3 vret;
			int nLen = 0;

			if (WorldToScreen2(vPos, vret, nLen))
			{	
								
				DWORD dwRenji = pMM->RPM<DWORD>(dw1 + GameData::RENJI_OFFSET, sizeof(DWORD));

				DWORD dwStatus = pMM->RPM<DWORD>(dw1 + GameData::STATUS_OFFSET, sizeof(DWORD));
				if (global::nDebug)
				{
					if (nLen-1==1)
					{
						printf("%d %x %x\n", dwStatus,dwTeamId,dwPeople[i]);
					}
					
				}
				float x = vret.x;
				float y = vret.y;
				float h = vret.z;
				float w = vret.z / 2;

				float aimx = vret.x;
				float aimy = vret.y + vret.z / 4;
				

				if (dwStatus == 1 || dwStatus == 4 )
				{
					y = vret.y+ vret.z / 5;
					h = h - vret.z / 5;
					aimy = aimy + vret.z / 6;
				}
				if (dwStatus == 2 || dwStatus == 5 || dwStatus == 7)
				{
					y = vret.y + vret.z / 4;
					h = h - vret.z / 4;
					aimx = aimx + vret.z / 10;
					aimy = aimy + vret.z / 4;
				}
				if (global::bAimOpen)
				{
					//if (bLockAim == FALSE)//没锁定的时候再计算最小屏幕距离
					if(1)
					{
						FLOAT CurrentDistance = GetDistance(pDxm->s_width / 2.0, pDxm->s_height / 2.0, aimx, aimy);
						if (fMin == 0)
						{
							fMin = CurrentDistance;
							g_AimX = aimx;
							g_AimY = aimy;
						}
						else
						{
							if (CurrentDistance < fMin)
							{
								fMin = CurrentDistance;
								g_AimX = aimx;
								g_AimY = aimy;
							}
						}
					}
				}
				
				//pDxm->DrawPoint(aimx, aimy, 10, 10, TextBlack);

				if (dwRenji == 0)//是人机
				{
					pDxm->DrawString(x , y, TextBlue, pDxm->pFont, "[%dm]", nLen - 1);
				}
				else
				{
					pDxm->DrawString(x, y, TextPink, pDxm->pFont, "[%dm]", nLen - 1);
				}
				pDxm->DrawBox(x - vret.z / 4, y, w, h, TextWhite);
				pDxm->DrawBlood(x - vret.z / 4 - 5, y, h, w, fBlood);				
			}
		}
	}
	if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0 && global::bAimOpen)
	//if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0 && global::bAimOpen)
	{
		bLockAim = TRUE;
		float boneX = g_AimX - pDxm->s_width / 2.0;
		float boneY = g_AimY - pDxm->s_height / 2.0;
		
		if (boneX >= -60.0 && boneX <= 60.0 && boneY >= -60.0 && boneY <= 60.0)
		{
			mouse_event(MOUSEEVENTF_MOVE, boneX/5, boneY/5, NULL, NULL);//除以5因为游戏中移动这个相对距离的话实际上会有更大的偏差
		}
	}
	else
	{
		bLockAim = FALSE;
		g_AimX = 0;
		g_AimY = 0;
	}

	pDxm->DrawString(pDxm->s_width/2-300, pDxm->s_height/11, DARKORANGE, pDxm->pFont, "附近存在[%d]敌人", nRealPeopleCount/2);
#pragma endregion

#pragma region 物品
	if (global::bGoods)
	{
		int nNowGoodCount = 0;
		for (size_t i = 0; i < dwGoodsCount; i++)
		{
			DWORD_PTR t1 = pMM->RPM<DWORD_PTR>(dwGoods[i] + GameData::B_GOODS_OFFSET, sizeof(DWORD_PTR));
			//if (t1 == 0xa8a0613)
			if(1)
			{
				nNowGoodCount++;
				int nId = pMM->RPM<int>(dwGoods[i] + GameData::GOODS_ID_OFFSET, sizeof(int));

				char szGoodName[20] = { 0 };
				switch (nId)
				{
				case 1098798372:
					strcat(szGoodName, "三级包");
					break;
				case 1078228938:
					strcat(szGoodName, "SCAR-L");
					break;
				case 1070342297:
					strcat(szGoodName, "M416");
					break;
				case 1075583328:
					strcat(szGoodName, "QBZ");
					break;
				case 1075923558:
					strcat(szGoodName, "SKS");
					break;
				case 1081607025:
					strcat(szGoodName, "SLR");
					break;
				case 1084841537:
					strcat(szGoodName, "三级甲");
					break;
				case 1103479324:
					strcat(szGoodName, "三级头");
					break;
				case 1089312141:
					strcat(szGoodName, "八倍镜");
					break;
				case 1090071692:
					strcat(szGoodName, "六倍镜");
					break;
				case 1091232615:
					strcat(szGoodName, "四倍镜");
					break;
				case 1080193518:
					strcat(szGoodName, "7.62mm");
					break;
				case 1079905560:
					strcat(szGoodName, "5.56mm");
					break;
				default:
					break;
				}
				if (szGoodName[0] == 0)
				{
					break;
				}
				printf("%s\n",szGoodName);

				D3DXVECTOR3 vPosGoods = pMM->RPM<D3DXVECTOR3>(dwGoods[i] + GameData::GOODS_POS_OFFSET, sizeof(D3DXVECTOR3));
				D3DXVECTOR3 vret;
				int nLen = 0;

				if (WorldToScreenGoods(vPosGoods, vret, nLen))
				{
					printf("draw %f %f %d\n",vret.x,vret.y,nLen);
					pDxm->DrawString(vret.x, vret.y, TextRed, pDxm->pFont, "%s[%dm]", szGoodName, nLen);
				}
			}
		}

	}
	
#pragma endregion
#pragma region 车辆
	if (global::bVehicle)
	{
		for (size_t i = 0; i < dwCarsCount; i++)
		{
			D3DXVECTOR3 vPosCars = pMM->RPM<D3DXVECTOR3>(dwCars[i] + GameData::GOODS_POS_OFFSET, sizeof(D3DXVECTOR3));
			D3DXVECTOR3 vret;
			int nLen = 0;
			if (WorldToScreenGoods(vPosCars, vret, nLen))
			{
				if (nLen>0 && nLen<1000)
				{
					pDxm->DrawString(vret.x, vret.y, TextCyan, pDxm->pFont, "座驾[%dm]", nLen);
				}
				
			}
		}
	}
	
#pragma endregion


}