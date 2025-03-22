#include "CMainView.h"
#include "CImGui_Manager.h"
#include "CEquipment.h"
#include "CGameInstance.h"
#include "CHierarchyNode.h"

CEquipment::CEquipment(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CItem(_pDevice, _pContext)
{
    m_eItemType = ITEM_EQUIP;
    XMStoreFloat4x4(&m_SocketMatrix, XMMatrixIdentity());
}

CEquipment::CEquipment(const CEquipment& _rhs)
    : CItem(_rhs),
    m_eEquipType(_rhs.m_eEquipType),
    m_SocketMatrix(_rhs.m_SocketMatrix)
{
}


HRESULT CEquipment::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;
    return S_OK;
}

HRESULT CEquipment::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;
    return S_OK;
}

void CEquipment::Tick(const _double& _dTimeDelta)
{
    __super::Tick(_dTimeDelta);
}

void CEquipment::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);
}

HRESULT CEquipment::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;
    return S_OK;
}

HRESULT CEquipment::Find_HierackyNode(CGameObject* _pGameObject, const string& _str)
{
    return S_OK;
}

CEquipment* CEquipment::ReturnSettingToValue(CCharacter* _pCharacter)
{
    return nullptr;
}

void CEquipment::Set_Model_Pivot(_fmatrix matPivot)
{
    if (nullptr != m_pModelCom)
    {
        m_pModelCom->Set_PivotTransform(matPivot);
    }
}

HRESULT CEquipment::Ready_Component()
{
    if (FAILED(__super::Ready_Component()))
        return E_FAIL;
    return S_OK;
}
HRESULT CEquipment::SetUp_ConstantTable()
{
    if (FAILED(__super::SetUp_ConstantTable()))
        return E_FAIL;
    return S_OK;
}

HRESULT CEquipment::Receive_ModelFiles()
{
    if (nullptr == m_pModelCom)
        return E_FAIL;

    m_pFileGroup = m_pModelCom->Get_ModelFileGroup();
    m_pFileData = m_pModelCom->Get_FileData();

    return S_OK;
}

#pragma region USE_TOOL_METHOD
_bool CEquipment::Extern_EditValue()
{
    return _bool();
}

_bool CEquipment::Extern_HierakeyView(const string& _str)
{
    return _bool();
}

HRESULT CEquipment::Create_Self(CGameObject** _ppOut)
{
    return E_NOTIMPL;
}

HRESULT CEquipment::Delete_Self()
{
    return E_NOTIMPL;
}

HRESULT CEquipment::Save_Data()
{
    return E_NOTIMPL;
}

HRESULT CEquipment::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
    return E_NOTIMPL;
}

_bool CEquipment::Is_Picking(_float3* _pOut)
{
    return _bool();
}

void CEquipment::Reset_Data()
{

}
#pragma endregion USE_TOOL_METHOD


void CEquipment::Free()
{
    __super::Free();
}
