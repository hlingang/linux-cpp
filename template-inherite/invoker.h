#ifndef __INVOKER_H__
#define __INVOKER_H__
#include <iostream>
using namespace std;

enum e_Cmd
{
    CMD_1 = 0,
    CMD_2 = 1,
};
template < typename, typename = void > struct __bindActor;
template < typename cmdtype > struct ActorAgent
{
    using type = typename __bindActor< cmdtype >::type;
};
template < typename... Args > struct _my_void
{
    using _type = void;
};

struct base_type
{
    virtual int init()
    {
        printf( "base_type::init()\n" );
        return 0;
    };
};
template < typename cmdtype, typename = void > struct __rebind
{
    using type = base_type;
};

template < int ncmd, int nsub, int nargs, template < int, int, int > class cmdtype >
struct __rebind< cmdtype< ncmd, nsub, nargs >,
                 typename _my_void< decltype( declval< cmdtype< ncmd, nsub, -1 > >().init() ) >::_type >
{
    using type = cmdtype< ncmd, nsub, -1 >;
};

template < typename cmdtype, typename > struct __bindActor
{
    using type = typename __rebind< cmdtype >::type;
};

template < int ncmd, int nsub, int nargs, template < int, int, int > class cmdtype >
struct __bindActor< cmdtype< ncmd, nsub, nargs >,
                    typename _my_void< decltype( declval< cmdtype< ncmd, nsub, nargs > >().init() ) >::_type >
{
    using type = cmdtype< ncmd, nsub, nargs >;
};
/*
1. 通用模板和(偏)特化模板是平等的，互斥的，但是两者的优先级不一样，特化模板的优先级高于通用模板
2. 通用模板可继承特化模板(支持递归继承)，用于自动初始化非特化的模板。
*/
template < int Cmd, int SubCmd = 0, int Args = -1 > struct CmdActor : public CmdActor< Cmd, SubCmd, -1 >
{
};

template < int Cmd, int SubCmd > struct CmdActor< Cmd, SubCmd, -1 > : public base_type
{
};

template <> struct CmdActor< CMD_1 > : public base_type
{
    int init()
    {
        cout << "CmdActor< CMD_1>::init()" << endl;
        return 0;
    }
};

template <> struct CmdActor< CMD_2 > : public base_type
{
    int init()
    {
        cout << "CmdActor< CMD_2>::init()" << endl;
        return 0;
    }
};

template <> struct CmdActor< CMD_2, 0, 0 > : CmdActor< CMD_2 >
{
    int init()
    {
        cout << "CmdActor< CMD_2, 0, 0 >::init()" << endl;
        return 0;
    }
};

template < typename cmdtype > typename ActorAgent< cmdtype >::type get_cmd_actor()
{
    return typename ActorAgent< cmdtype >::type();
}

#endif