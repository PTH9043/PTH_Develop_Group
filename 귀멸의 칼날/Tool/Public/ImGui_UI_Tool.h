#pragma once
#include "Tool_Defines.h"
#include "ImGuiClass_Tool.h"



BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CComponent;
class CVIBuffer;
class CTexture;
class CLayer;
class CUI_Base;
END

BEGIN(Tool)
class CImGui_UI_Tool : public CImGuiClass_Tool
{
	struct TextInfo
	{
		char Text_Input[1000] = "";
		char Text_Tag[100] = "";
		_float TextX = 0.f;
		_float TextY = 0.f;
		_float TextR = 255.f;
		_float TextG = 255.f;
		_float TextB = 255.f;
	};
	
protected:
	explicit CImGui_UI_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CImGui_UI_Tool() = default;

public:
	virtual HRESULT NativeConstruct();
	virtual HRESULT NativeConstruct_InnerValue();
	virtual HRESULT Tick(const _double& _dTimeDelta);
	virtual HRESULT Late_Tick(const _double& _dTimeDelta);
	virtual HRESULT Render();

	//picking로 선택된 UI의 위치를 설정한다
	void Picking();
	
	//UI오브젝트 목록을 보여주고 선택할수 있게 한다
	void ShowPrototypeObject();
	
	//선택된 UI의 위치와 사이즈를 드래그로 설정할수있다
	void Convert_Transform();
	
	//picking해서 충돌된 UIObject를 찾아서 선택된 UI로 설정한다 
	void Picking_Find_Object();
	
	//선택한 UI_Object를 Clone(Create)한다 +)create할때 이름을 붙여주는 기능
	void Create_UI_Object();
	
	//선택된 UI_Object의 NameTag를 바꾼다 
	void Change_UI_NameTag();

	//Texture파일들을 보여준다
	void Show_Texture();

	//선택된 Texture로 바꿔준다
	void Change_Texture();

	void Save();

	void Load();

	//선택된 UI_Object에게 효과를 준다 
	void Effect_UI(const _double& _dTimeDelta);

	//알파값을 조절하거나 서서히 사라지거나 짙어지게 한다 
	void Alpha_Control(const _double& _dTimeDelta);

	void Dialogue_UI();

	

private:

	
	const _tchar* prototypeObjectTag = nullptr;
	CGameObject* prototypeObject = nullptr;

	class CUI_Base* selectedObject = nullptr;
	
   map<const _tchar*, class CGameObject*> gameObject;
   FILEDATA* selectedTexture = nullptr;


private:
	_bool bPicking = false;
	_bool bFind = false;
	
	_bool bGrow = false;
	_bool bGrowDecrease = false;
	_bool bRight = false;
	_bool bLeft = false;
	_bool bDown = false;
	_bool bPlusAlpha = false;
	_bool bMinusAlpha = false;
	_bool bRepeatAlpha = false;
	_bool bNoneAlpha = false;
	
	_float speed = 0.f;
	_float maxsize = 0.f;
	_float minsize = 0.f;
	_float creattime = 0.f;
	_float deadtime = 0.f;
	_float movex = 0.f;
	_float movey = 0.f;

	_float alphaCreat = 0.f;
	_float alphaDead = 0.f;

	TextInfo text;
	
	_float2 v2MousePos = _float2(0.f, 0.f);
	_tchar strName[128]={0}; 
	
	char name_Input[100] = "";
	char prototype_Input[100] = "";
	char tag_Change_NameInput[100] = "";
	_char str[256] = {};
	_tchar m_szFONTS[MAX_PATH] = TEXT("");

	_string selectedtexname = "";
	
	vector<FILEDATA> SelectTexture;

	//vector<TextInfo> Textinfo;
	
protected:
	//ID3D11Device* m_pDevice = nullptr;
	//ID3D11DeviceContext* m_pContext = nullptr;
	WINDESC m_Windesc;
	ID3D11Resource* pResoure = nullptr;
	ID3D11ShaderResourceView* pView = nullptr;
	
public:
	static CImGui_UI_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	
	virtual void Free() override;
};

END