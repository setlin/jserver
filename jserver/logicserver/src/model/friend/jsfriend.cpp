/*=============================================================================
#     FileName: jsfriend.cpp
#         Desc: friend system
#       Author: Setlin
#      Version: 0.0.1
#       Create: 17:20 2015/05/10
#   LastChange: 17:20 2015/05/10
#      History:
=============================================================================*/

#include "jsfriend.h"
#include "../char/jschar.h"

#include "../../log.h"

#include "jsfrienddb.h"

// search friend
int friend_handler_searchfriend(struct net_pool* np, int id, int seqid, int charid, uint8_t* data, int datalength)
{
    int res = 0;
    
    return res;
}

static int friend_answer_addfriend(struct net_pool* np, int id, int seqid, int charid, int addres)
{
    int res = 0;

    uint8_t* protdata = (uint8_t*)malloc(DEFAULT_PROT_SIZE_SMALL);
    memset(protdata, 0 , DEFAULT_PROT_SIZE_SMALL);

    int protdatalength = 0;
    int elementsize    = 0;

    // [1] charid
    elementsize = prot_serialize_intvalue(protdata + protdatalength, (long)addres, DEFAULT_PROT_SIZE_SMALL - protdatalength);
    protdatalength += elementsize;

    // send to client
    prot_sendprot(np, id, seqid, charid, PROTID_FRIEND_ADDFRIEND_S2C, protdata, protdatalength);

    free(protdata);
    return res;
}


static int isfriendship(unsigned long charid, unsigned long friendid, int friendship)
{
    log_debug("isfriendship\n");
    int res = 1;

    res = JSFriendIsFriendship(charid, friendid, friendship);

    log_debug("res : %d\n", res);

    return res;
}


// add friend
int friend_handler_addfriend(struct net_pool* np, int id, int seqid, int charid, uint8_t* data, int datalength)
{
    log_debug("friend_handler_addfriend\n");
    int res = 0;

    long friendid = 0;
    prot_unserialize_intvalue(data, &friendid, datalength);

    log_debug("friendid : %ld\n", friendid);

    // get charinfo by charid
    JSCHARINFO jscharinfo;
    memset(&jscharinfo, 0, sizeof(JSCHARINFO));
    jscharinfo.charid = friendid;

    res = char_getcharinfobycharid(&jscharinfo);

    int addfriendres = 0;

    if(res == 0)
    {
        if(isfriendship(charid, friendid, FRIEND_CONF_RELATIONSHIP_ISFRIEND) == 0)
        {
            log_debug("XX they were friends alerady!\n");
            addfriendres = 1;   
        }
        else
        {
            // add friend
            JSFriendAddFriend(charid, friendid, np);
            addfriendres = 0;
        }
    }
    else
    {
        addfriendres = 1;   
    }

    friend_answer_addfriend(np, id, seqid, charid, addfriendres);
    return res;
}

// delete friend
int friend_handler_deletefriend(struct net_pool* np, int id, int seqid, int charid, uint8_t* data, int datalength)
{
    int res = 0;

    return res;
}

// load friend
int friend_handler_loadfriend(struct net_pool* np, int id, int seqid, int charid, uint8_t* data, int datalength)
{
    log_debug("friend_handler_loadfriend\n");
    int res = 0;
    
    uint8_t* protdata = (uint8_t*)malloc(DEFAULT_PROT_SIZE);
    memset(protdata, 0 , DEFAULT_PROT_SIZE);

    int protdatalength = 0;
    int elementsize    = 0;

    FRIENDLIST friendlist;
    JSFriendInfoGet(charid, &friendlist);
 
    FRIENDLIST::iterator itfriendlist;

    for(itfriendlist = friendlist.begin(); itfriendlist != friendlist.end(); itfriendlist++)
    {
        log_debug("itfriendlist->friendid : %d\n", itfriendlist->friendid);
        // get charinfo by charid
        
        JSCHARINFO jscharinfo;
        memset(&jscharinfo, 0, sizeof(JSCHARINFO));
        jscharinfo.charid = itfriendlist->friendid;
        char_getcharinfobycharid(&jscharinfo);



        // [1] charid
        elementsize = prot_serialize_intvalue(protdata + protdatalength, jscharinfo.charid, DEFAULT_PROT_SIZE - protdatalength);
        protdatalength += elementsize;

        // [2] charname
        elementsize = prot_serialize_stringvalue(protdata + protdatalength, jscharinfo.charname, DEFAULT_PROT_SIZE_SMALL - protdatalength, strlen((const char*)jscharinfo.charname));
        protdatalength += elementsize;

        // [3] gender
        jscharinfo.gender = 1;
        elementsize = prot_serialize_intvalue(protdata + protdatalength, jscharinfo.gender, DEFAULT_PROT_SIZE - protdatalength);
        protdatalength += elementsize;

        // [4] age
        elementsize = prot_serialize_intvalue(protdata + protdatalength, jscharinfo.age, DEFAULT_PROT_SIZE - protdatalength);
        protdatalength += elementsize;

        // [5] locality
        elementsize = prot_serialize_stringvalue(protdata + protdatalength, jscharinfo.locality, DEFAULT_PROT_SIZE_SMALL - protdatalength, strlen((const char*)jscharinfo.locality));
        protdatalength += elementsize;

        // [6] signature
        elementsize = prot_serialize_stringvalue(protdata + protdatalength, jscharinfo.signature, DEFAULT_PROT_SIZE_SMALL - protdatalength, strlen((const char*)jscharinfo.signature));
        protdatalength += elementsize;

    }

    // send to client
    prot_sendprot(np, id, seqid, charid, PROTID_FRIEND_GETFRIENDLIST_S2C, protdata, protdatalength);

    free(protdata);
    return res;
}

























