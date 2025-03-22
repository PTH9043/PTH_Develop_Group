#pragma once
#include "Client_Defines.h"
#include "CBase.h"
#include "CPlayer.h"
#include "CKnownKey_UI.h"
#include "CCheckPointInterface.h"

BEGIN(Engine)
class CUserInterface;
END

BEGIN(Client)

class CPlayerUIController  final : public CBase
{
protected:
	explicit CPlayerUIController(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CPlayerUIController() = default;

public:
	CKnownKey_UI* Get_KnownKey_E() { return m_pKnownKey_E; }
	CCheckPointInterface* Get_CheckPointInterface() { return m_pCheckPointInterface; }

public:
	HRESULT NativeConstruct(CPlayer* _pOwner);


private:
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;

	CKnownKey_UI*					m_pKnownKey_E = nullptr;
	CCheckPointInterface*		m_pCheckPointInterface = nullptr;
	// Owner
	CPlayer* m_pOwner = nullptr;
public:
	static CPlayerUIController* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CPlayer* _pOwner);
	virtual void Free() override;
};

END