#include "init.h"
#include "./model/chat/jschat.h"
#include "./model/login/jslogin.h"
#include "./model/char/jschar.h"
#include "./model/friend/jsfriend.h"


// initialize protocol handler
int init_prothandler(struct net_pool* np)
{
    log_debug("init_prothandler begin\n");
    // register protocol handler
    log_debug("protid : %d\n", PROTID_CHAT_CHANNEL_PERSONAL_C2S);
    NET_REG_PROTHANDLER(np, PROTID_CHAT_CHANNEL_PERSONAL_C2S, chat_handler_channel_person_c2s);

    log_debug("protid : %d\n", PROTID_DISTINGUISH_SERVER_C2G);
    NET_REG_PROTHANDLER(np, PROTID_DISTINGUISH_SERVER_C2G, login_handler_answer_prot100_c2g);

    log_debug("protid : %d\n", PROTID_DISTINGUISH_SERVER_G2C);
    NET_REG_PROTHANDLER(np, PROTID_DISTINGUISH_SERVER_G2C, login_handler_answer_prot100_g2c);

    log_debug("protid : %d\n", PROTID_CHAR_GETCHARINFO_C2S);
    NET_REG_PROTHANDLER(np, PROTID_CHAR_GETCHARINFO_C2S, char_handler_getcharinfo_c2s);

    log_debug("protid : %d\n", PROTID_FRIEND_ADDFRIEND_C2S);
    NET_REG_PROTHANDLER(np, PROTID_FRIEND_ADDFRIEND_C2S, friend_handler_addfriend);
    
    log_debug("protid : %d\n", PROTID_FRIEND_GETFRIENDLIST_C2S);
    NET_REG_PROTHANDLER(np, PROTID_FRIEND_GETFRIENDLIST_C2S, friend_handler_loadfriend);


    log_debug("init_prothandler finish\n");
}



