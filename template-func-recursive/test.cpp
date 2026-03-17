#include <iostream>
#include <string>

using namespace std;

static void get( int N )
{
    cout << N << endl;
}
/* // 函数不允许偏特化
template < int N > void get()
{
    cout << N << endl;
}
*/
template < int N, int... M > void get()
{
    // 递归调用模板函数
    get( N );
    // get< N >(); 函数不允许偏特化
    if constexpr ( sizeof...( M ) > 0 )
    {
        // cout << "template func recursive..." << endl;
        get< M... >();  // 模板函数不允许偏特化，只能在同一个函数实现里面进行 递归调用
    }
}
///////////////////////////////////  结构体 -[偏特化]- 实现 //////////////////////////////////////////
template < int N > struct _S_common_get
{
    static void get()
    {
        cout << N << endl;
    }
};
template < int... N > struct _S_get
{
    ;
};
template < int N, int... M > struct _S_get< N, M... > : _S_common_get< N >, _S_get< M... >
{
    using inherit = _S_get< M... >;
    using base    = _S_common_get< N >;
    static void get()
    {
        base::get();
        inherit::get();
    }
};

template < int N > struct _S_get< N > : public _S_common_get< N >
{
    using base = _S_common_get< N >;
    static void get()
    {
        base::get();
    }
};
/////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
    cout << "Template Function impl:" << endl;
    get< 1, 2, 3, 4, 5, 6, 7, 8, 9 >();
    cout << "Template Struct impl:" << endl;
    _S_get< 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 >::get();
    return 0;
}