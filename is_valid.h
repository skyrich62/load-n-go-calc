#ifndef IS_VALID_H_INCLUDED
#define IS_VALID_H_INCLUDED

#include <variant>

template< typename T, typename U> struct is_valid;

template< typename T, typename... Ts>
struct is_valid<T, std::variant< Ts... > > :
std::bool_constant< (std::is_same_v< T, Ts > || ...) >
{ };


#endif // IS_VALID_H_INCLUDED
