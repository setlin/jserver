/*=============================================================================
#     FileName: jsaccountinfo.cpp
#         Desc: db jsaccountinfo
#       Author: Setlin
#      Version: 0.0.1
#       Create: 21:06 2015/04/13
#   LastChange: 21:06 2015/04/13
#      History:
=============================================================================*/

#include "jsaccountinfo.h"

map<unsigned long, JSACCOUNTINFO> mapjsaccountinfo;

static int JSAccountInfoLoadData(char* szsql)
{
    log_debug("JSAccountInfoLoadData\n");
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

        JSACCOUNTINFO tempjsaccountinfo;
        memset(&tempjsaccountinfo, 0 ,sizeof(JSACCOUNTINFO));
        tempjsaccountinfo.charid  = atoi(row[0]);
        memcpy(tempjsaccountinfo.passwd, row[1], strlen(row[1]));
        tempjsaccountinfo.isValid = atoi(row[2]);

        mapjsaccountinfo.insert(pair<unsigned long, JSACCOUNTINFO>(tempjsaccountinfo.charid, tempjsaccountinfo));
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


int JSAccountInfoGet(PJSACCOUNTINFO pjsaccountinfo)
{
    log_debug("JSAccountInfoGet\n");

    int res = 0;

    unsigned long charid = pjsaccountinfo->charid;

    map<unsigned long, JSACCOUNTINFO>::iterator it = mapjsaccountinfo.find(charid);

    if(it != mapjsaccountinfo.end())
    {
        JSACCOUNTINFO tempjsaccountinfo;
        tempjsaccountinfo = it->second;
        memcpy(pjsaccountinfo, &tempjsaccountinfo, sizeof(JSACCOUNTINFO));
    }
    else
    {
        char szsqlformat[JSDB_MAXSIZE_SQL];
        memset(szsqlformat, '\0', sizeof(szsqlformat));

        char szsql[JSDB_MAXSIZE_SQL];
        memset(szsql, '\0', sizeof(szsql));

        sprintf(szsqlformat, "%s", "SELECT charid,passwd,isValid from jsaccountinfo where charid = %ld");
        sprintf(szsql, szsqlformat, charid);

        res = JSAccountInfoLoadData(szsql);

        if(res != -1)
        {
            JSAccountInfoGet(pjsaccountinfo);
        }
    }

    return res;
}





















