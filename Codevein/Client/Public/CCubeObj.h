#pragma once
#include"Client_Defines.h"
#include "CGameObject.h"

BEGIN(Engine)
class CCollider;
class CTransform;
class CVIBuffer_Cube;
class CRenderer;
END

BEGIN(Client)

class CCubeObj final : public CBase
{
protected:
	explicit CCubeObj(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CCubeObj() = default;
public:
	void Set_Color(const _vector& _v4Color);
	const _uint& Get_Index() const { return m_iIndex; }

	void Set_Position(const _vector& _vPos);
	CTransform* Get_Transform() { return m_pTransform; }
public:
	HRESULT NativeConstruct(const _vector& _vPosition, const _uint& _iIndex);
#ifdef _DEBUG
	HRESULT Render();
	HRESULT Add_Render(CRenderer* _pRenderer);
#endif
	_bool Is_Picking(_float3* _v3Pos);
protected:

protected:
#ifdef _DEBUG
	CCollider*						m_pCollider = nullptr;
#endif
	CVIBuffer_Cube*		m_pVIBufferCube = nullptr;
	CTransform*				m_pTransform = nullptr;
	_uint								m_iIndex = 0;
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
public:
	static CCubeObj* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _vector& _vPosition, const _uint& _iIndex);
	virtual void Free() override;
};

END