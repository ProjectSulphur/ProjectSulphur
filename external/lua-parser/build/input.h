#define LUA_CLASS(x)
#define LUA_FUNC(x)
#define LUA_ENUM(x)
#define LUA_NAME(x)

LUA_CLASS() class MyClass
{
public:

  LUA_NAME(MijnKlasse)

  LUA_FUNC() void Test(sulphur::foundation::Vector<T> bleh, float& x, float* y);

  LUA_ENUM() enum TestEnum
  {
    kFirstValue,
    kSecondValue
  };

  float some_other_test;
};