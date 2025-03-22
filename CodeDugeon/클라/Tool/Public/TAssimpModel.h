#pragma once
#include "UResource.h"

BEGIN(Engine)
class UShader;
END

BEGIN(Tool)
class TAssimpMeshContainer;
class TAssimpBoneNode;
class TAssimpAnimation;

/*
@ Date: 2024-02-04, Writer: 박태현
@ Explain
- Assimp를 로드하기 위한 Model 클래스
*/
class TAssimpModel : public UResource {
public:
	enum class TYPE { ANIM, NONANIM };

	using MESHCONTAINERS = VECTOR<SHPTR<TAssimpMeshContainer>>;
	using BONENODES = VECTOR<SHPTR<TAssimpBoneNode>>;
	using MATERIERS = VECTOR<SHPTR<MODELMATRIALDESC>>;
	using ANIMATIONS = VECTOR<SHPTR<TAssimpAnimation>>;
public:
	TAssimpModel(CSHPTRREF<UDevice> _spDevice);
	TAssimpModel(const TAssimpModel& _rhs);
	DESTRUCTOR(TAssimpModel)
public:
	// Get BoneNode
	SHPTR<TAssimpBoneNode> FindBoneNode(const _wstring& _strBoneNode);
	const _uint GetNumMeshContainers() const { return m_iNumMeshContainers; }
	const _uint GetNumMaterials() const { return m_iNumMaterials; }
	const TYPE GetModelType() const { return m_eModelType; }
	const MESHCONTAINERS& GetMeshContainers() const { return m_vecMeshContainers; }
	const MATERIERS& GetMaterials() const { return m_MaterialContainer; }
public:
	CLONE_MACRO(TAssimpModel, "TAssimpModel::Clone Failed")
		virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	HRESULT NativeConstruct(const TYPE& _eModelType, const _wstring& _wstrPath, const _float4x4& _vPivotMatrix = _float4x4::Identity,
		const _wstring& _wstrTextureFolder = L"Textures");
	HRESULT NativeConstruct(const TYPE& _eModelType, CSHPTRREF<FILEGROUP> _spFileGroup, CSHPTRREF<FILEDATA> _spFileData,
		const _float4x4& _mPivotMatrix = _float4x4::Identity, const _wstring& _wstrTextureFolder = L"Textures");
	HRESULT NativeConstruct(const TYPE& _eModelType, const PATHS& _vecPaths, const _wstring& _wstrFileName,
		const _float4x4& _mPivotMatrix = _float4x4::Identity, const _wstring& _wstrTextureFolder = L"Textures");
	HRESULT NativeConstruct(const TYPE& _eModelType, const _wstring& _wstrModelFolder, const _wstring& _wstrFileName,
		const _float4x4& _mPivotMatrix = _float4x4::Identity, const _wstring& _wstrTextureFolder = L"Textures");
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;

	// Render
	void Render(const _uint _iMeshIndex, const SRV_REGISTER _eRegister, const aiTextureFlags& _eTextureType,
		CSHPTRREF<UShader> _spShade);
public:
	// Get Data
	void GetData(MODELDESC& _tModelDesc);
	void GetData(ANIMMODELDESC& _tAnimModelDesc);
public:
	// Save NonAnimModel
	void SaveNonAnimModel(const _wstring& _wstrPath, _wstring& _wstrConvertPath);
	// Save Anim Model
	void SaveAnimModel(const _wstring& _wstrPath, _wstring& _wstrConvertPath);
	// Load Animation
	void LoadAnimationFBX(const _wstring& _wstrPath);
	// Load Animation
	void LoadAnimation(const _wstring& _wstrPath);
private:
	// CreateModel
	HRESULT CreateModel(const _wstring& _wstrPath);
	virtual HRESULT CreateMeshContainers(const _xmmatrix& _pivotMatrix = _float4x4::Identity);
	HRESULT CreateMaterials(CSHPTRREF<FILEGROUP> _spFileGroup);
	HRESULT CreateBoneNodes(aiNode* _pNode, SHPTR<TAssimpBoneNode> _pParents = nullptr, const _float _fDepths = 0.f);
	HRESULT CreateAnimation(const aiScene* _pAIScene);

	void ConvertPath(_wstring& _wstrConvertPath);
private:
	SHPTR<Assimp::Importer>		m_spImporter;
	const aiScene* m_pAIScene;
	// Mesh 
	MESHCONTAINERS						m_vecMeshContainers;
	_uint												m_iNumMeshContainers;
	// BoneNodes
	BONENODES									m_BoneNodeContainer;
	_uint												m_iNumBoneNodes;
	// Animation
	ANIMATIONS									m_vecAnimations;
	_uint												m_iNumAnimation;
	// Materials 
	MATERIERS									m_MaterialContainer;
	VECTOR<MODELMATERIALINFO>			m_MaterialInfoContainer;
	_uint												m_iNumMaterials;
	// PivotMatrix 
	_float4x4										m_mScaleMatrix;
	SHPTR<FILEGROUP>					m_spFileGroup;
	SHPTR<FILEDATA>						m_spFileData;
	TYPE												m_eModelType;

	_wstring											m_wstrModelName;
	_wstring											m_wstrTextureFolderName;
	_wstring											m_wstrTexturePath;

	LIST<_wstring>							m_lsTextureExts;
	_bool												m_isLoadAnimationCountIsZero;
};

END