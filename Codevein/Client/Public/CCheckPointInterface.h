#pragma once
#include "Client_Defines.h"
#include "CUserInterface.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)
class CMoveToCheckPointsImg;
class CButton;

class CCheckPointInterface : public CUserInterface
{
public:
	enum DIRECT_BUTTON
	{
		DB_LEFT, DB_RIGHT, DB_END
	};
protected:
	explicit CCheckPointInterface(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CCheckPointInterface(const CCheckPointInterface& _rhs);
	virtual ~CCheckPointInterface() = default;
public:
	const _bool& Is_PadeOutPm() const { return m_bPadeOutPM; }
	void Set_PadeOutPM(const _bool& _bIsPadeOutValue) { this->m_bPadeOutPM = _bIsPadeOutValue; }
	const _bool& Is_PadeOutState() const { return m_bPadeOutState; }
	void Set_PadeOutState(const _bool& _bIsPadeOutState) { this->m_bPadeOutState = _bIsPadeOutState; }

	class CCheckPoints* Get_CheckPoints();
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
protected:
	// Set Up Component
	HRESULT SetUp_ConstantValue();
	// Ready Component
	HRESULT Ready_Component();
	// Ready Others
	HRESULT Ready_Others();
private:
	// Move To Value
	void MoveToValue();
	// Pade
	void PadeOutState();
protected:
	// MainTexture
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRenderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CVIBuffer_Rect* m_pVIRectCom = nullptr;
private:
	vector< CMoveToCheckPointsImg*>	m_MoveToCheckPoints;
	CButton*												m_pButton[DB_END] = { nullptr };
	// Active Index
	_uint														m_iActiveIndex = 0;
	_uint														m_iActiveEndIndex = 5;
	_float2													m_v2ActivePosArr[7] = {};
	_bool														m_bMoveState = false;
	_bool														m_bPadeOutState = false;
	_bool														m_bPadeOutPM = false;
	_byte														m_byJudgeDirection = 0;
	_uint														m_iMoveIndex = 0;
public:
	static CCheckPointInterface* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END