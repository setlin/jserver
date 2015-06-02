#include "jsdb.h"
#include "log.h"

static const char* dbaddress = "127.0.0.1";
static const char* dbusrname = "root";
static const char* dbpasswd  = "root";
static const char* dbname    = "jsystem";
static unsigned short dbport = 3306;

MYSQL JSDB_globalMysql;

int JSDBInit()
{
    int res = 0;

    printf("JSDBInit\n");
    if(mysql_init(&JSDB_globalMysql) == NULL)
    {
        printf("[error]init db failure\n");
        return -1;
    }

    if(!mysql_real_connect(&JSDB_globalMysql, dbaddress, dbusrname, dbpasswd, dbname, dbport, NULL, 0))
    {
        printf("[error]connect db failure\n");
        res = -1;
    }


    return res;
}

int JSDBClose()
{
    int res = 0;

    mysql_close(&JSDB_globalMysql);

    return res;
}


int JSDBExecutesql(const char* szsql)
{
    int res = 0;
    if(mysql_ping(&JSDB_globalMysql) != 0)
    {
        printf("[erroor]mysql disconnect\n");
        return -1;
    }

    res = mysql_real_query(&JSDB_globalMysql, szsql, (unsigned int)strlen(szsql));
    if(!res)
    {
        printf("[debug]execute -[ %s ] -  successfully\n", szsql);
        return -1;
    }

    return res;
}

MYSQL_RES* JSDBQuery(const char* szsql)
{
    log_debug("JSDBQuery\n");

    int res = 0;

    if(mysql_ping(&JSDB_globalMysql) != 0)
    {
        printf("[error]mysql disconnect\n");
        return NULL;
    }

    // mysql查询
    log_debug("%s\n", szsql);

    res = mysql_real_query(&JSDB_globalMysql, szsql, (unsigned int)strlen(szsql));
    log_debug("XXXXXXXXX res : %d\n", res);
    if(!res)
    {
        printf("[debug]query -[ %s ]- successfully\n", szsql);

        // 查询结果集
        MYSQL_RES* RES = mysql_store_result(&JSDB_globalMysql);
        return RES;
        
        /*

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
            printf("rowno %d\n", rowno++);

            // 处理每一行的每一列
            int i = 0;
            for( ; i < fieldnum; ++i)
            {
                printf("\t[%s ]: %s\n", fields[i].name, row[i]);
            }
        }

        // 释放结果集内存
        mysql_free_result(RES);

        */
    }

    log_debug("No data in database\n");

    return NULL;
}























