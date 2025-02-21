#include <chrono>
#include <type_traits>
#include <iostream>

using namespace std;

struct my_demo
{
  void demo()
  {
    printf("i am demo\n");
  }
};

struct my_demo_no
{
  void demo_no()
  {
    printf("i am demo no\n");
  }
};

template <typename... T>
using my_void_t = void;

template <typename Tp, typename Ts = void>
struct demo_check_t : false_type
{
};

// 偏特化至少 需要两个模板参数 //
template <typename Tp> //< 编译器认为[类型Tp]为未知类型
struct demo_check_t<Tp, my_void_t<decltype(declval<Tp>().demo())>> : true_type
{
};
/*
// this code can not work(compile error)
template <typename Tp = void>
struct demo_check_t : false_type
{
};

// 偏特化至少 需要两个模板参数 //
template <typename Tp> //< 编译器认为[类型Tp]为未知类型
struct demo_check_t<my_void_t<decltype(declval<Tp>().demo())>> : true_type
{
};
*/

template <typename T>
bool demo_check(T &__t)
{
  return demo_check_t<T>::value;
}

int main()
{
  my_demo __my_demo;
  my_demo_no __my_demo_no;
  cout << "__my_demo    : " << demo_check(__my_demo) << endl;
  cout << "__my_demo_no : " << demo_check(__my_demo_no) << endl;
  return 0;
}