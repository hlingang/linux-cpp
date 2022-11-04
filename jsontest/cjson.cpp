#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

class CJson
{
public:
    using Value     = nlohmann::json;
    using value_t   = nlohmann::detail::value_t;
    using Iterator  = nlohmann::json::iterator;
    using ValueType = nlohmann::json::value_t;
    using SizeType  = nlohmann::json::size_type;
    // kType define
    enum kType
    {
        kNull   = ( uint8_t )value_t::null,             ///< null value
        kObject = ( uint8_t )value_t::object,           ///< object (unordered set of name/value pairs)
        kArray  = ( uint8_t )value_t::array,            ///< array (ordered collection of values)
        kString = ( uint8_t )value_t::string,           ///< string value
        kBool   = ( uint8_t )value_t::boolean,          ///< boolean value
        kInt    = ( uint8_t )value_t::number_integer,   ///< number value (signed integer)
        kUint   = ( uint8_t )value_t::number_unsigned,  ///< number value (unsigned integer)
        kFloat  = ( uint8_t )value_t::number_float,     ///< number value (floating-point)
        kBinary = ( uint8_t )value_t::binary,           ///< binary array (ordered collection of bytes)
        kTypeCount,
    };
    CJson() = default;
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
    template < typename T > void add( const string& key, T value )
    {
        data_[ key ] = value;
    }
    template < typename T > T add( const char* key, T value )
    {
        data_[ key ] = value;
        return value;
    }
    CJson add( const char* key, CJson& value )
    {
        data_[ key ] = value.getData();
        return value;
    }
    Value add( const char* key, Value& value )
    {
        data_[ key ] = value;
        return value;
    }
    template < typename T > T append( T& value )
    {
        data_.push_back( value );
        return value;
    }
    template < typename T > T append( T&& value )
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
        return data_.at( key ).get< T >();
    }
    template < typename T > T get( const char* key )
    {
        return data_.at( key ).get< T >();
    }
    CJson parseFile( const char* fileName )
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
    Iterator begin()
    {
        return data_.begin();
    }
    Iterator end()
    {
        return data_.end();
    }
    size_t size() const
    {
        return data_.size();
    }

private:
    Value data_;
};

int main()
{
    CJson array_json;
    CJson object_json;
    char  name[ 20 ] = "abcdefgh";
    object_json.add( "a", 60 );
    object_json.add( name, name );
    cout << object_json.get< string >( name ) << endl;
    object_json.toFile( "test001.json" );
    CJson tmp_json;
    tmp_json.add( "123", 123 );
    object_json.add( "obj", tmp_json );
    array_json.append( object_json );
    array_json.append( "qwert" );
    array_json.append( 123 );
    array_json.toFile( "test002.json" );
}
