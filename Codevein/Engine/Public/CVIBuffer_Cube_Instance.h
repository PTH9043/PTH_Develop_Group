#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cube_Instance final : public CVIBuffer
{
private:
	explicit CVIBuffer_Cube_Instance(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CVIBuffer_Cube_Instance(const CVIBuffer_Cube_Instance& _rhs);
	virtual ~CVIBuffer_Cube_Instance() = default;
public:
	const _uint& Get_StrideInstance() const { return m_iStrideInstance; }
	const _uint& Get_NumInstance() const { return m_iNumInstance; }
public:
	const vector<EFFECTPARTICLEDESC>& Get_CubeInstanceList_Info() { return m_CubeInstanceList; }
	vector<EFFECTPARTICLEDESC>& Get_CubeInstanceList_Modify() { return m_CubeInstanceList; }

	void Get_Data(VTXCUBEINSTANCE** _ppData, const _uint& _iIndex = 0);
	void Set_Data(VTXCUBEINSTANCE* _pData, const _uint& _iIndex = 0);

	EFFECTPARTICLEDESC* Get_InstanceData(const _uint& _iIndex);
	void Set_InstnaceData(const EFFECTPARTICLEDESC& _tInfo, const _uint& _iIndex);
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual HRESULT Render() override;

	HRESULT Change_Cube_Count(const _uint& _iNumInstance);
	HRESULT Reset_Cube_Count();
	// Delte Instancew
	void Delete_Instance(const _uint& _iIndex);
public:
	void Update(class CTransform* _pTransform, const _double& _dTimeDelta);
	void Late_Update(class CTransform* _pTransform, const _double& _dTimeDelta);
protected:
	void Update_Dir(class CTransform* _pTransform, const _double& _dTimeDelta);
public:
	void Reset_Data();
	// Save Load
	void Save(const HANDLE& _hFile, DWORD* _dwByte);
	void Load(const HANDLE& _hFile, DWORD* _dwByte);
protected:
	HRESULT Create_VertexBuffer_Instance();
	HRESULT Create_VertexBuffer_Instance(const _uint& _iNum);
	HRESULT Create_IndexBuffer_Instance(const _uint& _iNum);
private:
	vector<EFFECTPARTICLEDESC>		m_CubeInstanceList;
	ID3D11Buffer*											m_pVBInstance = nullptr;
	_uint															m_iStrideInstance = 0;
	_uint															m_iNumInstance = 0;
public:
	static CVIBuffer_Cube_Instance* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg);
	virtual void Free() override;
};

END