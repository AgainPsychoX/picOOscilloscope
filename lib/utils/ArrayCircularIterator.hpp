#include <array>

/// Helper iterator class to iterate over the array in a circular way.
/// Handles wrapping around (both forward and backward).
template <typename T, std::size_t size>
struct ArrayCircularIterator
{
	T* begin;
	T* pointer;

	inline ArrayCircularIterator(std::array<T, size>& array, std::size_t offset = 0) noexcept
		: begin(array.begin()), pointer(begin + normalizeOffset(offset))
	{}
	
	inline ArrayCircularIterator(T* begin, T* pointer) noexcept
		: begin(begin), pointer(pointer)
	{
		// assert(begin <= pointer);
	}
	inline ArrayCircularIterator(T* begin, std::size_t offset = 0) noexcept
		: ArrayCircularIterator(begin, begin + normalizeOffset(offset))
	{}

	inline /***/ T& get() /***/ { return *pointer; }
	inline const T& get() const { return *pointer; }

	inline /***/ T& operator*() /***/ { return get(); }
	inline const T& operator*() const { return get(); }

	ArrayCircularIterator operator+(std::size_t offset) const
	{
		while (offset >= size) [[unlikely]]
			offset -= size;
		if (offset >= samplesToEnd()) [[unlikely]]
			return ArrayCircularIterator(begin, pointer + offset - size);
		else
			return ArrayCircularIterator(begin, pointer + offset);
	}
	ArrayCircularIterator operator-(std::size_t offset) const
	{
		while (offset >= size) [[unlikely]]
			offset -= size;
		if (offset >= samplesFromStart()) [[unlikely]]
			return ArrayCircularIterator(begin, pointer - offset + size);
		else
			return ArrayCircularIterator(begin, pointer - offset);
	}

	inline std::size_t samplesFromStart() const { return pointer - begin; }
	inline std::size_t samplesToEnd()     const { return begin + size - pointer; }

	inline ArrayCircularIterator& operator+=(std::size_t offset) { return *this = *this + offset; }
	inline ArrayCircularIterator& operator-=(std::size_t offset) { return *this = *this - offset; }

	inline ArrayCircularIterator& operator++()
	{
		pointer++;
		if (pointer >= begin + size) [[unlikely]]
			pointer = begin;
		return *this;
	}
	inline ArrayCircularIterator& operator--()
	{
		if (pointer == begin) [[unlikely]]
			pointer = begin + size - 1;
		else
			pointer--;
		return *this;
	}

	inline ArrayCircularIterator operator++(int)
	{
		auto tmp = *this;
		++*this;
		return tmp;
	}

	inline ArrayCircularIterator operator--(int)
	{
		auto tmp = *this;
		--*this;
		return tmp;
	}

private:
	/// Normalize offset to the range [0, size). Used in the constructors.
	static inline std::size_t normalizeOffset(std::size_t offset)
	{
		while (offset >= size) [[unlikely]]
			offset -= size;
		return offset;
	}
};
