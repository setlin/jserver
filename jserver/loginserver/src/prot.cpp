#include "prot.h"
#include "log.h"

// type of data
// now support two types of data : integer(positive or negative) and string
// integer number
static const char T1 = 0x01; // positive 8bits
static const char t1 = 0xF1; // negative 8bits
static const char T2 = 0x02; // positive 16bits
static const char t2 = 0xF2; // negative 16bits
static const char T3 = 0x03; // positive 32bits
static const char t3 = 0xF3; // negative 32bits
// TODO: to support 64bits data
static const char T4 = 0x04; // positive 64bits
static const char t4 = 0xF4; // negative 64bits


// prot header
struct prot_header
{
    // connect id
    int connid;
    // protocol id
    int protid;
    // user id / character id
    int charid;
    // sequence id
    int seqid;
    // protocol length
    // int protlen;
};

// process prot data, and dispatch them
int prot_dispatch(struct net_pool* np, int id, int size, uint8_t* protdata)
{
    log_debug("prot_dispatch id : %d, size : %d\n", id, size);
    // get prot header data
    struct prot_header pheader;

    /*
    uint8_t* data = (uint8_t*)malloc(size + 1);
    memset(data, 0, size + 1);

    int res = net_recv(np, id, data, size);
    if(res == -1)
    {
        log_debugEx("recv later ...");
        return -1;
    }
    */

    // attention: char value[-128 - 127]
    // uint8_t value[0 - 255] [0 - 0xFF]

    int pheaderlen = prot_get_header(protdata, &pheader);

    log_debug("get connid = %d, protid = %d, charid = %d\n", pheader.connid, pheader.protid, pheader.charid);

    PROTHANDLER prothandler = NET_GET_PROTHANDLER(np, pheader.protid);

    if(!prothandler)
    {
        log_error("do not find the handler of %d", pheader.protid);
        log_error("maybe the server does not connect yet");
        // free(data);
        return -1;
    }

    prothandler(np, id, pheader.seqid, pheader.charid, protdata + pheaderlen, size - pheaderlen);

    // free(data);

    return 0;
}


// illustrate : sequence id
// client will maintain a variable, as sequence, when client send a protocol twice
// server will know it is the same request, and server will save the newest result
// if the sequence id equal to the newest sequence id, the server will return the newest result

// set prot_header
int prot_set_header(uint8_t* protdata, int id, int protid, int charid, int seqid, int protlen)
{

    protdata[0] = (protlen >> 24) & 0xFF;
    protdata[1] = (protlen >> 16) & 0xFF;
    protdata[2] = (protlen >> 8) & 0xFF;
    protdata[3] = (protlen ) & 0xFF;

    protdata[4] = (id >> 24) & 0xFF;
    protdata[5] = (id >> 16) & 0xFF;
    protdata[6] = (id >> 8) & 0xFF;
    protdata[7] = (id ) & 0xFF;

    protdata[8] = (protid >> 24) & 0xFF;
    protdata[9] = (protid >> 16) & 0xFF;
    protdata[10] = (protid >> 8) & 0xFF;
    protdata[11] = (protid ) & 0xFF;

    protdata[12] = (charid >> 24) & 0xFF;
    protdata[13] = (charid >> 16) & 0xFF;
    protdata[14] = (charid >> 8) & 0xFF;
    protdata[15] = (charid ) & 0xFF;

    protdata[16] = (seqid >> 24) & 0xFF;
    protdata[17] = (seqid >> 16) & 0xFF;
    protdata[18] = (seqid >> 8) & 0xFF;
    protdata[19] = (seqid ) & 0xFF;

    return 20;
}

// get prot_header
int prot_get_header(const uint8_t* data, struct prot_header* ph)
{
    ph->connid = (data[0] << 24)
                | (data[1] << 16)
                | (data[2] << 8)
                | (data[3]);

    ph->protid = (data[4] << 24)
                | (data[5] << 16)
                | (data[6] << 8)
                | (data[7]);

    ph->charid = (data[8] << 24)
                | (data[9] << 16)
                | (data[10] << 8)
                | (data[11]);

    ph->seqid = (data[12] << 24)
                | (data[13] << 16)
                | (data[14] << 8)
                | (data[15]);

    return 16;
}

// get prot header connid
int prot_get_header_connid(const uint8_t* data)
{
    int connid = (data[0] << 24)
                | (data[1] << 16)
                | (data[2] << 8)
                | (data[3]);

    return connid;
}

// serialize step
// step 1 : malloc a buffer
// step 2 : put data into buffer
// step 3 : memcpy buffer to wbuffer
// step 4 : free buffer
//
// unserialize step
// step 1 : get enough buffer from rbuffer
// step 2 : get data from buffer


// serialize and unserialize prot data
// serialize integer value
int prot_serialize_intvalue(uint8_t* buffer, long value, int bufferlen)
{
    // step 1 : judge the value is positive or negative
    // step 2 : put the type
    // step 3 : put the value

    bool positive = true;

    if(value < 0)
    {
        positive = false;
        value = -value;
    }

    // judge the bits

    if((value & 0xFFFFFFFF00000000) > 0) // 64bits
    {
        char T = positive ? T4 : t4;
        buffer[0] = T;

        buffer[1] = (value >> 56) & 0xFF;
        buffer[2] = (value >> 48) & 0xFF;
        buffer[3] = (value >> 40) & 0xFF;
        buffer[4] = (value >> 32) & 0xFF;
        buffer[5] = (value >> 24) & 0xFF;
        buffer[6] = (value >> 16) & 0xFF;
        buffer[7] = (value >> 8) & 0xFF;
        buffer[8] = (value) & 0xFF;

        return 9;
    }
    else if((value & 0xFFFF0000) > 0) // 32bits
    {
        char T = positive ? T3 : t3;
        buffer[0] = T;

        buffer[1] = (value >> 24) & 0xFF;
        buffer[2] = (value >> 16) & 0xFF;
        buffer[3] = (value >> 8) & 0xFF;
        buffer[4] = (value ) & 0xFF;

        return 5;
    }
    else if((value & 0xFF00) > 0) // 16bits
    {
        char T = positive ? T2 : t2;
        buffer[0] = T;

        buffer[1] = (value >> 8) & 0xFF;
        buffer[2] = (value) & 0xFF;

        return 3;
    }
    else
    {
        char T = positive ? T1 : t1;
        buffer[0] = T;

        buffer[1] = (value) & 0xFF;

        return 2;
    }

    return 0;
}

// unserialize integer value
int prot_unserialize_intvalue(uint8_t* buffer, long* value, int bufferlen)
{
    if(!buffer)
    {
        log_error("buffer is empty ...");
        return 0;
    }

    char T = buffer[0];
    bool positive = (T & 0xF0) == 0;
    T = T & 0xF;

    int size = 0;

    switch(T)
    {
        case T4:
            size = 9;
            if(bufferlen < size)
            {
                return 0;
            }
            *value = ((int64_t)buffer[1] << 56)
                    | ((int64_t)buffer[2] << 48)
                    | ((int64_t)buffer[3] << 40)
                    | ((int64_t)buffer[4] << 32)
                    | ((int64_t)buffer[5] << 24)
                    | ((int64_t)buffer[6] << 16)
                    | ((int64_t)buffer[7] << 8)
                    | ((int64_t)buffer[8]);
            break;
        case T3:
            size = 5;
            if(bufferlen < size)
            {
                return 0;
            }
            *value = ((int64_t)buffer[1] << 24)
                    | ((int64_t)buffer[2] << 16)
                    | ((int64_t)buffer[3] << 8)
                    | ((int64_t)buffer[4]);
            break;
        case T2:
            size = 3;
            if(bufferlen < size)
            {
                return 0;
            }
            *value = ((int64_t)buffer[1] << 8)
                    | ((int64_t)buffer[2]);
            break;
        case T1:
            size = 2;
            if(bufferlen < size)
            {
                return 0;
            }
            *value = (int64_t)buffer[1];
            break;
        default :
            log_debugEx("without integer type");
            return 0;
            break;
    }

    *value = positive ? (*value) : (-(*value));

    return size;
}

// serialize string value
int prot_serialize_stringvalue(uint8_t* buffer, uint8_t* value, int bufferlen, int datalength)
{
    if(datalength > bufferlen)
    {
        return 0;
    }

    int size = prot_serialize_intvalue(buffer, datalength, bufferlen);
    memcpy(buffer + size, value, datalength);

    return datalength + size;
}

// unserialize string value
int prot_unserialize_stringvalue(uint8_t* buffer, uint8_t* value, int bufferlen)
{
    long datalength = 0;
    int size = prot_unserialize_intvalue(buffer, &datalength, bufferlen);

    if(size == 0)
    {
        return 0;
    }

    memcpy(value, buffer + size,  datalength);

    return datalength + size;
}

// SendProt API
// prot_sendprot(np, id, 1001, charid, PROTID_LOGIN_CHECK_G2C, protdata);
int prot_sendprot(struct net_pool* np, int id, int seqid, int tocharid, int protid, uint8_t* protdata, int protdatalength)
{
    log_debug("protdata : %s\n", protdata);
    // step 1 : malloc data
    uint8_t* data = (uint8_t*)malloc(DEFAULT_PROT_SIZE);
    memset(data, 0, DEFAULT_PROT_SIZE);

    // step 2 : set prot header to data
    // step 3 : get header size, net_send_header
    // int protdatalength = strlen((const char*)protdata);

    int protheaderlength = 16;
    int headersize = protheaderlength + protdatalength;

    prot_set_header(data, id, protid, tocharid, seqid, headersize);

    // step 4 : set protdata to data + protheaderlength
    memcpy(data + 20, protdata, protdatalength);

    // step 5 : net_send
    net_send(np, id, data, headersize + 4);

    // step 6 : free(data)
    free(data);

    return 0;
}

// distinguish the server or client according to the unique and same code exist in gateway and server
int prot_distinguish_prot100(struct net_pool* np, int id, int seqid, int charid, uint8_t* data, int datalength)
{
    log_debug("[prot_distinguish_prot100] recv prot100\n");
    uint8_t* code = (uint8_t*)malloc(DEFAULT_PROT_SIZE_SMALL);

    prot_unserialize_stringvalue(data, code, datalength);

    log_debug("code from client : %s\n", code);
    log_debug("server code : %s\n", PROT_SERVER_100_PROTDATA);

    if(strcmp((char*)code, PROT_SERVER_100_PROTDATA) == 0)
    {
        log_debug("SERVER CONNECTED\n");
        net_set_serverid(np, id);
        prot_answer_prot100(np, id, 0);
    }
    else
    {
        log_debug("CLIENT CONNECTED\n");
        int serverid = net_get_serverid(np);
        if(serverid == -1)
        {
            log_debug("server does not connected\n");
            prot_answer_prot100(np, id, 1);
        }
        else
        {
            log_debug("server has connected\n");
            prot_answer_prot100(np, id, 0);
        }
    }

    free(code);

    return 0;
}

// answer prot 100
int prot_answer_prot100(struct net_pool* np, int id, int res)
{
    uint8_t* protdata = (uint8_t*)malloc(DEFAULT_PROT_SIZE_SMALL);

    int protdatalength = prot_serialize_intvalue(protdata, (long)res, DEFAULT_PROT_SIZE_SMALL);

    prot_sendprot(np, id, 1000, 1, PROTID_DISTINGUISH_SERVER_G2C, protdata, protdatalength);

    free(protdata);

    return 0;
}

// record charid
int prot_savecharid(struct net_pool* np, int id, int seqid, int charid, uint8_t* data, int datalength)
{
    long charidvalue = 0;

    prot_unserialize_intvalue(data, &charidvalue, datalength);

    return 0;
}























