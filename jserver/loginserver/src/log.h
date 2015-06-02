/*=============================================================================
#     FileName: log.h
#         Desc: for log
#       Author: Setlin
#      Version: 0.0.1
#       Create: 10:10 2015/03/23
#   LastChange: 10:10 2015/03/23
#      History:
=============================================================================*/
#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"

#define log_error(fmt, ...) fprintf(stderr, KRED "[error]\t[%s:%d]" fmt "\n" KNRM , __FILE__, __LINE__, ##__VA_ARGS__)
#define log_debugEx(fmt, ...) fprintf(stdout, "[debug]\t[%s:%d]" fmt "\n",  __FILE__, __LINE__, ##__VA_ARGS__)
#define log_infoEx(fmt, ...) fprintf(stdout, "[info]\t[%s:%d]" fmt "\n",  __FILE__, __LINE__, ##__VA_ARGS__)

#define log_debug(fmt, ...) fprintf(stdout, "[debug]\t" fmt, ##__VA_ARGS__)
#define log_info(fmt, ...) fprintf(stdout, "[info]\t" fmt, ##__VA_ARGS__)

#endif
