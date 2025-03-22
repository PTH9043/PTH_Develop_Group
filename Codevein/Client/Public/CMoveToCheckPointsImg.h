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
class CCheckPoints;

class CMoveToCheckPointsImg final : public CUserInterface
{
public:
	typedef struct tagMoveToCheckPointsImg
	{
		UIDESC	tUIDesc;
		_uint		iIndex = 0;
		_uint		iInitActivePass = 0;
	}MOVETOCHECKPOINTIMG;
protected:
	explicit CMoveToCheckPointsImg(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CMoveToCheckPointsImg(const CMoveToCheckPointsImg& _rhs);
	virtual ~CMoveToCheckPointsImg() = default;
public:
	void Set_MoveToPoint(const _float2& _v2MoveToPoint) { this->m_v2MoveToPoint = _v2MoveToPoint; }
	CCheckPoints* Get_CheckPoints() { return m_pCheckPoints; }

	void Set_InitActivePass(const _uint& _iInitActivePass) { this->m_iActivePass = _iInitActivePass; }
	const _uint& Get_ActivePass() const { return m_iActivePass; }

	void Set_ActvieFont(const _bool& _bIsActiveFont) { this->m_bActiveFont = _bIsActiveFont; }
	void Set_MovePoint(const _bool& _bIsMovePoint) { this->m_bMovePoint = _bIsMovePoint; }
	const _bool& Is_MovePoint() const { return m_bMovePoint; }

	const _uint& Get_MoveIndex() const { return m_iMoveIndex; }
	void Set_MoveIndex(const _uint& _iMoveIndex) { this->m_iMoveIndex = _iMoveIndex; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
protected:
	// Constant Table
	HRESULT SetUp_ConstantTable();
	// Ready Component
	HRESULT Ready_Component();
	// Ready CheckPoints
	HRESULT Find_CheckPoints();
private:
	void MoveToNextPoint(const _double& _dTimeDelta);
protected:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*				m_pRenderCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBuffer_Rect*		m_pVIRectCom = nullptr;
protected:
	CCheckPoints*		m_pCheckPoints = nullptr;
	_float2						m_v2MoveToPoint = _float2(0.f, 0.f);
	_uint							m_iIndex = 0;
	_uint							m_iActivePass = 0;
	_uint							m_iMoveIndex = 0;
protected:
	_bool							m_bActiveFont = false;
	_bool							m_bMovePoint = false;
public:
	static CMoveToCheckPointsImg* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END