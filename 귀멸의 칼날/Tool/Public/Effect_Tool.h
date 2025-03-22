#pragma once

#include "ImGuiClass_Tool.h"

BEGIN(Engine)
class CGameObject;
class CEffect;
END

BEGIN(Tool)

class CEffect_Tool final : public CImGuiClass_Tool
{
public:
	enum TEXTURETYPE { TYPE_IMAGE, TYPE_STATICMESH, TYPE_ANIMMESH, TYPE_INSTANCE, TYPE_END };
	enum NOISETYPE { NOISE_IMAGE, NOISE_STATICMESH, NOISE_ANIMMESH, NOISE_INSTANCE, NOISE_END };
	enum MASKTYPE { MASK_IMAGE, MASK_STATICMESH, MASK_ANIMMESH, MASK_INSTANCE, MASK_END };
	enum MESHTYPE { MESH_STATIC, MESH_ANIM, MESH_END };
private:
	explicit CEffect_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CEffect_Tool() = default;
public:
	virtual HRESULT NativeConstruct();
	// Native Inner Value (Reset ImGuiClass Vairable)
	virtual HRESULT NativeConstruct_InnerValue();
	virtual HRESULT Tick(const _double& _dTimeDelta);
	virtual HRESULT Late_Tick(const _double& _dTimeDelta);
	virtual HRESULT	Render();
	// Release Inner Value (Reset ImGuiClass Vairable)
	virtual HRESULT Release_InnerValue();
private:
	// Define ImGui (Begin ~ End)
	virtual void Show_Window();
private:
	// EffectObject 의 정보를 가져와서 세팅해줌
	void Get_EffectObjectDesc(_uint iCount);
	// EffectObject 의 정보를 Tool 에서 입력한 값으로 다시 세팅해줌
	void Set_EffectObjectDesc(_uint iCount);
	// Effect 의 정보를 가져와서 세팅
	void Get_EffectDesc(CEffect* pEffect);
	// Effect 의 정보를 Tool 에서 입력한 값으로 다시 세팅해줌
	void Set_EffectDesc(CEffect* pEffect);
	// Effect 생성 버튼 Draw
	void EffectButtonDraw(_uint iCount);
	// Effect 의 세부정보를 출력
	void ShowEffect(CEffect* pEffect, _uint iEffectType, _uint iCount);
	void Get_EffectObjectFrame(_uint iCount);
	HRESULT LoadTexture(vector<FILEDATA>& vecTextureData);
	void InitEffectDesc(EFFECTDESC* tEffectDesc);
	void DeleteEffect(wstring _strName, CEffect* pEffect, _uint iEffectObjectCount, _uint iEffectCount);

	HRESULT SaveEffect(CGameObject* pEffectObject, _wstring _strFileName);
	HRESULT LoadEffect(wstring _strFileName);



private:
	// EffectObject 의 목록 및 세부사항 출력
	void ShowEffectObject();
	void CreateEffect();
	void Load();
	// 0 = EffectObject, 1 = Effect
	void DeleteEffect(_int iType);
	void Find_Texture(PATHS _Paths, CEffect* pEffect, _uint iType);
	void Find_Mesh(PATHS _Paths, CEffect* pEffect, _uint iType);
	void RatioBar();

	void AllLoad();
	void AllSave();
	void NewSave(CGameObject* pEffectObject, _wstring _strFileName);

	void EffectCopy();

private:
	char m_szSaveName[MAX_PATH] = "";
	char m_szLoadName[MAX_PATH] = "";
	char m_szAddEfcPoolName[MAX_PATH] = "";
	WINDESC m_tWinDesc;
	vector<CGameObject*> m_vecEffect;
	vector<_wstring> m_vecTextureTag;
	list<_tchar*> m_StrList;
	// 툴의 텍스쳐 리스트박스에 쓰일 Index 를 관리하기위한 vector 컨테이너
	vector<int> m_vecTextureIndex[TYPE_END];
	// 툴의 노이즈 리스트박스에 쓰일 Index 를 관리하기위한 vector 컨테이너
	vector<int> m_vecNoiseIndex[NOISE_END];
	// 툴의 메쉬 리스트박스에 쓰일 Index 를 관리하기위한 vector 컨테이너
	vector<int> m_vecMeshIndex[MESH_END];
	vector<FILEDATA> m_vecDiffuseFileData;
	vector<FILEDATA> m_vecNoiseFileData;
	vector<FILEDATA> m_vecMeshFileData;
	EFFECTDESC m_tEffectObjectDesc;
	EFFECTDESC m_tEffectDesc;
	INSTANCEDESC m_tInstanceEffectDesc;
	INSTANCEDESC2 m_tNewInstanceDesc;
	_string m_strDiffuseText = "";
	_string m_strNoiseText = "";
	_string m_strMeshText = "";
	_string m_strAddObjPoolName = "";

	_float m_fMaxFrame = 0.f;
	_float m_fCurFrame = 0.f;

	_int m_iCurEffectObjectIndex = 0;
	_int m_iCurEffectIndex = 0;

	// Effect Parameter
	_float m_fEffectObjectPosArr[3] = { 0.f };
	_float m_fEffectObjectRotArr[3] = { 0.f };
	_float m_fEffectObjectSclArr[3] = { 0.f };

	_float m_fEffectPosArr[3] = { 0.f };
	_float m_fEffectRotArr[3] = { 0.f };
	_float m_fEffectSclArr[3] = { 0.f };

	_float m_fEffectDeltaScl[3] = { 0.f };

	_float m_fInstanceEffectSclArr[3] = { 0.f };

	_float m_fEffectDir[3] = { 0.f };
	_float m_fInstanceDir[3] = { 0.f };

	_float m_fEffectColor[4] = { 0.f };
	_float m_fEffectUVSpeed[2] = { 0.f };

	_float m_fScrollSpeeds[3] = { 0.f };
	_float m_fScales[3] = { 1.0f, 2.0f, 3.0f };
	_float m_fDistortion1[2] = { 0.1f, 0.2f };
	_float m_fDistortion2[2] = { 0.1f, 0.3f };
	_float m_fDistortion3[2] = { 0.1f, 0.1f };


	_float m_fInstanceMinPos[3] = { 0.f };
	_float m_fInstanceMaxPos[3] = { 0.f };


	_uint m_iImageEffectCount = 0;
	_uint m_iNoiseImageCount = 0;
	_uint m_iStaticMeshEffectCount = 0;
	_uint m_iNoiseMeshCount = 0;
	_uint m_iAnimMeshEffectCount = 0;
	_uint m_iInstanceEffectCount = 0;
	_uint m_iNoiseInstanceCount = 0;

	_int iRenderGroup = 0;

	_bool m_bPercentage = false;
	_bool m_bPause = false;

	_int m_iPoolCount = 0;

	_wstring m_strSelectDiffuse = L"";
	_wstring m_strSelectNoise = L"";
	_wstring m_strSelectStaticMesh = L"";
	_wstring m_strSelectAnimMesh = L"";

public:
	static CEffect_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END