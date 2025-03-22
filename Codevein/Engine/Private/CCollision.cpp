#include "CCollision.h"
#include "CTransform.h"

CCollision::CCollision(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CCollision::CCollision(const CCollision& _rhs)
	: CComponent(_rhs)
{
}

HRESULT CCollision::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CCollision::NativeConstruct_Clone(void* _pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(_pArg)))
		return E_FAIL;

	if (nullptr != _pArg)
	{
		COLLDESC tCollDesc = *static_cast<COLLDESC*>(_pArg);
		m_pTransform = tCollDesc.pTransform;
		m_v3Pos = tCollDesc.v3Pos;
		m_v3Size = tCollDesc.v3Size;

		Safe_AddRef(m_pTransform);
	}

	return S_OK;
}

void CCollision::Free()
{
	Safe_Release(m_pTransform);
	__super::Free();
}
