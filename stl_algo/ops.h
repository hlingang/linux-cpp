#ifndef __OPS_H_
#define __OPS_H_

template < typename Pred > struct Iter_Pred
{
    Pred __M_pred;
    Iter_Pred( Pred __pred ) : __M_pred( __pred ) {}
    template < typename Iter > bool operator()( Iter __it )
    {
        return __M_pred( *__it );
    }
};

template < typename Pred > Iter_Pred< Pred > __iter_pred( Pred __pred )
{
    return Iter_Pred< Pred >( __pred );
}

template < typename Value > struct Iter_Val
{
    const Value& __M_value;
    Iter_Val( const Value& __value ) : __M_value( __value ) {}
    template < typename Iter > bool operator()( Iter __it )
    {
        return ( __M_value == *__it );
    }
};

template < typename Value > Iter_Val< Value > __iter_val( const Value& __value )
{
    return Iter_Val< Value >( __value );
}
#endif