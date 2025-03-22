#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_ACOREAPP_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_ACOREAPP_H

BEGIN(Core)
class ACoreInstance;
class AService;

/*
@ Date: 2024-01-09
@ Writer: 박태현
@ Explain
- 프로그램을 시작하기 위한 App 클래스이다. 
*/
class CORE_DLL ACoreApp  {
public:
	ACoreApp();
	virtual ~ACoreApp();
public:
	virtual _bool Start() PURE;
protected:
	template<class T, class... Args>
	SHPTR<T> CreateServiceObject(Args&&... _args) {
		return CreateInitConstructor<T>(m_spCoreInstance, std::forward<Args>(_args)...);
	}
protected: /* get set*/
	CSHPTRREF<ACoreInstance>	GetCoreInstance() { return m_spCoreInstance; }
private:
	SHPTR<ACoreInstance> m_spCoreInstance;
};

END

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_ACOREAPP_H 
