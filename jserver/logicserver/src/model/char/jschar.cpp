/*=============================================================================
#     FileName: jschar.h
#         Desc: character
#       Author: Setlin
#      Version: 0.0.1
#       Create: 21:48 2015/05/10
#   LastChange: 21:48 2015/05/10
#      History:
=============================================================================*/

#include "jschar.h"
#include "../../log.h"


// getcharinfo
int char_handler_getcharinfo_c2s(struct net_pool* np, int id, int seqid, int charid, uint8_t* data, int datalength)
{
    log_debug("char_handler_getcharinfo_c2s\n");
    int res =0;

    JSCHARINFO jscharinfo;
    memset(&jscharinfo, 0, sizeof(JSCHARINFO));

    jscharinfo.charid = charid;
    JSCharDBInfoGet(&jscharinfo);

    uint8_t* protdata = (uint8_t*)malloc(DEFAULT_PROT_SIZE);
    memset(protdata, 0 , DEFAULT_PROT_SIZE);

    int protdatalength = 0;
    int elementsize    = 0;

    // [1] charid
    elementsize = prot_serialize_intvalue(protdata + protdatalength, jscharinfo.charid, DEFAULT_PROT_SIZE - protdatalength);
    protdatalength += elementsize;


    // [2] charname
    elementsize = prot_serialize_stringvalue(protdata + protdatalength, jscharinfo.charname, DEFAULT_PROT_SIZE_SMALL - protdatalength, strlen((const char*)jscharinfo.charname));
    protdatalength += elementsize;

    log_debug("XXXXXXXXXXXXXXXXxx charname : %s\n", jscharinfo.charname);

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

    // send to client
    prot_sendprot(np, id, seqid, charid, PROTID_CHAT_CHANNEL_PERSONAL_S2C, protdata, protdatalength);

    free(protdata);
    return res;
}


// get charname by charid
int char_getcharnamebycharid(unsigned long charid, uint8_t* charname)
{
    int res = 0;

    JSCHARINFO jscharinfo;
    memset(&jscharinfo, 0, sizeof(JSCHARINFO));

    jscharinfo.charid = charid;
    JSCharDBInfoGet(&jscharinfo);

    memcpy(charname, jscharinfo.charname, strlen((const char*)jscharinfo.charname));

    return res;
}


// get charinfo by charid
int char_getcharinfobycharid(PJSCHARINFO pjscharinfo)
{
    int res = 0;
    PJSCHARINFO temppjscharinfo = (PJSCHARINFO)malloc(sizeof(JSCHARINFO));
    memset(temppjscharinfo, 0 ,sizeof(JSCHARINFO));

    temppjscharinfo->charid = pjscharinfo->charid;

    res = JSCharDBInfoGet(temppjscharinfo);

    if(res == 0)
    {
        memcpy(pjscharinfo, temppjscharinfo, sizeof(JSCHARINFO));
    }

    free(temppjscharinfo);
    return res;
}






















