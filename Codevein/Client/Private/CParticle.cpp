#include "CMainView.h"
#include "CImGui_Manager.h"
#include "CParticle.h"
#include "CGameInstance.h"

CParticle::CParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CParticle::CParticle(const CParticle& _rhs)
    : CGameObject(_rhs), m_strPaticleName(_rhs.m_strPaticleName),
    m_fLifeTime(_rhs.m_fLifeTime), m_tLifeTimer(_rhs.m_tLifeTimer), m_bLoop(_rhs.m_bLoop),
    m_v3Scale(_rhs.m_v3Scale), m_v3Rot(_rhs.m_v3Rot)
{
    for (auto& iter : _rhs.m_ParticleList)
    {
        PARTICLEINFO tInfo;
        tInfo.iPassIndex = iter.iPassIndex;
        tInfo.pVIBufferCom = static_cast<CVIBuffer_Cube_Instance*>(iter.pVIBufferCom->Clone(nullptr));
        m_ParticleList.push_back(tInfo);
    }

    m_bActive = true;
}

void CParticle::Set_OwnerTransform(CTransform* _pOwnerTransform)
{
    if (nullptr == _pOwnerTransform)
        return;

    m_pOwnerTransform = _pOwnerTransform;
    Safe_AddRef(m_pOwnerTransform);
}

void CParticle::Turn_Off_LifeLoop()
{
    for (auto& iter : m_ParticleList)
    {
        _uint iIndex = 0;
        for (auto& value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
        {
            value.bLoop = false;
            iter.pVIBufferCom->Set_InstnaceData(value, iIndex++);
        }
    }
}

void CParticle::Turn_On_LifeLoop()
{
    for (auto& iter : m_ParticleList)
    {
        _uint iIndex = 0;
        for (auto& value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
        {
            value.bLoop = true;
            iter.pVIBufferCom->Set_InstnaceData(value, iIndex++);
        }
    }
}

HRESULT CParticle::NativeConstruct_Prototype()
{
	PARTICLEINFO tInfo;
	tInfo.pVIBufferCom = CVIBuffer_Cube_Instance::Create(m_pDevice, m_pContext);
	m_ParticleList.push_back(tInfo);
    m_bActive = true;
	if (FAILED(Ready_Comp()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle::NativeConstruct_Prototype(const _wstring& _strFileName)
{
	Load(_strFileName);

    if (FAILED(Ready_Comp()))
        return E_FAIL;

    m_bActive = true;
	return S_OK;
}

HRESULT CParticle::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Comp()))
		return E_FAIL;

	if (nullptr != m_pTransformCom)
	{
		m_pTransformCom->Rotate_Fix(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_v3Rot.x);
		m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_v3Rot.y);
		m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_v3Rot.z);
		m_pTransformCom->Set_Scale(m_v3Scale);
	}
    m_bActive = true;
	return S_OK;
}

void CParticle::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);

	if (false == m_bActive)
		return;

    if (nullptr != m_pOwnerTransform)
    {
        m_pTransformCom->Set_Pos(XMLoadFloat3(&m_v3OwnerInitPos) + m_pOwnerTransform->Get_Pos());
        m_pTransformCom->Rotate_Fix(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_v3OwnerInitRot.x);
        m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_v3OwnerInitRot.y);
        m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_v3OwnerInitRot.z);
    }

	for (auto& iter : m_ParticleList)
		if (nullptr != iter.pVIBufferCom)
			iter.pVIBufferCom->Update(m_pTransformCom, _dTimeDelta);
}

void CParticle::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);

	if (false == m_bActive)
		return;

	if (false == m_bLoop)
	{
		if (true == m_tLifeTimer.Is_Over(_dTimeDelta))
		{
			if (true == g_bGameRun)
				m_bActive = false;
            if (true == m_bNotReset)
            {
                for (auto& iter : m_ParticleList)
                    iter.pVIBufferCom->Reset_Data();
                m_tLifeTimer.Reset_Timer();
            }
		}
	}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CParticle::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	for (auto& iter : m_ParticleList)
	{
		if (FAILED(SetUp_Constant()))
			return E_FAIL;

		m_pShaderCom->Begin(0);
		iter.pVIBufferCom->Render();
	}

	return S_OK;
}

void CParticle::Reset()
{
    for (auto& iter : m_ParticleList)
        iter.pVIBufferCom->Reset_Data();
    m_tLifeTimer.Reset_Timer();
    m_bActive = false;
}

void CParticle::Modify_Value(CTexture* _pTexture, int* _iSelectIndex, int* _iSelectInstanceIndex, _float3* _pPos, _float* _fRandomValue, _uint* _iCreateInstanceCount, EFFECTPARTICLEDESC* _pEffectParticleDesc)
{
    if (nullptr == _pTexture || nullptr == _iSelectIndex || nullptr == _iSelectInstanceIndex || nullptr == _pPos
        || nullptr == _fRandomValue || nullptr == _iCreateInstanceCount || nullptr == _pEffectParticleDesc)
        return;

    if (ImGui::CollapsingHeader("Particle Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
        _float Position[3] = { _pPos->x,   _pPos->y,  _pPos->z };
        _float Rot[3] = { m_v3Rot.x,   m_v3Rot.y,  m_v3Rot.z };
        _float Scale[3] = { m_v3Scale.x,   m_v3Scale.y,  m_v3Scale.z };

        if (ImGui::DragFloat3("Position", Position, 0.5f, -10000000.f, 10000000.f, "%.2f"))
        {
            if (nullptr != m_pTransformCom)
                m_pTransformCom->Set_Pos(XMLoadFloat3(&_float3(Position[0], Position[1], Position[2])));

            *_pPos = _float3(Position[0], Position[1], Position[2]);
        }
        if (ImGui::DragFloat3("Rotate", Rot, 0.5f, -10000000.f, 10000000.f, "%.2f"))
        {
            m_v3Rot = _float3(Rot[0], Rot[1], Rot[2]);
            if (nullptr != m_pTransformCom)
            {
                m_pTransformCom->Rotate_Fix(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_v3Rot.x);
                m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_v3Rot.y);
                m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_v3Rot.z);
            }
        }
        if (ImGui::DragFloat3("Scale", Scale, 0.5f, -10000000.f, 10000000.f, "%.2f"))
        {
            m_v3Scale = _float3(Scale[0], Scale[1], Scale[2]);
            if (nullptr != m_pTransformCom)
                m_pTransformCom->Set_Scale(m_v3Scale);
        }
    }

    if (ImGui::CollapsingHeader("Particle Desc", ImGuiTreeNodeFlags_DefaultOpen))
    {
        string strEffectName;
        string strTextureName;

        strEffectName.assign(m_strPaticleName.begin(), m_strPaticleName.end());
        char Effect[MAX_PATH] = "";
        strcpy_s(Effect, strEffectName.c_str());

        ImGui::DragInt("ManyBufferInstance", (int*)_iCreateInstanceCount);
        if (ImGui::Button("Plus Piece"))
        {
            PARTICLEINFO tInfo;
            tInfo.pVIBufferCom = CVIBuffer_Cube_Instance::Create(m_pDevice, m_pContext);
            m_ParticleList.push_back(tInfo);
        }
        if (ImGui::Button("OppositeDirectionChange"))
        {
            for (auto& iter : m_ParticleList)
            {
                _uint iIndex = 0;
                for (auto& DESC : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                {
                    XMStoreFloat3(&DESC.vDirection, -XMLoadFloat3(&DESC.vDirection));
                    iter.pVIBufferCom->Set_InstnaceData(DESC, iIndex++);
                }
            }
        }
        if (ImGui::Button("RandomValue"))
        {
            BEGIN_SINGLETON(CGameInstance, pGameInstance)
            {
                for (auto& iter : m_ParticleList)
                {
                    _uint iIndex = 0;
                    for (auto& DESC : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                    {
                        DESC.fForce = rand() % 1000 * 0.001f * _fRandomValue[0] + 0.0001f;
                        DESC.fAcceleration = rand() % 10000 * 0.001f * _fRandomValue[1] + 0.0001f;
                        DESC.fJumpValue = rand() % 10000 * 0.001f * _fRandomValue[2] + 0.001f;
                        DESC.fJumpTime = rand() % 10000 * 0.001f * _fRandomValue[3] + 0.0001f;
                        DESC.fJumpTimeFast = rand() % 1000 * 0.001f * _fRandomValue[4] + 0.0001f;
                        DESC.vParticleColor.x = rand() % 1000 * 0.001f * _fRandomValue[5] + 0.0001f;
                        DESC.vParticleColor.y = rand() % 1000 * 0.001f * _fRandomValue[5] + 0.0001f;
                        DESC.vParticleColor.z = rand() % 1000 * 0.001f * _fRandomValue[5] + 0.0001f;
                        DESC.vParticleColor.w = rand() % 100 * 0.001f * _fRandomValue[5] + 0.0001f;
                        DESC.vBlendColor.x = rand() % 1000 * 0.001f * _fRandomValue[6] + 0.0001f;
                        DESC.vBlendColor.y = rand() % 1000 * 0.001f * _fRandomValue[6] + 0.0001f;
                        DESC.vBlendColor.z = rand() % 1000 * 0.001f * _fRandomValue[6] + 0.0001f;
                        DESC.vBlendColor.w = rand() % 100 * 0.001f * _fRandomValue[6] + 0.0001f;
                        DESC.fBlendSpeed = rand() % 10000 * 0.001f * _fRandomValue[7] + 0.0001f;
                        DESC.vRotSpeed.x = rand() % 180 * 0.01f * _fRandomValue[8] + 0.0001f;
                        DESC.vRotSpeed.y = rand() % 180 * 0.01f * _fRandomValue[8] + 0.0001f;
                        DESC.vRotSpeed.z = rand() % 180 * 0.01f * _fRandomValue[8] + 0.0001f;
                        {
                            _uint iValue = rand() % 2;
                            if (iValue == 0)
                                DESC.vDirection.x = (rand() % 100) * _fRandomValue[9] + 0.0001f;
                            else if (iValue == 1)
                                DESC.vDirection.x = -1 * (rand() % 100) * _fRandomValue[9] + 0.0001f;
                        }
                        {
                            _uint iValue = rand() % 2;
                            if (iValue == 0)
                                DESC.vDirection.y = (rand() % 100) * _fRandomValue[13] + 0.0001f;
                            else if (iValue == 1)
                                DESC.vDirection.y = -1 * (rand() % 100) * _fRandomValue[13] + 0.0001f;
                        }
                        {
                            _uint iValue = rand() % 2;
                            if (iValue == 0)
                                DESC.vDirection.z = (rand() % 100) * _fRandomValue[14] + 0.0001f;
                            else if (iValue == 1)
                                DESC.vDirection.z = -1 * (rand() % 100) * _fRandomValue[14] + 0.0001f;
                        }
                        DESC.vSize.x = rand() % 1000 * 0.001f * _fRandomValue[10] + 0.0001f;
                        DESC.vSize.y = rand() % 1000 * 0.001f * _fRandomValue[10] + 0.0001f;
                        DESC.vSize.z = rand() % 1000 * 0.001f * _fRandomValue[10] + 0.0001f;
                        {
                            _uint iValue = rand() % 2;
                            if (iValue == 0)
                                DESC.vInitPos.x = rand() % 10000 * 0.001f * _fRandomValue[11] + 0.0001f;
                            else if (iValue == 1)
                                DESC.vInitPos.x = rand() % 10000 * -0.001f * _fRandomValue[11] + 0.0001f;
                        }
                        {
                            _uint iValue = rand() % 2;
                            if (iValue == 0)
                                DESC.vInitPos.y = rand() % 10000 * 0.001f * _fRandomValue[15] + 0.0001f;
                            else if (iValue == 1)
                                DESC.vInitPos.y = rand() % 10000 * -0.001f * _fRandomValue[15] + 0.0001f;
                        }
                        {
                            _uint iValue = rand() % 2;
                            if (iValue == 0)
                                DESC.vInitPos.z = rand() % 10000 * 0.001f * _fRandomValue[16] + 0.0001f;
                            else if (iValue == 1)
                                DESC.vInitPos.z = rand() % 10000 * -0.001f * _fRandomValue[16] + 0.0001f;
                        }
                        DESC.fLifeTime = (_float)(rand() % ((_int)_fRandomValue[12] * 1000) + 0.00001f) * 0.001f;
                        iter.pVIBufferCom->Set_InstnaceData(DESC, iIndex);
                    }
                }
            }
            END_SINGLETON(pGameInstance);
        }
        if (ImGui::CollapsingHeader("SaveValue"))
        {
            _bool bEnable = false;
            _float fForce = 0;
            bEnable = ImGui::DragFloat("Force", &_pEffectParticleDesc->fForce, 0.01f);
            {
                if (true == bEnable)
                {
                    for (auto& iter : m_ParticleList)
                    {
                        _uint iIndex = 0;
                        for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                        {
                            Value.fForce = _pEffectParticleDesc->fForce;
                            iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                        }
                    }
                }
            }
            bEnable = ImGui::DragFloat("Acceleration", &_pEffectParticleDesc->fAcceleration, 0.01f);
            {
                if (true == bEnable)
                {
                    for (auto& iter : m_ParticleList)
                    {
                        _uint iIndex = 0;
                        for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                        {
                            Value.fAcceleration = _pEffectParticleDesc->fAcceleration;
                            iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                        }
                    }
                }
            }
            bEnable = ImGui::DragFloat("Mass", &_pEffectParticleDesc->fMass, 0.01f);
            {
                if (true == bEnable)
                {
                    for (auto& iter : m_ParticleList)
                    {
                        _uint iIndex = 0;
                        for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                        {
                            Value.fMass = _pEffectParticleDesc->fMass;
                            iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                        }
                    }
                }
            }
            bEnable = ImGui::DragFloat("JumpValue", &_pEffectParticleDesc->fJumpValue, 0.01f);
            {
                if (true == bEnable)
                {
                    for (auto& iter : m_ParticleList)
                    {
                        _uint iIndex = 0;
                        for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                        {
                            Value.fJumpValue = _pEffectParticleDesc->fJumpValue;
                            iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                        }
                    }
                }
            }
            bEnable = ImGui::DragFloat("JumpTime", &_pEffectParticleDesc->fJumpTime, 0.01f);
            {
                if (true == bEnable)
                {
                    for (auto& iter : m_ParticleList)
                    {
                        _uint iIndex = 0;
                        for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                        {
                            Value.fJumpTime = _pEffectParticleDesc->fJumpTime;
                            iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                        }
                    }
                }
            }
            bEnable = ImGui::DragFloat("JumpTimeFast", &_pEffectParticleDesc->fJumpTimeFast, 0.01f);
            {
                if (true == bEnable)
                {
                    for (auto& iter : m_ParticleList)
                    {
                        _uint iIndex = 0;
                        for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                        {
                            Value.fJumpTimeFast = _pEffectParticleDesc->fJumpTimeFast;
                            iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                        }
                    }
                }
            }
            {
                _float Arr[4] = { _pEffectParticleDesc->vParticleColor.x, _pEffectParticleDesc->vParticleColor.y,
                   _pEffectParticleDesc->vParticleColor.z, _pEffectParticleDesc->vParticleColor.w };
                bEnable = ImGui::DragFloat4("ParticleColor", Arr, 0.01f, -1.f, 1.f);
                for (auto& iter : m_ParticleList)
                {
                    _uint iIndex = 0;
                    for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                    {
                        if (Arr[0] != _pEffectParticleDesc->vParticleColor.x)
                            Value.vParticleColor.x = _pEffectParticleDesc->vParticleColor.x;
                        else if (Arr[1] != _pEffectParticleDesc->vParticleColor.y)
                            Value.vParticleColor.y = _pEffectParticleDesc->vParticleColor.y;
                        else if (Arr[2] != _pEffectParticleDesc->vParticleColor.z)
                            Value.vParticleColor.z = _pEffectParticleDesc->vParticleColor.z;
                        else if (Arr[3] != _pEffectParticleDesc->vParticleColor.w)
                            Value.vParticleColor.w = _pEffectParticleDesc->vParticleColor.w;
                        iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                    }
                }
                _pEffectParticleDesc->vParticleColor = _float4(Arr[0], Arr[1], Arr[2], Arr[3]);
            }
            {
                _float Arr[4] = { _pEffectParticleDesc->vBlendColor.x, _pEffectParticleDesc->vBlendColor.y,
                    _pEffectParticleDesc->vBlendColor.z, _pEffectParticleDesc->vBlendColor.w };
                bEnable = ImGui::DragFloat4("BlendColor", Arr, 0.01f, -1.f, 1.f);
                _pEffectParticleDesc->vBlendColor = _float4(Arr[0], Arr[1], Arr[2], Arr[3]);
                if (true == bEnable)
                {
                    for (auto& iter : m_ParticleList)
                    {
                        _uint iIndex = 0;
                        for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                        {
                            if (Arr[0] != _pEffectParticleDesc->vBlendColor.x)
                                Value.vBlendColor.x = _pEffectParticleDesc->vBlendColor.x;
                            else if (Arr[1] != _pEffectParticleDesc->vBlendColor.y)
                                Value.vBlendColor.y = _pEffectParticleDesc->vBlendColor.y;
                            else if (Arr[2] != _pEffectParticleDesc->vParticleColor.z)
                                Value.vBlendColor.z = _pEffectParticleDesc->vBlendColor.z;
                            else if (Arr[3] != _pEffectParticleDesc->vBlendColor.w)
                                Value.vBlendColor.w = _pEffectParticleDesc->vBlendColor.w;
                            iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                        }
                    }
                }
            }
            {
                bEnable = ImGui::DragFloat("BlendSpeed", &_pEffectParticleDesc->fBlendSpeed, 0.01f);
                _pEffectParticleDesc->fBlendSpeed = _pEffectParticleDesc->fBlendSpeed;
                if (true == bEnable)
                {
                    for (auto& iter : m_ParticleList)
                    {
                        _uint iIndex = 0;
                        for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                        {
                            Value.fBlendSpeed = _pEffectParticleDesc->fBlendSpeed;
                            iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                        }
                    }
                }
            }
            {
                _float Arr[3] = { _pEffectParticleDesc->vRotAnlge.x, _pEffectParticleDesc->vRotAnlge.y, _pEffectParticleDesc->vRotAnlge.z };
                bEnable = ImGui::DragFloat3("RotAnlge", Arr, 0.01f);
                for (auto& iter : m_ParticleList)
                {
                    _uint iIndex = 0;
                    for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                    {
                        if (Arr[0] != _pEffectParticleDesc->vRotAnlge.x)
                            Value.vRotAnlge.x = _pEffectParticleDesc->vRotAnlge.x;
                        else if (Arr[1] != _pEffectParticleDesc->vRotAnlge.y)
                            Value.vRotAnlge.y = _pEffectParticleDesc->vRotAnlge.y;
                        else if (Arr[2] != _pEffectParticleDesc->vRotAnlge.z)
                            Value.vRotAnlge.z = _pEffectParticleDesc->vRotAnlge.z;

                        iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                    }
                }
                _pEffectParticleDesc->vRotAnlge = _float3(Arr[0], Arr[1], Arr[2]);
            }
            {
                _float Arr[3] = { _pEffectParticleDesc->vRotSpeed.x, _pEffectParticleDesc->vRotSpeed.y, _pEffectParticleDesc->vRotSpeed.z };
                bEnable |= ImGui::DragFloat3("RotSpeed", Arr, 0.01f);
                for (auto& iter : m_ParticleList)
                {
                    _uint iIndex = 0;
                    for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                    {
                        if (Arr[0] != _pEffectParticleDesc->vRotSpeed.x)
                            Value.vRotSpeed.x = _pEffectParticleDesc->vRotSpeed.x;
                        else if (Arr[1] != _pEffectParticleDesc->vRotSpeed.y)
                            Value.vRotSpeed.y = _pEffectParticleDesc->vRotSpeed.y;
                        else if (Arr[2] != _pEffectParticleDesc->vRotSpeed.z)
                            Value.vRotSpeed.z = _pEffectParticleDesc->vRotSpeed.z;

                        iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                    }
                }
                _pEffectParticleDesc->vRotSpeed = _float3(Arr[0], Arr[1], Arr[2]);
            }
            {
                _float Arr[3] = { _pEffectParticleDesc->vRotTranslate.x, _pEffectParticleDesc->vRotTranslate.y, _pEffectParticleDesc->vRotTranslate.z };
                bEnable = ImGui::DragFloat3("RotTranslate", Arr, 0.01f);
                for (auto& iter : m_ParticleList)
                {
                    _uint iIndex = 0;
                    for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                    {
                        if (Arr[0] != _pEffectParticleDesc->vRotTranslate.x)
                            Value.vRotTranslate.x = _pEffectParticleDesc->vRotTranslate.x;
                        else if (Arr[1] != _pEffectParticleDesc->vRotTranslate.y)
                            Value.vRotTranslate.y = _pEffectParticleDesc->vRotTranslate.y;
                        else if (Arr[2] != _pEffectParticleDesc->vRotTranslate.z)
                            Value.vRotTranslate.z = _pEffectParticleDesc->vRotTranslate.z;

                        iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                    }
                }
                _pEffectParticleDesc->vRotTranslate = _float3(Arr[0], Arr[1], Arr[2]);
            }
            bEnable = ImGui::DragFloat("RotLength", &_pEffectParticleDesc->fRotLength, 0.01f);
            if (true == bEnable)
            {
                for (auto& iter : m_ParticleList)
                {
                    _uint iIndex = 0;
                    for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                    {
                        Value.fRotLength = _pEffectParticleDesc->fRotLength;
                        iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                    }
                }
            }
            {
                _float Arr[3] = { _pEffectParticleDesc->vDirection.x, _pEffectParticleDesc->vDirection.y, _pEffectParticleDesc->vDirection.z };
                bEnable = ImGui::DragFloat3("Direction", Arr, 0.01f);
                for (auto& iter : m_ParticleList)
                {
                    _uint iIndex = 0;
                    for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                    {
                        if (Arr[0] != _pEffectParticleDesc->vDirection.x)
                            Value.vDirection.x = _pEffectParticleDesc->vDirection.x;
                        else if (Arr[1] != _pEffectParticleDesc->vDirection.y)
                            Value.vDirection.y = _pEffectParticleDesc->vDirection.y;
                        else if (Arr[2] != _pEffectParticleDesc->vDirection.z)
                            Value.vDirection.z = _pEffectParticleDesc->vDirection.z;

                        iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                    }
                }
                _pEffectParticleDesc->vDirection = _float3(Arr[0], Arr[1], Arr[2]);
            }
            {
                _float Arr[3] = { _pEffectParticleDesc->vSize.x, _pEffectParticleDesc->vSize.y, _pEffectParticleDesc->vSize.z, };
                bEnable = ImGui::DragFloat3("Size", Arr, 0.01f);
                for (auto& iter : m_ParticleList)
                {
                    _uint iIndex = 0;
                    for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                    {
                        if (Arr[0] != _pEffectParticleDesc->vSize.x)
                            Value.vSize.x = _pEffectParticleDesc->vSize.x;
                        else if (Arr[1] != _pEffectParticleDesc->vSize.y)
                            Value.vSize.y = _pEffectParticleDesc->vSize.y;
                        else if (Arr[2] != _pEffectParticleDesc->vSize.z)
                            Value.vSize.z = _pEffectParticleDesc->vSize.z;

                        iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                    }
                }
                _pEffectParticleDesc->vSize = _float3(Arr[0], Arr[1], Arr[2]);
            }
            {
                _float Arr[3] = { _pEffectParticleDesc->vInitPos.x, _pEffectParticleDesc->vInitPos.y, _pEffectParticleDesc->vInitPos.z };
                bEnable = ImGui::DragFloat3("InitPos", Arr, 0.01f);
                if (true == bEnable)
                {
                    for (auto& iter : m_ParticleList)
                    {
                        _uint iIndex = 0;
                        for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                        {
                            if (Arr[0] != _pEffectParticleDesc->vInitPos.x)
                                Value.vInitPos.x = _pEffectParticleDesc->vInitPos.x;
                            else if (Arr[1] != _pEffectParticleDesc->vInitPos.y)
                                Value.vInitPos.y = _pEffectParticleDesc->vInitPos.y;
                            else if (Arr[2] != _pEffectParticleDesc->vInitPos.z)
                                Value.vInitPos.z = _pEffectParticleDesc->vInitPos.z;

                            iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                        }
                    }
                    _pEffectParticleDesc->vInitPos = _float3(Arr[0], Arr[1], Arr[2]);
                }
            }
            {
                bEnable = ImGui::Checkbox("Gravity", &_pEffectParticleDesc->bGravity);
                if (true == bEnable)
                {
                    for (auto& iter : m_ParticleList)
                    {
                        _uint iIndex = 0;
                        for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                        {
                            Value.bGravity = !Value.bGravity;
                            iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                        }
                    }
                }
            }
            {
                bEnable = ImGui::Checkbox("LoopTime", &_pEffectParticleDesc->bLoop);
                if (true == bEnable)
                {
                    for (auto& iter : m_ParticleList)
                    {
                        _uint iIndex = 0;
                        for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                        {
                            Value.bLoop = !Value.bLoop;
                            iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                        }
                    }
                }
            }
        }
        if (ImGui::CollapsingHeader("PlusValue"))
        {
            _bool bEnable = false;
            _float fForce = 0;
            bEnable = ImGui::DragFloat("Force", &_pEffectParticleDesc->fForce, 0.01f);
            {
                if (true == bEnable)
                {
                    for (auto& iter : m_ParticleList)
                    {
                        _uint iIndex = 0;
                        for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                        {
                            Value.fForce += _pEffectParticleDesc->fForce;
                            iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                        }
                    }
                }
            }
            bEnable = ImGui::DragFloat("Acceleration", &_pEffectParticleDesc->fAcceleration, 0.01f);
            {
                if (true == bEnable)
                {
                    for (auto& iter : m_ParticleList)
                    {
                        _uint iIndex = 0;
                        for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                        {
                            Value.fAcceleration += _pEffectParticleDesc->fAcceleration;
                            iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                        }
                    }
                }
            }
            bEnable = ImGui::DragFloat("Mass", &_pEffectParticleDesc->fMass, 0.01f);
            {
                if (true == bEnable)
                {
                    for (auto& iter : m_ParticleList)
                    {
                        _uint iIndex = 0;
                        for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                        {
                            Value.fMass += _pEffectParticleDesc->fMass;
                            iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                        }
                    }
                }
            }
            bEnable = ImGui::DragFloat("JumpValue", &_pEffectParticleDesc->fJumpValue, 0.01f);
            {
                if (true == bEnable)
                {
                    for (auto& iter : m_ParticleList)
                    {
                        _uint iIndex = 0;
                        for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                        {
                            Value.fJumpValue += _pEffectParticleDesc->fJumpValue;
                            iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                        }
                    }
                }
            }
            bEnable = ImGui::DragFloat("JumpTime", &_pEffectParticleDesc->fJumpTime, 0.01f);
            {
                if (true == bEnable)
                {
                    for (auto& iter : m_ParticleList)
                    {
                        _uint iIndex = 0;
                        for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                        {
                            Value.fJumpTime += _pEffectParticleDesc->fJumpTime;
                            iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                        }
                    }
                }
            }
            bEnable = ImGui::DragFloat("JumpTimeFast", &_pEffectParticleDesc->fJumpTimeFast, 0.01f);
            {
                if (true == bEnable)
                {
                    for (auto& iter : m_ParticleList)
                    {
                        _uint iIndex = 0;
                        for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                        {
                            Value.fJumpTimeFast += _pEffectParticleDesc->fJumpTimeFast;
                            iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                        }
                    }
                }
            }
            {
                _float Arr[4] = { _pEffectParticleDesc->vParticleColor.x, _pEffectParticleDesc->vParticleColor.y,
                   _pEffectParticleDesc->vParticleColor.z, _pEffectParticleDesc->vParticleColor.w };
                bEnable = ImGui::DragFloat4("ParticleColor", Arr, 0.01f, -1.f, 1.f);
                for (auto& iter : m_ParticleList)
                {
                    _uint iIndex = 0;
                    for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                    {
                        if (Arr[0] != _pEffectParticleDesc->vParticleColor.x)
                            Value.vParticleColor.x += _pEffectParticleDesc->vParticleColor.x;
                        else if (Arr[1] != _pEffectParticleDesc->vParticleColor.y)
                            Value.vParticleColor.y += _pEffectParticleDesc->vParticleColor.y;
                        else if (Arr[2] != _pEffectParticleDesc->vParticleColor.z)
                            Value.vParticleColor.z += _pEffectParticleDesc->vParticleColor.z;
                        else if (Arr[3] != _pEffectParticleDesc->vParticleColor.w)
                            Value.vParticleColor.w += _pEffectParticleDesc->vParticleColor.w;
                        iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                    }
                }
                _pEffectParticleDesc->vParticleColor = _float4(Arr[0], Arr[1], Arr[2], Arr[3]);
            }
            {
                _float Arr[4] = { _pEffectParticleDesc->vBlendColor.x, _pEffectParticleDesc->vBlendColor.y,
                    _pEffectParticleDesc->vBlendColor.z, _pEffectParticleDesc->vBlendColor.w };
                bEnable = ImGui::DragFloat4("BlendColor", Arr, 0.01f, -1.f, 1.f);
                _pEffectParticleDesc->vBlendColor = _float4(Arr[0], Arr[1], Arr[2], Arr[3]);
                if (true == bEnable)
                {
                    for (auto& iter : m_ParticleList)
                    {
                        _uint iIndex = 0;
                        for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                        {
                            if (Arr[0] != _pEffectParticleDesc->vBlendColor.x)
                                Value.vBlendColor.x += _pEffectParticleDesc->vBlendColor.x;
                            else if (Arr[1] != _pEffectParticleDesc->vBlendColor.y)
                                Value.vBlendColor.y += _pEffectParticleDesc->vParticleColor.y;
                            else if (Arr[2] != _pEffectParticleDesc->vBlendColor.z)
                                Value.vBlendColor.z += _pEffectParticleDesc->vBlendColor.z;
                            else if (Arr[3] != _pEffectParticleDesc->vBlendColor.w)
                                Value.vBlendColor.w += _pEffectParticleDesc->vBlendColor.w;
                            iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                        }
                    }
                }
            }
            {
                bEnable = ImGui::DragFloat("BlendSpeed", &_pEffectParticleDesc->fBlendSpeed, 0.01f);
                _pEffectParticleDesc->fBlendSpeed = _pEffectParticleDesc->fBlendSpeed;
                if (true == bEnable)
                {
                    for (auto& iter : m_ParticleList)
                    {
                        _uint iIndex = 0;
                        for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                        {
                            Value.fBlendSpeed += _pEffectParticleDesc->fBlendSpeed;
                            iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                        }
                    }
                }
            }
            {
                _float Arr[3] = { _pEffectParticleDesc->vRotAnlge.x, _pEffectParticleDesc->vRotAnlge.y, _pEffectParticleDesc->vRotAnlge.z };
                bEnable = ImGui::DragFloat3("RotAnlge", Arr, 0.01f);
                for (auto& iter : m_ParticleList)
                {
                    _uint iIndex = 0;
                    for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                    {
                        if (Arr[0] != _pEffectParticleDesc->vRotAnlge.x)
                            Value.vRotAnlge.x += _pEffectParticleDesc->vRotAnlge.x;
                        else if (Arr[1] != _pEffectParticleDesc->vRotAnlge.y)
                            Value.vRotAnlge.y += _pEffectParticleDesc->vRotAnlge.y;
                        else if (Arr[2] != _pEffectParticleDesc->vRotAnlge.z)
                            Value.vRotAnlge.z += _pEffectParticleDesc->vRotAnlge.z;

                        iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                    }
                }
                _pEffectParticleDesc->vRotAnlge = _float3(Arr[0], Arr[1], Arr[2]);
            }
            {
                _float Arr[3] = { _pEffectParticleDesc->vRotSpeed.x, _pEffectParticleDesc->vRotSpeed.y, _pEffectParticleDesc->vRotSpeed.z };
                bEnable |= ImGui::DragFloat3("RotSpeed", Arr, 0.01f);
                for (auto& iter : m_ParticleList)
                {
                    _uint iIndex = 0;
                    for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                    {
                        if (Arr[0] != _pEffectParticleDesc->vRotSpeed.x)
                            Value.vRotSpeed.x += _pEffectParticleDesc->vRotSpeed.x;
                        else if (Arr[1] != _pEffectParticleDesc->vRotSpeed.y)
                            Value.vRotSpeed.y += _pEffectParticleDesc->vRotSpeed.y;
                        else if (Arr[2] != _pEffectParticleDesc->vRotSpeed.z)
                            Value.vRotSpeed.z += _pEffectParticleDesc->vRotSpeed.z;

                        iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                    }
                }
                _pEffectParticleDesc->vRotSpeed = _float3(Arr[0], Arr[1], Arr[2]);
            }
            {
                _float Arr[3] = { _pEffectParticleDesc->vRotTranslate.x, _pEffectParticleDesc->vRotTranslate.y, _pEffectParticleDesc->vRotTranslate.z };
                bEnable = ImGui::DragFloat3("RotTranslate", Arr, 0.01f);
                for (auto& iter : m_ParticleList)
                {
                    _uint iIndex = 0;
                    for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                    {
                        if (Arr[0] != _pEffectParticleDesc->vRotTranslate.x)
                            Value.vRotTranslate.x += _pEffectParticleDesc->vRotTranslate.x;
                        else if (Arr[1] != _pEffectParticleDesc->vRotTranslate.y)
                            Value.vRotTranslate.y += _pEffectParticleDesc->vRotTranslate.y;
                        else if (Arr[2] != _pEffectParticleDesc->vRotTranslate.z)
                            Value.vRotTranslate.z += _pEffectParticleDesc->vRotTranslate.z;

                        iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                    }
                }
                _pEffectParticleDesc->vRotTranslate = _float3(Arr[0], Arr[1], Arr[2]);
            }
            bEnable = ImGui::DragFloat("RotLength", &_pEffectParticleDesc->fRotLength, 0.01f);
            if (true == bEnable)
            {
                for (auto& iter : m_ParticleList)
                {
                    _uint iIndex = 0;
                    for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                    {
                        Value.fRotLength += _pEffectParticleDesc->fRotLength;
                        iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                    }
                }
            }
            {
                _float Arr[3] = { _pEffectParticleDesc->vDirection.x, _pEffectParticleDesc->vDirection.y, _pEffectParticleDesc->vDirection.z };
                bEnable = ImGui::DragFloat3("Direction", Arr, 0.01f);
                for (auto& iter : m_ParticleList)
                {
                    _uint iIndex = 0;
                    for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                    {
                        if (Arr[0] != _pEffectParticleDesc->vDirection.x)
                            Value.vDirection.x += _pEffectParticleDesc->vDirection.x;
                        else if (Arr[1] != _pEffectParticleDesc->vDirection.y)
                            Value.vDirection.y += _pEffectParticleDesc->vDirection.y;
                        else if (Arr[2] != _pEffectParticleDesc->vDirection.z)
                            Value.vDirection.z += _pEffectParticleDesc->vDirection.z;

                        iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                    }
                }
                _pEffectParticleDesc->vDirection = _float3(Arr[0], Arr[1], Arr[2]);
            }
            {
                _float Arr[3] = { _pEffectParticleDesc->vSize.x, _pEffectParticleDesc->vSize.y, _pEffectParticleDesc->vSize.z, };
                bEnable = ImGui::DragFloat3("Size", Arr, 0.01f);
                for (auto& iter : m_ParticleList)
                {
                    _uint iIndex = 0;
                    for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                    {
                        if (Arr[0] != _pEffectParticleDesc->vSize.x)
                            Value.vSize.x += _pEffectParticleDesc->vSize.x;
                        else if (Arr[1] != _pEffectParticleDesc->vSize.y)
                            Value.vSize.y += _pEffectParticleDesc->vSize.y;
                        else if (Arr[2] != _pEffectParticleDesc->vSize.z)
                            Value.vSize.z += _pEffectParticleDesc->vSize.z;

                        iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                    }
                }
                _pEffectParticleDesc->vSize = _float3(Arr[0], Arr[1], Arr[2]);
            }
            {
                _float Arr[3] = { _pEffectParticleDesc->vInitPos.x, _pEffectParticleDesc->vInitPos.y, _pEffectParticleDesc->vInitPos.z };
                bEnable = ImGui::DragFloat3("InitPos", Arr, 0.01f);
                if (true == bEnable)
                {
                    for (auto& iter : m_ParticleList)
                    {
                        _uint iIndex = 0;
                        for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                        {
                            if (Arr[0] != _pEffectParticleDesc->vInitPos.x)
                                Value.vInitPos.x += _pEffectParticleDesc->vInitPos.x;
                            else if (Arr[1] != _pEffectParticleDesc->vInitPos.y)
                                Value.vInitPos.y += _pEffectParticleDesc->vInitPos.y;
                            else if (Arr[2] != _pEffectParticleDesc->vInitPos.z)
                                Value.vInitPos.z += _pEffectParticleDesc->vInitPos.z;

                            iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                        }
                    }
                    _pEffectParticleDesc->vInitPos = _float3(Arr[0], Arr[1], Arr[2]);
                }
            }
            bEnable = ImGui::DragFloat("LifeTime", &_pEffectParticleDesc->fLifeTime, 0.01f);
            if (true == bEnable)
            {
                for (auto& iter : m_ParticleList)
                {
                    _uint iIndex = 0;
                    for (auto& Value : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                    {
                        Value.fLifeTime += _pEffectParticleDesc->fLifeTime;
                        iter.pVIBufferCom->Set_InstnaceData(Value, iIndex);
                    }
                }
            }
        }
        ImGui::InputInt("Delete Piece Index", _iSelectIndex);
        if (ImGui::Button("Delete Piece"))
        {
            _uint iIndex = 0;
            for (vector< PARTICLEINFO>::iterator it = m_ParticleList.begin(); it != m_ParticleList.end();)
            {
                if (*_iSelectIndex == iIndex)
                {
                    Safe_Release((*it).pVIBufferCom);
                    m_ParticleList.erase(it);
                    break;
                }
                else
                    ++it;
                ++iIndex;
            }
        }
        if (ImGui::Button("Clear Piece"))
        {
            for (auto& iter : m_ParticleList)
                Safe_Release(iter.pVIBufferCom);

            m_ParticleList.clear();
            PARTICLEINFO tInfo;
            tInfo.pVIBufferCom = CVIBuffer_Cube_Instance::Create(m_pDevice, m_pContext);
            m_ParticleList.push_back(tInfo);
        }

        if (ImGui::Button("Save Effect"))
        {
            Save();
        }

        ImGui::InputText("Particle Name", Effect, sizeof(_char) * MAX_PATH);
        strEffectName = Effect;
        m_strPaticleName.assign(strEffectName.begin(), strEffectName.end());

        if (ImGui::DragFloat("LifeTime: ", &m_fLifeTime))
        {
            for (auto& iter : m_ParticleList)
                iter.pVIBufferCom->Reset_Data();
            m_tLifeTimer = CUSTIMER(m_fLifeTime);
            m_tLifeTimer.Reset_Timer();
        }

        ImGui::Checkbox("Loop", &m_bLoop);
        _uint i = 0;
        for (auto& iter : m_ParticleList)
        {
            char Tex[MAX_PATH] = "";
            sprintf_s(Tex, "Particle Piece: %d", i++);

            if (ImGui::TreeNodeEx(Tex))
            {
                ImGui::DragInt("Show Pass", (int*)(&iter.iPassIndex));

                if (ImGui::TreeNode("Particle Info"))
                {
                    if (ImGui::Button("CreateInstance_Many"))
                    {
                        iter.pVIBufferCom->Change_Cube_Count(*_iCreateInstanceCount);
                    }
                    char EffectTex[MAX_PATH] = "";
                    _uint iIndex = 0;

                    EFFECTPARTICLEDESC tDesc;

                    if (ImGui::Button("Plus Particle Instance"))
                        iter.pVIBufferCom->Change_Cube_Count(iter.pVIBufferCom->Get_NumInstance() + 1);

                    ImGui::InputInt("Delete Particle Instnace Index", _iSelectInstanceIndex);

                    if (ImGui::Button("Delete Particle Instnace"))
                    {
                        iter.pVIBufferCom->Delete_Instance(*_iSelectInstanceIndex);
                    }

                    if (ImGui::Button("Clear Particle Instance"))
                        iter.pVIBufferCom->Reset_Cube_Count();

                    for (auto& DESC : iter.pVIBufferCom->Get_CubeInstanceList_Modify())
                    {
                        sprintf_s(EffectTex, "Particle Rect Info: %d", iIndex);
                        _bool bEnable = false;
                        if (ImGui::TreeNode(EffectTex))
                        {
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
                            if (true == bEnable)
                                iter.pVIBufferCom->Set_InstnaceData(DESC, iIndex);

                            ImGui::TreePop();
                        }

                        ++iIndex;
                    }
                    ImGui::TreePop();
                }


                ImGui::TreePop();
            }
        }
    }
}

void CParticle::Save()
{
    _wstring strFilePath = L"..\\..\\Resource\\Json\\Particle\\";
    strFilePath.append(m_strPaticleName);
    strFilePath.append(L".bin");
    HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
    {
        CloseHandle(hFile);
        MSG_BOX("CModel::Load_Data_NonAnim To Failed");
        return;
    }
    DWORD dwByte;
    {
        _uint Length = (_uint)lstrlen(m_strPaticleName.c_str()) + 1;
        WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
        WriteFile(hFile, m_strPaticleName.c_str(), sizeof(_tchar) * Length, &dwByte, 0);
        WriteFile(hFile, &m_v3Rot, sizeof(_float3), &dwByte, 0);
        WriteFile(hFile, &m_v3Scale, sizeof(_float3), &dwByte, 0);
        _uint iSize = (_uint)m_ParticleList.size();
        WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
    }
    for (auto& iter : m_ParticleList)
    {
        if (nullptr != iter.pVIBufferCom)
            iter.pVIBufferCom->Save(hFile, &dwByte);
    }
    WriteFile(hFile, &m_fLifeTime, sizeof(_float), &dwByte, 0);
    WriteFile(hFile, &m_bLoop, sizeof(_bool), &dwByte, 0);

    CloseHandle(hFile);
}

void CParticle::Load(const _wstring& _strLoad)
{
    HANDLE hFile = CreateFile(_strLoad.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
    {
        CloseHandle(hFile);
        MSG_BOX("CEffect::Load To Failed");
        return;
    }
    DWORD dwByte;

    _uint iSize = 0;
    _uint iLength = 0;
    {
        ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
        _tchar* pName = new _tchar[iLength];
        ReadFile(hFile, pName, sizeof(_tchar) * iLength, &dwByte, nullptr);
        m_strPaticleName = pName;
        Safe_Delete_Array(pName);
    }
    ReadFile(hFile, &m_v3Rot, sizeof(_float3), &dwByte, 0);
    ReadFile(hFile, &m_v3Scale, sizeof(_float3), &dwByte, 0);
    ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
    for (_uint i = 0; i < iSize; ++i)
    {
        PARTICLEINFO tInfo;
        tInfo.pVIBufferCom = CVIBuffer_Cube_Instance::Create(m_pDevice, m_pContext);
        tInfo.pVIBufferCom->Load(hFile, &dwByte);
        m_ParticleList.push_back(tInfo);
    }

    ReadFile(hFile, &m_fLifeTime, sizeof(_float), &dwByte, 0);
    m_tLifeTimer = CUSTIMER(m_fLifeTime);
    ReadFile(hFile, &m_bLoop, sizeof(_bool), &dwByte, 0);

    CloseHandle(hFile);
}

HRESULT CParticle::Ready_Comp()
{
    if (FAILED(__super::Ready_Transform(COM_TRANSFORM)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC,
        PROTO_COMP_SHADER_PARTICLE, (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC,
        PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
        return E_FAIL;

	return S_OK;
}

HRESULT CParticle::SetUp_Constant()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (nullptr == m_pTransformCom)
        return E_FAIL;

    BEGIN_SINGLETON(CGameInstance, pGameInstnace)
    {
        _float4x4 ViewMatrix;
        XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

        if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstnace->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstnace->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Set_RawValue("g_OwnerFadeOut", &m_fOwnerFadeOut, sizeof(_float))))
            return E_FAIL;
    }
    END_SINGLETON(pGameInstnace);
    return S_OK;
}

CParticle* CParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticle* pInstance = new CParticle(pDevice, pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CParticle::Create To Failed");
    }
    return pInstance;
}

CParticle* CParticle::Create_LoadFile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& _strFileName)
{
    CParticle* pInstance = new CParticle(pDevice, pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype(_strFileName)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CParticle::Create_LoadFile To Failed");
    }
    return pInstance;
}

CGameObject* CParticle::Clone(void* pArg)
{
    CParticle* pInstance = new CParticle(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEffect::Clone To Failed");
    }
    return pInstance;
}

void CParticle::Free()
{
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pOwnerTransform);

    for (auto& iter : m_ParticleList)
        Safe_Release(iter.pVIBufferCom);

    __super::Free();
}
