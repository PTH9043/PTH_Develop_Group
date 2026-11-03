#pragma once
#include "CBase.h"

enum MATRIXTYPE
{
	MAT_VIEW, MAT_PROJ, MAT_END
};

class CCamera;

class CPipeLine : public CBase {
protected:
	explicit CPipeLine() = default;
	explicit CPipeLine(const CPipeLine& _rhs) :CBase(_rhs) {};
	virtual ~CPipeLine() = default;
public:
	virtual void Free() override;
public:
	HRESULT NativeConstruct() { return S_OK; }
public:
	// Add Camera
	void Add_Camera(const _wstring& _strCamera, const shared_ptr<CCamera>& _pCamera);
	void ChangeCamera(const _wstring& _strCamera);
	// Bind Cam Matrix
	void BindCameraMatrix();
	// ComBine Matrix
	_float4x4 CombineMatrix(const shared_ptr<class CActor>& _pActor);
	void ChangeScreenToPos(_float3& _vPos);
	void ChangeScreenToPos(const shared_ptr<class CActor>& _pActor, _float3& _vPos);
	void ChangeScreenToPos(const _float4x4& _WVP, _float3& _vPos);

	void ClearCamera();
public:
	void Bind_Matrix4x4(const MATRIXTYPE& _eType, const _float4x4& _vMatrix);
	void Bind_Matrix(const MATRIXTYPE& _eType, const _matrix& _matWorld);
	void Set_CamPosition(const _float3& _vCamPos) { this->m_vCamPosition = _vCamPos; }

	_float4x4 Get_Matrix4x4(const MATRIXTYPE& _eType) { return m_Matrixes[_eType]; }
	_matrix Get_Matrix(const MATRIXTYPE& _eType) { return XMLoadFloat4x4(&m_Matrixes[_eType]); }
	const _float3& Get_CamPos() const { return m_vCamPosition; }
private:
	array<_float4x4, MAT_END>												m_Matrixes{};
	_float3																					m_vCamPosition;
	// Cam Maps
	unordered_map<_wstring, weak_ptr<CCamera>>	m_CamMaps{};
	// Camera
	weak_ptr<CCamera>														m_pSelectCamera{ };
};	

