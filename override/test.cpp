#include <iostream>

using namespace std;

/*
1. 隐藏是一次针对一组方法（包括重载方法）
2. 覆写是一次针对一个方法 （不包括重载的方法）
3. 父类隐藏的方法可以通过将子类转换成父类的引用（或指针）来调用
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

    a1.test();
    // a1.test(10); // 无法调用
    ( ( A0& )a1 ).test( 10 );  // 转换成父类引用来调用父类隐藏的方法

    b1.test();
    ( ( B0& )b1 ).test();
    // b1.test(10); // 无法调用
    ( ( B0& )b1 ).test( 20 );
}