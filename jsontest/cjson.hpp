#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <type_traits>
using namespace std;
/**
 * \brief bool struct
 */
template < bool cond > struct BoolType
{
    static const bool value = cond;
};
/**
 * \brief And expression struct
 *
 * \tparam cond1 bool condition
 * \tparam cond2 bool condition
 */
template < bool cond1, bool cond2 > struct AndExpr : BoolType< false >
{
};
/**
 * \brief And expression struct
 *
 * \tparam null
 */
template <> struct AndExpr< true, true > : BoolType< true >
{
};
/**
 * \brief CJson class
 */
class CJson
{
public:
    using Value     = nlohmann::json;             ///< json value
    using Iterator  = nlohmann::json::iterator;   ///< iterator
    using ValueType = nlohmann::json::value_t;    ///< json value type
    using SizeType  = nlohmann::json::size_type;  ///< json size type
    CJson()         = default;
    CJson( const Value& value )
    {
        data_ = value;
    }
    CJson( const CJson& other )
    {
        data_ = other.data_;
    }
    CJson& operator=( const CJson& other )
    {
        data_ = other.data_;
        return *this;
    }
    template < typename T >
    typename std::enable_if< AndExpr< !std::is_same< T, Value >::value, !std::is_same< T, CJson >::value >::value,
                             void >::type
    add( const string& key, T value )
    {
        data_[ key ] = value;
    }
    template < typename T >
    typename std::enable_if< AndExpr< !std::is_same< T, Value >::value, !std::is_same< T, CJson >::value >::value,
                             T >::type
    add( const char* key, T value )
    {
        data_[ key ] = value;
        return value;
    }
    CJson add( const char* key, CJson& value )
    {
        data_[ key ] = value.data_;
        return value;
    }
    Value add( const char* key, Value& value )
    {
        data_[ key ] = value;
        return value;
    }
    template < typename T >
    typename std::enable_if< AndExpr< !std::is_same< T, Value >::value, !std::is_same< T, CJson >::value >::value,
                             T >::type
    append( T& value )
    {
        data_.push_back( value );
        return value;
    }
    template < typename T >
    typename std::enable_if< AndExpr< !std::is_same< T, Value >::value, !std::is_same< T, CJson >::value >::value,
                             T >::type
    append( T&& value )
    {
        data_.push_back( value );
        return value;
    }
    CJson append( CJson& value )
    {
        data_.push_back( value.getData() );
        return value;
    }
    Value append( Value& value )
    {
        data_.push_back( value );
        return value;
    }
    template < typename T > T get( const string& key )
    {
        return data_[ key ].get< T >();
    }
    template < typename T > T get( const char* key )
    {
        return data_[ key ].get< T >();
    }
    CJson fromFile( const char* fileName )
    {
        ifstream infile( fileName );
        Value    data;
        infile >> data;
        infile.close();
        return CJson( data );
    }
    void toFile( const char* fileName )
    {
        ofstream outfile( fileName );
        outfile << data_;
        outfile.close();
    }
    string toString()
    {
        stringstream outs;
        outs << data_;
        return outs.str();
    }
    bool isArray()
    {
        return data_.is_array();
    }
    bool isObject()
    {
        return data_.is_object();
    }
    void clear()
    {
        data_.clear();
    }
    ValueType getType()
    {
        return data_.type();
    }
    const Value& getData() const
    {
        return data_;
    }
    size_t size() const
    {
        return data_.size();
    }
    bool hasMember( const string& key )
    {
        return data_.contains( key );
    }
    bool empty()
    {
        return data_.empty();
    }
    SizeType remove( const string& key )
    {
        return data_.erase( key );
    }
    Iterator begin()
    {
        return data_.begin();
    }
    Iterator end()
    {
        return data_.end();
    }

private:
    Value data_;
};