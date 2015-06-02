/*=============================================================================
#     FileName: login.cpp
#         Desc: login model
#       Author: Setlin
#      Version: 0.0.1
#       Create: 21:04 2015/04/16
#   LastChange: 21:04 2015/04/16
#      History:
=============================================================================*/


#include "login.h"

static int answer_login_check(struct net_pool* np, int id, int charid, int res)
{
    uint8_t* protdata = (uint8_t*)malloc(DEFAULT_PROT_SIZE_SMALL);
    memset(protdata, 0, DEFAULT_PROT_SIZE_SMALL);
    int protdatalength = prot_serialize_intvalue(protdata, (long)res, DEFAULT_PROT_SIZE_SMALL);

    prot_sendprot(np, id, 1001, charid, PROTID_LOGIN_CHECK_G2C, protdata, protdatalength);

    free(protdata);
    return 0;
}


// login check
int login_check(struct net_pool* np, int id, int seqid, int fromcharid, uint8_t* data, int datalength)
{
    long charid = 0;
    uint8_t* passwd = (uint8_t*)malloc(DEFAULT_PROT_SIZE_SMALL);
    memset(passwd, 0, DEFAULT_PROT_SIZE_SMALL);

    int curpos = 0;
    int elementsize = 0;

    log_debug("datalength : %d\n", datalength);

    elementsize = prot_unserialize_intvalue(data + curpos, &charid, datalength - curpos);
    curpos += elementsize;

    log_debug("charid : %ld\n", charid);

    elementsize = prot_unserialize_stringvalue(data + curpos, passwd, datalength - curpos);

    log_debug("passwd : %s ----- \n", passwd);

    JSACCOUNTINFO jsaccountinfo;
    jsaccountinfo.charid = charid;

    int res = JSAccountInfoGet(&jsaccountinfo);

    if(res == -1)
    {
        log_debug("charid error\n");
        answer_login_check(np, id, fromcharid, 1);
    }
    else
    {
        log_debug("jsaccountinfo.passwd : %s ---\n", jsaccountinfo.passwd);

        if(strcmp((const char*)jsaccountinfo.passwd, (const char*)passwd) == 0)
        {
            log_debug("Login Successful\n");
            answer_login_check(np, id, fromcharid, 0);
        }
        else
        {
            log_debug("Login Failed\n");
            answer_login_check(np, id, fromcharid, 1);
        }
    }


    free(passwd);
    return 0;
}
























