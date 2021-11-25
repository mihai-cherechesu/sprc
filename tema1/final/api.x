/* */

enum err_type {
    SUCC = 0,
    FAIL = 1
};

enum op_type {
    LOGIN           = 0,
    LOGOUT          = 1,
    LOAD            = 2,
    STORE           = 3,
    ADD             = 4,
    DEL             = 5,
    UPDATE          = 6,
    READ            = 7,
    GET_STAT        = 8,
    GET_STAT_ALL    = 9
};

enum stat_type {
    MIN     = 0,
    MAX     = 1,
    AVG     = 2,
    MEDIAN  = 3
};

enum msg_type_ {
    REQ     = 0,
    REPL    = 1
};

/*
******************************************************************************* 
Custom helper structures.
******************************************************************************* 
*/

struct sensor_data {
    int sensor_id;
    float values<>;
};

struct sensor_stats {
    int sensor_id;
    float stats<4>;
};

/*
******************************************************************************* 
Request body structures.
******************************************************************************* 
*/

struct req_login {
    string username<>;
};

struct req_logout {
    int filler;
};

struct req_load {
    sensor_data sdl<>;
};

struct req_store {
    int filler;
};

struct req_add {
    sensor_data sd;
};

struct req_del {
    int sensor_id;
};

struct req_update {
    sensor_data sd;
};

struct req_read {
    int sensor_id;
};

struct req_get_stat {
    int sensor_id;
    stat_type s_type;
};

struct req_get_stat_all {
    int filler;
};

/*
******************************************************************************* 
Request structure.
******************************************************************************* 
*/
union rpc_request_body switch (op_type op) {
    case LOGIN:
        req_login r0;
    case LOGOUT:
        req_logout r1;
    case LOAD:
        req_load r2;
    case STORE:
        req_store r3;
    case ADD:
        req_add r4;
    case DEL:
        req_del r5;
    case UPDATE:
        req_update r6;
    case READ:
        req_read r7;
    case GET_STAT:
        req_get_stat r8;
    case GET_STAT_ALL:
        req_get_stat_all r9;
    default:
        void;
};

struct rpc_request {
    unsigned long session_key;
    rpc_request_body body;
};

/*
******************************************************************************* 
Reply body structures.
******************************************************************************* 
*/

struct repl_login {
    unsigned long new_session_key;
};

struct repl_logout {
    int filler;
};

struct repl_load {
    int filler;
};

struct repl_store {
    sensor_data sdl<>;
};

struct repl_add {
    int filler;
};

struct repl_del {
    int filler;
};

struct repl_update {
    int filler;
};

struct repl_read {
    sensor_data sd;
};

struct repl_get_stat {
    float stat;
};

struct repl_get_stat_all {
    sensor_stats ssl<>;
};

/*
******************************************************************************* 
Reply structure.
******************************************************************************* 
*/
union rpc_reply_body switch (op_type op) {
    case LOGIN:
        repl_login r0;
    case LOGOUT:
        repl_logout r1;
    case LOAD:
        repl_load r2;
    case STORE:
        repl_store r3;
    case ADD:
        repl_add r4;
    case DEL:
        repl_del r5;
    case UPDATE:
        repl_update r6;
    case READ:
        repl_read r7;
    case GET_STAT:
        repl_get_stat r8;
    case GET_STAT_ALL:
        repl_get_stat_all r9;
    default:
        void;
};

struct rpc_reply {
    string success_message<>;
    rpc_reply_body body;
};

/*
******************************************************************************* 
Generic body union.
******************************************************************************* 
*/
union rpc_success switch (msg_type_ m_type) {
    case REQ:
        rpc_request req;
    case REPL:
        rpc_reply repl;
    default:
        void;
};


struct rpc_failure {
    string failure_message<>;
    unsigned int failure_code;
};

/*
******************************************************************************* 
Generic RPC message union.
******************************************************************************* 
*/
union rpc_msg_ switch (err_type err) {
    case SUCC:
        rpc_success success;
    case FAIL:
        rpc_failure failure;
    default:
        void;
};

program api_prog {
    version api_vers {
        rpc_msg_ LOGIN(rpc_msg_)          = 0;
        rpc_msg_ LOGOUT(rpc_msg_)         = 1;
        rpc_msg_ LOAD(rpc_msg_)           = 2;
        rpc_msg_ STORE(rpc_msg_)          = 3;
        rpc_msg_ ADD(rpc_msg_)            = 4;
        rpc_msg_ DEL(rpc_msg_)            = 5;
        rpc_msg_ UPDATE(rpc_msg_)         = 6;
        rpc_msg_ READ(rpc_msg_)           = 7;
        rpc_msg_ GET_STAT(rpc_msg_)       = 8;
        rpc_msg_ GET_STAT_ALL(rpc_msg_)   = 9;
    } = 1;
} = 123456789;