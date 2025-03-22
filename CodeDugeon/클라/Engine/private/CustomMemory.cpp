#include "EngineDefine.h"
#include "CustomMemory.h"
#include "UGameInstance.h"

namespace Engine {

	UMemoryPool::UMemoryPool(long long _AllocSize) :	m_AllocSize{ _AllocSize }
	{
		::InitializeSListHead(&m_Header);
	}

	UMemoryPool::~UMemoryPool()
	{
		while (UMemoryHeader* memory = static_cast<UMemoryHeader*>(::InterlockedPopEntrySList(&m_Header)))
			::_aligned_free(memory);
	}

	void UMemoryPool::Push(UMemoryHeader* ptr)
	{
		ptr->SetAllocSize(0);

		::InterlockedPushEntrySList(&m_Header, static_cast<PSLIST_ENTRY>(ptr));
	}

	UMemoryHeader* UMemoryPool::Pop()
	{
		UMemoryHeader* memory = static_cast<UMemoryHeader*>(::InterlockedPopEntrySList(&m_Header));

		// 없으면 새로 만들다
		if (memory == nullptr)
		{
			memory = reinterpret_cast<UMemoryHeader*>(::_aligned_malloc(m_AllocSize, SLIST_ALIGNMENT));
		}
		else
		{
			assert(memory->GetAllocSize() == 0);
		}
		return memory;
	}

	/*
	===========================================================
	UMemoryPool
	===========================================================
	UMemoryAdminster
	===========================================================
	*/

	UMemoryAdminstor::UMemoryAdminstor()
	{
		unsigned int TableIndex = 0;
		unsigned int MemoryIndex = 0;
		unsigned int SIZE = 16;
		MakeMemoryPool(SIZE, MemoryIndex, TableIndex, 512, 16);
		MakeMemoryPool(SIZE, MemoryIndex, TableIndex, 1024 + 512, 32);
		MakeMemoryPool(SIZE, MemoryIndex, TableIndex, 1024 + 1024 + 512, 128);
		MakeMemoryPool(SIZE, MemoryIndex, TableIndex, MAX_ALLOC_SIZE + 1, 256);
	}

	UMemoryAdminstor::~UMemoryAdminstor()
	{
		for (auto& iter : m_MemoryTable)
		{
			delete iter;
		}
	}

	void* UMemoryAdminstor::Allocate(unsigned long long _Size)
	{
		UMemoryHeader* Header = nullptr;
		unsigned long long AllocateSize = _Size + sizeof(UMemoryHeader);
		// 메모리 최대 크기를 꺼내오면 일반 할당 
		if (AllocateSize > MAX_ALLOC_SIZE)
		{
			Header = reinterpret_cast<UMemoryHeader*>(::malloc(AllocateSize));
		}
		else
		{
			// 메모리 풀에서 꺼내온다. 
			Header = m_PoolTable[AllocateSize]->Pop();
		}
		::ZeroMemory(Header, AllocateSize);
		return UMemoryHeader::AttachHeader(Header, AllocateSize);
	}

	void UMemoryAdminstor::Release(void* _Ptr)
	{
		UMemoryHeader* Header = UMemoryHeader::DetachHeader(_Ptr);

		const 	unsigned long long AllocSize = Header->GetAllocSize();
		if (AllocSize > MAX_ALLOC_SIZE)
		{
			::free(Header);
		}
		else
		{
			::memset(Header, 0, AllocSize);
			m_PoolTable[AllocSize]->Push(Header);
		}
	}

	void UMemoryAdminstor::MakeMemoryPool(unsigned int&  _Size, unsigned int& _MemoryIndex ,
		unsigned int& _TableIndex, const unsigned int  _Limited, const 	unsigned int  _AddValue)
	{
		for (; _Size < _Limited;)
		{
			UMemoryPool* pool = new UMemoryPool(_Size);
			m_MemoryTable[_MemoryIndex++] = pool;
			while (_TableIndex <= _Size)
			{
				m_PoolTable[_TableIndex++] = pool;
			}
			_Size += _AddValue;
		}
	}
	/*
	===========================================================
	UMemoryAdminster
	===========================================================
	MemoryGlobal
	===========================================================
	*/
	class MemoryGlobal {
	public:
		MemoryGlobal() : m_pMemoryAdiminster{ nullptr } {
			m_pMemoryAdiminster = new UMemoryAdminstor;
		}
		~MemoryGlobal() {
			delete m_pMemoryAdiminster;
		}
	public:
		UMemoryAdminstor* GetMemoryAdiminster() const { return m_pMemoryAdiminster; }

	private:
		UMemoryAdminstor* m_pMemoryAdiminster;
	};
	static MemoryGlobal g_MemoryGrobal;
	/*
	===========================================================
	MemoryGlobal
	===========================================================
	UBaseAllocator
	===========================================================
	*/
	void* UBaseAllocator::Alloc(size_t size)
	{
		return ::malloc(size);
	}

	void UBaseAllocator::Release(void* ptr)
	{
		::free(ptr);
	}
	/*
	===========================================================
	UBaseAllocator
	===========================================================
	UPoolAllocator
	===========================================================
	*/

	void* UPoolAllocator::Alloc(size_t _size)
	{
		return g_MemoryGrobal.GetMemoryAdiminster()->Allocate(_size);
	}

	void UPoolAllocator::Release(void* _ptr)
	{
		g_MemoryGrobal.GetMemoryAdiminster()->Release(_ptr);
	}

	void UPoolAllocator::Release(const void* _ptr)
	{
		g_MemoryGrobal.GetMemoryAdiminster()->Release(const_cast<void*>(_ptr));
	}
}