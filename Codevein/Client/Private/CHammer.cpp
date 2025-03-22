#include "CMainView.h"
#include "CImGui_Manager.h"
#include "CHammer.h"
#include "CGameInstance.h"

CHammer::CHammer(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CWeapon(_pDevice, _pContext)
{
    m_eWeaponType = WEAPON_TWOHAND;
}

CHammer::CHammer(const CHammer& _rhs)
    : CWeapon(_rhs)
{
}

HRESULT CHammer::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CHammer::NativeConstruct_Clone(void* pArg)
{
    if (nullptr != pArg)
    {
        m_tHammerDesc = *static_cast<HAMMERDESC*>(pArg);
        m_tItemDesc.strModelName = m_tHammerDesc.tItemDesc.strModelName;
        // Find HierackyNode
        Find_HierackyNode(m_tHammerDesc.tEquipDesc.pOwnerObject, m_tHammerDesc.tEquipDesc.strAttachNodeName);
    }

    if (FAILED(Ready_Component()))
        return E_FAIL;

    Setting_WeaponPivotPos(XMLoadFloat3(&m_tHammerDesc.tWeaponDesc.v3PivotColliderPos),
        XMLoadFloat3(&m_tHammerDesc.tWeaponDesc.v3PivotColliderSize));

    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;

    return S_OK;
}

void CHammer::Tick(const _double& _dTimeDelta)
{
    __super::Tick(_dTimeDelta);
}

void CHammer::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);
}

HRESULT CHammer::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CHammer::Ready_Component()
{
    if (FAILED(__super::Ready_Component()))
        return E_FAIL;
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(COM_MODEL, g_iCurrentLevel,
        m_tHammerDesc.tItemDesc.strModelName.c_str(), (CComponent**)&m_pModelCom)))
    {
        return E_FAIL;
    }
    return S_OK;
}

HRESULT CHammer::SetUp_ConstantTable()
{
    if (FAILED(__super::SetUp_ConstantTable()))
        return E_FAIL;

    return S_OK;
}
#pragma region USE_TOOL_METHOD
_bool CHammer::Extern_EditValue()
{
    return _bool();
}

_bool CHammer::Extern_HierakeyView(const string& _str)
{
    return __super::Extern_HierakeyView(_str);
}

HRESULT CHammer::Create_Self(CGameObject** _ppOut)
{
    return E_NOTIMPL;
}

HRESULT CHammer::Delete_Self()
{
    return E_NOTIMPL;
}

HRESULT CHammer::Save_Data()
{
    return E_NOTIMPL;
}

HRESULT CHammer::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
    return E_NOTIMPL;
}

_bool CHammer::Is_Picking(_float3* _pOut)
{
    return _bool();
}

void CHammer::Reset_Data()
{
}
#pragma endregion USE_TOOL_METHOD
CHammer* CHammer::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
    CHammer* pInstance = new CHammer(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CHammer::Create To Failed");
    }
    return pInstance;
}

CGameObject* CHammer::Clone(void* pArg)
{
    CHammer* pInstance = new CHammer(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CHammer::Create To Failed");
    }
    return pInstance;
}

void CHammer::Free()
{
    __super::Free();
}
