#pragma once
#include "Component.h"

BEGIN(Engine)
class CMeshContainer;
class CQuadTree;
class CAnimation;
class CHierarchyNode;
class CTexture;

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_ANIM, TYPE_NONANIM, TYPE_END };
public:
	const _uint& Get_NumMeshContainers() const { return m_iNumMeshContainers; }
	FILEGROUP* Get_FileGroup() const { return m_pFileGroup; }
	FILEDATA* Get_FileData() const { return m_pFileData; }
	vector<CMeshContainer*> Get_MeshContainers() { return m_vecMeshContainers; }
	CHierarchyNode* Get_HierarchyNodes(const char* _pHierarchyNodeName);
	vector< CHierarchyNode*> Get_HierachyNodeList() { return m_vecHierachyNodes; }
	// 애니메이션 수정?? 
	CAnimation* Get_CurAnimation() { return m_pCurAnimations; }
	CAnimation* Get_NextAnimation();
	CAnimation* Get_PrevAnimation();
public:
	_matrix Get_PivotMatrix() { return XMLoadFloat4x4(&m_PivotMatrix); }
	const _uint& Get_CurAnimIndex() const { return m_iCurAnimIndex; }
	vector<CAnimation*> Get_Animations() { return m_vecAnimations; }
	CAnimation* Get_Animation(_string strAnimation);
	void Set_PivotTransform(_fmatrix _mPivot) { XMStoreFloat4x4(&m_PivotMatrix, _mPivot); }

	const _bool& Is_ChangeAnimation() const { return m_bChangeAnimation; }
	void Set_SetSupplementAnim(const _bool& _bIsSupplementAnim) { this->m_bSupplementAnim = _bIsSupplementAnim; }
public:
	void Set_Texture(CTexture* _pTexture, const TEXTYPE& _eTexType, const _uint& _iMaterialIndex = 0);
protected:
	explicit CModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CModel(const CModel& _rhs);
	virtual ~CModel() = default;
public:
	// NativeConstruct Prototype
	virtual HRESULT NativeConstruct_Prototype() override;
	HRESULT NativeConstruct_Prototype(const TYPE& _eModelType, FILEGROUP* _pFileGroup, FILEDATA* _pFileData,
		_fmatrix _mPivotMatrix = XMMatrixIdentity());
	HRESULT NativeConstruct_Prototype(const TYPE& _eModelType, FILEGROUP* _pFileGroup,
		const _wstring& _strName, _fmatrix _mPivotMatrix = XMMatrixIdentity());
	HRESULT NativeConstruct_Prototype(const TYPE& _eModelType, PATHS* _pPaths,
		const _wstring& _strName, _fmatrix _mPivotMatrix = XMMatrixIdentity());
	HRESULT NativeConstruct_Prototype(const TYPE& _eModelType, _wstring _strFilePath, _fmatrix _mPivotMatrix = XMMatrixIdentity());
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
public:
	// Update Animation
	void Update_Animation(const _double& _dTimeDelta);
	HRESULT Bind_SRV(class CShader* _pShader, const char* _pConstantName,
		const _uint& _iMeshContainerIndex, const ETextureType& _eTextureType);
	_bool Check_Texture(const _uint& _iMeshContainerIndex, const ETextureType& _eTextureType);
	// 이펙트 스태틱메쉬용 Bind_SRV
	HRESULT Effect_Bind_SRV(class CShader* _pShader, const char* _pConstantName,
		const _uint& _iMeshContainerIndex, const ETextureType& _eTextureType);
	// 일반 Render
	HRESULT Render(const _uint& _iMeshContainersIndex, CShader* _pShader, const  _uint& _iPassIndex);
	HRESULT Render_NonAnim(const _uint& _iMeshContainersIndex, CShader* _pShader, const _uint& _iPassIndex);
	HRESULT	Bind_BoneTexture(_float4x4* boneMatrix, CShader* _pShader);
	// Delete MeshContainers
	void Delete_MeshContainers(const _uint& _iMeshContainersIndex);
public:
	// 애니메이션 변경
	void Set_Animation(const _uint& _iAnimIndex);
	void Set_Animation(const char* _pName);
	void Change_Animation(const _uint& _iAnimation);
	void Change_Animation(const _char* _pName);
	void Change_SameAnimation(const _char* _pName);
	void Change_SameAnimationSup(const _char* _pName);
	void Repeat_Animation(const _char* _pName);
	void Set_NextAnimation(const _char* _pName);
protected:
	// Create MeshContainers
	HRESULT Create_MeshConstainers(const MODELDESC& _pData, _fmatrix _mPivotMatrix = XMMatrixIdentity());
	HRESULT Create_MeshConstainers_Anim(const ANIMMODELDESC& _pData, _fmatrix _mPivotMatrix = XMMatrixIdentity());
	HRESULT Create_Materials(const COMMODELDATA& _pData);
	HRESULT Create_HierachyNodes(const vector<HNODEDESC>& _tDescList);
	HRESULT Create_Animation(const vector<ANIMDESC>& _tDescList);
	HRESULT	Create_BoneTexture();
protected:
	// 로드 함수 (바이너리 로드 용)
	HRESULT Load_Data_NonAnim(MODELDESC& _tModelDesc, _int iIndex = 0);
	HRESULT Load_Data_Anim(ANIMMODELDESC& _tModelDesc, _int iIndex = 0);
protected:
	// Mesh Container
	typedef vector<CMeshContainer*>	MESHCONTAINERS;
	MESHCONTAINERS							m_vecMeshContainers;
	_uint													m_iNumMeshContainers = 0;
protected:
	// Material
	_uint													m_iNumMatrials = 0;
	typedef vector<MODELMATRIALDESC> MATRIALS;
	MATRIALS											m_vecMaterials;
protected:
	// HierakyNode 
	typedef vector<CHierarchyNode*> HIERARCHYNODES;
	HIERARCHYNODES							m_vecHierachyNodes;
protected:
	// ANimation
	typedef vector<CAnimation*> ANIMATIONS;
	ANIMATIONS									m_vecAnimations;
	// Animations
	CAnimation*									m_pCurAnimations = nullptr;
	CAnimation*									m_pNextAnimations = nullptr;
	CAnimation*									m_pPrevAnimation = nullptr;
protected:
	// Model Name
	FILEGROUP*										m_pFileGroup = nullptr;
	FILEDATA*											m_pFileData = nullptr;
private:
	_wstring											m_strEffectFilePath = L"";
protected:
	// Cur Anim Index
	_uint													m_iCurAnimIndex = 0;
	_uint													m_iNextAnimIndex = 0;
	_float4x4											m_PivotMatrix;
private:
	// Change Animation
	_bool													m_bChangeAnimation = false;
private:
	// BoneMatrix
	ID3D11Texture2D*							m_pBoneTexture = nullptr;
	ID3D11ShaderResourceView*	m_pBoneSRV = nullptr;
	// Prev BoneMatrix
	ID3D11Texture2D*							m_pPrevBoneTexture = nullptr;
	ID3D11ShaderResourceView*	m_pPrevBoneSRV = nullptr;
	// Type 
	TYPE													m_eType = TYPE_END;
	_bool													m_bSupplementAnim = true;
public:
	static CModel* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eModelType, FILEGROUP* _pFileGroup,
		FILEDATA* _pFileData, _fmatrix _mPivotMatrix = XMMatrixIdentity());
	static CModel* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eModelType, FILEGROUP* _pFileGroup,
		const _wstring& _strName, _fmatrix _mPivotMatrix = XMMatrixIdentity());
	static CModel* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eModelType, PATHS* _pPaths,
		const _wstring& _strName, _fmatrix _mPivotMatrix = XMMatrixIdentity());
	static CModel* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eModelType, _wstring _strFilePath, _fmatrix _mPivotMatrix = XMMatrixIdentity());

	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END
