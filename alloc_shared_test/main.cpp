#include <stdio.h>
#include <memory>

namespace alloc
{
	template<class _Ty>
	class allocator
	{	// generic allocator for objects of class _Ty
	public:
		static_assert(!std::is_const<_Ty>::value,
			"The C++ Standard forbids containers of const elements "
			"because allocator<const T> is ill-formed.");

		typedef void _Not_user_specialized;

		typedef _Ty value_type;

		typedef value_type *pointer;
		typedef const value_type *const_pointer;

		typedef value_type& reference;
		typedef const value_type& const_reference;

		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

		typedef std::true_type propagate_on_container_move_assignment;
		typedef std::true_type is_always_equal;

		template<class _Other>
		struct rebind
		{	// convert this type to allocator<_Other>
			typedef allocator<_Other> other;
		};

		pointer address(reference _Val) const _NOEXCEPT
		{	// return address of mutable _Val
			return (_STD addressof(_Val));
		}

		const_pointer address(const_reference _Val) const _NOEXCEPT
		{	// return address of nonmutable _Val
			return (_STD addressof(_Val));
		}

		allocator() _THROW0()
		{	// construct default allocator (do nothing)
		}

		allocator(const allocator<_Ty>&) _THROW0()
		{	// construct by copying (do nothing)
		}

		template<class _Other>
		allocator(const allocator<_Other>&) _THROW0()
		{	// construct from a related allocator (do nothing)
		}

		template<class _Other>
		allocator<_Ty>& operator=(const allocator<_Other>&)
		{	// assign from a related allocator (do nothing)
			return (*this);
		}

		void deallocate(pointer _Ptr, size_type _Count)
		{	// deallocate object at _Ptr
			//_Deallocate(_Ptr, _Count, sizeof(_Ty));
			printf("deallocate %I64d * %I64d\n", sizeof(_Ty), _Count);
			::free(_Ptr);
		}

		_DECLSPEC_ALLOCATOR pointer allocate(size_type _Count)
		{	// allocate array of _Count elements
			//return (static_cast<pointer>(_Allocate(_Count, sizeof(_Ty))));
			printf("allocate %I64d * %I64d\n", sizeof(_Ty), _Count);
			return static_cast<pointer>(::malloc(sizeof(_Ty) * _Count));
		}

		_DECLSPEC_ALLOCATOR pointer allocate(size_type _Count, const void *)
		{	// allocate array of _Count elements, ignore hint
			return (allocate(_Count));
		}

		template<class _Objty,
			class... _Types>
			void construct(_Objty *_Ptr, _Types&&... _Args)
		{	// construct _Objty(_Types...) at _Ptr
			::new ((void *)_Ptr) _Objty(_STD forward<_Types>(_Args)...);
		}


		template<class _Uty>
		void destroy(_Uty *_Ptr)
		{	// destroy object at _Ptr
			_Ptr->~_Uty();
		}

		size_t max_size() const _NOEXCEPT
		{	// estimate maximum array size
			return ((size_t)(-1) / sizeof(_Ty));
		}
	};

	// CLASS allocator<void>
	template<>
	class allocator<void>
	{	// generic allocator for type void
	public:
		typedef void _Not_user_specialized;

		typedef void value_type;

		typedef void *pointer;
		typedef const void *const_pointer;

		template<class _Other>
		struct rebind
		{	// convert this type to an allocator<_Other>
			typedef allocator<_Other> other;
		};

		allocator() _THROW0()
		{	// construct default allocator (do nothing)
		}

		allocator(const allocator<void>&) _THROW0()
		{	// construct by copying (do nothing)
		}

		template<class _Other>
		allocator(const allocator<_Other>&) _THROW0()
		{	// construct from related allocator (do nothing)
		}

		template<class _Other>
		allocator<void>& operator=(const allocator<_Other>&)
		{	// assign from a related allocator (do nothing)
			return (*this);
		}
	};
}

template <class T, class... Args>
T* UnsafeNew(Args&&... args)
{
	void* mem = ::malloc(sizeof(T));
	printf("unsafenew ptr: %I64d, size: %I64d\n", (int64_t)mem, sizeof(T));
	return new (mem)T(std::forward<Args>(args)...);
}

template <class T, class... Args>
std::shared_ptr<T> New1(Args&&... args)
{
	return std::shared_ptr<T>(UnsafeNew<T>(args...));
}

template <class T, class... Args>
std::shared_ptr<T> New2(Args&&... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

template <class T, class... Args>
std::shared_ptr<T> New3(Args&&... args)
{
	alloc::allocator<T> aloc;
	return std::allocate_shared<T>(aloc, std::forward<Args>(args)...);
}

void* operator new(std::size_t n)
{
	void* mem = ::malloc(n);
	printf("new ptr(%I64d), size = %I64d\n", (int64_t)mem, n);
	return mem;
}

void* operator new[](std::size_t n)
{
	void* mem = ::malloc(n);
	printf("new[] ptr(%I64d), size = %I64d\n", (int64_t)mem, n);
	return mem;
}

void operator delete(void* mem)
{
	printf("delete ptr(%I64d)\n", (int64_t)mem);
	::free(mem);
}

void operator delete[](void* mem)
{
	printf("delete[] ptr(%I64d)\n", (int64_t)mem);
	::free(mem);
}


class Dummy
{
public:
	int64_t a;
};

int main()
{
	printf("sizeof(Dummy) = %I64d\n", sizeof(Dummy));

	printf("\n");
	printf("new1: no allocator with shared_ptr(..)\n");
	{
		auto dd = New1<Dummy>();
		printf("ff\n");
	}

	printf("\n");
	printf("new2: no allocator with make_shared\n");
	{
		auto dd = New2<Dummy>();
		printf("ff\n");
	}

	printf("\n");
	printf("new3 allocator\n");
	{
		auto dd = New3<Dummy>();
		printf("ff\n");
	}
	printf("\n");
	printf("end\n");
}