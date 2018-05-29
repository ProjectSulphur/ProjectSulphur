#define ENUM_FLAG_OPERATOR(T,X) inline T operator X (T lhs, T rhs) { return (T) (static_cast<std::underlying_type_t <T>>(lhs) X static_cast<std::underlying_type_t <T>>(rhs)); } 
#define ENUM_FLAGS(T) \
enum struct T : unsigned int; \
inline T operator ~ (T t) { return (T) (~static_cast<std::underlying_type_t <T>>(t)); } \
inline bool operator == (T lhs, unsigned int rhs) { return static_cast<std::underlying_type_t <T>>(lhs) == static_cast<std::underlying_type_t <T>>(rhs); } \
inline bool operator != (T lhs, unsigned int rhs) { return static_cast<std::underlying_type_t <T>>(lhs) != static_cast<std::underlying_type_t <T>>(rhs); } \
ENUM_FLAG_OPERATOR(T,|) \
ENUM_FLAG_OPERATOR(T,^) \
ENUM_FLAG_OPERATOR(T,&) \
enum struct T : unsigned int