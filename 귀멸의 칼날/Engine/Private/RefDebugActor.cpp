#include "RefDebugActor.h"
#include "Base.h"
#include<direct.h>
#include"Method.h"

CRefDebugActor::CRefDebugActor()
{
}

HRESULT CRefDebugActor::NativeConstruct()
{
#ifdef _DEBUG
	_string sPath = "..\\Bin\\";
	sPath.append("Log");

	_int Success = _mkdir(sPath.c_str());

	{
		_string sPath = "..\\Bin\\";
		sPath.append("Log\\Free");

		sPath.append(".txt");
		m_Out.open(sPath.c_str());
	}
#endif
	return S_OK;
}

HRESULT CRefDebugActor::Clear()
{
#ifdef _DEBUG
	ofstream fout;

	_string sPath = "..\\Bin\\";
	sPath.append("Log");
	sPath.append("\\");
	sPath.append("Remain");
	sPath.append(".txt");

	fout;
	fout.open(sPath.c_str());

	for (auto& iter : m_RefList)
		if (nullptr != iter.pInstance)
			fout << typeid(*iter.pInstance).name() << " Ref: " << to_string(*iter.pRefCount) << endl;

	fout.close();

	m_Out.close();
#endif
	return S_OK;
}
#ifdef _DEBUG
DEBUGREMAINDESC* CRefDebugActor::Add_RefCount_BaseActor(CBase* _pInstance, _ulong* _pRefCount)
{
	DEBUGREMAINDESC tDesc;
	tDesc.pInstance = _pInstance;
	tDesc.pRefCount = _pRefCount;
	m_RefList.push_back(tDesc);
	return &m_RefList.back();
}

void CRefDebugActor::Add_DebugExplain( const _char* _pName, EXPLAINS _Explain, const CHECK_ADD_EXPLAIN& _eCheck, 
	DEBUGREMAINDESC* _pDesc)
{
	_string Sentence = Combine_Sentence(_Explain);
	//m_strExplains.append(Sentence);

	if (CRefDebugActor::EX_FREE == _eCheck)
	{
		_string Count = to_string(m_iCount++);
		Count.append(".");
		Count.append(Sentence);
		Count.append("\n");
		m_Out << Count << endl;
		if (_pDesc)
		{
			_pDesc->pInstance = nullptr;
			_pDesc->pRefCount = nullptr;
		}
	}
}
void CRefDebugActor::Clear_Explain()
{
	m_strExplains = "";
}

void CRefDebugActor::Save_Text(const _string& _strName, const _string Log)
{
	_string sPath = "..\\Bin\\";
	sPath.append("Log");
	sPath.append("\\");
	sPath.append(_strName);
	sPath.append(".txt");

	ofstream fout;
	fout.open(sPath.c_str());

	fout << Log << endl;

	fout.close();
}

_string CRefDebugActor::Combine_Sentence(EXPLAINS _Explain)
{
	_string str = "Ref:: ";

	for (auto& iter : _Explain)
	{
		str.append(iter);
		str.append(" ");
	}

	return str;
}
#endif
CRefDebugActor* CRefDebugActor::Create()
{
	CRefDebugActor* pInstance = new CRefDebugActor;
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CRefDebugActor::Create To Failed");
#endif
	}
	return pInstance;
}

void CRefDebugActor::Free()
{
#ifdef _DEBUG
	Sleep(500);
	ofstream fout;

	_string sPath = "..\\Bin\\";
	sPath.append("Log");
	sPath.append("\\");
	sPath.append("Remain");
	sPath.append(".txt");

	fout;
	fout.open(sPath.c_str());

	_bool bIsRemain = false;

	for (auto& iter : m_RefList)
		if(nullptr != iter.pInstance)
		{
			bIsRemain = true;
			fout << typeid(*iter.pInstance).name() << " Ref: " << to_string(*iter.pRefCount) << iter.strExplain << endl;
		}


	if (true == bIsRemain)
	{
		ShellExecute(NULL, L"open", L"..\\Bin\\Log\\Remain.txt", NULL, NULL, SW_SHOWNORMAL);
	}
	m_Out.close();
#endif
}
