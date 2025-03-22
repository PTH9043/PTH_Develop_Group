#pragma once
#include "CBase.h"
#include "Client_Defines.h"

BEGIN(Client)

class CManager : public CBase
{
protected:
	explicit CManager();
	explicit CManager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CManager() = default;

protected:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

protected:
	virtual void Free() override;
};

END