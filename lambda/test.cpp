#include <iostream>

using namespace std;

class A
{
public:
    A( int& _a, int* _c ) : a( _a ), c( _c ), b( 90 ) {}

    void test() const
    {
        a  = 100;
        *c = 101;
        // b  = 99;
        // const 成员函数不允许直接修改成员变量的值，但是允许通过引用和指针间接修改
    }

private:
    int& a;
    int  b = 200;
    int* c;
};

int main()
{
    int a = 1;
    int b = 22;
    int c = 33;

    auto f = [ = ]() mutable -> void {
        a = a + 1;
        b = 23;
        c -= 1;
        // 隐式捕获所有的局部变量(值拷贝方式), 为所有拷贝值构建数据成员, 默认是const 成员函数
    };
    auto p = [ & ]() {
        a = a + 1;
        b = 23;
        c -= 1;
        // 隐式捕获所有的局部变量(引用捕获方式), 无需构建成员变量,  默认是const 成员函数
    };
    f();
    cout << "a=" << a << ", b=" << b << ", c=" << c << endl;
    p();
    cout << "a=" << a << ", b=" << b << ", c=" << c << endl;
}