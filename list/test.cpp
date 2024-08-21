#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <forward_list>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <pthread.h>
#include <regex>
#include <set>
#include <shared_mutex>
#include <sstream>
#include <stdio.h>
#include <string>
#include <thread>
#include <type_traits>
#include <unistd.h>
#include <utility>
#include <vector>
#include <array>
#include <atomic>
#include <cstddef>
#include <cassert>
#include <cstdio>
#include <node.h>
#include <tree.h>

#include "nlist.hpp"
#include "fwlist.hpp"

using namespace std;

class TX
{
public:
    TX( int __x )
    {
        _M_ptr = new int( __x );
        cout << "create TX(" << __x << ")" << endl;
    }
    ~TX()
    {
        cout << "*~TX(" << *_M_ptr << ")" << endl;
    }

private:
    int* _M_ptr;
};

void test_abc()
{
    cout << "abc" << endl;
}

void test_123()
{
    cout << "123" << endl;
}

int main( int argc, char* argv[] )
{

    struct node* n0 = create_node( 2 );
    struct node* n1 = create_node( 1 );
    struct node* n2 = create_node( 0 );

    n0->next = n1;
    n1->next = n2;
    // remove_node(0, &n0);
    insert_node( 8, &n0 );
    insert_node( 6, &n0 );
    insert_node( 12, &n0 );
    insert_node( 4, &n0 );
    insert_node( 5, &n0 );
    insert_node( 6, &n0 );
    print_node( &n0 );

    node* N0 = nullptr;
    node* M0 = nullptr;
    node* H0 = nullptr;
    copy_node( &N0, &n0, ( void* )node_greater );
    copy_node( &M0, &n0, ( void* )node_less );
    copy_node( &H0, &n0, nullptr );
    printf( "-------------node greater--------------\n" );
    printf( "node size=%d\n", node_size( &N0 ) );
    print_node( &N0 );
    printf( "---------------node less--------------\n" );
    printf( "node size=%d\n", node_size( &M0 ) );
    print_node( &M0 );
    printf( "---------------node null--------------\n" );
    printf( "node size=%d\n", node_size( &H0 ) );
    print_node( &H0 );
    // reverse_node(&n0);
    // print_node(&n0);
    tree* t0 = nullptr;
    insert_tree( &t0, "aaaa", 8 );
    insert_tree( &t0, "aaa", 10 );
    insert_tree( &t0, "aa", 5 );
    insert_tree( &t0, "bbb", 2 );
    insert_tree( &t0, "cccc", 100 );
    insert_tree( &t0, "bbbb", 102 );
    insert_tree( &t0, "dddd", 98 );

    printf( "----------t0----------\n" );
    printf( "t0 tree size = %d\n", tree_size( &t0 ) );
    print_tree( &t0 );

    remove_tree( &t0, "aaa" );
    printf( "----------t0 remove(aaa)----------\n" );
    printf( "t0 tree size After remove(aaa) = %d\n", tree_size( &t0 ) );
    print_tree( &t0 );

    tree* T0 = nullptr;
    copy_tree( &T0, &t0 );
    printf( "----------T0----------\n" );
    printf( "T0 tree size = %d\n", tree_size( &T0 ) );
    print_tree( &T0 );
    free_tree( &T0 );
    printf( "----------T0 After Free----------\n" );
    printf( "T0 tree size(After Free) = %d\n", tree_size( &T0 ) );
    print_tree( &T0 );
    const char* nlist[] = { "aaaa", "aaa", "aa", "bbb", "cccc", "bbbb", "dddd", nullptr };

    const char** __name = nlist;
    for ( ;; )
    {
        if ( !*__name )
        {
            break;
        }
        tree* __tree = find_tree( &t0, *__name );
        if ( __tree )
        {
            printf( "name=%s, val=%d\n", *__name, __tree->val );
        }
        ++__name;
    }

    List< int > iList( 2, 16 );
    iList.assign( 3, 15 );
    iList.emplace( 1 );
    iList.emplace( 10 );
    iList.emplace( 20 );
    iList.erase( iList.begin() );
    iList.insert( iList.begin(), iList.begin(), iList.end() );

    for ( auto& val : iList )
    {
        cout << "iList val = " << val << endl;
    }
    cout << "-----------------------------------" << endl;
    iList.reverse();
    for ( auto& val : iList )
    {
        cout << "iList val = " << val << endl;
    }
    iList.clear();

    FWList< int > fwList;
    fwList.assign( 10, 3 );
    for ( auto& val : fwList )
    {
        cout << "fwList val = " << val << endl;
    }
    cout << "---------------------------------" << endl;
    fwList.clear();
    fwList.emplace( 1024 );
    fwList.insert_after( fwList.begin_before(), 24 );
    fwList.insert_after( fwList.begin_before(), 32 );

    fwList.insert_after( fwList.begin_before(), 32 );

    fwList.insert_after( fwList.begin_before(), fwList.begin(), fwList.end() );
    fwList.erase_after( fwList.begin_before() );
    fwList.erase_after( fwList.begin() );

    for ( auto& val : fwList )
    {
        cout << "fwList val = " << val << endl;
    }
    map< string, int > __map{ { "a", 10 }, { "b", 89 } };
    return 0;
}
