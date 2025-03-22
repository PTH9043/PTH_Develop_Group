#include "..\Public\Base.h"
#include "Debug_Manager.h"
#include "RefDebugActor.h"
#include "ConsolDebugActor.h"
#include "Method.h"
#include <ostream>
#include <fstream>

CBase::CBase()
{
#ifdef _DEBUG
	CDebug_Manager* pManager = GET_INSTANCE(CDebug_Manager);
	{
		m_pRefActor = dynamic_cast<CRefDebugActor*>(pManager->Get_DebugActor(CDebug_Manager::DEBUG_REFACTOR));
		// Make Explain	
		if (m_pRefActor)
		{
			m_pRemainDesc = m_pRefActor->Add_RefCount_BaseActor(this, &m_dwRefCnt);
		}
	}
	RELEASE_INSTANCE(CDebug_Manager);
#endif
}

CBase::~CBase()
{

}
#ifdef _DEBUG
unsigned long CBase::AddRef(const _char* FILE , const _uint& LINE , const _char* FUNC)
{
	if (m_pRefActor)
	{
	 	if (nullptr != m_pRemainDesc)
		{
			_string Name = typeid(*this).name();
			EXPLAINS Ref = Make_Explain(Name.c_str());
			_string str = "\n";
			str.append(FILE);
			str.append("\n");
			str.append(to_string(LINE));
			str.append("\n");
			str.append(FUNC);
			str.append("\n");
			str.append("-AddRef");
			str.append("\n");
			Ref.push_back(str);
			m_pRemainDesc->strExplain.append(str);
		}
	}
	return ++m_dwRefCnt;	
}

unsigned long CBase::Release(const _char* FILE , const _uint& LINE , const _char* FUNC)
{
	if (0 == m_dwRefCnt)
	{ 
		if (m_pRefActor)
		{
			// Make Explain	
			_string Name = typeid(*this).name();
			EXPLAINS Ref = Make_Explain(Name.c_str());
			_string str = "\n";
			str.append(FILE);
			str.append("\n");
			str.append(to_string(LINE));
			str.append("\n");
			str.append(FUNC);
			str.append("\n");
			str.append("-Free");
			str.append("\n");
			Ref.push_back(str);
			m_pRefActor->Add_DebugExplain(Name.c_str(), Ref, CRefDebugActor::EX_FREE, m_pRemainDesc);
			m_pRefActor = nullptr;
			m_pRemainDesc = nullptr;
		}
		Free();

		delete this;

		return 0;
	}
	else
	{
		if (m_pRefActor)
		{
			if (nullptr != m_pRemainDesc)
			{
				_string Name = typeid(*this).name();
				_string str = "\n";
				str.append(FILE);
				str.append("\n");
				str.append(to_string(LINE));
				str.append("\n");
				str.append(FUNC);
				str.append("\n");
				str.append("-Release");
				str.append("\n");
				EXPLAINS Ref = Make_Explain(Name.c_str());
				Ref.push_back(str);
				m_pRemainDesc->strExplain.append(str);
			}
		}
		return m_dwRefCnt--;
	}
}
EXPLAINS CBase::Make_Explain(const _string& _strName)
{
	_string strTypeID = "--<ClassName: ";
	_string strTime = "";
	strTypeID.append(_strName);
	strTypeID.append(">-- ");
	return EXPLAINS{ strTypeID };
}
#endif
void CBase::Free()
{
	
}
