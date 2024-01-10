#include <iostream>
#include <unistd.h>

using namespace std;

struct node
{
    int   value;
    node* next;
    node( int i ) : value( i ), next( NULL ) {}
};

void printNode( node* root )
{
    while ( root )
    {
        cout << "val: " << root->value << endl;
        root = root->next;
    }
}

void remove_if( node** head, int n )
{
    for ( node** curr = head; *curr; )
    {
        node* entry = *curr;
        if ( entry->value == n )
        {
            *curr = entry->next;
            free( entry );
        }
        else
        {
            curr = &entry->next;
        }
    }
}
void remove_if( node* head, int n )
{
    node* cur = head;
    if ( cur && cur->value == n )
    {
        head = cur->next;
        return;
    }
    while ( cur->next )
    {
        node* entry = cur->next;
        if ( entry->value == n )
        {
            cur->next = entry->next;
        }
        else
        {
            cur = cur->next;
        }
    }
}

int main()
{
    node* p1  = ( node* )malloc( sizeof( node ) );
    node* p2  = ( node* )malloc( sizeof( node ) );
    node* p3  = ( node* )malloc( sizeof( node ) );
    p1->value = 1;
    p1->next  = p2;
    p2->value = 2;
    p2->next  = p3;
    p3->value = 3;
    p3->next  = NULL;
    printNode( p1 );
    // remove_if( &p1, 2 );
    remove_if( p1, 3 );
    printNode( p1 );

    return 0;
}