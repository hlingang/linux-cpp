#include <stdio.h>

// 数组的初始化器 //
// c++ 20 开始支持 //

int main()
{
    int a[] = {
        /* 未指定的索引自动初始化为 0，即 a[0]=0 */
        [1]       = 2,
        [2 ... 3] = 10,  // 冒号中间有空格分隔 //
        [10]      = 1,
    };
    size_t n = sizeof( a ) / sizeof( a[ 0 ] );
    printf( "Array size = %zu\n", n );
    for ( int i = 0; i < n; i++ )
    {
        printf( "a[%d] = %d\n", i, a[ i ] );
    }
}