/*=============================================================================
#     FileName: jslogdb.cpp
#         Desc: log data to database
#       Author: Setlin
#      Version: 0.0.1
#       Create: 18:39 2015/05/20
#   LastChange: 18:39 2015/05/20
#      History:
=============================================================================*/


#include "jslogdb.h"
#include "jsstd.h"

#include <sys/types.h>
#include <sys/wait.h>


#include "jsdb.h"


int JSLogDBByFork(struct net_pool* np)
{
    int res = 0;
    log_debug("JSLogDBByFork\n");


    // new a thread

    pid_t fpid;

    // first fork
    if((fpid = fork()) < 0)
    {
        log_error("fork 1 error");
    }
    // first child process
    else if(fpid == 0)
    {
        if((fpid = fork()) < 0)
        {
            log_error("fork 2 error");
        }
        // first child = second child's parent
        else if(fpid > 0)
        {
            // kill the first child
            exit(0);
        }

        // now in the second child, wait its parent free source
        sleep(2);
        log_debug("second child: begin log data to database ...\n");

        SQLLIST* psqllist = new SQLLIST;
        net_sqllist_get(np, psqllist);
        
        if(psqllist->empty())
        {
            log_debug("Yamielo\n");
        }
        else
        {
            SQLLIST::iterator it = psqllist->begin();

            for(; it != psqllist->end(); ++it)
            {
                log_debug("OOXX %s\n", *it);
                JSDBExecutesql(*it);
            }

            // char* tempSzSQL = psqllist->front();
            // log_debug("Create SQL : %s\n", tempSzSQL);
        }

        log_debug("second child ----\n");
        delete psqllist;
        log_debug("second child: log data to database finish ...\n");
        // charge by root
        exit(0);
    }
    // first parent
    else
    {
        if(waitpid(fpid, NULL, 0) != fpid) // wait for first child
        {
            log_error("waitpid error");
        }

        log_debug("parent ...\n");
        log_debug("==clear the sqllist==\n");

        net_sqllist_clear(np);
    }



    return res;
}



















