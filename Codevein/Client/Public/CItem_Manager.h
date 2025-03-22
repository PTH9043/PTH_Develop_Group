#pragma once
#include "CManager.h"
#include "CWeapon.h"

BEGIN(Engine)
class CModel;
END


BEGIN(Client)

class CItem_Manager final : public CManager
{
private:
	explicit CItem_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CItem_Manager() = default;

public:

public:
	HRESULT NativeConstruct();

	// Save Data
	HRESULT Save_Data(const _string& _strFolderPath);
	// Load Data 
	HRESULT Load_Data(const _string& _strFolderPath);

protected:

public:
	static CItem_Manager* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END