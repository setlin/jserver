/*=============================================================================
#     FileName: jslogin.cpp
#         Desc: login logic
#       Author: Setlin
#      Version: 0.0.1
#       Create: 15:29 2015/04/07
#   LastChange: 15:29 2015/04/07
#      History:
=============================================================================*/

#include "jslogin.h"
#include "../../log.h"

static int login_answer_prot100_res(struct net_pool* np, int id, int res)
{
    uint8_t* protdata = (uint8_t*)malloc(DEFAULT_PROT_SIZE_SMALL);

    int protdatalength = prot_serialize_intvalue(protdata, (long)res, DEFAULT_PROT_SIZE_SMALL);

    prot_sendprot(np, id, 1000, 1, PROTID_DISTINGUISH_SERVER_G2C, protdata, protdatalength);

    free(protdata);
}

// check the prot 100 from client
int login_handler_answer_prot100_c2g(struct net_pool* np, int id, int seqid, int charid, uint8_t* data, int datalength)
{
    uint8_t* code = (uint8_t*)malloc(DEFAULT_PROT_SIZE_SMALL);
    prot_unserialize_stringvalue(data, code, datalength);

    if(strcmp((char*)code, PROT_100_CLIENT_PROTDATA) == 0)
    {
        log_debug("Client Login\n");
        login_answer_prot100_res(np, id, 0);
    }
    else
    {
        log_debug("Unknow client\n");
        login_answer_prot100_res(np, id, 1);
    }

    return 0;
}

// check the prot 100 from client
int login_handler_answer_prot100_g2c(struct net_pool* np, int id, int seqid, int charid, uint8_t* data, int datalength)
{
    int res = 0;

    long prot100res = 0;
    // FIXME
    /*
    log_debug("use data:\n");
    for(int i = 0; i <  datalength; ++i)
    {
        printf("%d\t", data[i]);
    }
    printf("\n");
    */

    // before 
    // prot_unserialize_intvalue(data, &res, datalength);
    // the res is no long enouh to save the unserialize value

    prot_unserialize_intvalue(data, &prot100res, datalength);
    log_debug("res = %d\n", res);

    if(prot100res == 0)
    {
        log_debug("connect to gateway successful\n");
    }
    else
    {
        log_debug("connect to gateway failure\n");
    }

    return 0;
}






























