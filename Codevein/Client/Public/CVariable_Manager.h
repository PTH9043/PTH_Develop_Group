#pragma once
#include "CManager.h"

BEGIN(Engine)
class CCollider;
class CRenderer;
END

BEGIN(Client)

class CVariable_Manager : public CManager
{
public:
	typedef struct tagAnimFastDesc
	{
		_float			fStartFastSection = 0.f;
		_float			fEndFastSection = 0.f;
		_float			fFastValue = 0.f;
	}ANIMFASTVALUE;
	typedef struct tagAnimClipDesc
	{
		_float			fChange = 0.f;
		_float			fEndValue = 0.f;
		_char			pName[MAX_PATH] = "";
	}ANIMCLIPVALUE;
protected:
	explicit CVariable_Manager();
	virtual ~CVariable_Manager() = default;
public:
	const ANIMFASTVALUE& Get_AnimFastValue() const { return m_tAnimFastValue; }
	void Set_AnimFastValue(const ANIMFASTVALUE& _tAnimFastValue) { this->m_tAnimFastValue = _tAnimFastValue; }

	const ANIMCLIPVALUE& Get_AnimClipValue() const { return m_tAnimClipValue; }
	void Set_AnimClipValue(const ANIMCLIPVALUE& _tAnimClipValue) { this->m_tAnimClipValue = _tAnimClipValue; }

#ifdef _DEBUG
	void Initialize_Debug();

	const _bool& Is_FindOffsetMatrix() const { return m_bFindOffsetMatrix; }
	void Set_FindOffsetMatrix(const _bool& _bIsFindOffsetMatrix) { m_bFindOffsetMatrix = _bIsFindOffsetMatrix; }

	void Set_TestColliderOffsetMatrix(_matrix _matWorld) { XMStoreFloat4x4(&m_fSaveOffsetMatrix, _matWorld); }

	void Control_Collider();

	void Render_TestColliderOffsetMatrix(CRenderer* _pRenderer);

	CCollider* Get_OBBCollider() { return m_pTestOBBCollider; }
	CCollider* Get_CircleCollider() { return m_pTestCircleCollider; }

	const _string& Get_NodeName() const { return m_strNodeName; }
	void Set_NodeName(const string& _strNodeName) { this->m_strNodeName = _strNodeName; }

	const _uint& Get_ColliderType() const { return m_iColliderType; }
#endif
public:
	// NativeConstruct
	HRESULT NativeConstruct(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
private:
	ANIMFASTVALUE	m_tAnimFastValue;
	ANIMCLIPVALUE		m_tAnimClipValue;

private:
#ifdef _DEBUG
	// Test Collider
	CCollider*					m_pTestOBBCollider = nullptr;
	CCollider*					m_pTestCircleCollider = nullptr;
	_float4x4					m_fSaveOffsetMatrix;
	_bool							m_bFindOffsetMatrix = false;
	_uint							m_iColliderType = 0;
	_string						m_strNodeName = "";
#endif
public:
	static CVariable_Manager* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END