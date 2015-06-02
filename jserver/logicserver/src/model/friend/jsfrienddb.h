/*=============================================================================
#     FileName: jsfrienddb.h
#         Desc: friend database
#       Author: Setlin
#      Version: 0.0.1
#       Create: 10:10 2015/03/26
#   LastChange: 10:10 2015/03/26
#      History:
=============================================================================*/


#include "../jsmemstd.h"

#include "../../net.h"


#define FRIEND_CONF_RELATIONSHIP_ISFRIEND 0


typedef struct _JSFRIENDINFO
{
    unsigned long id;
    unsigned long friendid;
    unsigned long isdelete;
    unsigned long addtime;
}JSFRIENDINFO, *PJSFRIENDINFO;


typedef list<JSFRIENDINFO> FRIENDLIST, *PFRIENDLIST;

int JSFriendInfoGet(unsigned long, PFRIENDLIST);

// add friend
int JSFriendAddFriend(unsigned long charid, unsigned long friendid, struct net_pool* np);


// is friendship
int JSFriendIsFriendship(unsigned long charid, unsigned long friendid, int friendship);


