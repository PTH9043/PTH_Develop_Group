#include "CTransform.h"
#include "CMethod.h"

shared_ptr<CTransform> CTransform::Create()
{
    shared_ptr<CTransform> pInstance = CSmartPointerFactory<CTransform>::CreateShared();
    if (FAILED(pInstance->NativeConstruct()))
    {
        Safe_Free(pInstance);
        DEBUG_MSG("CTransform::Create To Failed");
    }
    return pInstance;
}

void CTransform::Free()
{
    __super::Free();
}

HRESULT CTransform::NativeConstruct()
{
    return S_OK;
}

HRESULT CTransform::NativeConstruct(VOIDDATAS _tDatas){
    return S_OK;
}

void CTransform::Tick(const _double& _dTimeDelta) {
    UpdateMatrix();
}

void CTransform::Move_Forward(const _double& _dTimeDelta, const _float& _fSpeed)
{
    _vector vPosition = Get_Pos();
    _vector vLook = Get_Look();

    vPosition += XMVector3Normalize(vLook) * (_float)(_dTimeDelta * _fSpeed);

    _float fY = 0.f;
    vPosition = XMVectorSetY(vPosition, fY);
    Set_Pos(vPosition);
}

void CTransform::Move_Back(const _double& _dTimeDelta, const _float& _fSpeed)
{
    _vector vPosition = Get_Pos();
    _vector vLook = Get_Look();

    vPosition -= XMVector3Normalize(vLook) * (_float)(_dTimeDelta * _fSpeed);
    _float fY = 0.f;
    vPosition = XMVectorSetY(vPosition, fY);
    Set_Pos(vPosition);
}

void CTransform::Move_Left(const _double& _dTimeDelta, const _float& _fSpeed)
{
    _vector vPosition = Get_Pos();
    _vector vRight = Get_Right();

    vPosition -= XMVector3Normalize(vRight) * (_float)(_dTimeDelta * _fSpeed);
    _float fY = 0.f;
    vPosition = XMVectorSetY(vPosition, fY);
    Set_Pos(vPosition);
}

void CTransform::Move_Right(const _double& _dTimeDelta, const _float& _fSpeed)
{
    _vector vPosition = Get_Pos();
    _vector vRight = Get_Right();

    vPosition += XMVector3Normalize(vRight) * (_float)(_dTimeDelta * _fSpeed);
    _float fY = 0.f;
    vPosition = XMVectorSetY(vPosition, fY);
    Set_Pos(vPosition);
}

void CTransform::Move_Pos(const _fvector& _v4TargetPos)
{
    _vector vPosition = _v4TargetPos;
    _float fY = 0.f;
    vPosition = XMVectorSetY(vPosition, fY);
    Set_Pos(vPosition);
}


void CTransform::Translate_Pos(const _fvector& _v4Pos, const _double& _dTimeDelta, const _float& _fSpeed, const _float& _fLimitDistance)
{
    if (_fLimitDistance <= Compute_Distance(_v4Pos))
    {
        _vector vPosition = Get_Pos();
        _vector vDir = _v4Pos - vPosition;
        vPosition += XMVector3Normalize(vDir) * (_float)(_dTimeDelta * _fSpeed);
        Set_Pos(vPosition);
    }
}

void CTransform::Translate_Dir(const _fvector& _v4Dir, const _double& _dTimeDelta, const _float& _fSpeed)
{
    _vector vPosition = Get_Pos();
    vPosition += XMVector3Normalize(_v4Dir) * (_float)(_dTimeDelta * _fSpeed);
    Set_Pos(vPosition);
}

void CTransform::Translate_Trans(const shared_ptr<CTransform>& _pTransform, const _double& _dTimeDelta, const _float& _fSpeed, const _float& _fLimitDistance)
{
    if (nullptr == _pTransform)
        return;
    if (_fLimitDistance <= Compute_Distance(_pTransform->Get_Pos()))
    {
        _vector vPosition = Get_Pos();
        _vector vDir = _pTransform->Get_Pos() - vPosition;
        vPosition += XMVector3Normalize(vDir) * (_float)(_dTimeDelta * _fSpeed);
        Set_Pos(vPosition);
    }
}

void CTransform::Translate_PosNotY(const _fvector& _v4Pos, const _double& _dTimeDelta, const _float& _fSpeed, const _float& _fLimitDistance)
{
    if (_fLimitDistance <= Compute_Distance(_v4Pos))
    {
        _vector vPosition = Get_Pos();
        _vector vDir = _v4Pos - vPosition;
        vPosition += XMVector3Normalize(vDir) * (_float)(_dTimeDelta * _fSpeed);
        _float fY = 0.f;
        vPosition = XMVectorSetY(vPosition, fY);
        Set_Pos(vPosition);
    }
}

void CTransform::Translate_DirNotY(const _fvector& _v4Dir, const _double& _dTimeDelta, const _float& _fSpeed)
{
    _vector vPosition = Get_Pos();
    vPosition += XMVector3Normalize(_v4Dir) * (_float)(_dTimeDelta * _fSpeed);
    _float fY = 0.f;
    vPosition = XMVectorSetY(vPosition, fY);
    Set_Pos(vPosition);
}

void CTransform::Translate_TransNotY(const shared_ptr<CTransform>& _pTransform, const _double& _dTimeDelta, const _float& _fSpeed, const _float& _fLimitDistance)
{
    if (nullptr == _pTransform)
        return;
    if (_fLimitDistance <= Compute_Distance(_pTransform->Get_Pos()))
    {
        _vector vPosition = Get_Pos();
        _vector vDir = _pTransform->Get_Pos() - vPosition;
        vPosition += XMVector3Normalize(vDir) * (_float)(_dTimeDelta * _fSpeed);
        _float fY = 0.f;
        vPosition = XMVectorSetY(vPosition, fY);
        Set_Pos(vPosition);
    }
}

void CTransform::Rotate_Turn(const _fvector& _vAxis, const _double& _dTimeDelta, const _float& _fSpeed)
{
    _matrix RotationMatrix = XMMatrixRotationAxis(_vAxis, (_float)(_dTimeDelta * _fSpeed));

    _vector		vRight = Get_Right();
    _vector		vUp = Get_Up();
    _vector		vLook = Get_Look();

    // XMVector4Transform();
    Set_Right(XMVector3TransformNormal(vRight, RotationMatrix));
    Set_Up(XMVector3TransformNormal(vUp, RotationMatrix));
    Set_Look(XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotate_Turn(const _float3& _vAxis, const _double& _dTimeDelta, _float _fSpeed) {
    Rotate_Turn(XMLoadFloat3(&_vAxis), _dTimeDelta, _fSpeed);
}

void CTransform::Rotate_Fix(const _fvector& _vAxis, const _float& _fAngle)
{
    _matrix RotationMatrix;
    RotationMatrix = XMMatrixRotationAxis(_vAxis, XMConvertToRadians(_fAngle));
    _float3 v3Scale = Get_Scale();
    _float3 v3Right = _float3(1.f, 0.f, 0.f), v3Up = _float3(0.f, 1.f, 0.f), v3Look = _float3(0.f, 0.f, 1.f);
    _vector vRight = XMLoadFloat3(&v3Right), vUp = XMLoadFloat3(&v3Up), vLook = XMLoadFloat3(&v3Look);

    vRight = XMVector3TransformNormal(vRight * v3Scale.x, RotationMatrix);
    vUp = XMVector3TransformNormal(vUp * v3Scale.y, RotationMatrix);
    vLook = XMVector3TransformNormal(vLook * v3Scale.z, RotationMatrix);
    // XMVector4Transform();
    Set_Right(vRight);
    Set_Up(vUp);
    Set_Look(vLook);
}

void CTransform::Rotate_Fix(const _float3& _vAngle)
{
    _float3 v3Angle = _vAngle;

    v3Angle.x = XMConvertToRadians(v3Angle.x);
    v3Angle.y = XMConvertToRadians(v3Angle.y);
    v3Angle.z = XMConvertToRadians(v3Angle.z);
    _matrix RotationMatrix =
        XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&v3Angle));
    _float3 v3Scale = Get_Scale();
    _float3 v3Right = _float3(1.f, 0.f, 0.f), v3Up = _float3(0.f, 1.f, 0.f), v3Look = _float3(0.f, 0.f, 1.f);
    _vector		vRight = XMLoadFloat3(&v3Right) * v3Scale.x,
        vUp = XMLoadFloat3(&v3Up) * v3Scale.y, vLook = XMLoadFloat3(&v3Look) * v3Scale.z;

    vRight = XMVector3TransformNormal(vRight, RotationMatrix);
    vUp = XMVector3TransformNormal(vUp, RotationMatrix);
    vLook = XMVector3TransformNormal(vLook, RotationMatrix);

    Set_Right(vRight);
    Set_Up(vUp);
    Set_Look(vLook);
}

void CTransform::Chase(const _fvector& _vTargetPos, const _double& _dTimeDelta, const _float& _fSpeed, const _float& _fLimitDistance)
{
    LookAt(_vTargetPos);

    if (_fLimitDistance <= Compute_Distance(_vTargetPos))
        Move_Forward(_dTimeDelta, _fSpeed);
}

void CTransform::LookAt(const _fvector& _vTargetPos)
{
    _vector vPosition = Get_Pos();
    _vector vLook = XMVector3Normalize(_vTargetPos - vPosition);

    _float3 Look = vLook;

    if (isnan(Look.x) || isnan(Look.y) || isnan(Look.z))
        return;

    if (CMethod::Is_Vector_Zero(vLook))
        return;

    _vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
    _vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
    _float3 vScale = Get_Scale();

    Set_Right(vRight);
    Set_Up(vUp);
    Set_Look(vLook);

    Set_Scale(vScale);
}


_float CTransform::Compute_Distance(const _fvector& _vTargetPos)
{
    return XMVectorGetX(XMVector3Length(Get_Pos() - _vTargetPos));
}

const _float3 CTransform::Get_Scale() const
{
    _float3 v3Scale;

    v3Scale.x =  XMVectorGetX(XMVector3Length(Get_Right()));
    v3Scale.y = XMVectorGetY(XMVector3Length(Get_Up()));
    v3Scale.z = XMVectorGetZ(XMVector3Length(Get_Look()));

    return v3Scale;
}

const _float3 CTransform::Get_ParentsScale() const
{
    _float3 v3Scale;

    v3Scale.x = XMVectorGetX(XMVector3Length(Get_OriginRight()));
    v3Scale.y = XMVectorGetY(XMVector3Length(Get_OriginUp()));
    v3Scale.z = XMVectorGetZ(XMVector3Length(Get_OriginLook()));

    return v3Scale;
}

const _float4x4 CTransform::Get_WorldFloat4x4_TP() const
{
    _float4x4		TransposeMatrix;
    XMStoreFloat4x4(&TransposeMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix)));
    return TransposeMatrix;
}

const _float4x4 CTransform::Get_WorldFloat4x4Inv() const
{
    _float4x4 WorldInv;
    XMStoreFloat4x4(&WorldInv, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
    return WorldInv;
}

const _float4x4 CTransform::Get_ParentsToWorldFloat4x4() const
{
    if (nullptr == m_pParentsTransform.lock())
        return Get_WorldFloat4x4();

    return m_ChangeWorldMatrix;
}

const _matrix CTransform::Get_ParentsToWorldMatrix() const
{
    if (nullptr == m_pParentsTransform.lock())
        return Get_WorldMatrix();

    _float4x4 Matrix = Get_ParentsToWorldFloat4x4();
    return XMLoadFloat4x4(&Matrix);
}

const _float4x4 CTransform::Get_ParentsToWorldFloat4x4_TP() const
{
    if (nullptr == m_pParentsTransform.lock())
        return Get_WorldFloat4x4();

    _float4x4 WorldTp;
    XMStoreFloat4x4(&WorldTp, XMMatrixTranspose(Get_ParentsToWorldMatrix()));
    return WorldTp;
}

const _float4x4 CTransform::Get_ParentsToWorldFloat4x4Inv() const
{
    if (nullptr == m_pParentsTransform.lock())
        return Get_WorldFloat4x4Inv();

    _float4x4 WorldInv;
    XMStoreFloat4x4(&WorldInv, XMMatrixInverse(nullptr, Get_ParentsToWorldMatrix()));
    return WorldInv;
}

const _matrix CTransform::Get_ParentsToWorldMatrixInv() const
{
    if (nullptr == m_pParentsTransform.lock())
        return Get_WorldMatrixInv();

    return XMMatrixInverse(nullptr, Get_ParentsToWorldMatrix());
}

void CTransform::Set_Scale(const _float3& _vScale)
{
    _vector		vRight = Get_Right();
    _vector		vUp = Get_Up();
    _vector		vLook = Get_Look();

    vRight = XMVector3Normalize(vRight) * _vScale.x;
    vUp = XMVector3Normalize(vUp) * _vScale.y;
    vLook = XMVector3Normalize(vLook) * _vScale.z;

    Set_Right(vRight);
    Set_Up(vUp);
    Set_Look(vLook);
}

void CTransform::Bind_WorldMatrix(const _matrix& _fMatrix)
{
    _float3 vScale = Get_Scale();
    _float4x4 vMatrix;
    XMStoreFloat4x4(&vMatrix, _fMatrix);

    if (vScale.x == 0)
        vScale.x = 1;

    if (vScale.y == 0)
        vScale.y = 1;

    if (vScale.z == 0)
        vScale.z = 1;

    _vector vRight = XMLoadFloat4((_float4*)&vMatrix.m[STATE_RIGHT][0]);
    _vector vUp = XMLoadFloat4((_float4*)&vMatrix.m[STATE_UP][0]);
    _vector vLook = XMLoadFloat4((_float4*)&vMatrix.m[STATE_LOOK][0]);
    _vector vPos = XMLoadFloat4((_float4*)&vMatrix.m[STATE_POSITION][0]);

    Set_Right(XMVector3Normalize(vRight) * vScale.x);
    Set_Up(XMVector3Normalize(vUp) * vScale.y);
    Set_Look(XMVector3Normalize(vLook) * vScale.z);
    Set_Pos(vPos);
}

const _float CTransform::NearToDistance(const _float3& _vPos)
{
    _float3 CurPos = Get_PosF3();
    return CurPos.Distance(_vPos);
}

const _float CTransform::NearToDistanceSq(const _float3& _vPos)
{
    _float3 CurPos = Get_PosF3();
    return CurPos.DistanceSquared(_vPos);
}

void CTransform::UpdateMatrix(){

    if (nullptr != m_pParentsTransform.lock()) {

        _matrix Matirx = m_pParentsTransform.lock()->Get_ParentsToWorldMatrix();

        if (true == m_bNotApplyScale) {
            Matirx = CMethod::XMMatrixSetScaling(BASIC_SCALE, Matirx);
        }
        if (true == m_bNotApplyPos) {
            Matirx = CMethod::XMMatrixSetFloat3(VECTOR3_ZERO,  MATROW_POS ,Matirx);
        }
        if (true == m_bNotApplyRotate) {
            Matirx = CMethod::XMMatrixSetRotationFix(VECTOR3_ZERO ,Matirx);
        }

        XMStoreFloat4x4(&m_ChangeWorldMatrix, XMLoadFloat4x4(&m_WorldMatrix) * 
            Matirx);

    }
    else {
        XMStoreFloat4x4(&m_ChangeWorldMatrix, XMLoadFloat4x4(&m_WorldMatrix));
    }
}

void CTransform::Set_ParentsTransform(const weak_ptr<CTransform>& _pWeakPtr)
{
    if (nullptr == _pWeakPtr.lock())
        return;

    m_pParentsTransform = _pWeakPtr;
    UpdateMatrix();
}
