#pragma once
#include "GameObject_Tool.h"

BEGIN(Engine)
class CModel;
class CHierarchyNode;
class CChannel;
class CMeshContainer;
class CAnimationClip;
class CAnimator;
class CBonesCollider;
END

BEGIN(Tool)

class CCutSceneModel_Tool final : public CGameObject_Tool
{
public:
	typedef struct tagOrderAnimClip
	{
		CAnimationClip* pAnimClip = nullptr;
		_double dTime = 0.;
	}ORDERANIM;
	typedef struct tagCutSceneAnimDesc
	{
		// Total Play Time
		_double												dTotalPlayTime = 0.;
		CAnimator* pOwnerAnimator = nullptr;
		CTransform* pOwnerTransform = nullptr;
		// AnimClip Vector 
		vector< CAnimationClip*>			AnimClipVector;
		CAnimationClip* pCurAnimClip = nullptr;
		CAnimationClip* pPrevAnimClip = nullptr;
		CAnimationClip* pNextAnimClip = nullptr;
		CAnimationClip* pSelectAnimClip = nullptr;
		CAnimationClip* pDeleteAnimClip = nullptr;

		_string												strFindName = "";
		list<CAnimationClip*>					FindAnimList;
		FILEDATA* pFileData = nullptr;
		vector<ORDERANIM>					ClipOrder;
		_bool													bInitStart = false;
		_float3												vSavePosition = _float3(0.f, 0.f, 0.f);
		_float													fAddTime = 0.f;
		_float													fCurrentTime = 0.f;

		void Add_AnimationClip();
		void Update_AnimClip(const _float& _fTimeAcc);
		void Control_AnimClip(const _float& _fTimeAcc);
		void RootBoneTransform();
		void ShowList();
		CAnimationClip* Get_AnimClip(const _float& _fTimeAcc);
		void Calculate_TotalPlayTime();
		void Just_Calculate_TotalPlayTime();
		void Remove_AnimClip();
		void Reset();
		void Reset_All();
		void End_Anim();
		// Receive Clip Order
		void Receive_ClipOrder(vector<ORDERANIM> _AnimOrderClip);
	}CUTSCENEANIMDESC;

private:
	explicit CCutSceneModel_Tool(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CCutSceneModel_Tool(const CCutSceneModel_Tool& _rhs);
	virtual ~CCutSceneModel_Tool() = default;
public:
	FILEDATA* Get_FileData() { return m_pFileData; }
	CAnimator* Get_Animator() { return m_pAnimatorCom; }
	CModel* Get_Model() { return m_pModelCom; }
	CUTSCENEANIMDESC* Get_CutSceneAnimDesc() { return &m_tCutSceneAnimDesc; }

	void Set_PlayCutScene(const _bool& _bPlayCutScene) { this->m_bCutScenePlay = _bPlayCutScene; }
public:
	virtual const char* Get_Name() override;
	virtual const _tchar* Get_Proto() override;
	virtual const _tchar* Get_Layer() override;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
	// Load Cut Scene Model
	HRESULT Load_CutSceneModel(FILEGROUP* _pModelFileGroup, FILEDATA* _pModelFileData);

	void  Cut_Tick(const _double& _dTimeDelta);
	void  Cut_Late_Tick(const _double& _dTimeDelta);

	void Save(HANDLE& _hFile, DWORD* _pByte);
	void Load(HANDLE& _hFile, DWORD* _pByte);
protected:
	// Ready, SetUp
	HRESULT Ready_Component();
	HRESULT SetUp_ConstantValue();
protected:
	// 애니메이터 컴포넌트
	CAnimator* m_pAnimatorCom = nullptr;
	CModel* m_pModelCom = nullptr;
	FILEDATA* m_pFileData = nullptr;
	// Rotation
	_float3											m_v3Rotation = _float3(0.f, 0.f, 0.f);
	_float4x4										m_PrevWorldMatrix;
	// Input Trigger Value
	_string											m_strInputTriggerValue = "";
public:
	// 태현 추가
#pragma region USE_TOOL_METHOD
	// Push Trigger
	void Push_Trigger(const _string& _strTrigger);
	// Extern EditValue
	virtual _bool Extern_EditValue();
	// Extern Hierakey Value
	virtual _bool Extern_HierakeyView(const string& _str);
	// Create Self 
	virtual HRESULT Create_Self(CGameObject** _ppOut);
	// Delete Self
	virtual HRESULT Delete_Self();
	// Save Data 
	virtual HRESULT Save_Data();
	// Load Data 
	virtual HRESULT Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData = nullptr);
	// Get Picking
	virtual _bool Is_Picking(_float3* _pOut = nullptr);
	// Reset Data 
	virtual void Reset_Data();
	// Add AnimationClip
	void  Show_AnimationClip();
#pragma endregion USE_TOOL_METHOD
protected:
	// Cut Scene Anim Desc
	CUTSCENEANIMDESC m_tCutSceneAnimDesc;
	// Scene Order Index
	_uint	m_iSceneOrder_Index = 0;
	// Select Order Index
	_uint m_iSelectOrder_Index = 0;
	// Play Anim Time
	_float		m_fPlayAnimTime = 0.f;
	_bool		m_pAnimPlay = false;
	_bool		m_bCutScenePlay = false;
public:
	static CCutSceneModel_Tool* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END