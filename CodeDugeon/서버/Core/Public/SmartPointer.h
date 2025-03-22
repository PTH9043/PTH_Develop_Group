#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_SMARTPOINTER_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_SMARTPOINTER_H

namespace Core
{

#ifdef _WIN64
	using llong = long long;
	using p_size = long long;
#else
	using llong = long;
	using p_size = long;
#endif

	template<class T>
	class ARefCounter;

	template<class T>
	class ASharedPtr;

	template<class T>
	class AWeakPtr;

	template<class T>
	class AEnableSharedFromThis;

	template<typename Tp, typename = void>
	struct _Can_enable_shared : std::false_type
	{};

	template<typename Tp>
	struct _Can_enable_shared<Tp, std::void_t<typename Tp::_Esft_type>>
		: std::is_convertible<std::remove_cv_t<Tp>*, typename Tp::_Esft_type*>::type
	{};


	/*
	@ Date: 2023-12-26
	@ Writer: 박태현
	@ Explain: ReferenceCounter -> 값을 매겨주는 카운터, CAS를 통해 관리
	*/
	template<class T>
	class  ARefCounter {

		template<class T, class U>
		friend ASharedPtr<T> static_shared_cast(const ASharedPtr<U>& _rhs);

		template<class T, class U>
		friend ASharedPtr<T> dynamic_shared_cast(const ASharedPtr<U>& _rhs);

		template<class T, class U>
		friend ASharedPtr<T> reinterpret_shared_cast(const ASharedPtr<U>& _rhs);

		template<class T, class U>
		friend ASharedPtr<T> const_shared_cast(const ASharedPtr<U>& _rhs);
	public:
		ARefCounter(T* _ptr) :
			m_Ptr(_ptr), m_RefStrong(1), m_RefWeak(1) {
		}
		ARefCounter(T* _ptr, llong _strong, llong _weak) :
			m_Ptr(_ptr), m_RefStrong{ _strong }, m_RefWeak{ _weak } {
		}

		~ARefCounter() { }
	public:

		ARefCounter<T>* ComparePointerToStrong()
		{
			while (true)
			{
				llong value = m_RefStrong.load();

				if (value > 0)
				{
					if (true == CAS_VALUE(m_RefStrong, value, value + 1))
					{
						return this;
					}
					else
					{
						continue;
					}
				}

				return nullptr;
			}
			return nullptr;
		}
		llong ReleasePointerToStrong()
		{
			llong value{ 0 };
			// Strong의 값이 0
			while (true)
			{
				value = m_RefStrong.load();
				if (true == CAS_VALUE(m_RefStrong, value, value - 1))
				{
					if (value <= 1)
					{
						Core::MemoryRelease<ARefCounter<T>>(this);
					}
					return --value;
				}
			}
			return value;
		}
		ARefCounter<T>* ComparePointerToWeak()
		{
			while (true)
			{
				llong value = m_RefWeak.load();
				if (true == CAS_VALUE(m_RefWeak, value, value + 1))
				{
					return this;
				}
			}
			return nullptr;
		}
		llong ReleasePointerToWeak()
		{
			while (true)
			{
				llong value = m_RefWeak.load();
				if (true == CAS_VALUE(m_RefWeak, value, value - 1))
				{
					value -= 1;
					return value;
				}
			}
			return m_RefWeak.load();
		}

		llong GetUseCount() { return m_RefStrong.load() + m_RefWeak.load(); }

		const std::atomic<llong>& GetRefStrongCount() { return m_RefStrong; }
		const std::atomic<llong>& GetRefWeakCount() { return m_RefWeak; }

		T* get() const { return m_Ptr; }
		T** getAddress() const { return &m_Ptr; }

		void SetShared(T* _shared) { m_Ptr = _shared; }
	private:
		/*
		@ Date: 2023-12-26
		@ Writer: 박태현
		@ Explain 
		- _origin의 값이 _old와 같을 때 _weak를 넣고 true 리턴
		*/
		bool CAS_VALUE(std::atomic<llong>& _origin, llong _old, llong _weak)
		{
			return _origin.compare_exchange_strong(_old, _weak);
		}
		/*
		@ Date: 2023-12-26
		@ Writer: 박태현
		@ Explain
		- 메모리를 재정렬시킨 후 ptr를 set한다.
		*/
		void SetSharedPtr(T* _ptr)
		{
			std::atomic_thread_fence(std::memory_order_seq_cst);
			m_Ptr = _ptr;
		}
	private:
		std::atomic<llong>		m_RefStrong;
		std::atomic<llong>		m_RefWeak;
		mutable T*					m_Ptr;
	};

	/*
	@ Date: 2023-12-26
	@ Writer: 박태현
	@ Explain
	- std::shared_ptr을 벤치마크 한 스마트 포인터로 멀티쓰레드에서 안전한 스마트 포인터이다.
	*/
	template<class T>
	class   ASharedPtr {

		friend class ARefCounter<T>;
		friend class AWeakPtr<T>;
		friend  class AEnableSharedFromThis<T>;

		template<class T, class U>
		friend ASharedPtr<T> static_shared_cast(const ASharedPtr<U>& _rhs);
		template<class T, class U>
		friend ASharedPtr<T> dynamic_shared_cast(const ASharedPtr<U>& _rhs);
		template<class T, class U>
		friend ASharedPtr<T> reinterpret_shared_cast(const ASharedPtr<U>& _rhs);
		template<class T, class U>
		friend ASharedPtr<T> const_shared_cast(const ASharedPtr<U>& _rhs);

		using cas_ptr = std::atomic<llong>;
	public:
		ASharedPtr() : m_RefCounter{ nullptr } { }
		ASharedPtr(ARefCounter<T>* _Ref) : m_RefCounter{ _Ref } {	}
		ASharedPtr(nullptr_t) : m_RefCounter{ nullptr } {}
		template<class U>
			requires std::is_base_of_v<T, U>
		ASharedPtr(const ASharedPtr<U>& _rhs) : m_RefCounter{ nullptr }
		{
			m_RefCounter = reinterpret_cast<ARefCounter<T>*>(_rhs.AddShared());
		}
		ASharedPtr(const ASharedPtr& _rhs) : m_RefCounter{ nullptr } { Move(_rhs); }

		ASharedPtr(ASharedPtr&& _rhs) noexcept : m_RefCounter{ _rhs.m_RefCounter } { _rhs.m_RefCounter = nullptr; }

		ASharedPtr(const AWeakPtr<T>& _rhs) : m_RefCounter{ _rhs.AddShared() } {  }

		~ASharedPtr()
		{
			Release(m_RefCounter);
		}
	private:
		/*
		@ Date: 2023-12-26
		@ Writer: 박태현
		@ Explain
		- EnableSharedFromThis 클래스를 상속 받는 클래스에 불러올 SharedPtr( 자기 자신의 Weak 포인터를 
		가지게 하기 위함이다.
		*/
		void _Enable_shared_from_this(T* _ptr, std::true_type) {
			_ptr->SetSharedFromThis(*this);
		}
		void _Enable_shared_from_this(T* _ptr, std::false_type) {}
	public:
		void SetEnableShared(T* _ptr, ARefCounter<T>* new_ctr)
		{
			m_RefCounter = new_ctr;
			_Enable_shared_from_this(_ptr, std::bool_constant<std::conjunction_v<_Can_enable_shared<T>>>{});
		}

	public:

		ASharedPtr& operator =(nullptr_t)
		{
			ARefCounter<T>* prevptr{ nullptr };
			while (true)
			{
				prevptr = m_RefCounter;
				if (prevptr == nullptr)
					return *this;

				if (true == CAS_POINTER(&m_RefCounter, prevptr, nullptr))
				{
					Release(prevptr);
					return *this;
				}
			}
			return *this;
		}

		ASharedPtr& operator =(const ASharedPtr& _rhs)
		{
			return Move(_rhs);
		}

		ASharedPtr& operator =(ASharedPtr&& _rhs)
		{
			ARefCounter<T>* prevptr{ nullptr };
			ARefCounter<T>* otherptr{ nullptr };

			while (true)
			{
				prevptr = m_RefCounter;
				otherptr = _rhs.m_RefCounter;
				// 상대방이 null일 때
				if (otherptr == nullptr)
				{
					if (true == CAS_POINTER(&m_RefCounter, prevptr, nullptr))
					{
						return *this;
					}
					continue;
				}
				// 자기 자신과 같을 때
				if (otherptr == prevptr)
				{
					_rhs.m_RefCounter = nullptr;
					return *this;
				}
				// 상대방과 다른 때
				if (true == CAS_POINTER(&m_RefCounter, prevptr, otherptr))
				{
					_rhs.m_RefCounter = nullptr;
					return *this;
				}
			}
			return *this;
		}

		template<class U, typename std::enable_if_t<std::is_convertible_v<U*, T*>, int> = 0>
			requires std::is_base_of_v<T, U>
		ASharedPtr& operator =(const ASharedPtr<U>& _rhs)
		{
			ARefCounter<T>* prevptr{ nullptr };
			ARefCounter<T>* otherptr{ nullptr };

			while (true)
			{
				prevptr = m_RefCounter;
				otherptr = reinterpret_cast<ARefCounter<T>*>(_rhs.AddShared());
				// 다른 sharedPtr이 NULL 일때, 현재 값을 Release 한다. 
				if (otherptr == nullptr)
				{
					if (true == CAS_POINTER(&m_RefCounter, prevptr, nullptr))
					{
						if (nullptr != prevptr)
						{
							Release(prevptr);
						}
						return *this;
					}
					continue;
				}
				// 같은 값일 때, 한쪽 rRelease 
				if (otherptr == prevptr)
				{
					Release(otherptr);
					return *this;
				}
				// 다른 값일 때, 해당 값과 현재 값을 교환 한 후 본래 값을 Release 한다. 
				if (true == CAS_POINTER(&m_RefCounter, prevptr, otherptr))
				{
					if (nullptr != prevptr)
					{
						Release(prevptr);
					}
					return *this;
				}
				Release(otherptr);
			}

			return *this;
		}

		ASharedPtr& operator =(const AWeakPtr<T>& _rhs)
		{
			ARefCounter<T>* prevptr{ nullptr };
			ARefCounter<T>* otherptr{ nullptr };

			while (true)
			{
				prevptr = m_RefCounter;
				otherptr = _rhs.AddShared();
				// 다른 녀석이 null 일때 
				if (otherptr == nullptr)
				{
					if (true == CAS_POINTER(&m_RefCounter, prevptr, nullptr))
					{
						if (nullptr != prevptr)
						{
							Release(prevptr);
						}
						return *this;
					}
					continue;
				}
				// 값이 같을 떄
				if (otherptr == prevptr)
				{
					Release(otherptr);
					return *this;
				}
				// 값이 다를 때
				if (true == CAS_POINTER(&m_RefCounter, prevptr, otherptr))
				{
					if (nullptr != prevptr)
					{
						Release(prevptr);
					}
					return *this;
				}
				// 모두 실패하면 Add 한 값을 Release하고 다시 
				Release(otherptr);
			}

			return *this;
		}

		T* operator->() const { return get(); }
		T& operator*() { return *get(); }

		bool operator ==(nullptr_t _ptr) const { return m_RefCounter == _ptr; }
		bool operator !=(nullptr_t _ptr) const { return m_RefCounter != _ptr; }

		bool operator==(const ASharedPtr& other) const {	return m_RefCounter == other.m_RefCounter;	}
		bool operator!=(const ASharedPtr& other) const { return m_RefCounter != other.m_RefCounter; }

		template<class T2>
		bool operator ==(const T2*& _rhs) const { return m_RefCounter == _rhs; }

		template<class T2>
		bool operator !=(const T2*& _rhs) const { return m_RefCounter != _rhs; }

		template<class T2>
		bool operator ==(const ASharedPtr<T2>& _rhs) const { return m_RefCounter == _rhs.m_RefCounter; }

		template<class T2>
		bool operator !=(const ASharedPtr<T2>& _rhs) const { return m_RefCounter != _rhs.m_RefCounter; }


		ASharedPtr<T>* operator&() { return this; }

		T* get() const { if (nullptr == m_RefCounter) return nullptr; return m_RefCounter->get(); }
		T** getAddressof() const { if (nullptr == m_RefCounter) return nullptr; return m_RefCounter->getAddress(); }

		void reset()
		{
			ARefCounter<T>* prevref{ nullptr };
			while (true)
			{
				prevref = m_RefCounter;
				if (prevref == nullptr)
					return;

				if (true == CAS_POINTER(&m_RefCounter, prevref, nullptr))
				{
					Release(prevref);
					return;
				}
			}
		}

		llong use_count()
		{
			return m_RefCounter->GetUseCount();
		}

		operator bool()
		{
			ARefCounter<T>* currptr = m_RefCounter;

			if (nullptr != currptr)
				if (currptr->GetUseCount() > 0)
					return true;

			return false;
		}

		ARefCounter<T>* AddShared() const
		{
			ARefCounter<T>* prev{ nullptr };
			ARefCounter<T>* curr{ nullptr };
			ARefCounter<T>* getptr{ nullptr };

			while (true)
			{
				prev = m_RefCounter;
				if (nullptr == prev)
					return nullptr;

				getptr = prev->ComparePointerToStrong();
				curr = m_RefCounter;
				// 현재 가져온 ptr과 curr 값들이 같으면 리턴
				if (getptr == curr)
					return curr;
				// 실패하면 Release 
				if (getptr != nullptr)
					prev->ReleasePointerToStrong();
			}
			return nullptr;
		}
	private:
		/*
		@ Date: 2023-12-26
		@ Writer: 박태현
		@ Explain
		- Refcounter의 값을 NULL로 초기화
		*/
		void RefValueNull()
		{
			ARefCounter<T>* prev{ nullptr };
			ARefCounter<T>* curr{ nullptr };

			while (true)
			{
				prev = m_RefCounter;
				if (nullptr == prev)
					return;

				if (true == CAS_POINTER(&m_RefCounter, prev, nullptr))
					return;
			}
		}
		ARefCounter<T>* AddWeak() const
		{
			ARefCounter<T>* prev{ nullptr };
			ARefCounter<T>* curr{ nullptr };
			ARefCounter<T>* getptr{ nullptr };

			while (true)
			{
				prev = m_RefCounter;
				if (nullptr == prev)
					return nullptr;

				getptr = prev->ComparePointerToWeak();
				curr = m_RefCounter;

				if (getptr == curr)
					return curr;

				if (getptr != nullptr)
					prev->ReleasePointerToWeak();
			}
			return nullptr;
		}

		bool CAS_POINTER(ARefCounter<T>** _Counter, ARefCounter<T>* _old, ARefCounter<T>* _new) const
		{
			llong value1 = reinterpret_cast<llong>(_old);
			llong value2 = reinterpret_cast<llong>(_new);
			return std::atomic_compare_exchange_strong(reinterpret_cast<std::atomic<llong>*>(_Counter), &value1, value2);
		}

		void Release(ARefCounter<T>*& _ref)
		{
			if (nullptr == _ref)
				return;

			llong value = _ref->ReleasePointerToStrong();
			if (value <= 0)
			{
				_ref = nullptr;
			}
		}

		ASharedPtr<T>& Move(const ASharedPtr<T>& _rhs)
		{
			ARefCounter<T>* prevptr{ nullptr };
			ARefCounter<T>* otherptr{ nullptr };

			while (true)
			{
				prevptr = m_RefCounter;
				otherptr = _rhs.AddShared();

				if (otherptr == nullptr)
				{
					if (true == CAS_POINTER(&m_RefCounter, prevptr, nullptr))
					{
						if (nullptr != prevptr)
						{
							Release(prevptr);
						}
						return *this;
					}
					continue;
				}

				if (otherptr == prevptr)
				{
					Release(otherptr);
					return *this;
				}

				if (true == CAS_POINTER(&m_RefCounter, prevptr, otherptr))
				{
					if (nullptr != prevptr)
					{
						Release(prevptr);
					}
					return *this;
				}
				Release(otherptr);
			}
		}

	private:
		mutable ARefCounter<T>* m_RefCounter;
	};

	template<class T>
	bool operator==(nullptr_t _lhs, const ASharedPtr<T>& _rhs) { return _lhs == _rhs.get(); }
	template<class T>
	bool operator!=(nullptr_t _lhs, const ASharedPtr<T>& _rhs) { return _lhs != _rhs.get(); }
	template<class T, class U>
	bool operator==(U* _lhs, const ASharedPtr<T>& _rhs) { return _lhs == _rhs.get(); }
	template<class T, class U>
	bool operator!=(U* _lhs, const ASharedPtr<T>& _rhs) { return _lhs != _rhs.get(); }

	/*
	@ Date: 2023-12-26
	@ Writer: 박태현
	@ Explain
	- weak_ptr 대신 멀티쓰레드에서 사용할 수 있는 WeakPtr 정의
	*/
	template<class T>
	class   AWeakPtr {
		friend class ARefCounter<T>;
		friend class ASharedPtr<T>;
		friend class AEnableSharedFromThis<T>;
	public:
		AWeakPtr() : m_RefCounter{ nullptr } { }
		AWeakPtr(ARefCounter<T>* _Ref) : m_RefCounter{ _Ref->ComparePointerToWeak() } {	}
		AWeakPtr(nullptr_t) : m_RefCounter{ nullptr } {}
		template<class U>
		AWeakPtr(const AWeakPtr<U>& _rhs) : m_RefCounter{ _rhs.AddWeak() } {}
		AWeakPtr(const AWeakPtr& _rhs) : m_RefCounter{ _rhs.AddWeak() } {	}
		AWeakPtr(AWeakPtr&& _rhs) : m_RefCounter{ _rhs.m_RefCounter } { _rhs.m_RefCounter = nullptr; }
		AWeakPtr(ASharedPtr<T> _sharedPtr) : m_RefCounter{ _sharedPtr.AddWeak() } { }
		~AWeakPtr() { Release(m_RefCounter); }
	public:

		AWeakPtr& operator =(nullptr_t)
		{
			ARefCounter<T>* prevptr{ nullptr };
			while (true)
			{
				prevptr = m_RefCounter;
				if (prevptr == nullptr)
					return *this;

				if (true == CAS_POINTER(&m_RefCounter, prevptr, nullptr))
				{
					Release(prevptr);
					return *this;
				}
			}
			return *this;
		}

		AWeakPtr& operator =(const ASharedPtr<T>& _rhs)
		{
			ARefCounter<T>* prevptr{ nullptr };
			ARefCounter<T>* otherptr{ nullptr };

			while (true)
			{
				prevptr = m_RefCounter;
				otherptr = _rhs.AddWeak();

				if (otherptr == nullptr)
				{
					if (true == CAS_POINTER(&m_RefCounter, prevptr, nullptr))
					{
						if (nullptr != prevptr)
						{
							Release(prevptr);
						}
						return *this;
					}
					continue;
				}

				if (otherptr == prevptr)
				{
					Release(otherptr);
					return *this;
				}

				if (true == CAS_POINTER(&m_RefCounter, prevptr, otherptr))
				{
					if (nullptr != prevptr)
					{
						Release(prevptr);
					}
					return *this;
				}
				Release(otherptr);
			}

			return *this;
		}

		AWeakPtr& operator =(const AWeakPtr<T>& _rhs)
		{
			ARefCounter<T>* prevptr{ nullptr };
			ARefCounter<T>* otherptr{ nullptr };

			while (true)
			{
				prevptr = m_RefCounter;
				otherptr = _rhs.AddWeak();

				if (otherptr == nullptr)
				{
					if (true == CAS_POINTER(&m_RefCounter, prevptr, nullptr))
					{
						if (nullptr != prevptr)
						{
							Release(prevptr);
						}
						return *this;
					}
					continue;
				}

				if (otherptr == prevptr)
				{
					Release(otherptr);
					return *this;
				}

				if (true == CAS_POINTER(&m_RefCounter, prevptr, otherptr))
				{
					if (nullptr != prevptr)
					{
						Release(prevptr);
					}
					return *this;
				}
				Release(otherptr);
			}

			return *this;
		}

		bool operator ==(nullptr_t _ptr) const { return m_RefCounter == _ptr; }
		bool operator !=(nullptr_t _ptr) const { return m_RefCounter != _ptr; }

		template<class T2>
		bool operator ==(const T2*& _rhs) const { return m_RefCounter == _rhs; }

		template<class T2>
		bool operator !=(const T2*& _rhs) const { return m_RefCounter != _rhs; }

		template<class T2>
		bool operator ==(const ASharedPtr<T2>& _rhs) const { return m_RefCounter == _rhs.m_RefCounter; }

		template<class T2>
		bool operator !=(const ASharedPtr<T2>& _rhs) const { return m_RefCounter != _rhs.m_RefCounter; }

		ASharedPtr<T> lock() const
		{ 
			if (nullptr == m_RefCounter) 
				return nullptr;  
			return ASharedPtr<T>(*this); 
		}

		void reset()
		{
			ARefCounter<T>* prevref{ nullptr };
			while (true)
			{
				prevref = m_RefCounter;
				if (prevref == nullptr)
					return;

				if (true == CAS_POINTER(&m_RefCounter, prevref, nullptr))
				{
					Release(prevref);
					return;
				}
			}
		}

		llong use_count() const
		{
			return m_RefCounter->GetUseCount();
		}

		operator bool() const
		{
			ARefCounter<T>* currptr = m_RefCounter;

			if (nullptr != currptr)
				if (currptr->GetUseCount() > 0)
					return true;

			return false;
		}

		void SetEnableShared(const ASharedPtr<T>& _rhs)
		{
			m_RefCounter = _rhs.m_RefCounter->ComparePointerToWeak();
		}
	private:

		ARefCounter<T>* AddShared() const
		{
			ARefCounter<T>* prev{ nullptr };
			ARefCounter<T>* curr{ nullptr };
			ARefCounter<T>* getptr{ nullptr };

			while (true)
			{
				prev = m_RefCounter;
				if (nullptr == prev)
					return nullptr;

				getptr = prev->ComparePointerToStrong();
				curr = m_RefCounter;

				if (getptr == curr)
					return curr;

				if (getptr != nullptr)
					prev->ReleasePointerToStrong();
			}
			return nullptr;
		}

		ARefCounter<T>* AddWeak() const
		{
			ARefCounter<T>* prev{ nullptr };
			ARefCounter<T>* curr{ nullptr };
			ARefCounter<T>* getptr{ nullptr };

			while (true)
			{
				prev = m_RefCounter;
				if (nullptr == prev)
					return nullptr;

				getptr = prev->ComparePointerToWeak();
				curr = m_RefCounter;

				if (getptr == curr)
					return curr;

				if (getptr != nullptr)
					prev->ReleasePointerToWeak();
			}
			return nullptr;
		}

		bool CAS_POINTER(ARefCounter<T>** _Counter, ARefCounter<T>* _old, ARefCounter<T>* _new) const
		{
			llong value1 = reinterpret_cast<llong>(_old);
			llong value2 = reinterpret_cast<llong>(_new);
			return std::atomic_compare_exchange_strong(reinterpret_cast<std::atomic<llong>*>(_Counter), &value1, value2);
		}

		void Release(ARefCounter<T>*& _ptr)
		{
			if (nullptr == _ptr)
				return;

			llong value = _ptr->ReleasePointerToWeak();
			if (value <= 0)
			{
				m_RefCounter = nullptr;
			}
		}
	private:
		mutable ARefCounter<T>* m_RefCounter;
	};

	template<typename T>
	class AEnableSharedFromThis {
		friend  class ASharedPtr<T>;
		friend  class ARefCounter<T>;
	protected:
		AEnableSharedFromThis()
			: m_Wptr()
		{}

		AEnableSharedFromThis(const AEnableSharedFromThis& other)
			: m_Wptr()
		{}

		AEnableSharedFromThis& operator=(const AEnableSharedFromThis&)
		{
			return (*this);
		}

		~AEnableSharedFromThis() = default;
	public:
		using _Esft_type = AEnableSharedFromThis;

		ASharedPtr<T> shared_from_this()
		{
			return (ASharedPtr<T>(m_Wptr));
		}

		AWeakPtr<T> weak_from_this()
		{
			return m_Wptr;
		}

		void SetSharedFromThis(const ASharedPtr<T>& _ptr) { m_Wptr.SetEnableShared(_ptr); }
	private:
		AWeakPtr<T> m_Wptr;
	};

	template<class T, class ...Args>
	ASharedPtr<T> MakeShared(Args&&... _args)
	{
		ASharedPtr<T> SharedPtr;
		T* ptr =  Core::MemoryAlloc<T>(std::forward<Args>(_args)...);
		ARefCounter<T>* pRefCounter = Core::MemoryAlloc<ARefCounter<T>>(ptr);
		SharedPtr.SetEnableShared(ptr, pRefCounter);
		return std::move(SharedPtr);
	}

	template<class T, class U>
	ASharedPtr<T> static_shared_cast(const ASharedPtr<U>& _rhs)
	{
		ARefCounter<T>* RefCounter = reinterpret_cast<ARefCounter<T>*>(_rhs.AddShared());
		RefCounter->SetShared(static_cast<T*>(_rhs.m_RefCounter->get()));
		return ASharedPtr<T> {RefCounter};
	}

	template<class T, class U>
	ASharedPtr<T> dynamic_shared_cast(const ASharedPtr<U>& _rhs)
	{
		ARefCounter<T>* RefCounter = reinterpret_cast<ARefCounter<T>*>(_rhs.AddShared());

		T* ptr = dynamic_cast<T*>(_rhs.m_RefCounter->get());
		RETURN_CHECK(nullptr == ptr, nullptr)

		RefCounter->SetShared(ptr);
		return ASharedPtr<T> {RefCounter};
	}

	template<class T, class U>
	ASharedPtr<T> reinterpret_shared_cast(const ASharedPtr<U>& _rhs)
	{
		ARefCounter<T>* RefCounter = reinterpret_cast<ARefCounter<T>*>(_rhs.AddShared());
		RefCounter->SetShared(reinterpret_cast<T*>(_rhs.m_RefCounter->m_Ptr));
		return ASharedPtr<T> {RefCounter};
	}

	template<class T, class U>
	ASharedPtr<T> const_shared_cast(const ASharedPtr<U>& _rhs)
	{
		ARefCounter<T>* RefCounter = reinterpret_cast<ARefCounter<T>*>(_rhs.AddShared());
		RefCounter->SetShared(const_cast<T*>(_rhs.m_RefCounter->m_Ptr));
		return ASharedPtr<T> {RefCounter};
	}
}
/*
@ Date: 2023-12-26
@ Writer: 박태현
@ Explain
- type 재정의
*/
namespace Core {

	template<class T>
	using SHPTR = ASharedPtr<T>;

	template<class T>
	using CSHPTR = const SHPTR<T>;

	template<class T>
	using CSHPTRREF = const SHPTR<T>&;

	template<class T>
	using WKPTR = AWeakPtr<T>;

	template<class T>
	using CWKPTR = const WKPTR<T>;

	template<class T>
	using CWKPTRREF = const WKPTR<T>&;
}


namespace std
{
	/*
	@ Date: 2023-12-26,  Writer: 박태현
	@ Explain
	- HASH opeartor() 연산에 대한 SharedPtr 재정의, concurrent_ 컨테이너에 사용하기 위함
	*/
	template <typename T>
	struct hash<Core::ASharedPtr<T>> {
		size_t operator()(const Core::ASharedPtr<T>& ptr) const {
			return std::hash<T*>()(ptr.get());
		}
	};
	/*
	@ Date: 2023-12-26,  Writer: 박태현
	@ Explain
	- Equal 연산에 대한 SharedPtr 재정의, concurrent_ 컨테이너에 사용하기 위함
	*/
	template <typename T>
	struct equal_to<Core::ASharedPtr<T>> {
		bool operator()(const Core::ASharedPtr<T>& lhs, const Core::ASharedPtr<T>& rhs) const {
			return lhs.get() == rhs.get();
		}
	};
	/*
	@ Date: 2023-12-26,  Writer: 박태현
	@ Explain
	- less 연산에 대한 SharedPtr 재정의, concurrent_ 컨테이너에 사용하기 위함
	*/
	template <typename T>
	struct less<Core::ASharedPtr<T>> {
		bool operator()(const Core::ASharedPtr<T>& lhs, Core::ASharedPtr<T>& rhs) const {
			return lhs.get() < rhs.get();
		}
	};

	/*
	@ Date: 2023-12-26,  Writer: 박태현
	@ Explain
	- less 연산에 대한 SharedPtr 재정의, concurrent_ 컨테이너에 사용하기 위함
	*/
	template <typename T>
	struct less_equal<Core::ASharedPtr<T>> {
		bool operator()(const Core::ASharedPtr<T>& lhs, Core::ASharedPtr<T>& rhs) const {
			return lhs.get() <= rhs.get();
		}
	};
}

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_SMARTPOINTER_H