#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CModel;
class CHierarchyNode;
class CChannel;
class CMeshContainer;
class CAnimationClip;
class CAnimator;
class CBonesCollider;
class CGameObject;
class CTransform;
class CAnimator;
END

BEGIN(Client)
class CCharacter;

class CCutScene_ModelController final : public CBase
{
public:
public:
	typedef struct tagOrderAnimClip
	{
		_string	strAnimClipName = "";
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
		_string												strCurAnimClip = "";
		_string												strPrevAnimClip = "";
		_string												strNextAnimClip = "";
		_string												strFindName = "";
		FILEDATA* pFileData = nullptr;
		vector<ORDERANIM>					ClipOrder;
		_bool													bInitStart = false;
		_float3												vSavePosition = _float3(0.f, 0.f, 0.f);

		void Control_AnimClip(const _float& _fTimeAcc);
		void RootBoneTransform(const _float4x4& _Matrix);
		void Reset();
	}CUTSCENEANIMDESC;

	CUTSCENEANIMDESC* Get_CutSceneAnimDesc() { return &m_tCutSceneAnimDesc; }
private:
	explicit  CCutScene_ModelController(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CCutScene_ModelController() = default;
public:
	CTransform* Get_Transform() { return m_tCutSceneAnimDesc.pOwnerTransform; }
public:
	virtual HRESULT NativeConstrut();

	void Tick(const _double& _dTimeDelta);

	void Load(HANDLE& _hFile, DWORD* _pByte);
	void Recieve_Data(CGameObject* _pGameObject);
	void Recieve_Data(CAnimator* _pAnimator, CTransform* _pTransform);
protected:
	ID3D11Device*						m_pDevice = nullptr;
	ID3D11DeviceContext*			m_pContext = nullptr;
	// Cut Scene Anim Desc
	CUTSCENEANIMDESC			m_tCutSceneAnimDesc;
public:
	static CCutScene_ModelController* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END