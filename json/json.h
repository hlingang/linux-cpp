#ifndef __JSON_H_
#define __JSON_H_

#include <cstddef>
#include <cstdlib>
#include <map>
#include <string>
#include <cstring>
namespace json
{

enum value_type
{
    e_int,
    e_double,
    e_string,
    e_array,
    e_object,
    e_null,
};

class CZString
{
public:
    CZString( int index ) : index_( index ) {}
    CZString( const char* s ) : cstr_( s ){};
    CZString( const std::string& s ) : cstr_( s ){};

private:
    int         index_;
    std::string cstr_;
};
struct Data
{
    int type;
    union
    {
        int    i;
        double d;
        struct
        {
            int   _len;
            char* _str;
        } s;
    } val;
};
class Json
{
public:
    Json()
    {
        _data.type = e_null;
        _obj       = new std::map< CZString, Json* >;
    }
    Json( int i ) : Json()
    {
        _data.type  = e_int;
        _data.val.i = i;
    }
    Json( double d ) : Json()
    {
        _data.type  = e_double;
        _data.val.d = d;
    }
    Json( const char* s ) : Json()
    {
        _data.type = e_string;
        int n      = strlen( s );
        if ( n > 0 )
        {
            char* p = ( char* )malloc( n + 1 );
            memcpy( p, s, n );
            p[ n ]           = '\0';
            _data.val.s._len = n;
            _data.val.s._str = p;
        }
    }
    Json( const std::string& s ) : Json()
    {
        _data.type = e_string;
        int n      = s.size();
        if ( n > 0 )
        {
            char* p = ( char* )malloc( n + 1 );
            memcpy( p, s.c_str(), n );
            p[ n ]           = '\0';
            _data.val.s._len = n;
            _data.val.s._str = p;
        }
    }
    Json& operator[]( const CZString& sz )
    {
        return *( ( *_obj )[ sz ] );
    }
    Json& operator[]( const int index )
    {
        CZString sz( index );
        return *( ( *_obj )[ sz ] );
    }
    Json& operator[]( const char* s )
    {
        CZString sz( s );
        return *( ( *_obj )[ sz ] );
    }
    ~Json()
    {
        if ( _data.type == e_string )
        {
            if ( _data.val.s._len > 0 )
            {
                free( _data.val.s._str );
                _data.val.s._len = 0;
            }
            else if ( _data.type == e_array || _data.type == e_object )
            {
                _obj->clear();
            }
            if ( _obj )
            {
                delete _obj;
                _obj = nullptr;
            }
        }
    }

private:
    Data                         _data;
    std::map< CZString, Json* >* _obj;
};

};  // namespace json

#endif