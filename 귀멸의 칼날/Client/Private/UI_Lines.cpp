#include "UI_Lines.h"
#include "GameInstance.h"
#include "Battle_Manager.h"


CUI_Lines::CUI_Lines(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CUI_Base(_pDevice, _pContext)
{
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

CUI_Lines::CUI_Lines(const CUI_Lines& _rhs)
    : CUI_Base(_rhs)
{
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

HRESULT CUI_Lines::NativeConstruct_Prototype()
{
    return S_OK;
}

HRESULT CUI_Lines::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;

    if (FAILED(SetUp_Components()))
        return E_FAIL;

    m_iType = *(_int*)pArg;
	
    m_fAlpha = 0.f; //�ӽ�
    //Set_Size(_float2(1280.f, 720.f));
	
 
    /*if (m_iType == TEXTURE_LINES)
        Set_Pos(_float2(0.f, -50.f));
    
    else if (m_iType == TEXTURE_NAME)
        Set_Pos(_float2(-50.f, 0.f));*/

    if (g_iCurrentLevel == LEVEL_BATTLE)
    {
        TextIndex = 0;
        MaxText = 13;
        SoundTime = 1;
        SoundTotalTime = 7;
    }
    else
    {
        TextIndex = 15;
        MaxText = 27;
        SoundTime = 3;
        SoundTotalTime = 10;
    }
	
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

    m_PlayerName = new _tchar[30];

    m_Player2Name = new _tchar[30];

    m_pTransformCom->Set_Scale(_float3(1280.f, 720.f, 1.f));
    m_pTransformCom->Set_Pos(XMVectorSet(0.f, -60.f, 0.0f, 1.f));

    m_pNameTransform->Set_Scale(_float3(1280.f, 720.f, 1.f));
    m_pNameTransform->Set_Pos(XMVectorSet(-50.f, 0.f, 0.0f, 1.f));

    

    return S_OK;
}

void CUI_Lines::Tick(const _double& _dTimeDelta)
{
    __super::Tick(_dTimeDelta);
	
 

    Render_Time(_dTimeDelta);
    Find_Texture();
}

void CUI_Lines::Late_Tick(const _double& _dTimeDelta)
{
    if (nullptr == m_pRendererCom)
        return;
	
    if (m_fAlpha != 0.f)
    m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Lines::Render()
{
    if (nullptr == m_pVIBufferCom)
        return E_FAIL;

    if (FAILED(SetUp_ConstantTable()))
        return E_FAIL;
	
    m_pShaderCom->Begin(1);
    m_pVIBufferCom->Render();

    if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pNameTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
        return E_FAIL;
	
    if (FAILED(m_pTextureCom->SetUp_ConstantTable_Index(m_pShaderCom, "g_DiffuseTexture", PlayerIndex)))
        return E_FAIL;
	
    m_pShaderCom->Begin(1);
    m_pVIBufferCom->Render();

    return S_OK;
}

void CUI_Lines::Extern_ImGuiClass()
{
   

}

void CUI_Lines::Find_Texture()
{

        m_Lines_Texture = m_pTextureCom->Get_TextureNames();

        if (TextIndex < 10)
        {
            wsprintf(m_PlayerName, TEXT("0%d_Player"), TextIndex); // ���̸�
            wsprintf(m_Player2Name, TEXT("0%d_Player2"), TextIndex); //��� �̸�
        }
        else if (TextIndex >= 10)
        {
            wsprintf(m_PlayerName, TEXT("%d_Player"), TextIndex); // ���̸�
            wsprintf(m_Player2Name, TEXT("%d_Player2"), TextIndex); // ����̸� 

        }


        for (auto& Pair : m_Lines_Texture)
        {
            if (Pair.first == m_PlayerName) //�ؽ����� �̸��� ���� ������ �̸��� ���ٸ� 
            {
                if (g_iCurrentLevel == LEVEL_BATTLE)//��Ʋ�������� ���̸� index
                    PlayerIndex = 14;
                else
                    PlayerIndex = 28;//������������ ���̸� index
            }


            else if (Pair.first == m_Player2Name) //
            {
                if (g_iCurrentLevel == LEVEL_BATTLE)//��Ʋ�������� ����̸� index
                    PlayerIndex = 15;
                   
                else
                    PlayerIndex = 29;//������������ ����̸� index
            }
        }
    }


void CUI_Lines::Render_Time(const _double& _dTimeDelta)
{
    BossLines += _dTimeDelta;
    if (BossLines > 1.0)
    {
        BossDealy++;
        BossLines = 0.0;
    }


    if (g_iCurrentLevel == LEVEL_BATTLE)
    {
        CBattle_Manager* pBattleInstance = GET_INSTANCE(CBattle_Manager);
        {
            if (true == pBattleInstance->Get_BattleInfo().bBattleInit) // Get_BattleInit())
                StartDelay = true;
         
        }
        RELEASE_INSTANCE(CBattle_Manager);

        if (StartDelay == true) //��Ʋ���������� ������ ��簡 �����Բ� 
            TextTimeAcc += _dTimeDelta;
    }
        else if(g_iCurrentLevel == LEVEL_GAMEPLAY && BossDealy >= 5) //������������ 5���� �ð��� �帣��  
            TextTimeAcc += _dTimeDelta;

        //=========================
        if (TextTimeAcc > 1.0)
        {
            TextTime++;
            TextTimeAcc = 0.0;
        }

        if (TextIndex > MaxText) //�������� ���� �ʿ� 
            m_fAlpha = 0.f;

        if (TextTime == SoundTime && TextRender == false) //3�ʵ� ��� ���� 
        {
            if (TextIndex < MaxText)
            {
                if (g_iCurrentLevel == LEVEL_GAMEPLAY)
                {
                    wsprintf(m_LinesName, TEXT("TEONI_%d"), TextIndex + 1);

                    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
                    pGameInstance->Play_In_Silence(m_LinesName, 1.f, 0.2f);
                    RELEASE_INSTANCE(CGameInstance);
                }
                else if (g_iCurrentLevel == LEVEL_BATTLE)
                {
                    wsprintf(m_LinesName, TEXT("SABITO_%d"), TextIndex + 1);

                    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
                    pGameInstance->Play_In_Silence(m_LinesName, 1.f, 0.2f);
                    RELEASE_INSTANCE(CGameInstance);
                }
                
              TextIndex++; //��� index���� 
              m_fAlpha = 1.f;
              TextRender = true;
            }
        }

        if (TextTime >= SoundTotalTime)//3�ʺ��� 10�ʱ��� ��簡 ���� 
        {
            m_fAlpha = 0.f;
            TextTime = 0;
            TextRender = false;
            return;
        }


    }
	
HRESULT CUI_Lines::SetUp_Components()
{
    /* For.Com_Renderer */
    if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(COM_TEXTURE, LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Lines"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_UI, (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(COM_VIBUFFER, LEVEL_STATIC, PROTO_COMP_VIBUFFER_RECT, (CComponent**)&m_pVIBufferCom)))
        return E_FAIL;

    /* For.Com_NameTransform*/
    if (FAILED(__super::Add_Component(L"Com_NameTranform", LEVEL_STATIC, PROTO_COMP_TRANSFORM, (CComponent**)&m_pNameTransform)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Lines::SetUp_ConstantTable()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    _float4x4		ViewMatrix;

    XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

    //_float4x4 World = m_pTransformCom->Get_WorldFloat4x4_TP();

    //if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &World, sizeof(_float4x4))))
    //    return E_FAIL;

    if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
        return E_FAIL;


    if (FAILED(m_pTextureCom->SetUp_ConstantTable_Index(m_pShaderCom, "g_DiffuseTexture", TextIndex)))
        return E_FAIL;


    return S_OK;
}

CUI_Lines* CUI_Lines::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Lines* pInstance = new CUI_Lines(pDevice, pContext);

    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        MSG_BOX("Failed to Created : CUI_Lines");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CUI_Base* CUI_Lines::Clone(void* pArg)
{
    CUI_Lines* pInstance = new CUI_Lines(*this);

    if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
    {
        MSG_BOX("Failed to Cloned : CUI_Lines");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_Lines::Free()
{
    __super::Free();

    if (m_PlayerName != nullptr)
        Safe_Delete_Array(m_PlayerName);

    if (m_Player2Name != nullptr)
        Safe_Delete_Array(m_Player2Name);

    //if (m_LinesName != nullptr)
    //    Safe_Delete_Array(m_LinesName);

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pNameTransform);
}
