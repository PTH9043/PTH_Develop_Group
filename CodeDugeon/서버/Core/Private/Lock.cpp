#include "CoreDefines.h"
#include "Lock.h"

namespace Core
{
	/*
	============================================
	RWLock 
	============================================
	FastSpinLock
	============================================
	*/

	AFastSpinLock::AFastSpinLock() : m_ReadCount{ 0 }, m_WriteFlag{ false } { }

	AFastSpinLock::AFastSpinLock(const AFastSpinLock& _rhs) :
		m_ReadCount{ _rhs.m_ReadCount.load() }, m_WriteFlag{ _rhs.m_WriteFlag.load() } {}

	AFastSpinLock& AFastSpinLock::operator=(const AFastSpinLock& _lock)
	{
		m_ReadCount.store(_lock.m_ReadCount, std::memory_order_seq_cst);
		m_WriteFlag.store(_lock.m_WriteFlag, std::memory_order_seq_cst);
		return *this;
	}

	AFastSpinLock::~AFastSpinLock(){ }

	void AFastSpinLock::EnterWriteLock()
	{
		while (true) {
			m_WriteWaiters.fetch_add(1, std::memory_order_relaxed);

			bool expected = false;
			if (m_WriteFlag.compare_exchange_strong(expected, true, std::memory_order_acquire)) {
				while (m_ReadCount.load(std::memory_order_acquire) != 0) {
					adaptive_spin_wait();
				}
				m_WriteWaiters.fetch_sub(1, std::memory_order_relaxed);
				break;
			}

			m_WriteWaiters.fetch_sub(1, std::memory_order_relaxed);
			adaptive_spin_wait();
		}
	}

	void AFastSpinLock::LeaveWriteLock()
	{
		m_WriteFlag.store(false, std::memory_order_release);
	}

	void AFastSpinLock::EnterReadLock()
	{
		while (true)
		{
			while (m_WriteFlag.load(std::memory_order_relaxed)) {
				adaptive_spin_wait();
			}

			m_ReadWaiters.fetch_add(1, std::memory_order_relaxed);

			if (!m_WriteFlag.load(std::memory_order_acquire)) {
				m_ReadCount.fetch_add(1, std::memory_order_acquire);
				m_ReadWaiters.fetch_sub(1, std::memory_order_relaxed);
				break;
			}

			m_ReadWaiters.fetch_sub(1, std::memory_order_relaxed);
		}
	}

	void AFastSpinLock::LeaveReadLock()
	{
		m_ReadCount.fetch_sub(1, std::memory_order_release);
	}

	void AFastSpinLock::adaptive_spin_wait()
	{
		static thread_local int spin_count = 0;
		++spin_count;

		// Exponential backoff: increasing wait time with each iteration
		if (spin_count < 10) {
			std::this_thread::yield();
		}
		else {
			std::this_thread::sleep_for(std::chrono::nanoseconds(spin_count * 10));
		}

		if (spin_count > 1000) {
			spin_count = 0;
		}
	}

	/*
	============================================
	AFastSpinLock
	============================================
	AReadFastSpinLockguard
	============================================
	*/


	AReadFastSpinLockguard::AReadFastSpinLockguard() : m_SpinLock{nullptr}
	{
	}
	AReadFastSpinLockguard::AReadFastSpinLockguard(AFastSpinLock* _pSpinLock) : m_SpinLock{_pSpinLock}
	{
		m_SpinLock->EnterReadLock();
	}
	AReadFastSpinLockguard::AReadFastSpinLockguard(const AFastSpinLock* _pSpinLock) : m_SpinLock{ const_cast<AFastSpinLock*>(_pSpinLock) }
	{
		m_SpinLock->EnterReadLock();
	}
	AReadFastSpinLockguard::AReadFastSpinLockguard(const AReadFastSpinLockguard& _rhs) : m_SpinLock{_rhs.m_SpinLock}
	{
		_rhs.m_SpinLock = nullptr;
	}
	AReadFastSpinLockguard::~AReadFastSpinLockguard()
	{
		if (nullptr != m_SpinLock)
		{
			m_SpinLock->LeaveReadLock();
		}
	}

	/*
	============================================
	AReadFastSpinLockguard
	============================================
	AWriteFastSpinLockguard
	============================================
	*/

	AWriteFastSpinLockguard::AWriteFastSpinLockguard() : m_SpinLock{ nullptr }
	{
	}
	AWriteFastSpinLockguard::AWriteFastSpinLockguard(AFastSpinLock* _pSpinLock) : m_SpinLock{ _pSpinLock }
	{
		m_SpinLock->EnterWriteLock();
	}
	AWriteFastSpinLockguard::AWriteFastSpinLockguard(const AFastSpinLock* _pSpinLock) : m_SpinLock{ const_cast<AFastSpinLock*>(_pSpinLock) }
	{
		m_SpinLock->EnterWriteLock();
	}
	AWriteFastSpinLockguard::AWriteFastSpinLockguard(const AWriteFastSpinLockguard& _rhs) : m_SpinLock{ _rhs.m_SpinLock }
	{
		_rhs.m_SpinLock = nullptr;
	}
	AWriteFastSpinLockguard::~AWriteFastSpinLockguard()
	{
		if (nullptr != m_SpinLock)
		{
			m_SpinLock->LeaveWriteLock();
		}
	}
}