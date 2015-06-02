#ifndef _JSDB_H_
#define _JSDB_H_


#include "jsstd.h"
#include <mysql/mysql.h>

extern MYSQL JSDB_globalMysql;

extern int JSDBInit();
extern int JSDBClose();
extern int JSDBExecutesql(const char*);

extern MYSQL_RES* JSDBQuery(const char*);

#endif























