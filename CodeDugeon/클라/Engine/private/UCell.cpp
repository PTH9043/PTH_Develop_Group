#include "EngineDefine.h"
#include "UCell.h"
#include "UVIBufferCell.h"
#include "URenderer.h"
#include "UGameInstance.h"
#include "UTransform.h"
#ifdef _EDIT_NAVI
#include "UDefaultCell.h"
#endif


UCell::UCell(CSHPTRREF<UDevice> _spDevice) : UObject(_spDevice),
m_arrPoints{},
m_arrLines{},
m_arrNormals{},
m_arrNeighbors{ -1, -1, -1 },
m_vCenterPos{},
m_iIndex{ 0 },
m_vPlane{},
m_f3Color{},
m_bisJumpable{ false }
#ifdef _EDIT_NAVI
,m_spCellVIBuffer{ nullptr },
m_spCellPawn{ nullptr }
#endif
{
}

void UCell::Free()
{
}

HRESULT UCell::NativeConstruct()
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(), E_FAIL);

	return S_OK;
}

HRESULT UCell::NativeConstruct(ARRAY<_float3, POINT_END>& _Points, const _uint _iIndex)
{
	m_arrPoints[POINT_A] = _Points[POINT_A];
	m_arrPoints[POINT_B] = _Points[POINT_B];
	m_arrPoints[POINT_C] = _Points[POINT_C];

	m_iIndex = _iIndex;
	ResortPoints();

#ifdef _EDIT_NAVI
	m_spCellVIBuffer = CreateConstructorNative<UVIBufferCell>(GetDevice(), m_arrPoints);
	RETURN_CHECK(nullptr == m_spCellVIBuffer, E_FAIL);
#endif

	_float3 vCenterPos{};
	for (auto& iter : m_arrPoints) {
		m_vCenterPos += iter;
	}
	m_vCenterPos /= 3.f;

#ifdef _EDIT_NAVI
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_spCellPawn = static_pointer_cast<UDefaultCell>(spGameInstance->CloneActorAdd(
		PROTO_ACTOR_DEUBGGINGDEFAULTCELL, { &m_spCellVIBuffer }));

#endif

	return S_OK;
}

HRESULT UCell::NativeConstruct(const CELLDECS& _tCellDesc)
{
	for (_uint i = 0; i < POINT_END; ++i)
	{
		m_arrPoints[i] = _tCellDesc.vPoints[i];
		m_arrNormals[i] = _tCellDesc.vNormal[i];
	}
	for (_uint i = 0; i < LINE_END; ++i)
	{
		m_arrLines[i] = _tCellDesc.vLine[i];
		m_arrNeighbors[i] = _tCellDesc.iNeighbor[i];
	}
	m_iIndex = _tCellDesc.iIndex;
	m_bisJumpable = _tCellDesc.bisJumpable;

	ResortPoints();

#ifdef _EDIT_NAVI
	m_spCellVIBuffer = CreateConstructorNative<UVIBufferCell>(GetDevice(), m_arrPoints);
	RETURN_CHECK(nullptr == m_spCellVIBuffer, E_FAIL);
#endif

	_float3 vCenterPos{};
	for (auto& iter : m_arrPoints) {
		m_vCenterPos += iter;
	}
	m_vCenterPos /= 3.f;

#ifdef _EDIT_NAVI
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_spCellPawn = static_pointer_cast<UDefaultCell>(spGameInstance->CloneActorAdd(
		PROTO_ACTOR_DEUBGGINGDEFAULTCELL, { &m_spCellVIBuffer }));
#endif

	return S_OK;
}

#ifdef _EDIT_NAVI
void UCell::AddCellRenderGroup()
{
	m_spCellPawn->AddRenderer(RENDERID::RI_NONALPHA_LAST);
}

void UCell::ReBuffer()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_spCellVIBuffer.reset();
	m_spCellVIBuffer = CreateConstructorNative<UVIBufferCell>(spGameInstance->GetDevice(), m_arrPoints);
}

void UCell::ReRender()
{
	RETURN_CHECK(nullptr == m_spCellPawn, ;);
	m_spCellPawn->GetVIBuffer().reset();
	m_spCellPawn->SetVIBuffer(m_arrPoints);
}
#endif

_bool UCell::IsIn(const _float3& _vPos, _int& _iNeightborIndex, _float3& _vLine)
{
	for (_uint i = 0; i < LINE_END; ++i) {
		// 변에 대한 점, 법선 벡터, 변을 구성하는 두 점을 가져옵니다.
		_float3 vPointA, vPointB;
		_float3 vNormal = m_arrNormals[i];
		_float3 vLine = m_arrLines[i];

		switch (i) {
		case LINE_AB:
			vPointA = m_arrPoints[POINT_A];
			vPointB = m_arrPoints[POINT_B];
			break;
		case LINE_BC:
			vPointA = m_arrPoints[POINT_B];
			vPointB = m_arrPoints[POINT_C];
			break;
		case LINE_CA:
			vPointA = m_arrPoints[POINT_C];
			vPointB = m_arrPoints[POINT_A];
			break;
		}

		// 점 _vPos와 변의 점들로부터 벡터를 만듭니다.
		_float3 vDir = _vPos - vPointA;

		// 평면 방정식을 사용하여 점이 변의 어느 쪽에 있는지 확인합니다.
		float dotProduct = vDir.Dot(vNormal);
		if (dotProduct > 0) {
			_iNeightborIndex = m_arrNeighbors[i];
			_vLine = vLine;
			return false;
		}
	}

	return true;
}



_bool UCell::IsComparePoints(const _float3& _vPointA, const _float3& _vPointB)
{
	if (DirectX::XMVector3Equal(XMLoadFloat3(&m_arrPoints[POINT_A]), _vPointA))
	{
		RETURN_CHECK(DirectX::XMVector3Equal(m_arrPoints[POINT_B], _vPointB), true);
		RETURN_CHECK(DirectX::XMVector3Equal(m_arrPoints[POINT_C], _vPointB), true);
	}

	if (DirectX::XMVector3Equal(XMLoadFloat3(&m_arrPoints[POINT_B]), _vPointA))
	{
		RETURN_CHECK(DirectX::XMVector3Equal(m_arrPoints[POINT_C], _vPointB), true);
		RETURN_CHECK(DirectX::XMVector3Equal(m_arrPoints[POINT_A], _vPointB), true);
	}

	if (DirectX::XMVector3Equal(XMLoadFloat3(&m_arrPoints[POINT_C]), _vPointA))
	{
		RETURN_CHECK(DirectX::XMVector3Equal(m_arrPoints[POINT_A], _vPointB), true);
		RETURN_CHECK(DirectX::XMVector3Equal(m_arrPoints[POINT_B], _vPointB), true);
	}
	return false;
}

_bool UCell::IsComparePoint(CSHPTRREF<UCell> _pCell)
{
	RETURN_CHECK(nullptr == _pCell, false);
	using namespace DirectX;
	for (_uint i = 0; i < POINT_END; ++i) {
		if (XMVector3Equal(m_arrPoints[i], _pCell->GetPoint(POINT_A)))
		{
			for (_uint j = 0; j < POINT_END; ++j) {
				if (i == j)
					continue;

				RETURN_CHECK(XMVector3Equal(m_arrPoints[j], _pCell->GetPoint(POINT_B)), true);
				RETURN_CHECK(XMVector3Equal(m_arrPoints[j], _pCell->GetPoint(POINT_C)), true);
			}
		}
		if (XMVector3Equal(m_arrPoints[i], _pCell->GetPoint(POINT_B)))
		{
			for (_uint j = 0; j < POINT_END; ++j) {
				if (i == j)
					continue;

				RETURN_CHECK(XMVector3Equal(m_arrPoints[j], _pCell->GetPoint(POINT_A)), true);
				RETURN_CHECK(XMVector3Equal(m_arrPoints[j], _pCell->GetPoint(POINT_C)), true);
			}
		}
		if (XMVector3Equal(m_arrPoints[i], _pCell->GetPoint(POINT_C)))
		{
			for (_uint j = 0; j < POINT_END; ++j) {
				if (i == j)
					continue;

				RETURN_CHECK(XMVector3Equal(m_arrPoints[j], _pCell->GetPoint(POINT_B)), true);
				RETURN_CHECK(XMVector3Equal(m_arrPoints[j], _pCell->GetPoint(POINT_C)), true);
			}
		}

	}
	return false;
}

const _float UCell::ComputeHeight(const _float3& _vPosition)
{
	return _float3(DirectX::XMPlaneDotCoord(m_vPlane, _vPosition)).y + _vPosition.y;
}


void UCell::CalculateCrossResult(ARRAY<_float3, POINT_END>& _arrPointsEnd)
{
	_float3 p1{ m_arrPoints[POINT_A].x, 0.f, m_arrPoints[POINT_A].z };
	_float3 p2{ m_arrPoints[POINT_B].x, 0.f, m_arrPoints[POINT_B].z };
	_float3 p3{ m_arrPoints[POINT_C].x, 0.f, m_arrPoints[POINT_C].z };

	_float3 L1 = p2 - p1;
	_float3 L2 = p3 - p2;
	_float3 L3 = p1 - p3;

	_float3 vCross1 = DirectX::XMVector3Cross(L1, L2);
	_float3 vCross2 = DirectX::XMVector3Cross(L2, L3);
	_float3 vCross3 = DirectX::XMVector3Cross(L3, L1);

	_arrPointsEnd[POINT_A] = vCross1;
	_arrPointsEnd[POINT_B] = vCross2;
	_arrPointsEnd[POINT_C] = vCross3;
}

void UCell::ResortPoints()
{
	// Resort Points
	// 같은 시계 방향으로 점들이 배치되도록 하는 것
	bool bIsTrue = true;
	while (bIsTrue) {
		ARRAY<_float3, POINT_END> arrPosList;
		CalculateCrossResult(arrPosList);

		if (arrPosList[0].y < 0)
		{
			std::swap(m_arrPoints[POINT_A], m_arrPoints[POINT_B]);
			continue;
		}
		if (arrPosList[1].y < 0)
		{
			std::swap(m_arrPoints[POINT_B], m_arrPoints[POINT_C]);
			continue;
		}
		if (arrPosList[2].y < 0)
		{
			std::swap(m_arrPoints[POINT_C], m_arrPoints[POINT_A]);
			continue;
		}
		bIsTrue = false;
	}
	MakeLineAndNormal();
}

void UCell::MakeLineAndNormal()
{
	m_arrLines[LINE_AB] = m_arrPoints[POINT_B] - m_arrPoints[POINT_A];
	m_arrLines[LINE_BC] = m_arrPoints[POINT_C] - m_arrPoints[POINT_B];
	m_arrLines[LINE_CA] = m_arrPoints[POINT_A] - m_arrPoints[POINT_C];

	m_arrNormals[LINE_AB] = _float3(m_arrLines[LINE_AB].z * -1.f, 0.f, m_arrLines[LINE_AB].x);
	m_arrNormals[LINE_BC] = _float3(m_arrLines[LINE_BC].z * -1.f, 0.f, m_arrLines[LINE_BC].x);
	m_arrNormals[LINE_CA] = _float3(m_arrLines[LINE_CA].z * -1.f, 0.f, m_arrLines[LINE_CA].x);

	m_arrNormals[LINE_AB].Normalize();
	m_arrNormals[LINE_BC].Normalize();
	m_arrNormals[LINE_CA].Normalize();
	m_vPlane = DirectX::XMPlaneFromPoints(m_arrPoints[POINT_B], m_arrPoints[POINT_A], m_arrPoints[POINT_C]);
}

_float3 UCell::GetClosestPointOnEdges(const _float3& position) const
{
	_float3 closestPoint = position;
	_float minDistanceSquared = FLT_MAX;

	// 각 변에 대해 가장 가까운 점을 찾음
	for (size_t i = 0; i < LINE_END; ++i) {
		_float3 pointOnEdge = ClosestPointOnLine(m_arrPoints[i], m_arrPoints[(i + 1) % LINE_END], position);
		_float distanceSquared = DirectX::PTH::Vector3::DistanceSquared(position, pointOnEdge);
		if (distanceSquared < minDistanceSquared) {
			minDistanceSquared = distanceSquared;
			closestPoint = pointOnEdge;
		}
	}

	return closestPoint;
}

_float3 UCell::ClosestPointOnLine(const _float3& lineStart, const _float3& lineEnd, const _float3& point) const
{
	// 라인 시작점부터 끝점까지의 벡터
	_float3 lineDirection = lineEnd - lineStart;

	// 라인 시작점부터 입력된 점까지의 벡터
	_float3 pointToLineStart = point - lineStart;

	// 라인 방향과 입력된 점까지의 벡터 사이의 내적 값
	float t = DirectX::PTH::Vector3::Dot(pointToLineStart, lineDirection) / DirectX::PTH::Vector3::Dot(lineDirection, lineDirection);

	// t 값을 사용하여 입력된 점에서 가장 가까운 점을 계산
	_float3 closestPoint = lineStart + lineDirection * t;


	if (t > 1.0f) {
		closestPoint = lineEnd;
	}

	else if (t < 0.0f) {
		closestPoint = lineStart;
	}


	return closestPoint;
}

_float UCell::GetHeightAtXZ(const _float& x, const _float& z) const {

	// Points of the triangle
	_float3 P1 = m_arrPoints[POINT_A];
	_float3 P2 = m_arrPoints[POINT_B];
	_float3 P3 = m_arrPoints[POINT_C];

	// Calculate the plane equation coefficients
	_float A = (P2.y - P1.y) * (P3.z - P1.z) - (P2.z - P1.z) * (P3.y - P1.y);
	_float B = (P2.z - P1.z) * (P3.x - P1.x) - (P2.x - P1.x) * (P3.z - P1.z);
	_float C = (P2.x - P1.x) * (P3.y - P1.y) - (P2.y - P1.y) * (P3.x - P1.x);
	_float D = -(A * P1.x + B * P1.y + C * P1.z);

	_float y = -(A * x + C * z + D) / B;

	return y;
}

_float3 UCell::GetXZatHeight(const _float& y) const {
	// Points of the triangle
	_float3 P1 = m_arrPoints[0];
	_float3 P2 = m_arrPoints[1];
	_float3 P3 = m_arrPoints[2];

	// Calculate the plane equation coefficients
	_float A = (P2.y - P1.y) * (P3.z - P1.z) - (P2.z - P1.z) * (P3.y - P1.y);
	_float B = (P2.z - P1.z) * (P3.x - P1.x) - (P2.x - P1.x) * (P3.z - P1.z);
	_float C = (P2.x - P1.x) * (P3.y - P1.y) - (P2.y - P1.y) * (P3.x - P1.x);
	_float D = -(A * P1.x + B * P1.y + C * P1.z);

	// Choose arbitrary values for x and z
	_float x = P1.x;
	_float z = P1.z;

	// Calculate x and z based on the given y
	if (B != 0) {
		_float x = P1.x;
		_float z = -(A * x + B * y + D) / C;
	}
	else {
		// If B is zero, we need another approach
		// Choose arbitrary values for y and solve for z
		if (A != 0) {
			x = -(B * y + C * z + D) / A;
		}
		else if (C != 0) {
			z = -(A * x + B * y + D) / C;
		}
	}

	return _float3(x, y, z);
}