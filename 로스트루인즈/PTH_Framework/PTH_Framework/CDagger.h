#pragma once
#include "CSword.h"

class CDagger : public CSword
{
public:
	explicit CDagger();
	explicit CDagger(CObj * pOwner, const TCHAR * szImgPath, const wstring & strName, const ESWORDID & eSwordID, const int & iPower, const int& iGold);
	virtual ~CDagger();
public:
	// CEquip을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;

	virtual CItem* Copy_Item() override;

	virtual void Obj_Active(const bool& IsActive) override;
protected:
	virtual void Update_Rect() override;
	// Rotate 
	virtual void Rotate();
	// override
	virtual void Update_OnwerInfo() override;
	virtual  void Fall() override;
};

