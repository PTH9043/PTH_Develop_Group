#pragma once

#include "GameObject.h"
#include "EffectObject.h"

BEGIN(Engine)

class CModel;
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CVIBuffer_Effect_Instance;

class ENGINE_DLL CEffect abstract : public CGameObject
{
public:
	enum EFFECTTYPE { EFFECT_IMAGE, EFFECT_STATICMESH, EFFECT_ANIMMESH, EFFECT_INSTANCE, EFFECT_END };
	enum TEXTURETYPE { TEXTURE_DIFFUSE, TEXTURE_NOISE, TEXTURE_MASK, TEXTURE_END };
protected:
	explicit CEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CEffect(const CEffect& _rhs);
	virtual ~CEffect() = default;

protected:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual void Tick(const _double & _dTimeDelta) override;
	virtual void Late_Tick(const _double & _dTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	virtual HRESULT Set_Texture(_wstring strFileName) = 0;

public:
	void SetUp_Position();
	HRESULT Init_Texture();
	HRESULT Client_Init_Texture(_uint iType, wstring _strPath);

public:
	_uint Get_EffectType() { return (_uint)m_eEffectType; }
	EFFECTDESC Get_EffectDesc() { return m_tEffectDesc; }
	wstring Get_TexturePath(_uint iType);
	wstring Get_PrototypePath() { return m_strPrototypePath; }
	wstring Get_NoisePath() { return m_strNoisePath; }
	wstring Get_MeshPrototypePath() { return m_strMeshPrototypePath; }
	_uint Get_TextureIndex(_uint iType);
	_uint Get_MeshIndex() { return m_iMeshIndex; }
	wstring Get_EffectName() { return m_strName; }
	_wstring Get_MeshProtoTag() { return m_strMeshProtoTag; }


	void Set_EffectDesc(EFFECTDESC tEffectDesc) {
		m_tEffectDesc = tEffectDesc;
		//m_fCurAcc = m_tEffectDesc.fAccCurTime;
	}
	void Set_Parent(CGameObject * pGameObject);
	void Set_TexturePath(_uint iType, wstring strPath);
	void Set_PrototypePath(wstring strPath);
	void Set_NoisePath(wstring strPath);
	void Set_MeshPrototypePath(wstring strPath);
	void Set_TextureIndex(_uint iType, _uint iIndex);
	void Set_MeshIndex(_uint iIndex) { m_iMeshIndex = iIndex; }
	void Set_EffectName(_wstring strName) { m_strName = strName; }
	void Set_EffectName(_tchar * strName) { m_strName = strName; }
	void Set_MeshProtoTag(_wstring strTag) { m_strMeshProtoTag = strTag; }
	void Set_ChangeLook();
	void Set_ParentLook(_float3 vLook) { m_vParentLook = vLook; }

	void EffectUpDate();
	void EffectReset();

public:
	void ChangeTranslation(const _double & _dTimeDelta);
	void RotationLoop();
	void RotationAngle();
	void ChangeScale();
	void SliceTexture(const _double & _dTimeDelta);
	void UVFlow(const _double & _dTimeDelta);
	void BillBoard();
	void Alpha(const _double & _dTimeDelta);

protected:
	CModel* m_pVIBufferCom_Model = nullptr;
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom[TEXTURE_END] = { nullptr };
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom_Rect = nullptr;
	CVIBuffer_Effect_Instance* m_pVIBufferCom_Instance = nullptr;
	CGameObject* m_pParent = nullptr;
	CTransform* m_pEffectTransform = nullptr;

	EFFECTTYPE m_eEffectType = EFFECT_END;
	EFFECTDESC m_tEffectDesc;
	wstring m_strPrototypePath = L"";
	wstring m_strNoisePath = L"";
	wstring m_strMeshPrototypePath = L"";
	_wstring m_strName = L"";
	_wstring m_strMeshProtoTag = L"";


	_float3 m_vOriginScale = _float3(1.f, 1.f, 1.f);
	_float3 m_vOriginRot = _float3(0.f, 0.f, 0.f);
	_float3 m_vDeltaPos = _float3(0.f, 0.f, 0.f);
	_float3 fDeltaScale = _float3(1.f, 1.f, 1.f);
	_float2 m_vUV_Start = _float2(0.f, 0.f);
	_float2 m_vUV_End = _float2(0.f, 0.f);
	_float2 m_vUV_Delta = _float2(0.f, 0.f);
	_float2 m_vUV_Dir = _float2(0.f, 0.f);
	_float3 m_vParentLook = _float3(0.f, 0.f, 0.f);

	_float m_fDelay = 0.f;
	_float fDeltaValue = 0.f;
	_double m_fSliceTime = 0.0;
	_float m_fDeltaSliceX = 0.f;
	_float m_fDeltaSliceY = 0.f;
	_double m_fFrameSpeed = 0.0;
	_float m_fTime = 0.f;
	_float m_fRatioTime = 0.f;
	_float m_fEnableTime = 0.f;
	_double m_fLifeTime = 0.0;
	_float m_fAlphaTime = 0.f;
	_float m_fAlpha = 1.f;
	_float m_fDeltaX = 0.f;
	_float m_fCurAcc = 0.f;
	_float m_fRatio = 0.f;
	_float m_fDissolveTime = 0.f;

	_int m_iSliceCount = 0;
	_int m_iSliceCurCount = 0;

	_bool m_bAddScale = true;
	_bool m_bAddRot = true;
	_bool m_bSliceFirst = true;
	_bool m_bEnable = true;
	_bool m_bDisable = false;
	_bool m_bPlayFirst = true;
	_bool m_bSlice = true;

	_uint m_iTextureIndex = 0;
	_uint m_iNoiseIndex = 0;
	_uint m_iMeshIndex = 0;

	_float m_fPerSecondIndex = 0.f;
	_float m_fCurUseIndex = 0.f;
	_int m_iSliceType = 0;

public:
	virtual CEffect* Clone(void* _pArg) = 0;
	virtual void Free() override;
};

END