#pragma once
#include "UVIBuffer.h"

BEGIN(Engine)
class UModel;
class UBoneNode;
class UMeshFilter;
/*
@ Date: 2024-02-04, Writer: 박태현
@ Explain
- 모델의 메쉬 정보를 들고 있는 컨테이너
*/
class  UMeshContainer : public UVIBuffer{
public:
	UMeshContainer(CSHPTRREF <UDevice> _spDevice);
	UMeshContainer(const UMeshContainer& _rhs);
	DESTRUCTOR(UMeshContainer)
public:
	const _uint GetMaterialIndex() const { return m_iMaterialIndex; }
	const _uint GetNumBones() const { return m_iNumBones; }
public:
	CLONE_MACRO(UMeshContainer, "UMeshContainer::Clone To Failed")
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	HRESULT NativeConstruct(void* _pData, CSHPTRREF<UModel> _spModel, const _int _iMeshIndex);
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;
	HRESULT SetUpBoneMatrix(ARRAY<_float4x4, MAX_BONE_SIZE>& _arrBones);
	HRESULT SetUpBoneMatrix(ARRAY<_float4x4, MAX_BONE_SIZE>& _arrBones, const _float4x4& _PivotMatrix);
	void RenderAnimModel(CSHPTRREF<UMeshFilter> _spMeshShowController, 
		CSHPTRREF<UShader> _spShader, CSHPTRREF<UCommand> _spCommands, const _uint _iMeshIndex, const _uint& _iInstanceCnt = 1);
public: /* get set */
	void SetMaterialShowEnable(CSHPTRREF<UMeshFilter> _spMeshShowController);

	const _uint GetMeshIndex() const { return m_iMeshIndex; }
	const _wstring& GetMeshName() const { return m_wstrName; }
private:
	HRESULT ReadyVertices(void* _pData, CSHPTRREF<UModel> _spModel);
	HRESULT ReadyAnimVertices(void* _pData, CSHPTRREF<UModel> _spModel);
	HRESULT ReadyIndicies(void* _pData);
private:
	/* Make To Vertex */
	HRESULT MakeVertices(void* _pVertices, void* _pPosition, const _uint _iVertexSize, const _uint _iVertexCnt
	, const _float3& _vMinPos, const _float3& _vMaxPos, const _wstring& _wstrName, const _uint& _iMaterialIndex);

	void ReceiveBoneNode(CSHPTRREF<UModel> _spModel, const VECTOR<_wstring>& _convecBoneNameList, const _uint& _iBoneNodeCnt);
private:
	using BONENODES = VECTOR<SHPTR<UBoneNode>>;
	_uint							m_iMaterialIndex;
	_uint							m_iMeshIndex;
	_bool							m_isMaterialShowEnable;
	_uint							m_iNumBones;
	_uint							m_iNumBuffers;
	_wstring						m_wstrName;
	BONENODES				m_BoneNodeContainer;
};

END