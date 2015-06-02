/*=============================================================================
#     FileName: jschat.cpp
#         Desc: chat model
#       Author: Setlin
#      Version: 0.0.1
#       Create: 10:43 2015/03/30
#   LastChange: 10:45 2015/03/30
#      History:
=============================================================================*/

#include "jschat.h"
#include "../../log.h"

#include "../char/jschar.h"

#define CHAT_MAX_CONTENT_SIZE 1024

// personal channel's data
struct chat_handler_channel_person_c2s_data
{
    int friendid;
    uint8_t content[CHAT_MAX_CONTENT_SIZE];
};

// personal channel's handler
int chat_handler_channel_person_c2s(struct net_pool* np, int id, int seqid, int charid, uint8_t* data, int datalength)
{
    log_debug("id : %d, charid : %d\n", id, charid);

    log_debug("prot data : %s\n", data);

    int curpos = 0;
    int elementsize = 0;

    chat_handler_channel_person_c2s_data cpdata;
    memset(&cpdata, 0, sizeof(cpdata));

    elementsize = prot_unserialize_intvalue(data + curpos, (long*)(&(cpdata.friendid)), datalength - curpos);
    curpos += elementsize;

    elementsize = prot_unserialize_stringvalue(data + curpos, cpdata.content, datalength - curpos);

    log_debug("friendid : %d\ncontent : %s\n", cpdata.friendid, cpdata.content);

    uint8_t* protdata = (uint8_t*)malloc(DEFAULT_PROT_SIZE);

    int protdatalength = 0;
    elementsize    = 0;

    unsigned long curtime = jsutil_getcurtime();

    uint8_t* charname = (uint8_t*)malloc(DEFAULT_PROT_SIZE_SMALL);
    memset(charname, 0, DEFAULT_PROT_SIZE_SMALL);
    char_getcharnamebycharid(charid, charname);

    // [1] friend name
    elementsize = prot_serialize_stringvalue(protdata + protdatalength, charname, DEFAULT_PROT_SIZE - protdatalength, strlen((const char*)charname));
    protdatalength += elementsize;

    // [2] chat time : unix time
    elementsize = prot_serialize_intvalue(protdata + protdatalength, curtime, DEFAULT_PROT_SIZE - protdatalength);
    protdatalength += elementsize;

    // [3] content
    elementsize = prot_serialize_stringvalue(protdata + protdatalength, cpdata.content, DEFAULT_PROT_SIZE - protdatalength, strlen((const char*)cpdata.content));
    protdatalength += elementsize;

    // send to friend
    // TODO : friend is online
    prot_sendprot(np, id, seqid, cpdata.friendid, PROTID_CHAT_CHANNEL_PERSONAL_S2C, protdata, protdatalength);

    free(protdata);
    free(charname);
    return 0;
}
















