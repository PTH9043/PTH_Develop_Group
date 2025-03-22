#pragma once
#include "CUserInterface.h"
#include "CCharacter.h"
#include "CText.h"

class CBar : public CUserInterface
{
protected:
	explicit CBar();
	explicit CBar(CCharacter* pOwner, const TCHAR* szMainPath, 
		const TCHAR* szBgPath, const RGBINFO& tInfo, const int& iBarValue);
	virtual ~CBar();
protected:
	// Character
	CCharacter*		m_pCharacter;
	// Divine Value
	const int		DIVINE_VALUE;
	const int		MUTI_VALUE;
	int				m_iBarValue;
	// Main Image
	CMyBmp*			m_pBarImg;
	// Remains 
	CMyBmp*			m_pBackgroundImg;
	// 체력의 상태를 나타내기 위한 TEXT
	CText*			m_pValText;
	// RGB Info 
	RGBINFO			m_tTranspercyRGB;

	bool				m_bChange;
public:
	// CObj을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;

	void Init_Pos(const VECTOR2& v2Pos);
protected:
	virtual void Update_Rect() override;
	// Calculate Bar
	virtual void Calculate_Bar(){}
public:
	void Set_Character(CCharacter*& pCharacter) { this->m_pCharacter = pCharacter; }
};

