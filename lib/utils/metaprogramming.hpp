#include <type_traits>

namespace mp {

////////////////////////////////////////
// type list filtering from https://codereview.stackexchange.com/a/115774

template <typename... Ts> 
struct list
{
	constexpr static auto length = sizeof...(Ts);
};

template <typename, typename> struct list_append_impl;

template <typename... Ts, typename... Us>
struct list_append_impl<list<Ts...>, list<Us...>> {
	using type = list<Ts..., Us...>;
};

template <template <typename> class, typename...>
struct filter_impl;

template <template <typename> class Predicate>
struct filter_impl<Predicate> {
	using type = list<>;
};

template <template <typename> class Predicate, typename T, typename... Rest>
struct filter_impl<Predicate, T, Rest...> {
	using type = typename list_append_impl<
		std::conditional_t<
			Predicate<T>::value,
			list<T>,
			list<>
		>,
		typename filter_impl<Predicate, Rest...>::type
	>::type;
};

template <template <typename> class Predicate, typename... Ts>
using filter = typename filter_impl<Predicate, Ts...>::type;

} // namespace mp