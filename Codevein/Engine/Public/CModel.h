#pragma once
#include "CComponent.h"
#include "CHierarchyNode.h"
#include "CMeshContainer.h"
#include "CAnimation.h"
#include "CChannel.h"

BEGIN(Engine)
class CMeshContainer;
class CQuadTree;
class CAnimation;
class CHierarchyNode;

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_ANIM, TYPE_NONANIM, TYPE_END };
public:
	const _uint& Get_NumMeshContainers() const { return m_iNumMeshContainers; }
	FILEGROUP* Get_ModelFileGroup() const { return m_pModelFileGroup; }
	FILEDATA* Get_FileData() const { return m_pFileData; }
	vector<CMeshContainer*> Get_MeshContainers() { return m_vecMeshContainers; }
	CHierarchyNode* Get_HierarchyNodes(const char* pHierarchyNodeName);
	vector< CHierarchyNode*> Get_HierachyNodeList() { return m_vecHierachyNodes; }
	CAnimation* Get_CurAnimation() { return m_pCurAnimations; }
	CAnimation* Get_NextAnimation();
	CAnimation* Get_PrevAnimation();
public:
	_matrix Get_PivotMatrix() { return XMLoadFloat4x4(&m_PivotMatrix); }
	const _uint& Get_CurAnimIndex() const { return m_iCurAnimIndex; }
	vector<CAnimation*> Get_Animations() { return m_vecAnimations;}
	void Set_PivotTransform(_fmatrix _mPivot) { XMStoreFloat4x4(&m_PivotMatrix, _mPivot); }

	const _bool& Is_ChangeAnimation() const { return m_bChangeAnimation; }
public:
	void Set_Texture(CTexture* _pTexture, const TEXTYPE& _eTexType, const _uint& _iMaterialIndex = 0);
protected:
	explicit CModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CModel(const CModel& _rhs);
	virtual ~CModel() = default;
public:
	// Save Animation
	void Save_Animation(FILEGROUP* _pFileGroup);
	// Load Animation
	void Load_Animation(FILEGROUP* _pFileGroup);
public:
	// NativeConstruct Prototype
	virtual HRESULT NativeConstruct_Prototype() override;
	HRESULT NativeConstruct_Prototype(const TYPE& _eModelType, FILEGROUP* _pFileGroup, FILEDATA* _pFileData, _fmatrix _mPivotMatrix = XMMatrixIdentity());
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
public:
	// Update Animation
	void Update_Animation(const _double& _dTimeDelta);
	HRESULT Bind_SRV(class CShader* _pShader, const char* _pConstantName,
		const _uint& _iMeshContainerIndex, const ETextureType& _eTextureType);
	HRESULT Render(const _uint& _iMeshContainersIndex, CShader* _pShader, const  _uint& _iPassIndex);
	HRESULT Render_NonAnim(const _uint& _iMeshContainersIndex, CShader* _pShader, const _uint& _iPassIndex);
public:
	void Set_Animation(const _uint& _iAnimIndex);
	void Set_Animation(const char* _pName);
	void Change_Animation(const _uint& _iAnimation);
	void Change_Animation(const _char* _pName);
	void Change_SameAnimation(const _char* _pName);
	void Change_SameAnimationSup(const char* _pName);
	void Repeat_Animation(const _char* _pName);
	void Set_NextAnimation(const _char* _pName);
protected:
	// Create MeshContainers
	HRESULT Create_MeshConstainers( const MODELDESC& _pData, _fmatrix _mPivotMatrix = XMMatrixIdentity());
	HRESULT Create_MeshConstainers_Anim(const ANIMMODELDESC& _pData, _fmatrix _mPivotMatrix = XMMatrixIdentity());
	HRESULT Create_Materials(const COMMODELDATA& _pData);
	HRESULT Create_HierachyNodes(const vector<HNODEDESC>& _tDescList);
	HRESULT Create_Animation(const vector<ANIMDESC>& _tDescList);
protected:
	HRESULT Load_Data_NonAnim(MODELDESC& _tModelDesc);
	HRESULT Load_Data_Anim(ANIMMODELDESC& _tModelDesc);
protected:
	typedef vector<CMeshContainer*>	MESHCONTAINERS;
	MESHCONTAINERS		m_vecMeshContainers;
	_uint								m_iNumMeshContainers = 0;
protected:
	_uint								m_iNumMatrials = 0;
	typedef vector<MODELMATRIALDESC*> MATRIALS;
	MATRIALS						m_vecMaterials;

protected:
	typedef vector<CHierarchyNode*> HIERARCHYNODES;
	HIERARCHYNODES		m_vecHierachyNodes;

protected:
	typedef vector<CAnimation*> ANIMATIONS;
	ANIMATIONS				m_vecAnimations;
	// Cur Animations
	CAnimation*				m_pCurAnimations = nullptr; 
	CAnimation*				m_pNextAnimations = nullptr;
	CAnimation*				m_pPrevAnimation = nullptr;
protected:
	// Model Name
	FILEGROUP*					m_pModelFileGroup = nullptr;
	FILEDATA*						m_pFileData = nullptr;
protected:
	// Cur Anim Index
	_uint								m_iCurAnimIndex = 0;
	_uint								m_iNextAnimIndex = 0;
	_float4x4						m_PivotMatrix;
private:
	_bool								m_bChangeAnimation = false;
public:
	static CModel* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eModelType, FILEGROUP* _pFileGroup, 
		FILEDATA* _pFileData, _fmatrix _mPivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END
