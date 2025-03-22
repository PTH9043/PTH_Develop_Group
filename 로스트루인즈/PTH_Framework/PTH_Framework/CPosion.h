#pragma once
#include "CItem.h"

class CPosion : public CItem
{
public:
	explicit CPosion();
	explicit CPosion(CObj* pOwner, const TCHAR* szImgPath, const wstring& strName, const EPOSIONTYPE& ePosionType,
		const int& iGold, const int& iRecoverValue);
	virtual ~CPosion();
protected:
	EPOSIONTYPE m_ePosionType;
	int m_iRecoverValue;
public:
	// 
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2& v2ScrollPos) override;
	virtual void Collision_Actor(CObj*& pObj) override;
	virtual void Release() override;

	virtual CItem* Copy_Item() override;
protected:
	virtual void Update_Rect() override;

public:
	// get 
	const EPOSIONTYPE& Get_PosionType() const { return m_ePosionType; }
	const int& Get_RecoverValue() const { return m_iRecoverValue; }
	// set
	void Set_PosionType(const EPOSIONTYPE& ePosionType) { this->m_ePosionType = ePosionType; }
};

