#include "stl_algo.h"
#include <iostream>
#include <algorithm>

using namespace std;

int main()
{
    vector< int > px   = { 1, 2, 3, 4, 5, 6 };
    auto          __it = __find_n_aux( px.begin(), px.end(), 3, 5 );
    // remove( px.begin(), px.end(), 5 );
    __shuffle( px.begin(), px.end() );
    __for_each( px.begin(), px.end(), []( int val ) { cout << "val = " << val << endl; } );
    cout << distance( px.begin(), __it ) << endl;
    int arr[] = { 1, 2, 3, 4 };
    cout << arr[ 0 ] << endl;
    cout << arr[ 1 ] << endl;
    vector< int > qx1 = { 1, 2, 3, 4 };
    vector< int > qx2 = { 1, 2, 3, 4, 5 };
    cout << "equal = " << equal( qx1.begin(), qx1.end(), qx2.begin() ) << endl;
    cout << "equal = " << equal( qx1.begin(), qx1.end(), qx2.begin(), qx2.end() ) << endl;
    vector< int > ax = { 20, 1, 2, 10, 10, 5, 1, 5, 5, 5, 5, 30 };
    auto          it = __unique( ax.begin(), ax.end() );
    cout << "distance = " << distance( ax.begin(), it ) << endl;
    cout << "value = " << *it << endl;
    __for_each( ax.begin(), ax.end(), []( int val ) { cout << "val = " << val << endl; } );
    ax = { 1, 2 };
    ax.reserve( 10 );
    __for_each( ax.begin(), ax.end(), []( int val ) { cout << "val = " << val << endl; } );
}