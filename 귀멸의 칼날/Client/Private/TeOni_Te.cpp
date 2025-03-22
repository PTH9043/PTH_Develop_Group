#include "TeOni_Te.h"
#include "GameInstance.h"
#include "Camera_Manager.h"
#include "Camera_Free.h"

TeOni_Te::TeOni_Te(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CItem(_pDevice, _pContext)
{
}

TeOni_Te::TeOni_Te(const CItem& _rhs)
	: CItem(_rhs)
{
}

HRESULT TeOni_Te::NativeConstruct_Prototype(void)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_eItemType = ITEM_BULLET;
	return S_OK;
}

HRESULT TeOni_Te::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pTeOni = pGameInstance->Find_GameObject(g_iCurrentLevel, LAYER_MONSTER, 0);
	if (nullptr != pTeOni)
		m_pOniTransform = pTeOni->Get_Transform();
	Safe_AddRef(m_pOniTransform);

	_int iRanx = pGameInstance->Get_RandomCount() % 10;
	if (0 == iRanx % 2)
		iRanx *= -1;
	_int iRany = pGameInstance->Get_RandomCount() % 10;
	if (0 == iRany % 2)
		iRany *= -1;

	m_vDir = XMVectorSet((_float)iRanx, 0.f, (_float)iRany, 0.f);
	RELEASE_INSTANCE(CGameInstance);

	_vector vPos = m_pOniTransform->Get_Pos();
	_vector vRight = XMVector3Normalize(m_pOniTransform->Get_Right());
	_vector vUp = XMVector3Normalize(m_pOniTransform->Get_Up());

	m_pTransformCom->Set_Pos(vPos + vUp * 40.f);
	m_pTransformCom->Rotate_Fix(m_pTransformCom->Get_Look(), 10.f * (float)iRanx);

	return S_OK;
}

void TeOni_Te::Tick(const _double& _dTimeDelta)
{
	//if (false == m_bDirCheck)
	//{
	//	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	//	pGameInstance->Play_SoundW(TEXT("TEONI_HIT1_T"), 0.6f);
	//	RELEASE_INSTANCE(CGameInstance);

	//	m_bDirCheck = true;
	//}

	m_dDeleteTime += _dTimeDelta;

	m_pTransformCom->Rotate_Turn(m_pTransformCom->Get_Right(), _dTimeDelta, 15.f);
	m_pTransformCom->Set_Scale(_float3{ 0.5f, 0.5f, 0.5f });

	if(2.f <= m_dDeleteTime)
	{
		Delete_Self();
	}

	_float fJumpLength = 30.f; 
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_uint iRan = pGameInstance->Get_RandomCount() % 5;
	RELEASE_INSTANCE(CGameInstance);
	
	_vector vMovePos = Compute_MovePos_ByDist(_dTimeDelta, XMVector3Normalize(m_vDir), fJumpLength + (_float)iRan, 15.f + (_float)iRan, 0.4f, 1.f);
	_vector vNewPos = m_pTransformCom->Get_Pos() + vMovePos;
	m_pTransformCom->Set_Pos(vNewPos);

	__super::Tick(_dTimeDelta);
}

void TeOni_Te::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT TeOni_Te::Render(void)
{
	//if (FAILED(__super::Render()))
	//	return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (nullptr != m_pModelCom)
	{
		_uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();

		for (_uint i = 0; i < iNumMeshContainers; ++i)
		{
			if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE)))
				return E_FAIL;

			m_pModelCom->Render_NonAnim(i, m_pShaderCom, 12);
		}
	}

	return S_OK;
}

HRESULT TeOni_Te::RenderLight(void)
{
	if (FAILED(__super::RenderLight()))
		return E_FAIL;

	return S_OK;
}

HRESULT TeOni_Te::SetUp_ConstantTable(void)
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	_float4 edgecolor = _float4(1.f, 0.f, 0.f, 0.f);   
	if (FAILED(m_pShaderCom->Set_RawValue("g_vEdgeGlowColor", &edgecolor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT TeOni_Te::SetUp_Components(void)
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

	// 모델 세팅 
	if (FAILED(Add_Component(COM_MODEL, g_iCurrentLevel,
		PROTO_COMP_MODEL_ITEM_TEONITE, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT TeOni_Te::SetUp_LoadData(void* _pArg)
{
	return S_OK;
}

HRESULT TeOni_Te::Delete_Self(void)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_ITEM, this);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

_vector TeOni_Te::Compute_MovePos_ByDist(const _double& _dTimeDelta, _vector vMoveDir, _float fDist, _float fMaxHeight, _float fMaxHeightRatio, _float fPlayTime)
{
	// 최대 이동할 거리를 이용하여 총 플레이타임과의 계산을 통해 프레임당 이동거리를 계산한다.
	if (0.f == fPlayTime)
		return XMVectorZero();

	// 거리에 따른 프레임당 이동거리
	_vector vMovePos = XMVector3Normalize(vMoveDir) * (fDist / fPlayTime) * _float(_dTimeDelta);
	vMovePos = XMVectorSetY(vMovePos, 0.f);

	
	_float fGravity = 0.f;
	_float fHeight = 0.f;

	// 총 재생시간 * 최대 높이 도달하는 비율값이 0일때의 중력적용값
	if (fMaxHeightRatio * fPlayTime * fMaxHeight == 0.f)
	{
		fGravity = 400.f;
		fHeight = 0.f;
	}
	else
	{
		fGravity = 2 * fMaxHeight / pow(fMaxHeightRatio * fPlayTime, 2.f);
		fHeight = fGravity * fPlayTime * fMaxHeightRatio;
	}
	// FallTimeAcc값을 갖고 프레임당 Y값을 생성한다.
	_float fY = (fHeight - fGravity * (_float)m_dFallTimeAcc) * _float(_dTimeDelta);
	vMovePos = XMVectorSetY(vMovePos, fY);

	m_dFallTimeAcc += _dTimeDelta;

	return vMovePos;
}

TeOni_Te* TeOni_Te::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	TeOni_Te* pInstance = new TeOni_Te(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("TeOni_Te::Create To Failed");
#endif
	}
	return pInstance;
}

CGameObject* TeOni_Te::Clone(void* pArg)
{
	TeOni_Te* pInstance = new TeOni_Te(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("TeOni_Te::Clone To Failed");
#endif
	}
	return pInstance;
}

void TeOni_Te::Free(void)
{
	Safe_Release(m_pOniTransform);
	__super::Free();
}
