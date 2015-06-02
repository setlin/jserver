/*=============================================================================
#     FileName: jsfrienddb.cpp
#         Desc: friend database
#       Author: Setlin
#      Version: 0.0.1
#       Create: 20:32 2015/05/10
#   LastChange: 20:32 2015/05/10
#      History:
=============================================================================*/


#include "../char/jschar.h"
#include "jsfrienddb.h"

#include "../../jsutil.h"

map<unsigned long, PFRIENDLIST> mapjsfriendinfo;


static int JSFriendInfoLoadData(PFRIENDLIST pfriendlist, char* szsql)
{
    log_debug("JSFriendInfoLoadData\n");
    int res = 0;

    MYSQL_RES* RES;
    RES = JSDBQuery(szsql);
    if(!RES)
    {
        log_debug("Without RES\n");
        return -1;
    }


    // 得到结果集列数
    int fieldnum = mysql_num_fields(RES);

    // 得到结果集列数据
    MYSQL_FIELD* fields = mysql_fetch_fields(RES);

    // 结果集行
    MYSQL_ROW row;

    // 循环处理行数据
    int rowno = 0;
    while((row = mysql_fetch_row(RES)))
    {
        // 处理每一行的每一列
        int i = 0;
        rowno++;
        // 将结果保存至内存中

        JSFRIENDINFO tempjsfriendinfo;
        memset(&tempjsfriendinfo, 0 ,sizeof(JSFRIENDINFO));
        tempjsfriendinfo.id       = atoi(row[0]);
        tempjsfriendinfo.friendid = atoi(row[1]);
        tempjsfriendinfo.isdelete = atoi(row[2]);
        tempjsfriendinfo.addtime  = atoi(row[3]);

        log_debug("XXXXXXXXXXXXXXXXX friendid : %ld\n", tempjsfriendinfo.friendid);

        pfriendlist->push_back(tempjsfriendinfo);
    }

    // 释放结果集内存
    mysql_free_result(RES);

    log_debug("rowno : %d\n", rowno);

    if(rowno == 0)
    {
        return -1;
    }

    return res;
}



int JSFriendInfoGet(unsigned long charid, PFRIENDLIST pfriendlist)
{
    log_debug("JSFriendInfoGet\n");

    int res = 0;

    map<unsigned long, PFRIENDLIST>::iterator it = mapjsfriendinfo.find(charid);

    if(it != mapjsfriendinfo.end())
    {
        log_debug("some data....\n");

        PFRIENDLIST temppfriendlist;
        temppfriendlist = it->second;

        FRIENDLIST::iterator itfriendlist;

        for(itfriendlist = temppfriendlist->begin(); itfriendlist != temppfriendlist->end(); ++itfriendlist)
        {
            log_debug("XXXXXXXXXXXXXXX itfriendlist->friendid : %ld\n", itfriendlist->friendid);
            pfriendlist->push_back(*itfriendlist);
        }
    }
    else
    {
        PFRIENDLIST temppfriendlist = new FRIENDLIST;

        mapjsfriendinfo.insert(pair<unsigned long, PFRIENDLIST>(charid, temppfriendlist));

        char szsqlformat[JSDB_MAXSIZE_SQL];
        memset(szsqlformat, '\0', sizeof(szsqlformat));

        char szsql[JSDB_MAXSIZE_SQL];
        memset(szsql, '\0', sizeof(szsql));

        sprintf(szsqlformat, "%s", "SELECT id,friendid,isdelete,addtime from jsfriendinfo where charid = %ld");
        sprintf(szsql, szsqlformat, charid);

        res = JSFriendInfoLoadData(temppfriendlist, szsql);

        if(res != -1)
        {
            JSFriendInfoGet(charid, pfriendlist);
        }
        else
        {
            log_debug("no data\n");
        }
    }

    return res;

}


static int friendmemdb_addfriend(unsigned long charid, unsigned long friendid, unsigned long curtime)
{
    int res = 0;

    JSFRIENDINFO tempjsfriendinfo;

    // charid 
    memset(&tempjsfriendinfo, 0 ,sizeof(JSFRIENDINFO));
    // TODO: save the auto_increament id in memory
    // FIXME: auto_increament id
    tempjsfriendinfo.id       = 1;
    tempjsfriendinfo.friendid = friendid;
    tempjsfriendinfo.isdelete = FRIEND_CONF_RELATIONSHIP_ISFRIEND;
    tempjsfriendinfo.addtime  = curtime;

    map<unsigned long, PFRIENDLIST>::iterator it = mapjsfriendinfo.find(charid);

    if(it != mapjsfriendinfo.end())
    {
        PFRIENDLIST temppfriendlist;
        temppfriendlist = it->second;

        temppfriendlist->push_back(tempjsfriendinfo);
    }
    else
    {
        PFRIENDLIST temppfriendlist = new FRIENDLIST;
        temppfriendlist->push_back(tempjsfriendinfo);

        mapjsfriendinfo.insert(pair<unsigned long, PFRIENDLIST>(charid, temppfriendlist));
    }

    // friendid
    memset(&tempjsfriendinfo, 0 ,sizeof(JSFRIENDINFO));
    // TODO: save the auto_increament id in memory
    // FIXME: auto_increament id
    tempjsfriendinfo.id       = 1;
    tempjsfriendinfo.friendid = charid;
    tempjsfriendinfo.isdelete = FRIEND_CONF_RELATIONSHIP_ISFRIEND;
    tempjsfriendinfo.addtime  = curtime;

    it = mapjsfriendinfo.find(friendid);

    if(it != mapjsfriendinfo.end())
    {
        PFRIENDLIST temppfriendlist;
        temppfriendlist = it->second;

        temppfriendlist->push_back(tempjsfriendinfo);
    }
    else
    {
        PFRIENDLIST temppfriendlist = new FRIENDLIST;
        temppfriendlist->push_back(tempjsfriendinfo);

        mapjsfriendinfo.insert(pair<unsigned long, PFRIENDLIST>(friendid, temppfriendlist));
    }

    return res;
}


// add friend
int JSFriendAddFriend(unsigned long charid, unsigned long friendid, struct net_pool* np)
{
    int res = 0;

    unsigned long  curtime = jsutil_getcurtime();

    // [1] log to sqllist
    char szsqlformat[JSDB_MAXSIZE_SQL];
    memset(szsqlformat, '\0', sizeof(szsqlformat));

    char* pszsql1 = (char*)malloc(JSDB_MAXSIZE_SQL);
    memset(pszsql1, '\0', sizeof(JSDB_MAXSIZE_SQL));

    sprintf(szsqlformat, "%s", "INSERT INTO jsfriendinfo(charid, friendid, isdelete, addtime) values(%ld, %ld, %d, %ld)");

    sprintf(pszsql1, szsqlformat, charid, friendid, FRIEND_CONF_RELATIONSHIP_ISFRIEND, curtime);
    net_sqllist_set(np, pszsql1);

    char* pszsql2 = (char*)malloc(JSDB_MAXSIZE_SQL);
    memset(pszsql2, '\0', sizeof(JSDB_MAXSIZE_SQL));
    sprintf(pszsql2, szsqlformat, friendid, charid, FRIEND_CONF_RELATIONSHIP_ISFRIEND, curtime);
    net_sqllist_set(np, pszsql2);


    // [2] load to memory database
    friendmemdb_addfriend(charid, friendid, curtime);

    return res;
}


// is friendship
int JSFriendIsFriendship(unsigned long charid, unsigned long friendid, int friendship)
{
    log_debug("JSFriendIsFriendship\n");

    int res = 1;

    FRIENDLIST tempfriendlist;
    JSFriendInfoGet(charid, &tempfriendlist);

    if(tempfriendlist.empty())
    {
        log_debug("XXXXXXXXXXXXXXXXX There friend not datas\n");
    }
    else
    {
        FRIENDLIST::iterator itfriendlist;

        for(itfriendlist = tempfriendlist.begin(); itfriendlist != tempfriendlist.end(); itfriendlist++)
        {
            log_debug("itfriendlist->friendid : %ld, friendid : %ld\n", itfriendlist->friendid, friendid);
            if(itfriendlist->friendid == friendid)
            {
                res = 0;
                break;
            }
        }
    }

    return res;
}









