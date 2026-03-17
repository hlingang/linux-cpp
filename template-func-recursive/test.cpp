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
        cout << "template func recursive..." << endl;
        get< M... >();
    }
}

int main()
{
    get< 1, 2, 3, 4, 5, 6, 7, 8, 9 >();
    return 0;
}