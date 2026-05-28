
#include <A.h>
#include <B.h>
using namespace std;
/*
1. 前置声明，仅声明类名称，用于函数形参引用，或者指针类型字段声明
2. 声明类结构，但不包含具体实现
3. 声明类结构，并包含具体实现
*/

int main()
{
    A __a{ 99 };
    B __b;
    __b.test( __a );
    return 0;
}
