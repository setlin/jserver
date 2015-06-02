/*=============================================================================
#     FileName: prot.h
#         Desc: process protocol
#       Author: Setlin
#      Version: 0.0.1
#       Create: 10:41 2015/03/30
#   LastChange: 10:42 2015/03/30
#      History:
=============================================================================*/
#ifndef _PROT_H_
#define _PROT_H_

#include "protid.h"
#include "net.h"


// server 100 protdata
// special code
#define PROT_SERVER_100_PROTDATA "uGq0fjOdOAL3pZyojC1J"

// process prot data, and dispatch them
int prot_dispatch(struct net_pool* np, int id, int size, uint8_t* protdata);

// prot header
struct prot_header;

// set prot_header
int prot_set_header(uint8_t* protdata, int id, int protid, int charid, int seqid/*, int protlen*/);

// get prot_header
int prot_get_header(const uint8_t* data, struct prot_header* ph);
// get prot header connid
int prot_get_header_connid(struct net_pool* np, const uint8_t* data);

// serialize and unserialize prot data
// serialize integer value
// return value : insert buffer length
int prot_serialize_intvalue(uint8_t* buffer, long value, int bufferlen);
// unserialize integer value
int prot_unserialize_intvalue(uint8_t* buffer, long* value, int bufferlen);
// serialize string value
int prot_serialize_stringvalue(uint8_t* buffer, uint8_t* value, int bufferlen, int datalength);
// unserialize string value
int prot_unserialize_stringvalue(uint8_t* buffer, uint8_t* value, int bufferlen);

// API need to be realized
// SendProt(struct net_pool* np, int id, int tocharid, int protid, const uint8_t* protdata)
// {
//  step 1 : get header size, net_send_header
//  step 2 : malloc data
//  step 3 : set prot header to data
//  step 4 : set protdata to data + protheaderlength
//  step 5 : net_send
//  step 6 : free(data)
// }
// call net_send_header()
//      net_send()
// int net_send_header(struct net_pool* np, int id, int sz);
// send data to client
// net pool, unique id, data, datasize
// int net_send(struct net_pool* np, int id, const char* data, int sz);
//
// protlen = strlen(protdata);
// totallen = 20 + protlen

// SendProt API
int prot_sendprot(struct net_pool* np, int id, int seqid, int tocharid, int protid, uint8_t* protdata, int protdatalength);

// distinguish the server or client according to the unique and same code exist in gateway and server
int prot_distinguish_prot100(struct net_pool* np, int id, int seqid, int charid, uint8_t* data, int datalength);
// answer prot 100
int prot_answer_prot100(struct net_pool* np, int id, int res);

// record charid
int prot_savecharid(struct net_pool* np, int id, int seqid, int charid, uint8_t* data, int datalength);

#endif




























