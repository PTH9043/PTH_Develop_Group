#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_LOCKFREE_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_LOCKFREE_H

namespace Core
{
	static constexpr unsigned  long long MASK_VALUE = 0xFFFFFFFFFFFFFFFE;
	static constexpr int CHECK_MARKING{ 0x1 };
	/*
	@ Date: 2023-12-29, Writer: 박태현
	@ Explain: 멀티쓰레드 환경에서 하나의 포인터에 CAS가 몰리는 것을 방지하기 위해 쓰레드를 잠시 대기시키는 클래스
	*/
	class Backoff {
	public:
		Backoff(int _minDelay, int _maxDelay) : m_UniformBackOff{ _minDelay, _maxDelay }, m_Limit{ 0 }, m_MaxDelay{ _maxDelay } {}

		void Relax() {
			int delay = 0;
			m_Limit = m_UniformBackOff(m_Dre);
			m_Limit *= 2;
			if (m_Limit > m_MaxDelay)
				m_Limit = m_MaxDelay;
			std::this_thread::sleep_for(std::chrono::microseconds(delay));
		}
	private:
		std::default_random_engine					m_Dre;
		std::uniform_int_distribution<int>		m_UniformBackOff;
		int																m_MaxDelay;
		int																m_Limit;
	};

	/*
	@ Date: 2023-12-31, Writer: 박태현
	@ Explain: EBR Controller를 정의함
	*/
	template<class NODE>
	class EBRController {
	public:
		EBRController() : m_Epoch{ 0 }, m_RemainCount{ 0 }, m_Retired{},
			m_Reservation{}, m_CurrentNumThreads{ } {}
		~EBRController()
		{
			clear_ebr();
		}

		void Initialize(int _currThread, int _remainCount)
		{
			m_CurrentNumThreads = _currThread;
			m_RemainCount = _remainCount;

			for (int i = 0; i < m_CurrentNumThreads; ++i)
			{
				m_Reservation[i] = 0;
			}
		}

		// 만들어진 시간들 중 가장 먼저 호출된 시간을 가져온다. 
		unsigned int get_min_reservation() {
			unsigned int min_re = 0xffffffff;
			for (int i = 0; i < m_CurrentNumThreads; ++i) {
				min_re = std::min(min_re, m_Reservation[i].load(std::memory_order_relaxed));
			}
			return min_re;
		}

		// 호출된 시간이 가장 작은 것을 기준으로 메모리 해제
		void empty() {
			unsigned int max_safe_epoch = get_min_reservation();

			while (false == m_Retired[TLS::g_ThreadID].empty()) {
				auto f = m_Retired[TLS::g_ThreadID].front();
				if (f->retireEpoch >= max_safe_epoch)
					break;
				m_Retired[TLS::g_ThreadID].pop();
				Core::MemoryRelease(f);
			}
		}

		// RemainCount보다 크면 Empty 호출 
		void retire(NODE* ptr) {
			m_Retired[TLS::g_ThreadID].push(ptr);
			ptr->retireEpoch = m_Epoch.load(std::memory_order_relaxed);
			if (m_Retired[TLS::g_ThreadID].size() >= m_RemainCount) {
				empty();
			}
		}

		// Reservation에 예약
		void start_op() {
			m_Reservation[TLS::g_ThreadID].store(m_Epoch.fetch_add(1, std::memory_order_relaxed), std::memory_order_relaxed);
		}

		// Reservation을 해제한다. 
		void end_op() {
			m_Reservation[TLS::g_ThreadID].store(ENDOP_VALUE, std::memory_order_relaxed);
		}

	private:
		// 프로그램 종료시 모든 메모리를 해제한다. 
		void clear_ebr()
		{
			for (int i = 0; i < m_CurrentNumThreads; ++i)
			{
				while (!m_Retired[i].empty())
				{
					NODE* node = m_Retired[i].front();
					Core::MemoryRelease(node);
					m_Retired[i].pop();
				}
			}
			m_Epoch = 0;
		}

	private:
		static constexpr	unsigned int												ENDOP_VALUE{ 0xffffffff };
		std::atomic_uint																		m_Epoch;
		unsigned int																				m_RemainCount;
		std::array<std::queue<NODE*>, TLS::MAX_THREAD>	m_Retired;
		std::array<std::atomic_uint, TLS::MAX_THREAD>			m_Reservation;
		int																								m_CurrentNumThreads;
	};

	namespace PTHStack {

		/*
		@ Date: 2023-12-29, Writer: 박태현
		@ Explain: Stack의 노드
		*/
		template<class T>
		struct __declspec(align(16)) NODE {

			NODE<T>() : next{ nullptr }, retireEpoch{ 0 }
			{
				if constexpr ((true == std::is_pointer<T>()) || (true == SmartPointer<T>))
					value = nullptr;
				else
					value = T();
			}
			NODE<T>(T _value) : next{ nullptr }, retireEpoch{ 0 }, value{ _value }{}

			~NODE<T>()
			{
				if constexpr ((std::is_pointer<T>()))
				{
					Core::MemoryAlloc(value);
				}
			}
		public:
			T									value;
			NODE<T>* volatile	next;
			std::atomic_uint		retireEpoch;
		};
		/*
		@ Date: 2023-12-29, Writer: 박태현
		@ Explain: LockFreeStack + Backoff
		*/
		template<class T>
		class  LockFreeStack {
		public:
			LockFreeStack() : m_BackOff{ 1, 50 }, m_Top{ nullptr } { }
			~LockFreeStack() {
				clear();
			}

			void Initialize(int _currentThread, int _remainCount)
			{
				m_EBRController.Initialize(_currentThread, _remainCount);
			}

			// 맨 위에 있는 값을 꺼내온다. 
			T Top()
			{
				NODE<T>* pNode = m_Top;
				if (nullptr == pNode)
				{
					return T();
				}
				return pNode->value;
			}

			// Top에 값을 밀어 넣는다. 
			void Push(T _value)
			{
				m_EBRController.start_op();
				NODE<T>* newNode = Core::MemoryAlloc<NODE<T>>( _value );
				while (true) {
					NODE<T>* p = m_Top;
					newNode->next = p;
					// 카스 성공
					if (true == CAS(p, newNode)) {
						m_EBRController.end_op();
						return;
					}
					// 실패시 잠시 대기
					m_BackOff.Relax();
				}
			}
			// Top으로부터 값을 꺼낸다. 
			T Pop() {
				m_EBRController.start_op();
				while (true)
				{
					NODE<T>* ptr = m_Top;
					if (ptr == nullptr)
					{
						m_EBRController.end_op();
						return T();
					}
					NODE<T>* next = ptr->next;
					T data = ptr->value;

					if (false == CAS(ptr, next))
					{
						m_BackOff.Relax();
						continue;
					}
					m_EBRController.retire(ptr);
					m_EBRController.end_op();
					return data;
				}
			}

			bool IsEmpty()
			{
				NODE<T>* pNode = m_Top;
				return pNode == nullptr;
			}

			void clear()
			{
				NODE<T>* p = m_Top;
				while (p != nullptr) {
					NODE<T>* t = p;
					p = p->next;
					Core::MemoryRelease<NODE<T>>(t);
				}
				m_Top = nullptr;
			}
		private:
			bool CAS(NODE<T>* _old, NODE<T>* _new)
			{
				return std::atomic_compare_exchange_strong(reinterpret_cast<volatile std::atomic_llong*>(&m_Top),
					reinterpret_cast<long long*>(&_old),
					reinterpret_cast<long long>(_new));
			}
		private:
			EBRController<NODE<T>>		m_EBRController;
			Core::Backoff							m_BackOff;
			NODE<T>* volatile					m_Top;
		};
	}

	namespace PTHList {

		/*
		@ Date: 2024-01-21, Writer: 박태현
		@ Explain: List의 노드이다. 
		*/
		template<class T>
		class __declspec(align(16)) LFNODE {
		public:
			T value;
			unsigned int retireEpoch;
			// remove 되었는지 확인 
			unsigned long long next;

			LFNODE() : retireEpoch{ 0 }, next{ 0 } {
				if constexpr ((true == std::is_pointer<T>()) || (true == SmartPointer<T>))
					value = nullptr;
				else
					value = T();
			}
			LFNODE(T _value) : retireEpoch{ 0 }, next{ 0 } {
				value = _value;
			}
			~LFNODE() 
			{
				if constexpr ((std::is_pointer<T>()))
				{
					Core::MemoryAlloc(value);
				}
			}
			LFNODE* GetNext() { 
				return reinterpret_cast<LFNODE*>(next & MASK_VALUE);
			}
			void SetNext(LFNODE* ptr) {
				next = reinterpret_cast<unsigned long long>(ptr);
			}
			LFNODE* GetNextWithMark(bool* mark) {
				unsigned long long  temp = next;
				*mark = (temp % 2) == 1;
				return reinterpret_cast<LFNODE*>(temp & MASK_VALUE);
			}
			bool CAS(unsigned long long old_value, unsigned long long new_value)
			{
				return atomic_compare_exchange_strong(
					reinterpret_cast<std::atomic_ullong*>(&next),
					&old_value, new_value);
			}
			bool CAS(LFNODE* old_next, LFNODE* new_next, bool old_mark, bool new_mark) {
				unsigned long long old_value = reinterpret_cast<unsigned long long>(old_next);
				if (old_mark) old_value = old_value | CHECK_MARKING;
				else old_value = old_value & MASK_VALUE;
				unsigned long long new_value = reinterpret_cast<unsigned long long>(new_next);
				if (new_mark) new_value = new_value | CHECK_MARKING;
				else new_value = new_value & MASK_VALUE;
				return CAS(old_value, new_value);
			}
			bool TryMark(LFNODE* ptr)
			{
				unsigned long long old_value = reinterpret_cast<unsigned long long>(ptr) & MASK_VALUE;
				unsigned long long new_value = old_value | CHECK_MARKING;
				return CAS(old_value, new_value);
			}
			bool IsMarked() {
				return (0 != (next & CHECK_MARKING));
			}

			const T& operator ->() const { return value; }
			T& operator ->() { return value; }

			const T& operator*() const { return value; }
			T& operator*() { return value; }
		};

		template<class T>
		class LockFreeList {
		public:
			LockFreeList() {
				m_Head.SetNext(&m_Tail);
			}
			~LockFreeList() {	Clear();}

			void Initialize(int _currThreadNum, int _remainCount) {
				m_EBRController.Initialize(_currThreadNum, _remainCount);
			}

			void Clear() {
				while (m_Head.GetNext() != &m_Tail) {
					LFNODE<T>* temp = m_Head.GetNext();
					m_Head.next = temp->next;
				    
				}
			}
		
			bool Insert(T _value) {
				LFNODE<T>* newNode = Core::MemoryAlloc<LFNODE<T>>(_value);
				LFNODE<T>* last = &m_Tail;
				m_EBRController.start_op();
				LFNODE<T>* prev{ nullptr }, * curr{ nullptr };
				while (true) {
					Find(_value, prev, curr);
					if (last != prev) {
						m_EBRController.end_op();
						UPoolAllocator::Release(newNode);
				//		delete newNode;
						return false;
					}
					newNode->SetNext(curr);
					if (false == prev->CAS(curr, newNode, false, false)) {
						continue;
					}
					m_EBRController.end_op();
					return true;
				}
				return false;
			}
			bool Remove(T _value) {
				LFNODE<T> *prev{ nullptr }, * curr{ nullptr };
				LFNODE<T>* last = &m_Tail;
				m_EBRController.start_op();
				while (true) {
					Find(_value, prev, curr);
					if (last == curr) {
						m_EBRController.end_op();
						return false;
					}
					LFNODE<T>* succ = curr->GetNext();
					if (false == curr->TryMark(succ)) {
						continue;
					}
					bool isSucc = prev->CAS(curr, succ, false, false);
					if (true == isSucc) {
						m_EBRController.retire(curr);
					}
					m_EBRController.end_op();
					return true;
				}
				return false;
			}
			bool IsContains(T _value) {
				m_EBRController.start_op();
				LFNODE<T>* curr = &m_Head;
				LFNODE<T>* last = &m_Tail;
				while (curr->value == _value) {
					curr = curr->GetNext();
					if (last == curr)
					{
						m_EBRController.end_op();
						return false;
					}
				}
				int ret = (false == curr->IsMarked()) && (_value == curr->value);
				m_EBRController.end_op();
				return ret;
			}

			auto begin() const { return &m_Head; }
			auto end() const { return &m_Tail; }

			auto begin() { return &m_Head; }
			auto end()  { return &m_Tail; }

			_int Size() {
				_int num = 0;
				LFNODE<T>* curr = &m_Head;
				LFNODE<T>* last = &m_Tail;
				while (true) {
					curr = curr->GetNext();
					if (last == curr)
					{
						m_EBRController.end_op();
						return num;
					}
					++num;
				}
				return num;
			}
		private:
			void Find(T _value, LFNODE<T>*& _prev, LFNODE<T>*& _curr)
			{
			retry:
				LFNODE<T>* prev = &m_Head;
				LFNODE<T>* last = &m_Tail;
				LFNODE<T>* curr = prev->GetNext();
				while (last != curr)
				{
					bool isRemoved{ false };
					LFNODE<T>* succ = curr->GetNextWithMark(&isRemoved);
					while (true == isRemoved) {
						if (false == prev->CAS(curr, succ, false, false)) {
							goto retry;
						}
						m_EBRController.retire(curr);
						curr = succ;
						succ = curr->GetNextWithMark(&isRemoved);
					}
					if (curr->value == _value) {
						_prev = prev; _curr = curr;
						return;
					}
					prev = curr;
					curr = curr->GetNext();
				}
				_prev = prev; _curr = curr;
			}
		private:
			EBRController<LFNODE<T>>	m_EBRController;
			LFNODE<T>									m_Head;
			LFNODE<T>									m_Tail;
		};
	}

	template<class T>
	using CONSTACK = Core::PTHStack::LockFreeStack<T>;

	template<class T>
	using CONLIST = Core::PTHList::LockFreeList<T>;
}


#endif // _SERVERFRAMEWORK_CORE_PUBLIC_LOCKFREE_H