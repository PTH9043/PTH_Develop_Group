#pragma once 
#include "Client_Defines.h"
#include "CBase.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CObjParents : public CBase
{
private:
	explicit CObjParents();
	virtual ~CObjParents() = default;

public:
	void Add_Child(CGameObject* _pGameObject);
	list<CGameObject*> Get_ChildObj() { return m_ChildObj; }

public:
	HRESULT NativeConstruct();
	// Render List
	void Render_List();

protected:
	typedef list<CGameObject*> CHILDOBJ;
	CHILDOBJ m_ChildObj;
public:
	static CObjParents* Create();
	virtual void Free() override;
};

END