/*=============================================================================
#     FileName: protid.h
#         Desc: record protid
#       Author: Setlin
#      Version: 0.0.1
#       Create: 16:58 2015/04/03
#   LastChange: 16:58 2015/04/03
#      History:
=============================================================================*/
#ifndef _PROTID_H_
#define _PROTID_H_


// protocol id
// from odd to even
// C2S odd
// S2C even
// increase by 100

// [sys 100 - 300]
// [[---------------------------
// distinguish server or client
#define PROTID_DISTINGUISH_SERVER_C2G 100
#define PROTID_DISTINGUISH_SERVER_G2C 101

// on login
#define PROTID_ONLOGIN_S2C 201
#define PROTID_ONLOGIN_C2S 202

// ---------------------------]]


// [logic model 1001 - 10000]
// [chat 1001 - 1100]
#define PROTID_CHAT_CHANNEL_PERSONAL_C2S 1001
#define PROTID_CHAT_CHANNEL_PERSONAL_S2C 1002

// [char 1101 - 1200]
#define PROTID_CHAR_GETCHARINFO_C2S 1101
#define PROTID_CHAR_GETCHARINFO_S2C 1102


// [friend 1201 - 1300]
#define PROTID_FRIEND_GETFRIENDLIST_C2S 1201
#define PROTID_FRIEND_GETFRIENDLIST_S2C 1202

#define PROTID_FRIEND_ADDFRIEND_C2S 1203
#define PROTID_FRIEND_ADDFRIEND_S2C 1204

#endif




















