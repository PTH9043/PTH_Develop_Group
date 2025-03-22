#include "CCheckPointInterface.h"
#include "CGameInstance.h"
#include "CButton.h"
#include "CMoveToCheckPointsImg.h"
#include "CCheckPoints.h"

CCheckPointInterface::CCheckPointInterface(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CUserInterface(_pDevice, _pContext)
{
}

CCheckPointInterface::CCheckPointInterface(const CCheckPointInterface& _rhs)
    : CUserInterface(_rhs)
{
}

CCheckPoints* CCheckPointInterface::Get_CheckPoints()
{
    CCheckPoints* pInstnace = nullptr;
    for (auto& iter : m_MoveToCheckPoints)
    {
        if (3 == iter->Get_MoveIndex())
        {
            pInstnace = iter->Get_CheckPoints();
        }
    }
    return pInstnace;
}

HRESULT CCheckPointInterface::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCheckPointInterface::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;

    if (nullptr != pArg)
    {
        m_tUIDesc = *static_cast<UIDESC*>(pArg);
    }

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_Others()))
        return E_FAIL;
    _uint iValue = static_cast<_uint>(m_MoveToCheckPoints.size());
    m_iActiveEndIndex = iValue;
    return S_OK;
}

void CCheckPointInterface::Tick(const _double& _dTimeDelta)
{
    if (false == m_bActive)
        return;

    __super::Tick(_dTimeDelta);

    for (_uint i = m_iActiveIndex; i < m_iActiveEndIndex; ++i)
    {
        m_MoveToCheckPoints[i]->Set_Active(true);
    }

    MoveToValue();
    PadeOutState();
}

void CCheckPointInterface::Late_Tick(const _double& _dTimeDelta)
{
    if (false == m_bActive)
    {
        for (auto& iter : m_MoveToCheckPoints)
            iter->Set_Active(false);
        return;
    }


    __super::Late_Tick(_dTimeDelta);

    if (nullptr != m_pRenderCom)
        m_pRenderCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CCheckPointInterface::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (FAILED(SetUp_ConstantValue()))
        return E_FAIL;

    m_pVIRectCom->Render();
    return S_OK;
}

HRESULT CCheckPointInterface::SetUp_ConstantValue()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (nullptr == m_pRectTransformCom || nullptr == m_pTransformCom || nullptr == m_pTextureCom)
        return E_FAIL;

    _float4x4 ViewMatrix;
    XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

    if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_pRectTransformCom->Get_RectDesc().ProjMatrix, sizeof(_float4x4))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom,
        "g_DiffuseTexture", m_tUIDesc.strImgName.c_str())))
        return E_FAIL;

    m_pShaderCom->Begin(4);

    return S_OK;
}

HRESULT CCheckPointInterface::Ready_Component()
{
    if (FAILED(__super::Ready_Transform(COM_TRANSFORM)))
        return E_FAIL;

    {
        CRectTransform::RECTDESC tRectDesc;
        tRectDesc.v2Pos = m_tUIDesc.v2Pos;
        tRectDesc.v2Size = m_tUIDesc.v2Size;
        XMStoreFloat4x4(&tRectDesc.ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
        if (FAILED(__super::Ready_RectTransform(COM_RECTRANSFORM, &tRectDesc)))
            return E_FAIL;

        m_pRectTransformCom->Update_Rect(m_pTransformCom, g_iWinSizeX, g_iWinSizeY, m_tUIDesc.fZBufferOrder);
    }

    if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXTEX, (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRenderCom)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(COM_VIBUFFER, LEVEL_STATIC, PROTO_COMP_VIBUFFER_RECT, (CComponent**)&m_pVIRectCom)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(COM_TEXTURE, LEVEL_STATIC, PROTO_COMP_TEXTURE_UIGROUP, (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCheckPointInterface::Ready_Others()
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        // Move To CheckPoint
        {
            CMoveToCheckPointsImg::MOVETOCHECKPOINTIMG tDesc;
            tDesc.iIndex = 0;
            tDesc.iInitActivePass = 0;
            tDesc.tUIDesc = UIDESC(L"시작", _float2((_float)g_iWinSizeX / 2.f, 150.f), _float2(300.f, 200.f), 0.1f);
            CMoveToCheckPointsImg* pInstance = static_cast<CMoveToCheckPointsImg*>
                (pGameInstance->Clone_GameObject_Add(g_iCurrentLevel, LAYER_UI, PROTO_GOB_UI_MOVETOCHECKPOINTIMG, &tDesc));
            Safe_AddRef(pInstance);
            m_MoveToCheckPoints.push_back(pInstance);
        }
        // Arrow
        {
            CMoveToCheckPointsImg::MOVETOCHECKPOINTIMG tDesc;
            tDesc.iIndex = 1;
            tDesc.iInitActivePass = 3;
            tDesc.tUIDesc = UIDESC(L"중간", _float2((_float)g_iWinSizeX / 2.f + 310.f, 150.f), _float2(300.f, 200.f), 0.1f);
            CMoveToCheckPointsImg* pInstance = static_cast<CMoveToCheckPointsImg*>
                (pGameInstance->Clone_GameObject_Add(g_iCurrentLevel, LAYER_UI, PROTO_GOB_UI_MOVETOCHECKPOINTIMG, &tDesc));
            Safe_AddRef(pInstance);
            m_MoveToCheckPoints.push_back(pInstance);
        }
        // Arrow
        {
            CMoveToCheckPointsImg::MOVETOCHECKPOINTIMG tDesc;
            tDesc.iIndex = 2;
            tDesc.iInitActivePass = 3;
            tDesc.tUIDesc = UIDESC(L"문앞", _float2((_float)g_iWinSizeX / 2.f + 620.f, 150.f), _float2(300.f, 200.f), 0.1f);
            CMoveToCheckPointsImg* pInstance = static_cast<CMoveToCheckPointsImg*>
                (pGameInstance->Clone_GameObject_Add(g_iCurrentLevel, LAYER_UI, PROTO_GOB_UI_MOVETOCHECKPOINTIMG, &tDesc));
            Safe_AddRef(pInstance);
            m_MoveToCheckPoints.push_back(pInstance);
        }
    }
    END_SINGLETON(pGameInstance);

    _uint iIndex = 3;
    for (auto& iter : m_MoveToCheckPoints)
    {
        iter->Set_MoveIndex(iIndex++);
    }
    m_v2ActivePosArr[0] = _float2((_float)g_iWinSizeX / 2.f - 920.f, 150.f);
    m_v2ActivePosArr[1] = _float2((_float)g_iWinSizeX / 2.f - 620.f, 150.f);
    m_v2ActivePosArr[2] = _float2((_float)g_iWinSizeX / 2.f - 310.f, 150.f);
    m_v2ActivePosArr[3] = _float2((_float)g_iWinSizeX / 2.f, 150.f);
    m_v2ActivePosArr[4] = _float2((_float)g_iWinSizeX / 2.f + 310.f, 150.f);
    m_v2ActivePosArr[5] = _float2((_float)g_iWinSizeX / 2.f + 620.f, 150.f);
    m_v2ActivePosArr[6] = _float2((_float)g_iWinSizeX / 2.f + 920.f, 150.f);
    return S_OK;
}

void CCheckPointInterface::MoveToValue()
{
    if (true == m_bMoveState)
    {
        _bool bIsTrue = true;
        for (_uint i = m_iActiveIndex; i < m_iActiveEndIndex; ++i)
        {
            if (true == m_MoveToCheckPoints[i]->Is_MovePoint())
                bIsTrue = false;
        }
        if (true == bIsTrue)
        {
            m_bMoveState = false;

            for (_uint i = m_iActiveIndex; i < m_iActiveEndIndex; ++i)
            {
                if (3 == m_MoveToCheckPoints[i]->Get_MoveIndex())
                    m_MoveToCheckPoints[i]->Set_InitActivePass(0);
                else
                    m_MoveToCheckPoints[i]->Set_InitActivePass(3);
            }

            if (1 == m_byJudgeDirection)
            {
                if (m_iMoveIndex >= 2)
                {
                    if(m_iMoveIndex < static_cast<_uint>(m_MoveToCheckPoints.size()) - 1)
                    {
                        m_iActiveIndex += (_uint)m_byJudgeDirection;
                        m_iActiveEndIndex += (_uint)m_byJudgeDirection;
                    }
                }
            }
            else
            {
                if (m_iMoveIndex > 0)
                {
                    if (m_iActiveIndex > 0)
                    {
                        m_iActiveIndex += (_uint)m_byJudgeDirection;
                        m_iActiveEndIndex += (_uint)m_byJudgeDirection;
                    }
                }
            }
        }
    }
    else
    {
        BEGIN_SINGLETON(CGameInstance, pGameInstance);
        {
            _bool bKeyLeft = pGameInstance->Get_DIKeyDown(DIK_LEFT), bKeyRight = pGameInstance->Get_DIKeyDown(DIK_RIGHT);
            if (true == bKeyLeft)
            {
                _uint iValue = static_cast<_uint>(m_MoveToCheckPoints.size());
                if (iValue - 1 > m_iMoveIndex)
                {
                    for (_uint i = m_iActiveIndex; i < m_iActiveEndIndex; ++i)
                    {
                        _uint iIndex = m_MoveToCheckPoints[i]->Get_MoveIndex() - 1;
                        m_MoveToCheckPoints[i]->Set_MoveToPoint(m_v2ActivePosArr[iIndex]);
                        m_MoveToCheckPoints[i]->Set_MoveIndex(iIndex);
                        m_MoveToCheckPoints[i]->Set_MovePoint(true);
                    }
                    m_byJudgeDirection = 1;
                    m_bMoveState = true;
                
                    ++m_iMoveIndex;
                }
            }
            else if (true == bKeyRight)
            {
                _uint iValue = static_cast<_uint>(m_MoveToCheckPoints.size());
                if (0 < m_iMoveIndex)
                {
                    for (_uint i = m_iActiveIndex; i < m_iActiveEndIndex; ++i)
                    {
                        _uint iIndex = m_MoveToCheckPoints[i]->Get_MoveIndex() + 1;
                        m_MoveToCheckPoints[i]->Set_MoveToPoint(m_v2ActivePosArr[iIndex]);
                        m_MoveToCheckPoints[i]->Set_MoveIndex(iIndex);
                        m_MoveToCheckPoints[i]->Set_MovePoint(true);
                    }
                    m_byJudgeDirection = -1;
                    m_bMoveState = true;

                    --m_iMoveIndex;
                }
            }
        }
        END_SINGLETON(pGameInstance);
    }
}

void CCheckPointInterface::PadeOutState()
{
    if (false == m_bPadeOutState && false == m_bMoveState)
    {
        BEGIN_SINGLETON(CGameInstance, pGameInstance)
        {
            if (true == pGameInstance->Get_DIKeyDown(DIK_SPACE))
            {
                m_bPadeOutState = true;
            }
        }
        END_SINGLETON(pGameInstance);
    }
    else
    {
        if (true == m_bPadeOutState)
        {
            for (_uint i = m_iActiveIndex; i < m_iActiveEndIndex; ++i)
            {
                m_MoveToCheckPoints[i]->Set_Active(false);
            }

            if (false == m_bPadeOutPM)
            {
                m_pRenderCom->Plus_PadeOutValue(0.01f);
                if (1 == m_pRenderCom->Get_PadeOutValue())
                {
                    m_bPadeOutPM = true;
                }
            }
            else
            {
                m_pRenderCom->Plus_PadeOutValue(-0.01f);
            }
        }
    }
}

CCheckPointInterface* CCheckPointInterface::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
    CCheckPointInterface* pInstance = new CCheckPointInterface(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CCheckPointInterface::Create To Failed");
    }
    return pInstance;
}

CGameObject* CCheckPointInterface::Clone(void* _pArg)
{
    CCheckPointInterface* pInstance = new CCheckPointInterface(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CCheckPointInterface::Clone To Failed");
    }
    return pInstance;
}

void CCheckPointInterface::Free()
{
    for (auto& iter : m_MoveToCheckPoints)
        Safe_Release(iter);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIRectCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pRenderCom);

    __super::Free();
}
