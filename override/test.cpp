#include <iostream>

using namespace std;

/*
1. 隐藏是一次针对一组方法（包括重载方法）
2. 覆写是一次针对一个方法 （不包括重载的方法）
3. 父类隐藏的方法可以通过将子类转换成父类的引用（或指针）来调用
4. 模板函数不能为虚函数（虚函数需要在编译器确定虚函数表）
5. 子类会继承父类的所有虚函数表（指针）
*/

struct A0
{
    int  a;
    void test()
    {
        cout << "A0-test" << endl;
    }
    void test( int a )
    {
        cout << "A0-test-" << a << endl;
    }
    template < typename T > void test( T t )
    {
        cout << "template A0 - " << t << endl;
    }
};

struct A1 : public A0
{
    void test()
    {
        cout << "A1-test" << endl;
    }
};

struct B0
{
    int          a;
    virtual void test()
    {
        cout << "B0-test" << endl;
    }
    void test( int a )
    {
        cout << "B0-test-" << a << endl;
    }
};

struct B1 : public B0
{
    void test()
    {
        cout << "B1-test" << endl;
    }
};

void test( A0& a )
{
    a.test( 10 );
}

void test( B0& b )
{
    b.test( 10 );
}

int main()
{
    A0 a0;
    A1 a1;
    B0 b0;
    B1 b1;

    a1.test();  // 静态绑定
    // a1.test(10); // 无法调用
    ( ( A0& )a1 ).test( 10 );  // 静态绑定  // 转换成父类引用来调用父类隐藏的方法

    b1.test();             // 静态绑定
    ( ( B0& )b1 ).test();  // 动态绑定
    // b1.test(10); // 无法调用
    ( ( B0& )b1 ).test( 20 );  // 静态绑定
    cout << "sizeof(A0) = " << sizeof( A0 ) << endl;
    cout << "sizeof(A1) = " << sizeof( A1 ) << endl;

    cout << "sizeof(B0) = " << sizeof( B0 ) << endl;
    cout << "sizeof(B1) = " << sizeof( B1 ) << endl;
}