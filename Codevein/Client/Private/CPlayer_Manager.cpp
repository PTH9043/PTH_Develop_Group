#include "CMainView.h"
#include "CImGui_Manager.h"
#include "CPlayer_Manager.h"
#include "CPlayer.h"
#include "CGameInstance.h"
#include "CCamera_Free.h"

CPlayer_Manager::CPlayer_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CManager(_pDevice, _pContext)
{
}

HRESULT CPlayer_Manager::NativeConstruct()
{
    return S_OK;
}

void CPlayer_Manager::Spawn_Player(const LEVEL& _eLevel, const _float3& _v3Pos)
{
    BEGIN_SINGLETON(CGameInstance , pGameInstance)
    {
        m_pPlayer = (CPlayer*)pGameInstance->Clone_GameObject_Add(_eLevel, LAYER_PLAYER, PROTO_GOB_PLAYER, &m_tPlayerDesc);
        Safe_AddRef(m_pPlayer);

        if (nullptr == m_pPlayer)
        {
            END_SINGLETON(pGameInstance);
            return;
        }

        CTransform* pTransform = static_cast<CTransform*>(m_pPlayer->Get_Component(COM_TRANSFORM));
        if (nullptr == pTransform)
        {
            END_SINGLETON(pGameInstance);
            return;
        }

        pTransform->Set_Pos(XMLoadFloat3(&_v3Pos));
    }
    END_SINGLETON(pGameInstance);
}

void CPlayer_Manager::Delete_Player(const LEVEL& _eLevel)
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        pGameInstance->Add_DeleteObject(_eLevel, LAYER_PLAYER, m_pPlayer);
        Safe_Release(m_pPlayer);
        m_pPlayer = nullptr;
    }
    END_SINGLETON(pGameInstance);
}

void CPlayer_Manager::Reset_Player(const _float3& _v3Pos)
{
    CTransform* pTransform = static_cast<CTransform*>(m_pPlayer->Get_Component(COM_TRANSFORM));
    if (nullptr == pTransform)
        return;
    // Set Position
    pTransform->Set_Pos(XMLoadFloat3(&_v3Pos));
    m_v3PlayerSpawnPos = _v3Pos;
}

HRESULT CPlayer_Manager::Save_Data(const _string& _strFolderPath)
{


    return S_OK;
}

HRESULT CPlayer_Manager::Load_Data(const _string& _strFolderPath)
{
    return E_NOTIMPL;
}

CPlayer_Manager* CPlayer_Manager::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPlayer_Manager* pInstance = new CPlayer_Manager(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CPlayer_Manager::Create To Failed");
    }
    return pInstance;
}

void CPlayer_Manager::Free()
{
    Safe_Release(m_pPlayer);
    __super::Free();
}
