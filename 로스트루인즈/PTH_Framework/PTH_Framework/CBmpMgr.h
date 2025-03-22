#pragma once
#include "Include.h"
#include "CMyBmp.h"
#include "CManager.h"

class CBmpMgr final : public CManager
{
public:
	explicit CBmpMgr();
	explicit CBmpMgr(CGameMgr*& pGameMgr);
	virtual ~CBmpMgr();
protected:
	map<const TCHAR*, CMyBmp*>	m_mapBit;

public:
	// CManager을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Release() override;

	// Load Bmp
	void Insert_Bmp(const TCHAR* pFilePath, const TCHAR* pImgKey);
	// FInd Image Bmp
	CMyBmp* Find_Image_MyBmp(const TCHAR* pImgKey);
	// ID
	// Find Image 
	HDC Find_Image(const TCHAR* pImgKey);

	void Init_Map_Data();
	void Init_Player_Info();
	void Init_Monster_Info();
	void Init_UI_Info();
	void Init_Item_Info();
	void Init_Effect_Info();
};

