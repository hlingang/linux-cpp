
#include "invoker.h"
int main()
{
    // ActorAgent< CmdActor< CMD_1, 0 > >::type().init();
    // ActorAgent< CmdActor< CMD_2 > >::type().init();
    // ActorAgent< CmdActor< CMD_2, 0, 0 > >::type().init();
    // ActorAgent< CmdActor< CMD_2, 0, 1 > >::type().init();
    // ActorAgent< CmdActor< CMD_2, 0, 2 > >::type().init();
    // printf( "-----------------------------\n" );
    // get_cmd_actor< CmdActor< CMD_1, 0 > >().init();
    // get_cmd_actor< CmdActor< CMD_2, 0 > >().init();
    // get_cmd_actor< CmdActor< CMD_2, 0, 0 > >().init();
    // get_cmd_actor< CmdActor< CMD_2, 0, 1 > >().init();
    // get_cmd_actor< CmdActor< CMD_2, 0, 2 > >().init();

    CmdActor< CMD_1, 0 >().init();
    CmdActor< CMD_2 >().init();
    CmdActor< CMD_2, 0, 0 >().init();
    CmdActor< CMD_2, 0, 1 >().init();
    CmdActor< CMD_2, 0, 2 >().init();
}
