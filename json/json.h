#ifndef __JSON_H_
#define __JSON_H_

#include <bitset>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
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
    CZString() : type( e_null ) {}
    CZString( int index ) : CZString()
    {
        if ( index >= 0 )
        {
            type   = e_int;
            index_ = index;
        }
    }
    CZString( const char* s ) : CZString()
    {
        if ( strlen( s ) > 0 )
        {
            type  = e_string;
            cstr_ = s;
        }
    }
    CZString( const std::string& s ) : cstr_( s ){};
    bool operator<( const CZString& rth ) const
    {
        switch ( type )
        {
        case e_int:
            return index_ < rth.index_;
        case e_string:
            return cstr_ < rth.cstr_;
        default:
            return false;
        }
    }
    std::string toString() const
    {
        if ( type == e_int )
        {
            return std::to_string( index_ );
        }
        else
        {
            return "\"" + cstr_ + "\"";
        }
    }

private:
    value_type  type;
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
    typedef std::map< CZString, Json >::iterator Iterator;
    Json()
    {
        memset( &_data, 0x00, sizeof( _data ) );
        _data.type = e_null;
        _obj       = new std::map< CZString, Json >;
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
    template < size_t N > Json( const char s[ N ] ) : Json()
    {
        _data.type = e_string;
        if ( N > 0 )
        {
            char* p = ( char* )malloc( N );
            memcpy( p, s, N );
            _data.val.s._len = N - 1;
            _data.val.s._str = p;
        }
    }
    Json& operator=( int i )
    {
        _data.type  = e_int;
        _data.val.i = i;
        return *this;
    }
    Json& operator[]( const CZString& sz )
    {
        setType( e_object );
        return ( *_obj )[ sz ];
    }
    Json& operator[]( const int index )
    {
        setType( e_array );
        CZString sz( index );
        return ( *_obj )[ sz ];
    }
    Json& operator[]( const char* s )
    {
        setType( e_object );
        CZString sz( s );
        return ( *_obj )[ sz ];
    }
    Json( const Json& rth )
    {
        _data.type = rth._data.type;
        switch ( _data.type )
        {
        case e_int:
            _data.val.i = rth._data.val.i;
            break;
        case e_double:
            _data.val.d = rth._data.val.d;
            break;
        case e_string:
            if ( rth._data.val.s._len > 0 )
            {
                size_t n = rth._data.val.s._len;
                char*  p = ( char* )malloc( n + 1 );
                memcpy( p, rth._data.val.s._str, n );
                p[ n ]           = '\0';
                _data.val.s._str = p;
                _data.val.s._len = n;
            }
            break;
        case e_array:
        case e_object:
            _obj = new std::map< CZString, Json >( *rth._obj );
            break;
        }
    }
    const Json& operator=( const Json& rth )
    {
        _data.type = rth._data.type;
        switch ( _data.type )
        {
        case e_int:

            _data.val.i = rth._data.val.i;
            break;
        case e_double:
            _data.val.d = rth._data.val.d;
            break;
        case e_string:
            if ( _data.val.s._str && _data.val.s._len > 0 )
            {
                free( _data.val.s._str );
                _data.val.s._len = 0;
            }
            if ( rth._data.val.s._len > 0 )
            {
                size_t n = rth._data.val.s._len;
                char*  p = ( char* )malloc( n + 1 );
                memcpy( p, rth._data.val.s._str, n );
                p[ n ]           = '\0';
                _data.val.s._str = p;
                _data.val.s._len = n;
            }
            break;
        case e_array:
        case e_object:
            if ( _obj )
            {
                delete _obj;
            }
            _obj = new std::map< CZString, Json >( *rth._obj );
            break;
        }
        return *this;
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
    size_t size() const
    {
        return _obj->size();
    }
    Json& append( const Json& j )
    {
        if ( _data.type == e_null )
        {
            setType( e_array );
        }
        ( *_obj )[ size() ] = j;
        return _obj->at( size() - 1 );
    }
    std::string toString() const
    {
        std::ostringstream ss;
        bool               start = false;
        switch ( _data.type )
        {
        case e_null:
            ss << "null";
            break;
        case e_int:
            ss << _data.val.i;
            break;
        case e_double:
            ss << _data.val.d;
            break;
        case e_string:
            ss << "\"" << _data.val.s._str << "\"";
            break;
        case e_array:
            start = false;
            ss << "[";
            for ( auto it = _obj->begin(); it != _obj->end(); it++ )
            {
                if ( start )
                {
                    ss << ", " << it->second.toString();
                }
                else
                {
                    ss << it->second.toString();
                    start = true;
                }
            }
            ss << "]";
            break;
        case e_object:
            start = false;
            ss << "{";
            for ( auto it = _obj->begin(); it != _obj->end(); it++ )
            {
                if ( start )
                {
                    ss << ", " << it->first.toString() << ":" << it->second.toString();
                }
                else
                {
                    ss << it->first.toString() << ":" << it->second.toString();
                    start = true;
                }
            }
            ss << "}";
            break;
        }
        return ss.str();
    }
    int getDataType() const
    {
        return _data.type;
    }
    Iterator begin()
    {
        return _obj->begin();
    }
    Iterator end()
    {
        return _obj->end();
    }

private:
    void setType( int type_ )
    {
        _data.type = type_;
    }

private:
    Data                        _data;
    std::map< CZString, Json >* _obj;
};

inline std::ostream& operator<<( std::ostream& out, const json::Json& j )
{
    out << j.toString();
    return out;
}
};  // namespace json

#endif