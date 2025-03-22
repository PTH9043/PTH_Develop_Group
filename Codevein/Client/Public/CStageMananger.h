#pragma once
#include "CManager.h"
#include "CStage.h"

BEGIN(Client)

class CStageManager final : public CManager
{
protected:
	explicit CStageManager();
	explicit CStageManager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CStageManager() = default;
public:
	CStage* Get_Stage() { return m_pStage; }
public:
	HRESULT NativeConstruct();
public:
	HRESULT Save_Data(const _string& _strPath);
	HRESULT Load_Data(const _string& _strPath, const LEVEL& _eLevel);
	void Reset() { Safe_Release(m_pStage); }
private:
	CStage* m_pStage = nullptr;
public:
	static CStageManager* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END