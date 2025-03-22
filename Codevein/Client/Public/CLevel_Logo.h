#pragma once
#include "Client_Defines.h"
#include "CLevel.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CUI_LoadImg;
class CLevel_Logo final : public CLevel
{
private:
	explicit CLevel_Logo(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Logo() = default;
public:
	virtual HRESULT NativeConstruct() override;
	virtual void Tick(const _double& _dDeltaSeconds)override;
	virtual void Late_Tick(const _double& _dDeltaSeconds)override;
	virtual HRESULT Render()override;
private:
	HRESULT Ready_Layer_Background(const _tchar* _pLayerTag);
	HRESULT Ready_Layer_Button(const _tchar* _pLayerTag);
	HRESULT Ready_Layer_Text(const _tchar* _pLayerTag);
	HRESULT Ready_LoadingImg();
private:
	// Update Img 
	void Update_LoadImg(const _double& _dTimeDelta);
private:
	CGameObject* m_pBackground		= nullptr;
	CGameObject* m_pTitleText			= nullptr;
	CGameObject* m_pStartButton		= nullptr;
	// Loading
	vector<CUI_LoadImg*> m_LoadingImgList;
private:
	_uint m_iInitIndex = 0;
	_float m_fDelayTime = 0.f;
public:
	static CLevel* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END