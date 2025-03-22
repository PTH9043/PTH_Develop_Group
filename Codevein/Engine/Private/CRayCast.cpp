#include "CRayCast.h"
#include "CGameInstance.h"
#include "CGameObject.h"

IMPLEMENT_SINGLETON(CRayCast)


CRayCast::CRayCast()
{
}

void CRayCast::Free()
{
}



//
//_bool CRayCast::ShootRayCast_Circle(const _fvector& _v4ShootPos, const _fvector& _v4Dir, 
//    const _float& _fDistance, const _fvector& _v4TargetPos, const _float& _fRadius, _float3* _pOut)
//{	
//	// 구의 경계 (X - C) * (X - C) = r * 2 (X는 식, C는 중심 좌표, r은 반지름)
//	// X 에 (R(t) = P + tD (P시작 위치, t 거리, D 방향)
//	//  P + tD - C * P + td - C = r* 2 (P - C)를 M으로 치환
//	//  (M + tD) * (M + td) = r * 2
//	//  (td * td) + 2(td * M) * (M * M) - r * 2 = 0 (d는 단위 백터 = 1)
//	// M * M + 2(tM) - (r * 2) + (t * t)
//	// 근의 공식 b * b - a * c <= 0 을 이용
//    _vector v4Dis = _v4ShootPos - _v4TargetPos;
//    _float b = XMVectorGetX(XMVector3Dot(v4Dis, _v4Dir));
//    _float c = XMVectorGetX(XMVector3Dot(v4Dis, v4Dis)) - (_fRadius * _fRadius);
//	// 속도 저하 우려
//	if (c > 0.f && b > 0.f)
//		return false;
//
//	_float fJudge = b * b - c;
//	if (fJudge < 0.f)
//		return false;
//
//	_float d = sqrtf(fJudge);
//	if (fJudge < 0.f)
//		return false;
//
//	_float t1 = -b - d;
//	_float t2 = -b - d;
//
//	if (t1 >= 0.f && t1 < _fDistance)
//		return true;
//
//	if (t2 >= 0.f && t2 < _fDistance)
//		return true;
//
//	return false;
//}
//
//_bool CRayCast::ShootRayCast_Circle(const _fvector& _v3ShootPos, const _fvector& _v3EndPos, const _fvector& _v3TargetPos, const _float& _fRadius, _gvector* _pOut)
//{
//	_vector v3Dis = _v3ShootPos - _v3TargetPos;
//	_vector _v3Dis = _v3ShootPos - _v3EndPos;
//	_vector _v3Dir = XMVector3Length(_v3Dis);
//
//	_float b = XMVectorGetX(XMVector3Length(XMVector3Dot(v3Dis, _v3Dir)));
//	_float c = XMVectorGetX(XMVector3Length(XMVector3Dot(v3Dis, v3Dis)));
//
//	if (c > 0.f && b > 0.f)
//		return false;
//
//	_float fJudge = b * b - c;
//	if (fJudge < 0.f)
//		return false;
//
//	_float d = sqrtf(fJudge);
//	_float t1 = b - d;
//	_float t2 = -b - d;
//	_float fDistance = XMVectorGetX(XMVector3Length(_v3Dir));
//
//	if (t1 >= 0.f && t1 < fDistance)
//		return true;
//
//	if (t2 > 0.f && t2 < fDistance)
//		return true;
//
//	return false;
//}
//
//_bool CRayCast::ShootRayCast_OBBCube(const _fvector& _v3ShootPos, const _fvector& _v3Dir,
//	const _float& _fDistance, const _fvector& _v3Size, const _matrix& matWorldInv)
//{
//	_vector v3ShootPos = _v3ShootPos;
//	_vector v3EndPos = _v3ShootPos + _v3Dir * _fDistance;
//
//	XMVector3TransformCoord(v3ShootPos, matWorldInv);
//	XMVector3TransformCoord(v3EndPos, matWorldInv);
//
//	_float3 v3Zero = _float3(0.f, 0.f, 0.f);
//	return LineToAABB(v3ShootPos, v3EndPos, XMLoadFloat3(&v3Zero), _v3Size);
//}
//
//_bool CRayCast::ShootRayCast_ABBCube(const _fvector& _v3ShootPos, const _fvector& _v3Dir, const _float& _fDistance, const _fvector& _v3TargetPos, const _gvector& _v3Size)
//{
//	_vector v3EndPos = _v3ShootPos + _v3Dir * _fDistance;
//	return LineToAABB(_v3ShootPos, v3EndPos, _v3TargetPos, _v3Size);
//}
//
//_bool CRayCast::ShootRayCast_OBBCube(const _fvector& _v3ShootPos, const _fvector& _v3EndPos, 
//	const _fvector& _v3Size, const _matrix& _matWorldInv)
//{	// 발사 위치
//	_vector v3ShootPos = _v3ShootPos;
//	_vector v3EndPos = _v3EndPos;
//	XMVector3TransformCoord(v3ShootPos, _matWorldInv);
//	XMVector3TransformCoord(v3EndPos, _matWorldInv);
//
//	_float3 v3Zero = _float3(0.f, 0.f, 0.f);
//	return LineToAABB(v3ShootPos, v3EndPos, XMLoadFloat3(&v3Zero), _v3Size);
//}
//
//_bool CRayCast::ShootRayCast_ABBCube(const _fvector& _v3ShootPos, const _fvector& _v3EndPos, const _fvector& _v3TargetPos, const _gvector& _v3Size)
//{
//	// 발사 위치
//	return LineToAABB(_v3ShootPos, _v3EndPos, _v3TargetPos, _v3Size);
//}
//
//_bool CRayCast::LineToAABB(const _fvector& _v3ShootPos, const _fvector& _v3EndPos, const _fvector& _v3TargetPos, const _gvector& _v3Size)
//{
//	_vector vMin = _v3TargetPos - _v3Size * 0.5f;
//	_vector vMax = _v3TargetPos + _v3Size * 0.5f;
//	_vector vDisToLine = _v3EndPos - _v3ShootPos;
//
//	_float3 v3ShootPos, v3DisToLine, v3Min, v3Max;
//
//	XMStoreFloat3(&v3ShootPos, _v3ShootPos);
//	XMStoreFloat3(&v3DisToLine, vDisToLine);
//	XMStoreFloat3(&v3Min, vMin);
//	XMStoreFloat3(&v3Max, vMax);
//
//	_float fMinValue = 0.f, fMaxValue = 0.f;
//	// X 영역 확인
//	if (v3DisToLine.x == 0.f)
//	{
//		if ((v3ShootPos.x < v3Min.x) || (v3ShootPos.x > v3Max.x))
//			return false;
//		fMinValue = 0.f;
//		fMaxValue = 1.f;
//	}
//	else
//	{
//		_float checkT1 = (v3Min.x - v3ShootPos.x) / (v3DisToLine.x);
//		_float checkT2 = (v3Max.x - v3ShootPos.x) / (v3DisToLine.x);
//		if (checkT1 < checkT2)
//		{
//			fMinValue = checkT1;
//			fMaxValue = checkT2;
//		}
//		else
//		{
//			fMinValue = checkT1;
//			fMaxValue = checkT2;
//		}
//	}
//	if ((fMaxValue < 0.f) || (fMinValue > 1.f))
//		return false;
//
//	if (v3DisToLine.y == 0.f)
//	{
//		if ((v3ShootPos.y < v3Min.y) || (v3ShootPos.y > v3Max.y))
//			return false;
//		fMinValue = 0.f;
//		fMaxValue = 1.f;
//	}
//	else
//	{
//		_float checkT1 = (v3Min.y - v3ShootPos.y) / (v3DisToLine.y);
//		_float checkT2 = (v3Max.y - v3ShootPos.y) / (v3DisToLine.y);
//		if (checkT1 < checkT2)
//		{
//			fMinValue = checkT1;
//			fMaxValue = checkT2;
//		}
//		else
//		{
//			fMinValue = checkT1;
//			fMaxValue = checkT2;
//		}
//	}
//	if ((fMaxValue < 0.f) || (fMinValue > 1.f))
//		return false;
//	if (v3DisToLine.z == 0.f)
//	{
//		if ((v3ShootPos.z < v3Min.z) || (v3ShootPos.z > v3Max.z))
//			return false;
//		fMinValue = 0.f;
//		fMaxValue = 1.f;
//	}
//	else
//	{
//		_float checkT1 = (v3Min.z - v3ShootPos.z) / (v3DisToLine.z);
//		_float checkT2 = (v3Max.z - v3ShootPos.z) / (v3DisToLine.z);
//		if (checkT1 < checkT2)
//		{
//			fMinValue = checkT1;
//			fMaxValue = checkT2;
//		}
//		else
//		{
//			fMinValue = checkT1;
//			fMaxValue = checkT2;
//		}
//	}
//	if ((fMaxValue < 0.f) || (fMinValue > 1.f))
//		return false;
//
//	return true;
//}