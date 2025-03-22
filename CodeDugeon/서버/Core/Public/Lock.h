#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_LOCK_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_LOCK_H

namespace Core {

	class UDeadLockProfiler;


	/*
	@ Date: 2024-01-06,  Writer: 박태현
	@ Explain
	- AWS 버전에서 따온 SpinLock
	*/
	class CORE_DLL AFastSpinLock {
	public:
		AFastSpinLock();
		AFastSpinLock(const AFastSpinLock& _rhs);
		AFastSpinLock& operator =(const AFastSpinLock& _lock);
		~AFastSpinLock();

		/// exclusive mode
		void EnterWriteLock();
		void LeaveWriteLock();

		/// shared mode
		void EnterReadLock();
		void LeaveReadLock();
	private:
		void adaptive_spin_wait();
	private:
		std::atomic<_llong>		m_ReadCount;
		std::atomic<_bool>			m_WriteFlag;
		std::atomic<_int>			m_ReadWaiters;
		std::atomic<_int>			m_WriteWaiters;
	};

	class CORE_DLL AReadFastSpinLockguard {
	public:
		AReadFastSpinLockguard();
		AReadFastSpinLockguard(AFastSpinLock* _pSpinLock);
		AReadFastSpinLockguard(const AFastSpinLock* _pSpinLock);
		AReadFastSpinLockguard(const AReadFastSpinLockguard& _rhs);
		~AReadFastSpinLockguard();
	private:
		mutable AFastSpinLock* m_SpinLock;
	};

	class CORE_DLL AWriteFastSpinLockguard {
	public:
		AWriteFastSpinLockguard();
		AWriteFastSpinLockguard(AFastSpinLock* _pSpinLock);
		AWriteFastSpinLockguard(const AFastSpinLock* _pSpinLock);
		AWriteFastSpinLockguard(const AWriteFastSpinLockguard& _rhs);
		~AWriteFastSpinLockguard();
	private:
		mutable AFastSpinLock* m_SpinLock;
	};

}
#endif 