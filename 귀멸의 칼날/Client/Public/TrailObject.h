#pragma once
#include "Pawn.h"

BEGIN(Engine)
class CModel;
class CHierarchyNode;
class CTransform;
class CVIBuffer_Trail;
class CTexture;
END

BEGIN(Client)

class CTrailObject : public CPawn
{
public:
	typedef struct tagTrailObject
	{
		tagTrailObject() : pTargetModel(nullptr), pTargetTrasform(nullptr),
			strStartBoneName(""), strFinishBoneName("")
		{
			XMStoreFloat4x4(&StartMatrix, XMMatrixIdentity());
			XMStoreFloat4x4(&EndMatrix, XMMatrixIdentity());
			XMStoreFloat4x4(&PivotMatrix, XMMatrixIdentity());
		}

		CModel*				pTargetModel = nullptr;
		CTransform*		pTargetTrasform = nullptr;
		_string					strStartBoneName = "";
		_string					strFinishBoneName = "";
		// Bone Matrix
		_float4x4				StartMatrix = _float4x4();
		_float4x4				EndMatrix = _float4x4();
		_float4x4				PivotMatrix = _float4x4();
	}TRAILOBJ;
protected:
	explicit CTrailObject(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CTrailObject(const CTrailObject& _rhs);
	virtual ~CTrailObject() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT	RenderLight() override;

	virtual void Extern_ImGuiClass() override;

	void Change_Texture(const _wstring& _strChangeTexture);
	void Return_Texture();

	void Update_TrailMatrix(const _float& _fStartValue, const _float& _fEndValue);
protected:
	// Set Up Constant
	virtual HRESULT SetUp_ConstantTable() override;
	// SetUp Components
	virtual HRESULT SetUp_Components() override;
	// SetUp Bones
	HRESULT SetUp_Bones(void* _pData);
protected:
	// Update Transform
	void Update_Transform();
protected:
	CHierarchyNode*		m_pStartBone = nullptr;
	CHierarchyNode*		m_pEndBone = nullptr;
	CModel*						m_pTargetModel = nullptr;
	CTransform*				m_pTargetTransform = nullptr;
	CVIBuffer_Trail*			m_pVIBuffer_Trail = nullptr;
	CTexture*						m_pNoiseTextureCom = nullptr;
	CTexture*						m_pTrailTextureCom = nullptr;
	// Bone Matrix
	_float4x4						m_StartMatrix;
	_float4x4						m_EndMatrix;

	_float								m_fDeltaTime = 0.f;

	_wstring						m_strTrailTextureName = L"T_e_Plc_P1002_Atk_Trail001_C";
	_wstring						m_strChangeTextureName = L"";
public:
	static  CTrailObject* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END