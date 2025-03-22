#include "CMainView.h"
#include "CImGui_Manager.h"
#include "CEffect.h"
#include "CGameInstance.h"
#include "CData_Manager.h"
#include "CParticle.h"

void CEffect::UVCONTROL::Translate_Move(const _double& _dTimeDelta)
{
    _vector vDir = XMLoadFloat2(&vUVRealDireciton);
    XMStoreFloat2(&vResultUVPos, vDir * fUvCurSpeed * (_float)(_dTimeDelta));

    if (true == bUVRestrict)
    {
        _vector vDir = XMLoadFloat2(&vUVRealDireciton);
        XMStoreFloat2(&vEndResultUVPos, vDir * fUvCurSpeed * (_float)(_dTimeDelta));
    }   

    if (vResultUVPos.x > 1)
        vResultUVPos.x = 1.f;
    if (vResultUVPos.y > 1)
        vResultUVPos.y= 1.f;
    if (vEndResultUVPos.x > 1)
        vEndResultUVPos.x = 1.f;
    if (vEndResultUVPos.y > 1)
        vEndResultUVPos.y = 1.f;
}


void CEffect::tagUVControl::Reset()
{
    vResultUVPos = vCurUVPos;
    vEndResultUVPos = vUVEndPos;
    XMStoreFloat2(&vUVRealDireciton, XMVector2Normalize(XMLoadFloat2(&vUVDirection)));
}


CEffect::CEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject(_pDevice, _pContext)
{
}

CEffect::CEffect(const CEffect& _rhs)
    : CGameObject(_rhs) , m_tEffectParticleDesc(_rhs.m_tEffectParticleDesc)
    , m_strEffectPath(_rhs.m_strEffectPath)
{
    if (nullptr != _rhs.m_pModelCom)
        m_pModelCom = static_cast<CModel*>(_rhs.m_pModelCom->Clone(nullptr));
}

void CEffect::Set_OwnerTransform(CTransform* _pTransform)
{
    if (nullptr == _pTransform)
        return;

    m_pOwnerTransform = _pTransform;
    Safe_AddRef(_pTransform);

    m_pTransformCom->Set_Pos(XMLoadFloat3(&m_tEffectParticleDesc.vInitPos));

    m_pParticle->Set_OwnerTransform(m_pOwnerTransform);
}

HRESULT CEffect::NativeConstruct_Prototype()
{
    if (FAILED(Ready_Comp()))
        return E_FAIL;
    m_bActive = true;
    return S_OK;
}

HRESULT CEffect::NativeConstruct_Clone(void* pArg)
{
    return S_OK;
}

void CEffect::Tick(const _double& _dTimeDelta)
{
    if (false == m_bActive)
        return;

    if (true == m_bZeroAcceleration)
    {
        if(m_tEffectParticleDesc.fResultForce < 0)
            m_tEffectParticleDesc.fResultForce = 0;
    }

    m_tEffectParticleDesc.Update_Data(_dTimeDelta);
    m_tUVControl.Translate_Move(_dTimeDelta);
    if(nullptr != m_pOwnerTransform)
    m_pTransformCom->Set_Pos(m_tEffectParticleDesc.Translate_Pos(m_pTransformCom->Get_Pos(), _dTimeDelta));

    m_pTransformCom->Rotate_Fix(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_tEffectParticleDesc.vResultRotAnlge.x);
    m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_tEffectParticleDesc.vResultRotAnlge.y);
    m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_tEffectParticleDesc.vResultRotAnlge.z);

    _matrix Coord = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_tEffectParticleDesc.vRotTranslate.x),
        XMConvertToRadians(m_tEffectParticleDesc.vRotTranslate.y),
        XMConvertToRadians(m_tEffectParticleDesc.vRotTranslate.z));

    _vector vPos = XMVector3TransformNormal(m_pTransformCom->Get_Pos(), Coord)* m_tEffectParticleDesc.fRotLength;
    m_pTransformCom->Set_Pos(vPos + m_pTransformCom->Get_Pos());

    if (nullptr != m_pCollider)
    {
        if(nullptr == m_pOwnerTransform)
            m_pCollider->Tick(m_pTransformCom->Get_WorldMatrix());
        else
            m_pCollider->Tick(m_pTransformCom->Get_WorldMatrix() * m_pOwnerTransform->Get_WorldMatrix());
    }

    if (nullptr != m_pParticle)
    {
        _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix() *
            m_pOwnerTransform->Get_WorldMatrix();
        CTransform* pTransform = static_cast<CTransform*>(m_pParticle->Get_Component(COM_TRANSFORM));
        if (nullptr != pTransform)
        {
            pTransform->Set_Pos(WorldMatrix.r[3]);
        }
        m_pParticle->Tick(_dTimeDelta);
    }
}

void CEffect::Late_Tick(const _double& _dTimeDelta)
{
    if (false == m_bActive)
        return;

    if (true == m_tEffectParticleDesc.bLoop)
    {
        if (m_tEffectParticleDesc.fSaveTime > m_tEffectParticleDesc.fLifeTime)
        {
            m_pTransformCom->Set_Pos(XMLoadFloat3(&m_tEffectParticleDesc.vInitPos));
            m_pTransformCom->Set_Scale(m_tEffectParticleDesc.vSize);
            m_pTransformCom->Rotate_Fix(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_tEffectParticleDesc.vRotAnlge.x);
            m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_tEffectParticleDesc.vRotAnlge.y);
            m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_tEffectParticleDesc.vRotAnlge.z);
            m_tEffectParticleDesc.Reset();
            m_tUVControl.Reset();
        }
    }
    
    if (nullptr != m_pRendererCom)
        m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

    if (nullptr != m_pParticle)
    {
        m_pParticle->Set_OwnerFadeOut( (m_tEffectParticleDesc.vResultBlendColor.w));
        m_pParticle->Late_Tick(_dTimeDelta);
    }
#ifdef _DEBUG
    if (nullptr != m_pRendererCom)
        m_pRendererCom->Add_Components(m_pCollider);
#endif
}

HRESULT CEffect::Render()
{
    if (FAILED(SetUp_Constant()))
        return E_FAIL;
    if(nullptr != m_pModelCom)
    {
        _uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();

        for (_uint i = 0; i < iNumMeshContainers; ++i)
        {
            if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TEXTYPE::TextureType_DIFFUSE)))
                return E_FAIL;

            m_pModelCom->Render(i, m_pShaderCom, 5);
        }
    }

    return S_OK;
}

void CEffect::Modify_Value(vector<CModel*>* _pModel, CParticle* _pParticle)
{
    if(ImGui::Button("Connect Paritcle"))
    {
        if (nullptr != _pParticle)
        {
            Safe_Release(m_pParticle);
            _wstring strPath = L"..\\..\\Resource\\Json\\Particle\\";
            strPath.append(_pParticle->Get_ParticleName());
            strPath.append(L".bin");
            m_pParticle = CParticle::Create_LoadFile(m_pDevice, m_pContext, strPath);
            if (nullptr != m_pParticle)
            {
                m_pParticle->Set_OwnerTransform(m_pTransformCom);
            }
        }
    }
    ImGui::Checkbox("Zero Acceration", &m_bZeroAcceleration);
    ImGui::Checkbox("Coll Value", &m_bCollider);

    ImGui::Checkbox("UV Control", &m_bUVControl);
    if (true == m_bUVControl)
    {
        if (ImGui::TreeNode("Model UV "))
        {
            UVCONTROL DESC = m_tUVControl;
            _bool bEnable;
            {
                _float Arr[2] = { DESC.vCurUVPos.x, DESC.vCurUVPos.y };
                bEnable |= ImGui::DragFloat2("UVStartPos", Arr, 0.01f, 0.f, 1.f);
                DESC.vCurUVPos = _float2(Arr[0], Arr[1]);
            }
            {
                _float Arr[2] = { DESC.vUVDirection.x, DESC.vUVDirection.y };
                bEnable |= ImGui::DragFloat2("UVDirection", Arr, 0.01f, 0.f, 1.f);
                DESC.vUVDirection = _float2(Arr[0], Arr[1]);
                XMStoreFloat2(&DESC.vUVRealDireciton, XMVector2Normalize(XMLoadFloat2(&DESC.vUVDirection)));
            }
            {
                bEnable |= ImGui::DragFloat("UVSpeed", &DESC.fUvCurSpeed, 0.01f, 0.f, 1.f);
            }
            {
                bEnable |= ImGui::Checkbox("UVRestrict", &DESC.bUVRestrict);
            }
            if (true == DESC.bUVRestrict)
            {
                {
                    _float Arr[2] = { DESC.vUVEndPos.x, DESC.vUVEndPos.y };
                    bEnable |= ImGui::DragFloat2("UVEndPos", Arr, 0.01f, 0.f, 1.f);
                    DESC.vUVEndPos = _float2(Arr[0], Arr[1]);
                }
                {
                    bEnable |= ImGui::DragFloat("UVEndSpeed", &DESC.fUvEndSpeed, 0.01f, 0.f, 1.f);
                }
            }

            m_tUVControl = DESC;

            ImGui::TreePop();
        }
    }

    if (true == m_bCollider)
    {
        if (ImGui::TreeNode("Effect Collider"))
        {
            _float3 Scale = m_pCollider->Get_Scale();
            {
                _float Arr[3] = { Scale.x, Scale.y, Scale.z };
                ImGui::DragFloat3("Effect Collider Scale", Arr, 0.01f);
                m_pCollider->Set_Scale(_float3(Arr[0], Arr[1], Arr[2]));
            }
            _float3 Translate = m_pCollider->Get_Translate();
            {
                _float Arr[3] = { Translate.x, Translate.y, Translate.z };
                ImGui::DragFloat3("Effect Collider Translate", Arr, 0.01f);
                m_pCollider->Set_Translate(_float3(Arr[0], Arr[1], Arr[2]));
            }
            ImGui::TreePop();
        }
    }

    if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 8 * ImGui::GetTextLineHeightWithSpacing())))
    {
        for (auto& Value : *_pModel)
        {
            FILEDATA* pFileData = Value->Get_FileData();
            if (nullptr != pFileData)
            {
                _wstring _str = pFileData->strFileName;
                string str;
                str.assign(_str.begin(), _str.end());
                if (ImGui::Selectable(str.c_str()))
                {
                    Safe_Release(m_pModelCom);
                    m_pModelCom = Value;
                    Safe_AddRef(m_pModelCom);

                    m_strEffectPath = m_pModelCom->Get_FileData()->strFileName;
                }
            }
        }
        ImGui::EndListBox();
    }

    if (ImGui::TreeNode("ModelEffect  Info"))
    {
        _bool bEnable = false;

        EFFECTPARTICLEDESC DESC = m_tEffectParticleDesc;
        bEnable |= ImGui::DragFloat("LifeTimeP", &DESC.fLifeTime, 0.01f);
        bEnable |= ImGui::DragFloat("Force", &DESC.fForce, 0.01f);
        bEnable |= ImGui::DragFloat("Acceleration", &DESC.fAcceleration, 0.01f);
        bEnable |= ImGui::DragFloat("Mass", &DESC.fMass, 0.01f);
        bEnable |= ImGui::DragFloat("JumpValue", &DESC.fJumpValue, 0.01f);
        bEnable |= ImGui::DragFloat("JumpTime", &DESC.fJumpTime, 0.01f);
        bEnable |= ImGui::DragFloat("JumpTimeFast", &DESC.fJumpTimeFast, 0.01f);
        {
            _float Arr[4] = { DESC.vParticleColor.x, DESC.vParticleColor.y, DESC.vParticleColor.z, DESC.vParticleColor.w };
            bEnable |= ImGui::DragFloat4("ParticleColor", Arr, 0.01f, -1.f, 1.f);
            DESC.vParticleColor = _float4(Arr[0], Arr[1], Arr[2], Arr[3]);
        }
        {
            _float Arr[4] = { DESC.vBlendColor.x, DESC.vBlendColor.y, DESC.vBlendColor.z, DESC.vBlendColor.w };
            bEnable |= ImGui::DragFloat4("BlendColor", Arr, 0.01f, -1.f, 1.f);
            DESC.vBlendColor = _float4(Arr[0], Arr[1], Arr[2], Arr[3]);
        }
        {
            bEnable |= ImGui::DragFloat("BlendSpeed", &DESC.fBlendSpeed, 0.01f);
            DESC.fBlendSpeed = DESC.fBlendSpeed;
        }
        {
            _float Arr[3] = { DESC.vRotAnlge.x, DESC.vRotAnlge.y, DESC.vRotAnlge.z };
            bEnable |= ImGui::DragFloat3("RotAnlge", Arr, 0.01f);
            DESC.vRotAnlge = _float3(Arr[0], Arr[1], Arr[2]);

            if (true == bEnable)
            {
                m_pTransformCom->Rotate_Fix(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_tEffectParticleDesc.vRotAnlge.x);
                m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_tEffectParticleDesc.vRotAnlge.y);
                m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_tEffectParticleDesc.vRotAnlge.z);
            }
        }
        {
            _float Arr[3] = { DESC.vRotSpeed.x, DESC.vRotSpeed.y, DESC.vRotSpeed.z };
            bEnable |= ImGui::DragFloat3("RotSpeed", Arr, 0.01f);
            DESC.vRotSpeed = _float3(Arr[0], Arr[1], Arr[2]);
        }
        {
            _float Arr[3] = { DESC.vRotTranslate.x, DESC.vRotTranslate.y, DESC.vRotTranslate.z };
            bEnable |= ImGui::DragFloat3("RotTranslate", Arr, 0.01f);
            DESC.vRotTranslate = _float3(Arr[0], Arr[1], Arr[2]);
        }
        bEnable |= ImGui::DragFloat("RotLength", &DESC.fRotLength, 0.01f);
        {
            _float Arr[3] = { DESC.vDirection.x, DESC.vDirection.y, DESC.vDirection.z };
            bEnable |= ImGui::DragFloat3("Direction", Arr, 0.01f);
            DESC.vDirection = _float3(Arr[0], Arr[1], Arr[2]);
        }
        {
            _float Arr[3] = { DESC.vSize.x, DESC.vSize.y, DESC.vSize.z, };
            bEnable |= ImGui::DragFloat3("Size", Arr, 0.01f);
            DESC.vSize = _float3(Arr[0], Arr[1], Arr[2]);
        }
        {
            _float Arr[3] = { DESC.vInitPos.x, DESC.vInitPos.y, DESC.vInitPos.z };
            bEnable |= ImGui::DragFloat3("InitPos", Arr, 0.01f);
            DESC.vInitPos = _float3(Arr[0], Arr[1], Arr[2]);
        }
        {
            bEnable |= ImGui::Checkbox("Gravity", &DESC.bGravity);
            bEnable |= ImGui::Checkbox("LifeLoop", &DESC.bLoop);
        }

        m_tEffectParticleDesc = DESC;

        ImGui::TreePop();
    }

    if (nullptr != m_pParticle)
    {
        {
            _float Arr[3] = { m_v3ParticleInitRot.x, m_v3ParticleInitRot.y, m_v3ParticleInitRot.z };
            ImGui::DragFloat3("Particle Init Rot", Arr, 0.01f);
            m_v3ParticleInitRot = _float3(Arr[0], Arr[1], Arr[2]);
        }
        {
            _float Arr[3] = { m_v3ParticleInitPos.x, m_v3ParticleInitPos.y, m_v3ParticleInitPos.z, };
            ImGui::DragFloat3("Particle Init Pos", Arr, 0.01f);
            m_v3ParticleInitPos = _float3(Arr[0], Arr[1], Arr[2]);
        }
        m_pParticle->Set_OwnerInitPos(XMLoadFloat3(&m_v3ParticleInitPos));
        m_pParticle->Set_OwnerInitRot(XMLoadFloat3(&m_v3ParticleInitRot));
    }
}

void CEffect::Reset_Data()
{
    m_pTransformCom->Set_Pos(XMLoadFloat3(&m_tEffectParticleDesc.vInitPos));
    m_pTransformCom->Set_Scale(m_tEffectParticleDesc.vSize);
    m_pTransformCom->Rotate_Fix(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_tEffectParticleDesc.vRotAnlge.x);
    m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_tEffectParticleDesc.vRotAnlge.y);
    m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_tEffectParticleDesc.vRotAnlge.z);
    m_tEffectParticleDesc.Reset();
    m_tUVControl.Reset();
}

void CEffect::Save(HANDLE& _hHandle, DWORD* _dwByte)
{
    {
        {
            _uint Length = (_uint)lstrlen(m_strEffectPath.c_str()) + 1;
            WriteFile(_hHandle, &Length, sizeof(_uint), _dwByte, 0);
            WriteFile(_hHandle, m_strEffectPath.c_str(), sizeof(_tchar) * Length, _dwByte, 0);
        }
        WriteFile(_hHandle, &m_tEffectParticleDesc, sizeof(EFFECTPARTICLEDESC), _dwByte, 0);
        WriteFile(_hHandle, &m_tUVControl, sizeof(UVCONTROL), _dwByte, 0);
        WriteFile(_hHandle, &m_bZeroAcceleration, sizeof(_bool), _dwByte, 0);
        WriteFile(_hHandle, &m_bUVControl, sizeof(_bool), _dwByte, 0);
        WriteFile(_hHandle, &m_bCollider, sizeof(_bool), _dwByte, 0);
        WriteFile(_hHandle, &m_pCollider->Get_Scale(), sizeof(_float3), _dwByte, 0);
        WriteFile(_hHandle, &m_pCollider->Get_Translate(), sizeof(_float3), _dwByte, 0);

        if (nullptr != m_pParticle)
        {
            _uint Length = (_uint)lstrlen(m_pParticle->Get_ParticleName().c_str()) + 1;
            WriteFile(_hHandle, &Length, sizeof(_uint), _dwByte, 0);
            WriteFile(_hHandle, m_pParticle->Get_ParticleName().c_str(), sizeof(_tchar) * Length, _dwByte, 0);

            WriteFile(_hHandle, &m_v3ParticleInitPos, sizeof(_float3), _dwByte, 0);
            WriteFile(_hHandle, &m_v3ParticleInitRot, sizeof(_float3), _dwByte, 0);
       }
    }
}

void CEffect::Load(HANDLE& _hHandle, DWORD* _dwByte)
{
    _uint iSize = 0;
    _uint iLength = 0;
    {
        ReadFile(_hHandle, &iLength, sizeof(_uint), _dwByte, nullptr);
        _tchar* pName = new _tchar[iLength];
        ReadFile(_hHandle, pName, sizeof(_tchar) * iLength, _dwByte, nullptr);
        m_strEffectPath = pName;
        Safe_Delete_Array(pName);
    }
    ReadFile(_hHandle, &m_tEffectParticleDesc, sizeof(EFFECTPARTICLEDESC), _dwByte, 0);
    ReadFile(_hHandle, &m_tUVControl, sizeof(UVCONTROL), _dwByte, 0);
    ReadFile(_hHandle, &m_bZeroAcceleration, sizeof(_bool), _dwByte, 0);
    ReadFile(_hHandle, &m_bUVControl, sizeof(_bool), _dwByte, 0);
    ReadFile(_hHandle, &m_bCollider, sizeof(_bool), _dwByte, 0); 
    _float3 v3Scale, v3Translate;
    ReadFile(_hHandle, &v3Scale, sizeof(_float3), _dwByte, 0);
    ReadFile(_hHandle, &v3Translate, sizeof(_float3), _dwByte, 0);

    {
        _wstring strParticleName = L"";
        ReadFile(_hHandle, &iLength, sizeof(_uint), _dwByte, nullptr);
        _tchar* pName = new _tchar[iLength];
        ReadFile(_hHandle, pName, sizeof(_tchar) * iLength, _dwByte, nullptr);
        strParticleName = pName;
        Safe_Delete_Array(pName);

        ReadFile(_hHandle, &m_v3ParticleInitPos, sizeof(_float3), _dwByte, 0);
        ReadFile(_hHandle, &m_v3ParticleInitRot, sizeof(_float3), _dwByte, 0);

        _wstring strPath = L"..\\..\\Resource\\Json\\Particle\\";
        strPath.append(strParticleName);
        strPath.append(L".bin");
        m_pParticle = CParticle::Create_LoadFile(m_pDevice, m_pContext, strPath);
        if (nullptr != m_pParticle)
        {
            m_pParticle->Set_OwnerTransform(m_pTransformCom);
        }
    }

    if (nullptr != m_pCollider)
    {
        m_pCollider->Set_Scale(v3Scale);
        m_pCollider->Set_Translate(v3Scale);
    }

    FILEGROUP* pFileGroup = nullptr;
    BEGIN_SINGLETON(CGameInstance, pGameInstnace)
    {
        pFileGroup = pGameInstnace->Find_Folder(&PATHS{ L"Game", L"Effect", L"FBX_Converter" });
    }
    END_SINGLETON(pGameInstnace);

    if (nullptr != pFileGroup)
    {
        FILEDATA* pFileData = pFileGroup->FindData(m_strEffectPath.c_str());
        m_pModelCom = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pFileGroup, pFileData
        , XMMatrixScaling(0.01f, 0.01f, 0.01f));
    }

    m_pTransformCom->Set_Pos(XMLoadFloat3(&m_tEffectParticleDesc.vInitPos));
    m_pTransformCom->Set_Scale(m_tEffectParticleDesc.vSize);
    m_pTransformCom->Rotate_Fix(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_tEffectParticleDesc.vRotAnlge.x);
    m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_tEffectParticleDesc.vRotAnlge.y);
    m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_tEffectParticleDesc.vRotAnlge.z);


    if (nullptr != m_pParticle)
    {
        m_pParticle->Set_OwnerInitPos(XMLoadFloat3(&m_v3ParticleInitPos));
        m_pParticle->Set_OwnerInitRot(XMLoadFloat3(&m_v3ParticleInitRot));
    }
}

void CEffect::Find_Region()
{
    if (nullptr == m_pNavigation)
    {
        BEGIN_SINGLETON(CData_Manager, pDataManager)
        {
            CStage* pStage = pDataManager->Get_StageMgr()->Get_Stage();
            if (nullptr != pStage)
            {
                CRegion* pRegion = pStage->Get_Region(0);

                if (nullptr != pRegion)
                {
                    m_pNavigation = pRegion->Get_Navigation();
                    Safe_AddRef(m_pNavigation);
                }
            }
        }
        END_SINGLETON(pDataManager);
    }
}

HRESULT CEffect::Ready_Comp()
{
    if (FAILED(__super::Ready_Transform(COM_TRANSFORM)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC,
      PROTO_COMP_SHADER_VTXMODEL, (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC,
        PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
        return E_FAIL;

    CCollider::COLLIDERDESC tDesc;
    tDesc.vScale = _float3(1.f, 1.f, 1.f);
    tDesc.vTranslation = _float3(0.f, 0.f, 0.f);
    if (FAILED(__super::Add_Component(COM_COLLCOLLIDER, LEVEL_STATIC,
        PROTO_COMP_SPHERECOLLIDER, (CComponent**)&m_pCollider, &tDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect::SetUp_Constant()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (nullptr == m_pTransformCom)
        return E_FAIL;

    BEGIN_SINGLETON(CGameInstance, pGameInstnace)
    {
        _float4x4 ViewMatrix;
        XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

        if (nullptr != m_pOwnerTransform)
        {
            _float4x4 WorldMatrtix;
            XMStoreFloat4x4(&WorldMatrtix, XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix() *
                m_pOwnerTransform->Get_WorldMatrix()));
            if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrtix, sizeof(_float4x4))))
                return E_FAIL;
        }
        else
        {
            if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
                return E_FAIL;
        }
        if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstnace->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstnace->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Set_RawValue("g_Color", &m_tEffectParticleDesc.vParticleColor, sizeof(_float4))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Set_RawValue("g_BlendColor", &m_tEffectParticleDesc.vResultBlendColor, sizeof(_float4))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Set_RawValue("g_UVControlPos", &m_tUVControl.vResultUVPos, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Set_RawValue("g_UVEndControlPos", &m_tUVControl.vEndResultUVPos, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Set_RawValue("g_bUVControl", &m_bUVControl, sizeof(_bool))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Set_RawValue("g_UVRestrict", &m_tUVControl.bUVRestrict, sizeof(_bool))))
            return E_FAIL;
    }
    END_SINGLETON(pGameInstnace);
    return S_OK;
}

CEffect* CEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffect* pInstance = new CEffect(pDevice, pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEffect::Create To Failed");
    }
    return pInstance;
}

CGameObject* CEffect::Clone(void* pArg)
{
    CEffect* pInstance = new CEffect(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEffect::Clone To Failed");
    }
    return pInstance;
}

void CEffect::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pOwnerTransform);
    Safe_Release(m_pCollider);
    Safe_Release(m_pNavigation);
    Safe_Release(m_pParticle);

    __super::Free();
}