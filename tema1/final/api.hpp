#ifndef _API_H_RPCGEN
#define _API_H_RPCGEN

#include <rpc/rpc.h>
#include <bits/stdc++.h>

using namespace std;

#define RPC_MSG_MAXLEN 	256
#define RPC_SDL_MAXLEN 	100
#define RPC_NUM_STATS	4

#define TKN_CMD			0
#define TKN_USERNAME	1
#define TKN_SID			1
#define TKN_SDATA_LEN	2
#define TKN_SDATA_VAL	3

#ifdef __cplusplus
extern "C" {
#endif

enum err_type {
	SUCC = 0,
	FAIL = 1,
};
typedef enum err_type err_type;

enum op_type {
	LOGIN = 0,
	LOGOUT = 1,
	LOAD = 2,
	STORE = 3,
	ADD = 4,
	DEL = 5,
	UPDATE = 6,
	READ = 7,
	GET_STAT = 8,
	GET_STAT_ALL = 9,
};
typedef enum op_type op_type;

enum stat_type {
	MIN = 0,
	MAX = 1,
	AVG = 2,
	MEDIAN = 3,
};
typedef enum stat_type stat_type;

enum msg_type_ {
	REQ = 0,
	REPL = 1,
};
typedef enum msg_type_ msg_type_;

struct sensor_data {
	int sensor_id;
	struct {
		u_int values_len;
		float *values_val;
	} values;
};
typedef struct sensor_data sensor_data;

struct sensor_stats {
	int sensor_id;
	struct {
		u_int stats_len;
		float *stats_val;
	} stats;
};
typedef struct sensor_stats sensor_stats;

struct req_login {
	char *username;
};
typedef struct req_login req_login;

struct req_logout {
	int filler;
};
typedef struct req_logout req_logout;

struct req_load {
	struct {
		u_int sdl_len;
		sensor_data *sdl_val;
	} sdl;
};
typedef struct req_load req_load;

struct req_store {
	int filler;
};
typedef struct req_store req_store;

struct req_add {
	sensor_data sd;
};
typedef struct req_add req_add;

struct req_del {
	int sensor_id;
};
typedef struct req_del req_del;

struct req_update {
	sensor_data sd;
};
typedef struct req_update req_update;

struct req_read {
	int sensor_id;
};
typedef struct req_read req_read;

struct req_get_stat {
	int sensor_id;
	stat_type s_type;
};
typedef struct req_get_stat req_get_stat;

struct req_get_stat_all {
	int filler;
};
typedef struct req_get_stat_all req_get_stat_all;

struct rpc_request_body {
	op_type op;
	union {
		req_login r0;
		req_logout r1;
		req_load r2;
		req_store r3;
		req_add r4;
		req_del r5;
		req_update r6;
		req_read r7;
		req_get_stat r8;
		req_get_stat_all r9;
	} rpc_request_body_u;
};
typedef struct rpc_request_body rpc_request_body;

struct rpc_request {
	u_long session_key;
	rpc_request_body body;
};
typedef struct rpc_request rpc_request;

struct repl_login {
	u_long new_session_key;
};
typedef struct repl_login repl_login;

struct repl_logout {
	int filler;
};
typedef struct repl_logout repl_logout;

struct repl_load {
	int filler;
};
typedef struct repl_load repl_load;

struct repl_store {
	struct {
		u_int sdl_len;
		sensor_data *sdl_val;
	} sdl;
};
typedef struct repl_store repl_store;

struct repl_add {
	int filler;
};
typedef struct repl_add repl_add;

struct repl_del {
	int filler;
};
typedef struct repl_del repl_del;

struct repl_update {
	int filler;
};
typedef struct repl_update repl_update;

struct repl_read {
	sensor_data sd;
};
typedef struct repl_read repl_read;

struct repl_get_stat {
	float stat;
};
typedef struct repl_get_stat repl_get_stat;

struct repl_get_stat_all {
	struct {
		u_int ssl_len;
		sensor_stats *ssl_val;
	} ssl;
};
typedef struct repl_get_stat_all repl_get_stat_all;

struct rpc_reply_body {
	op_type op;
	union {
		repl_login r0;
		repl_logout r1;
		repl_load r2;
		repl_store r3;
		repl_add r4;
		repl_del r5;
		repl_update r6;
		repl_read r7;
		repl_get_stat r8;
		repl_get_stat_all r9;
	} rpc_reply_body_u;
};
typedef struct rpc_reply_body rpc_reply_body;

struct rpc_reply {
	char *success_message;
	rpc_reply_body body;
};
typedef struct rpc_reply rpc_reply;

struct rpc_success {
	msg_type_ m_type;
	union {
		rpc_request req;
		rpc_reply repl;
	} rpc_success_u;
};
typedef struct rpc_success rpc_success;

struct rpc_failure {
	char *failure_message;
	u_int failure_code;
};
typedef struct rpc_failure rpc_failure;

struct rpc_msg_ {
	err_type err;
	union {
		rpc_success success;
		rpc_failure failure;
	} rpc_msg__u;
};
typedef struct rpc_msg_ rpc_msg_;

#define api_prog 123456789
#define api_vers 1

#if defined(__STDC__) || defined(__cplusplus)
#define LOGIN 0
extern  rpc_msg_ * login_1(rpc_msg_ *, CLIENT *);
extern  rpc_msg_ * login_1_svc(rpc_msg_ *, struct svc_req *);
#define LOGOUT 1
extern  rpc_msg_ * logout_1(rpc_msg_ *, CLIENT *);
extern  rpc_msg_ * logout_1_svc(rpc_msg_ *, struct svc_req *);
#define LOAD 2
extern  rpc_msg_ * load_1(rpc_msg_ *, CLIENT *);
extern  rpc_msg_ * load_1_svc(rpc_msg_ *, struct svc_req *);
#define STORE 3
extern  rpc_msg_ * store_1(rpc_msg_ *, CLIENT *);
extern  rpc_msg_ * store_1_svc(rpc_msg_ *, struct svc_req *);
#define ADD 4
extern  rpc_msg_ * add_1(rpc_msg_ *, CLIENT *);
extern  rpc_msg_ * add_1_svc(rpc_msg_ *, struct svc_req *);
#define DEL 5
extern  rpc_msg_ * del_1(rpc_msg_ *, CLIENT *);
extern  rpc_msg_ * del_1_svc(rpc_msg_ *, struct svc_req *);
#define UPDATE 6
extern  rpc_msg_ * update_1(rpc_msg_ *, CLIENT *);
extern  rpc_msg_ * update_1_svc(rpc_msg_ *, struct svc_req *);
#define READ 7
extern  rpc_msg_ * read_1(rpc_msg_ *, CLIENT *);
extern  rpc_msg_ * read_1_svc(rpc_msg_ *, struct svc_req *);
#define GET_STAT 8
extern  rpc_msg_ * get_stat_1(rpc_msg_ *, CLIENT *);
extern  rpc_msg_ * get_stat_1_svc(rpc_msg_ *, struct svc_req *);
#define GET_STAT_ALL 9
extern  rpc_msg_ * get_stat_all_1(rpc_msg_ *, CLIENT *);
extern  rpc_msg_ * get_stat_all_1_svc(rpc_msg_ *, struct svc_req *);
extern int api_prog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define LOGIN 0
extern  rpc_msg_ * login_1();
extern  rpc_msg_ * login_1_svc();
#define LOGOUT 1
extern  rpc_msg_ * logout_1();
extern  rpc_msg_ * logout_1_svc();
#define LOAD 2
extern  rpc_msg_ * load_1();
extern  rpc_msg_ * load_1_svc();
#define STORE 3
extern  rpc_msg_ * store_1();
extern  rpc_msg_ * store_1_svc();
#define ADD 4
extern  rpc_msg_ * add_1();
extern  rpc_msg_ * add_1_svc();
#define DEL 5
extern  rpc_msg_ * del_1();
extern  rpc_msg_ * del_1_svc();
#define UPDATE 6
extern  rpc_msg_ * update_1();
extern  rpc_msg_ * update_1_svc();
#define READ 7
extern  rpc_msg_ * read_1();
extern  rpc_msg_ * read_1_svc();
#define GET_STAT 8
extern  rpc_msg_ * get_stat_1();
extern  rpc_msg_ * get_stat_1_svc();
#define GET_STAT_ALL 9
extern  rpc_msg_ * get_stat_all_1();
extern  rpc_msg_ * get_stat_all_1_svc();
extern int api_prog_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_err_type (XDR *, err_type*);
extern  bool_t xdr_op_type (XDR *, op_type*);
extern  bool_t xdr_stat_type (XDR *, stat_type*);
extern  bool_t xdr_msg_type_ (XDR *, msg_type_*);
extern  bool_t xdr_sensor_data (XDR *, sensor_data*);
extern  bool_t xdr_sensor_stats (XDR *, sensor_stats*);
extern  bool_t xdr_req_login (XDR *, req_login*);
extern  bool_t xdr_req_logout (XDR *, req_logout*);
extern  bool_t xdr_req_load (XDR *, req_load*);
extern  bool_t xdr_req_store (XDR *, req_store*);
extern  bool_t xdr_req_add (XDR *, req_add*);
extern  bool_t xdr_req_del (XDR *, req_del*);
extern  bool_t xdr_req_update (XDR *, req_update*);
extern  bool_t xdr_req_read (XDR *, req_read*);
extern  bool_t xdr_req_get_stat (XDR *, req_get_stat*);
extern  bool_t xdr_req_get_stat_all (XDR *, req_get_stat_all*);
extern  bool_t xdr_rpc_request_body (XDR *, rpc_request_body*);
extern  bool_t xdr_rpc_request (XDR *, rpc_request*);
extern  bool_t xdr_repl_login (XDR *, repl_login*);
extern  bool_t xdr_repl_logout (XDR *, repl_logout*);
extern  bool_t xdr_repl_load (XDR *, repl_load*);
extern  bool_t xdr_repl_store (XDR *, repl_store*);
extern  bool_t xdr_repl_add (XDR *, repl_add*);
extern  bool_t xdr_repl_del (XDR *, repl_del*);
extern  bool_t xdr_repl_update (XDR *, repl_update*);
extern  bool_t xdr_repl_read (XDR *, repl_read*);
extern  bool_t xdr_repl_get_stat (XDR *, repl_get_stat*);
extern  bool_t xdr_repl_get_stat_all (XDR *, repl_get_stat_all*);
extern  bool_t xdr_rpc_reply_body (XDR *, rpc_reply_body*);
extern  bool_t xdr_rpc_reply (XDR *, rpc_reply*);
extern  bool_t xdr_rpc_success (XDR *, rpc_success*);
extern  bool_t xdr_rpc_failure (XDR *, rpc_failure*);
extern  bool_t xdr_rpc_msg_ (XDR *, rpc_msg_*);

#else /* K&R C */
extern bool_t xdr_err_type ();
extern bool_t xdr_op_type ();
extern bool_t xdr_stat_type ();
extern bool_t xdr_msg_type_ ();
extern bool_t xdr_sensor_data ();
extern bool_t xdr_sensor_stats ();
extern bool_t xdr_req_login ();
extern bool_t xdr_req_logout ();
extern bool_t xdr_req_load ();
extern bool_t xdr_req_store ();
extern bool_t xdr_req_add ();
extern bool_t xdr_req_del ();
extern bool_t xdr_req_update ();
extern bool_t xdr_req_read ();
extern bool_t xdr_req_get_stat ();
extern bool_t xdr_req_get_stat_all ();
extern bool_t xdr_rpc_request_body ();
extern bool_t xdr_rpc_request ();
extern bool_t xdr_repl_login ();
extern bool_t xdr_repl_logout ();
extern bool_t xdr_repl_load ();
extern bool_t xdr_repl_store ();
extern bool_t xdr_repl_add ();
extern bool_t xdr_repl_del ();
extern bool_t xdr_repl_update ();
extern bool_t xdr_repl_read ();
extern bool_t xdr_repl_get_stat ();
extern bool_t xdr_repl_get_stat_all ();
extern bool_t xdr_rpc_reply_body ();
extern bool_t xdr_rpc_reply ();
extern bool_t xdr_rpc_success ();
extern bool_t xdr_rpc_failure ();
extern bool_t xdr_rpc_msg_ ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_API_H_RPCGEN */
