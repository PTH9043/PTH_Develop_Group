#include "CMoveToCheckPointsImg.h"
#include "CGameInstance.h"
#include "CCheckPoints.h"

CMoveToCheckPointsImg::CMoveToCheckPointsImg(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CUserInterface(_pDevice, _pContext)
{
}

CMoveToCheckPointsImg::CMoveToCheckPointsImg(const CMoveToCheckPointsImg& _rhs)
    : CUserInterface(_rhs)
{
}

HRESULT CMoveToCheckPointsImg::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;
    return S_OK;
}

HRESULT CMoveToCheckPointsImg::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;

    if (nullptr != pArg)
    {
        MOVETOCHECKPOINTIMG tMovePointCheckDesc = *static_cast<MOVETOCHECKPOINTIMG*>(pArg);
        m_tUIDesc = tMovePointCheckDesc.tUIDesc;
        m_iIndex = tMovePointCheckDesc.iIndex;
        m_iActivePass = tMovePointCheckDesc.iInitActivePass;
    }

    if (FAILED(Ready_Component()))
        return E_FAIL;

    return S_OK;
}

void CMoveToCheckPointsImg::Tick(const _double& _dTimeDelta)
{
    __super::Tick(_dTimeDelta);

    Find_CheckPoints();
    MoveToNextPoint(_dTimeDelta);
    m_pRectTransformCom->Update_Rect(m_pTransformCom, g_iWinSizeX, g_iWinSizeY, m_tUIDesc.fZBufferOrder);
}

void CMoveToCheckPointsImg::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);

    if (false == m_bActive)
        return;

    if (nullptr != m_pRenderCom)
        m_pRenderCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMoveToCheckPointsImg::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (FAILED(SetUp_ConstantTable()))
        return E_FAIL;

    m_pVIRectCom->Render();
    return S_OK;
}

HRESULT CMoveToCheckPointsImg::SetUp_ConstantTable()
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

    m_pShaderCom->Begin(m_iActivePass);
    return S_OK;
}

HRESULT CMoveToCheckPointsImg::Ready_Component()
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

HRESULT CMoveToCheckPointsImg::Find_CheckPoints()
{
    if (nullptr == m_pCheckPoints)
    {
        BEGIN_SINGLETON(CGameInstance, pGameInstance);
        {
            m_pCheckPoints = static_cast<CCheckPoints*>(pGameInstance->Find_GameObject(g_iCurrentLevel, LAYER_CHECKPOINTS, m_iIndex));
        }
        END_SINGLETON(pGameInstance);
    }
    return S_OK;
}

void CMoveToCheckPointsImg::MoveToNextPoint(const _double& _dTimeDelta)
{
    if (true == m_bMovePoint)
    {
        _float2 v2Pos = m_pRectTransformCom->Get_RectDesc().v2Pos;
        _vector direction = XMLoadFloat2(&m_v2MoveToPoint) - XMLoadFloat2(&v2Pos);
        _float fLength = XMVectorGetX(XMVector2Length(XMLoadFloat2(&m_v2MoveToPoint) - XMLoadFloat2(&v2Pos)));
        if(1.f <= fLength)
        {
            _float2 v2Dir;
            XMStoreFloat2(&v2Dir, XMVector2Normalize(direction));
            v2Pos.x += (_float)(_dTimeDelta * 120.f * v2Dir.x);
            CRectTransform::RECTDESC tRect = m_pRectTransformCom->Get_RectDesc();
            tRect.v2Pos = v2Pos;
            m_pRectTransformCom->Set_RectDesc(tRect);
        }
        else
        {
            m_bMovePoint = false;
        }
    }
}

CMoveToCheckPointsImg* CMoveToCheckPointsImg::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
    CMoveToCheckPointsImg* pInstance = new CMoveToCheckPointsImg(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CMoveToCheckPointsImg::Create To Failed");
    }
    return pInstance;
}

CGameObject* CMoveToCheckPointsImg::Clone(void* _pArg)
{
    CMoveToCheckPointsImg* pInstance = new CMoveToCheckPointsImg(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CMoveToCheckPointsImg::Clone To Failed");
    }
    return pInstance;
}

void CMoveToCheckPointsImg::Free()
{
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIRectCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pRenderCom);

    __super::Free();
}
