#pragma once
#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Cinematic : public CCamera
{
private:
	explicit CCamera_Cinematic(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CCamera_Cinematic(const CCamera_Cinematic& _rhs);
	virtual ~CCamera_Cinematic() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
	virtual void Extern_ImGuiClass() override;

public:
	void Move_Camera_Position(const _double& _dTimeDelta);
	void Load_Data(const _wstring& _strName);

private:
	// Init CameraSetting
	_bool						m_bInitCameraSetting = false;
	// Cinematic Data 
	vector<CINEMATIC_CAMDATA>	m_CinematicDatas;
	// 현재 움직이고 있는 인덱스
	_uint						m_iCurIndex = 0;
	_float						m_fCamLerpSpeed = 0.f;
	_float4						m_vSlerpRotation = _float4(0.f, 0.f, 0.f, 0.f);

public:
	static CCamera_Cinematic* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END