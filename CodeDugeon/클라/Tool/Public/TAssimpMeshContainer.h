#pragma once 
#include "UVIBuffer.h"

BEGIN(Engine)

END

BEGIN(Tool)
class TAssimpBoneNode;
class TAssimpModel;
/*
@ Date: 2024-02-04, Writer: 박태현
@ Explain
- Assimp를 로드하기 위한 meshContainer
*/
class TAssimpMeshContainer : public UVIBuffer {
public:
	TAssimpMeshContainer(CSHPTRREF <UDevice> _spDevice);
	TAssimpMeshContainer(const TAssimpMeshContainer& _rhs);
	DESTRUCTOR(TAssimpMeshContainer)
public:
	const _wstring& GetMeshName() const { return m_wstrMeshName; }
public:
	CLONE_MACRO(TAssimpMeshContainer, "TAssimpMeshContainer::Clone To Failed")
		virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	HRESULT NativeConstruct(aiMesh* _pMesh, const _uint& _iType, CSHPTRREF<TAssimpModel> _pModel,
		const _xmmatrix& _mPivotMatrix = _float4x4::Identity);
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;
	HRESULT SetUpBoneMatrices(VECTOR<_float4x4>& _vecMatrixces, const _float4x4& _mPivotMatrix);
public:
	void GetData(MESHDESC& _stMeshDesc);
	void GetData(ANIMMESHDESC& _stAnimMeshDesc);
private:
	HRESULT ReadyVertices(aiMesh* _pMesh, CSHPTRREF<TAssimpModel> _spModel, const _xmmatrix& _mPivotMatrix);
	HRESULT ReadyAnimVertices(aiMesh* _pMesh, CSHPTRREF<TAssimpModel> _spModel);
	HRESULT ReadyIndices(aiMesh* _pMesh);
private:
	_uint																	m_iMaterialIndex;
	_uint																	m_iNumBones;
	_uint																	m_iNumBuffers;
	VECTOR <SHPTR<TAssimpBoneNode>>	m_BoneNodeContainer;
	_wstring																m_wstrName;
	void* m_pVertexData;
	VECTOR<_float2>											m_vecUVTexCoords;
	_float3																m_vMinVertex;
	_float3																m_vMaxVertex;
	_wstring																m_wstrMeshName;
};

END
