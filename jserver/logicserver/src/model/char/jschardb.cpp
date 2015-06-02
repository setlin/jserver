/*=============================================================================
#     FileName: jschardb.cpp
#         Desc: character database
#       Author: Setlin
#      Version: 0.0.1
#       Create: 21:59 2015/05/10
#   LastChange: 21:59 2015/05/10
#      History:
=============================================================================*/


#include "jschardb.h"

map<unsigned long, JSCHARINFO> mapjscharinfo;


static int JSCharInfoLoadData(char* szsql)
{
    log_debug("JSCharInfoLoadData\n");
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

        JSCHARINFO tempjscharinfo;
        memset(&tempjscharinfo, 0 ,sizeof(JSCHARINFO));
        tempjscharinfo.charid  = atoi(row[0]);
        memcpy(tempjscharinfo.charname, row[1], strlen(row[1]));
        tempjscharinfo.gender = atoi(row[2]);
        tempjscharinfo.age    = atoi(row[3]);
        memcpy(tempjscharinfo.locality, row[4], strlen(row[4]));
        memcpy(tempjscharinfo.signature, row[5], strlen(row[5]));
        memcpy(tempjscharinfo.grouplist, row[6], strlen(row[6]));

        mapjscharinfo.insert(pair<unsigned long, JSCHARINFO>(tempjscharinfo.charid, tempjscharinfo));
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


int JSCharDBInfoGet(PJSCHARINFO pjscharinfo)
{
    log_debug("JSCharDBInfoGet\n");

    int res = 0;

    unsigned long charid = pjscharinfo->charid;

    map<unsigned long, JSCHARINFO>::iterator it = mapjscharinfo.find(charid);

    if(it != mapjscharinfo.end())
    {
        JSCHARINFO tempjscharinfo;
        tempjscharinfo = it->second;
        memcpy(pjscharinfo, &tempjscharinfo, sizeof(JSCHARINFO));
    }
    else
    {
        char szsqlformat[JSDB_MAXSIZE_SQL];
        memset(szsqlformat, '\0', sizeof(szsqlformat));

        char szsql[JSDB_MAXSIZE_SQL];
        memset(szsql, '\0', sizeof(szsql));

        sprintf(szsqlformat, "%s", "SELECT charid,charname,gender,age,locality,signature,grouplist from jscharinfo where charid = %ld");
        sprintf(szsql, szsqlformat, charid);

        res = JSCharInfoLoadData(szsql);

        if(res != -1)
        {
            JSCharDBInfoGet(pjscharinfo);
        }
        else
        {
            log_debug("no data\n");
        }
    }

    return res;
}


int JSCharInfoClear(PJSCHARINFO pjscharinfo)
{
    log_debug("JSCharInfoClear\n");
    int res = 0;

    unsigned long charid = pjscharinfo->charid;

    map<unsigned long, JSCHARINFO>::iterator it = mapjscharinfo.find(charid);

    if(it != mapjscharinfo.end())
    {
        mapjscharinfo.erase(it);
    }
    else
    {
        res = -1;
    }

    return res;
}











































