#include "UI_Base.h"
#include "GameInstance.h"

CUI_Base::CUI_Base(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CUI_Base::CUI_Base(const CUI_Base& _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CUI_Base::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Base::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (FAILED(__super::Ready_Transform(COM_TRANSFORM)))
		return E_FAIL;


	m_pName = new _tchar[30];
	lstrcpy(m_pName, L"##");

	m_TextureName = new _tchar[30];
	lstrcpy(m_TextureName, L"Test2");

	m_PrototypeTag = new _tchar[100];
	lstrcpy(m_PrototypeTag, L"Prototype_GameObject_UI");

	return S_OK;
}

void CUI_Base::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);

	Start_Effect(_dTimeDelta);
	//tick에서 돌려도 될까? timedelta필요함 

	m_fDepthValue = (_float)m_Depth;
}

void CUI_Base::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CUI_Base::Render()
{
	return S_OK;
}

_bool CUI_Base::InterSect(_int mouseX, _int mouseY)
{

	if (mouseX > m_fX - (m_fSizeX / 2) && mouseX < m_fX + (m_fSizeX / 2) &&

		mouseY > m_fY - (m_fSizeY / 2) && mouseY < m_fY + (m_fSizeY / 2))
	{
		m_bInterSect = true;
	}
	else
	{
		m_bInterSect = false;
	}
	return m_bInterSect;
}

void CUI_Base::Serialize(HANDLE hFile, _int Currentlevel, CUI_Base* uiObject)
{
	DWORD    dwByte = 0;
	DWORD    dwStrByte = 0;

	FileInfo info;

	info.m_fX = uiObject->Get_PosX();
	info.m_fY = uiObject->Get_PosY();
	info.m_fSizeX = uiObject->Get_SizeX();
	info.m_fSizeY = uiObject->Get_SizeY();
	info.m_Depth = uiObject->Get_Depth();
	lstrcpy(info.m_pName, uiObject->Get_Name());
	lstrcpy(info.m_TextureName, uiObject->Get_Texture());
	lstrcpy(info.m_PrototypeTag, uiObject->Get_PrototypeTag());
	info.CurrentLevel = Currentlevel;

	info.m_State = uiObject->Get_State();
	info.m_Speed = uiObject->Get_Speed();
	info.m_Max = uiObject->Get_MaxSize();
	info.m_Min = uiObject->Get_MinSize();
	info.m_MoveX = uiObject->Get_MoveX();
	info.m_MoveY = uiObject->Get_MoveY();
	info.m_CreatTime = uiObject->Get_CreatTime();
	info.m_DeadTime = uiObject->Get_DeadTime();

	info.m_Alpha = uiObject->Get_Alpha();
	info.m_AlphaCreat = uiObject->Get_AlphaCreate();
	info.m_AlphaDead = uiObject->Get_AlphaEnd();
	info.m_AlphaState = uiObject->Get_AlphaState();

	WriteFile(hFile, &info, sizeof(FileInfo), &dwByte, nullptr);

	return;
}

//나중에 스피드등 맴버변수를 공유해서 쓰지않게 해야될거같당 

void CUI_Base::Start_Effect(_double timedelta)
{



	if (m_State == GROW)
		Grow_UI(m_Speed, m_MaxSize, m_CreatTime, m_DeadTime, timedelta);

	if (m_State == GROW_DECREASE)
		Grow_Decrease(true, m_Speed, m_MaxSize, m_MinSize, m_CreatTime, m_DeadTime, timedelta);

	if (m_State == RIGHT)
		Go_Right(m_MoveX, m_Speed, m_CreatTime, m_DeadTime, timedelta);

	if (m_State == LEFT)
		Go_Left(m_MoveX, m_Speed, m_CreatTime, m_DeadTime, timedelta);

	if (m_State == DOWN)
		Go_Down(m_MoveY, m_Speed, m_CreatTime, m_DeadTime, timedelta);

	if (m_AlphaState == PLUS_ALPHA)
		Plus_Alpha(0.f, m_fAlphaCreat, m_fAlphaEnd, timedelta);


	if (m_State == UIEFFET_NONE)
		return;
}


void CUI_Base::Grow_Combo(_float speed, _float maxsize, _float creat, _float dead, _double timedelta)
{
	if (m_bEffect == false)
		return;

	//m_Speed = speed; m_MaxSize = maxsize; m_CreatTime = creat; m_DeadTime = dead;

	//m_State = GROW;

	if (ftime > dead && dead != 0.f) return;

	ftime += (_float)timedelta;

	if (ftime < creat) return;

	m_fSizeX += speed;
	m_fSizeY += speed;

	if (m_fSizeX >= maxsize && m_fSizeY >= maxsize)
		m_bEffect = false;

}

void CUI_Base::Grow_UI(_float speed, _float maxsize, _float creat, _float dead, _double timedelta)
{
	//저장 

	if (m_bEffect == false)
		return;

	m_Speed = speed; m_MaxSize = maxsize; m_CreatTime = creat; m_DeadTime = dead;

	m_State = GROW;

	if (ftime > dead && dead != 0.f) return;

	ftime += (_float)timedelta;

	if (ftime < creat) return;

	m_fSizeX += speed;
	m_fSizeY += speed;

	if (m_fSizeX >= maxsize && m_fSizeY >= maxsize)
	{
		m_bEffect = false;
		Minus_Alpha(0.f, 0.f, 1.5f, timedelta);
	}
	else m_bEffect = true;

}
void CUI_Base::Grow_Decrease(_bool infinite, _float speed, _float maxsize, _float minsize, _float creat, _float dead, _double timedelta)
{

	if (m_bEffect == false)
		return;

	//저장
	m_Speed = speed; m_MaxSize = maxsize; m_MinSize = minsize; m_CreatTime = creat; m_DeadTime = dead;

	m_State = GROW_DECREASE;
	//bool infinite 설정해야함 

	if (ftime > dead && dead != 0.f) return;

	ftime += (_float)timedelta;

	if (ftime < creat) return;

	m_bEffect = true;

	if (retry == false)
	{
		if (m_fSizeX >= maxsize && m_fSizeY >= maxsize)
			retry = true;
		m_fSizeX += speed;
		m_fSizeY += speed;
	}

	if (retry == true)
	{
		if (m_fSizeX <= minsize && m_fSizeY <= minsize)
			if (infinite = true)
				retry = false;
			else
				return;
		m_fSizeX -= speed;
		m_fSizeY -= speed;
	}
}

void CUI_Base::Go_Right(_float movex, _float speed, _float creat, _float dead, _double timedelta)
{
	if (m_bEffect == false)
		return;

	//저장 
	m_MoveX = movex; m_Speed = speed; m_CreatTime = creat; m_DeadTime = dead;

	m_State = RIGHT;
	m_bEffect = true;

	if (ftime > dead && dead != 0.f) return;

	ftime += (_float)timedelta;

	if (ftime < creat) return;

	m_fX += speed;
	if (m_fX >= movex)
		m_bEffect = false;

}

void CUI_Base::Go_Left(_float movex, _float speed, _float creat, _float dead, _double timedelta)
{
	if (m_bEffect == false)
		return;

	//저장 
	m_MoveX = movex; m_Speed = speed; m_CreatTime = creat; m_DeadTime = dead;
	m_State = LEFT;
	m_bEffect = true;

	if (ftime > dead && dead != 0.f) return;

	ftime += (_float)timedelta;


	if (ftime < creat) return;

	m_fX -= speed;
	if (m_fX <= movex)
		m_bEffect = false;
}

void CUI_Base::Go_Down(_float movey, _float speed, _float creat, _float dead, _double timedelta)
{
	if (m_bEffect == false)
		return;

	m_MoveY = movey; m_Speed = speed; m_CreatTime = creat; m_DeadTime = dead;
	m_State = DOWN;
	m_bEffect = true;

	if (ftime > dead && dead != 0.f) return;

	ftime += (_float)timedelta;


	if (ftime < creat) return;

	m_fY -= speed;
	if (m_fY <= movey)
		m_bEffect = false;
}

void CUI_Base::Repeat_LR(_float movex, _float speed, _double timedelta)
{


	if (m_bEffect == false)
		return;

	_float maxpos = m_fX + movex;
	_float minpos = m_fX - movex;

	if (minpos <= m_fX && m_brepeat != true)
		m_fX -= speed;

	if (minpos >= m_fX)
		m_brepeat = true;

	if (m_brepeat == true)
	{
		m_fX += speed;

		if (m_fX == maxpos)
		{
			m_brepeat = false;
			m_bEffect = false;
		}
	}
}

void CUI_Base::L_Diagonal(_float4 possize, _float speed, _float creat, _float dead, _bool repeat, _double timedelta)
{

	m_bEffect = true;

	(_double)fDiagonaltime += timedelta;

	if (fDiagonaltime < creat) return;

	if (repeat == false) // 반복을 안할것이다 
	{
		if (m_brepeat == true)
			return;
	}

	m_brepeat = true;

	m_fSizeX = possize.z;
	m_fSizeY = possize.w;
	if (fDiagonaltime >= dead)
	{
		m_fX = possize.x;
		m_fY = possize.y;
		fDiagonaltime = creat;
	}
	else
	{
		m_fX -= speed;
		m_fY -= speed;
	}


}

void CUI_Base::Plus_Alpha(_float Alpha, _float AlphaCreat, _float Alphaend, _double timedelta)
{

	if (m_bEffect == false)
		return;
	
	m_bEffect = true;

	
	m_fAlphaCreat = AlphaCreat; m_fAlphaEnd = Alphaend;// m_fAlpha = Alpha;

	//m_AlphaState = PLUS_ALPHA;

	fAlphatime += (_float)timedelta;

	m_fAlpha = min(max(0, (fAlphatime - AlphaCreat) / Alphaend), 1);

	if (m_fAlpha >= 1.0f)
		m_bEffect = false;

}

void CUI_Base::Minus_Alpha(_float Alpha, _float AlphaCreat, _float Alphaend, _double timedelta)
{
	m_bAlpha = true;

	if (m_bAlpha == false)
		return;


	m_fAlphaCreat = AlphaCreat; m_fAlphaEnd = Alphaend;// m_fAlpha = Alpha;

	//m_AlphaState = MINUS_ALPHA;

	fAlphatime += (_float)timedelta;

	m_fAlpha = min(max(0, 1 - (fAlphatime - AlphaCreat) / Alphaend), 1);

	if (m_fAlpha <= 0.f)
		m_bAlpha = false;
}

void CUI_Base::Repeat_Alpha(_float Alpha, _float AlphaCreat, _float Delaytime, _float Alphaend, _double timedelta)
{
	if (m_bEffect == false)
		return;

	m_bEffect = true;

	m_fAlphaCreat = AlphaCreat; m_fAlphaEnd = Alphaend; m_fAlphaDelay = Delaytime;

	m_AlphaState = REPEAT_ALPHA;

	fAlphatime += (_float)timedelta;

	if (m_fAlpha <= 0.f)
		m_fAlpha = min(max(0, (fAlphatime - AlphaCreat) / Alphaend), 1);

	if (m_fAlpha >= 1.0f)
		m_fAlpha = min(max(0, 1 - (fAlphatime - AlphaCreat) / Alphaend), 1);
}

_bool CUI_Base::Is_MyName(_tchar* nametag)
{
	if (0 == lstrcmp(m_pName, nametag))
		return true;
	else
		return false;
}

_bool CUI_Base::Is_MyTexture(_tchar* texturetag)
{
	if (0 == lstrcmp(m_TextureName, texturetag))

		return true;

	else
		return false;
}


CGameObject* CUI_Base::Clone(void* _pArg)
{
	return nullptr;
}

void CUI_Base::Free()
{
	__super::Free();

	if (m_pName != nullptr)
      Safe_Delete_Array(m_pName);
	
	if (m_TextureName != nullptr)
		Safe_Delete_Array(m_TextureName);

	if (m_PrototypeTag != nullptr)
		Safe_Delete_Array(m_PrototypeTag);
	
}
