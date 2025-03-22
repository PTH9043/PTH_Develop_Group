#pragma once
#include "Tool_Defines.h"
#include "Component.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Tool)
class CMeshContainer_Tool;
class CHierarchyNode_Tool;
class CAnimation_Tool;
class CChannel_Tool;

class CModel_Tool final : public CComponent
{
public:
	enum TYPE { TYPE_ANIM, TYPE_NONANIM, TYPE_END };

public:
	const _uint& Get_NumMeshContainers() const { return m_iNumMeshContainers; }
	FILEGROUP* Get_ModelFileGroup() const { return m_pModelFileGroup; }
	FILEDATA* Get_FileData() const { return m_pFileData; }
	vector<CMeshContainer_Tool*> Get_MeshContainers() { return m_MeshContainers; }
	class CHierarchyNode_Tool* Get_HierarchyNodes(const char* pHierarchyNodeName);
	CAnimation_Tool* Get_Animation() { return m_pCurAnimation; }
	const _uint& Get_AnimationCount() const { return m_iNumAnimations; }

protected:
	explicit CModel_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CModel_Tool(const CModel_Tool& _rhs);
	virtual ~CModel_Tool() = default;


public:
	void Set_CurAnimIndex(const _uint& _iAnimIndex) { m_iCurrentAnimIndex = _iAnimIndex; }
	const _uint& Get_CurAnimIndex() const { return m_iCurrentAnimIndex; }
public:
	// NativeConstruct Prototype
	virtual HRESULT NativeConstruct_Prototype() override;
	
	HRESULT NativeConstruct_Prototype(const TYPE& _eModelType, FILEGROUP* _pFileGroup, const _uint& _iIndex, 
		const _wstring& _strTextureFolderName, const _fmatrix& _mPivotMatrix = XMMatrixIdentity());

	HRESULT NativeConstruct_Prototype(const TYPE& _eModelType, FILEGROUP* _pFileGroup, FILEDATA* _pFileData,
		const _wstring& _strTextureFolderName, const _fmatrix& _mPivotMatrix = XMMatrixIdentity());
	
	HRESULT NativeConstruct_Prototype(const TYPE& _eModelType, FILEGROUP* _pFileGroup, const _wstring& _strModelName,
		const _wstring& _strTextureFolderName, const _fmatrix& _mPivotMatrix = XMMatrixIdentity());
	
	HRESULT NativeConstruct_Prototype(const TYPE& _eModelType, PATHS* _pPaths, const _wstring& _strModelName,
		const _wstring& _strTextureFolderName, const _fmatrix& _mPivotMatrix = XMMatrixIdentity());
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
public:
	// Update Animation
	void Update_Animation(const _double& _dDeltaTime);
	HRESULT Bind_SRV(CShader* _pShader, const char* _pConstantName,
		const _uint& _iMeshContainerIndex, const enum aiTextureType& _eTextureType);
	HRESULT Render(const _uint& _iMeshContainersIndex, CShader* _pShader, const  _uint& _iPassIndex);
	// Create Animation
	HRESULT Create_Animation(FILEGROUP* _pFileGroup, FILEDATA* _pFileData);
public:
	void Set_Animation(const _uint& _iAnimIndex);
	void Set_Animation(const char* _pName);
protected:
	// Create MeshContainers
	HRESULT Create_MeshConstainers(const TYPE& _eType, const _fmatrix& _mPivotMatrix = XMMatrixIdentity());
	HRESULT Create_Materials(FILEGROUP*& _pFileGroup);
	HRESULT Create_HierachyNodes(struct aiNode* _pNode, class CHierarchyNode_Tool* _pParents, const _uint& _iDepths = 0);
	HRESULT Create_Animation();

protected:
	const  struct aiScene*	m_pScene = nullptr;
	class Importer*				m_Importer = nullptr;

protected:
	typedef vector<CMeshContainer_Tool*>	MESHCONTAINERS;
	MESHCONTAINERS		m_MeshContainers;
	_uint								m_iNumMeshContainers = 0;

protected:
	_uint								m_iNumMaterials = 0;
	typedef vector<MODELMATRIALDESC*> MATRIALS;
	MATRIALS						m_Materials;

private:
	vector<class CHierarchyNode_Tool*>			m_HierarchyNodes;
	typedef vector<class CHierarchyNode_Tool*>	HIERARCHYNODES;

private:
	_uint									m_iNumAnimations;
	vector<class CAnimation_Tool*>				m_Animations;
	typedef vector<class CAnimation_Tool*>		ANIMATIONS;

	CAnimation_Tool* m_pCurAnimation = nullptr;
protected:
	// Model Name
	FILEGROUP* m_pModelFileGroup = nullptr;
	FILEDATA* m_pFileData = nullptr;

private:
	_float4x4					m_PivotMatrix;
	_uint						m_iCurrentAnimIndex = 0;
	_wstring					 m_strTextureFolderName = L"";
public:
	// Get Data
	void Get_Data(MODELDESC& _tModelDesc);
	void Get_Data(ANIMMODELDESC& _tAnimModelDesc);

public:
	// Save NonAnimModel
	void Save_NonAnimModel();
	// Save Anim Model
	void Save_AnimModel();

public:
	static CModel_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eModelType,
		FILEGROUP* _pFileGroup, const _uint& _iIndex, const _wstring& _strTextureFolderName, const _fmatrix& _mPivotMatrix = XMMatrixIdentity());
	
	static CModel_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eModelType,
		FILEGROUP* _pFileGroup, FILEDATA* _pFileData, const _wstring& _strTextureFolderName, const _fmatrix& _mPivotMatrix = XMMatrixIdentity());

	static CModel_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eModelType,
		FILEGROUP* _pFileGroup, const _wstring& _strModelName, const _wstring& _strTextureFolderName, const _fmatrix& _mPivotMatrix = XMMatrixIdentity());
	
	static CModel_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eModelType,
		PATHS* _pPaths, const _wstring& _strModelName, const _wstring& _strTextureFolderName, const _fmatrix& _mPivotMatrix = XMMatrixIdentity());

	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END