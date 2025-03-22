#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_UDEADLOCKPROFILER_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_UDEADLOCKPROFILER_H

#include "ACoreBase.h"
#include <stack>

BEGIN(Core)

/*
@ Date: 2023-12-29
@ Writer: 박태현
@ Explain
- DeadLock을 검출하기 위한 프로파일러
*/
class UDeadLockProfiler final : public ACoreBase {
public:
	UDeadLockProfiler();
	NO_COPY(UDeadLockProfiler)
	DESTRUCTOR(UDeadLockProfiler)

public:

	void PushLock(const char* _DeadLockLog);
	void PopLock(const char* _DeadLockLog);
	void CheckCycle();
private:
	void Dfs(const _long _Here);

private:
	virtual void Free() override;

private:
	CONUNORMAP<const char*, _long>			m_NameTold;
	CONUNORMAP<_long, const char*>			m_IdToName;
	static thread_local	std::stack<_long>		s_LockStack;
	CONMAP<_long, CONSET<_long>>				m_LockHistory;

	MUTEX																m_Mutex;

	CONVECTOR<_long>										m_DiscoveredOrder; // 노드가 발견된 순서를 기록하는 배열
	std::atomic<_long>											m_DiscoveredCount; // 노드가 발견된 순서
	CONVECTOR<_bool>										m_Finished; // Dfs가 종료되었는지 여부
	CONVECTOR<_long>										m_Parent; 
};

END

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_UDEADLOCKPROFILER_H