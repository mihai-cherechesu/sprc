#include "api.hpp"

/* 
The following structure represents the in memory
structure of the database.

{
	client1_session_key: {
		sensor_id_1: (3, [v1, v2, v3]),
		sensor_id_2: (3, [v5, v9, v13])
	},
	client2_session_key: {
		sensor_id_1: (3, [v1, v2, v3]),
		sensor_id_2: (2, [v5, v9])
	}
}
*/
auto comp_char_ptr = [](char const *a, char const *b) { return strcmp(a, b) < 0; };

map<unsigned long, map<int, pair<int, float*>>> in_mem_db;
map<unsigned long, char*> session_to_username;
map<char*, unsigned long, decltype(comp_char_ptr)> username_to_session(comp_char_ptr);

unsigned long session_sequence = 1UL;

bool 
is_authorized(unsigned long key)
{
	return session_to_username.find(key) != session_to_username.end();
}

bool
username_exists(char *username)
{
	return username_to_session.find(username) != username_to_session.end();
}

bool
record_exists(int sensor_id, unsigned long key)
{
	map<int, pair<int, float*>> client_map = in_mem_db[key];
	return client_map.find(sensor_id) != client_map.end();
}

rpc_msg_
init_unauthorized_msg(unsigned long key)
{	
	char *failure_message = (char*) calloc(RPC_MSG_MAXLEN, sizeof(char));
	sprintf(failure_message, "%s %lu %s", "[UNAUTHORIZED]: session_key", key, "is invalid!");

	return { FAIL, { .failure = { failure_message, 403 } } };
}

rpc_msg_
init_username_conflict_msg(char *username)
{
	char *failure_message = (char*) calloc(RPC_MSG_MAXLEN, sizeof(char));
	sprintf(failure_message, "%s %s %s", "[CONFLICT]: username", username, "already exists!");

	return { FAIL, { .failure = { failure_message, 409 } } };
}

rpc_msg_
init_record_not_found_msg(int sensor_id)
{
	char *failure_message = (char*) calloc(RPC_MSG_MAXLEN, sizeof(char));
	sprintf(failure_message, "%s %d %s", "[NOT_FOUND]: sensor_id", sensor_id, "not found!");

	return { FAIL, { .failure = { failure_message, 404 } } };
}

rpc_msg_
init_sensor_conflict_msg(int sensor_id)
{
	char *failure_message = (char*) calloc(RPC_MSG_MAXLEN, sizeof(char));
	sprintf(failure_message, "%s %d %s", "[CONFLICT]: data already present for", sensor_id, ". Use update instead!");

	return { FAIL, { .failure = { failure_message, 409 } } };
}

void
authorize_client(unsigned long key, char *username)
{
	char *username_val = (char*) calloc(RPC_MSG_MAXLEN, sizeof(char));
	strcpy(username_val, username);

	session_to_username[key] = username_val;
	username_to_session[username_val] = key;
}

void
logout_client(unsigned long key, char *username)
{
	session_to_username.erase(key);
	username_to_session.erase(username);
}

float
compute_min(float *vals, unsigned int vals_len)
{
	return *min_element(vals, vals + vals_len);
}

float
compute_max(float *vals, unsigned int vals_len)
{
	return *max_element(vals, vals + vals_len);
}

float
compute_avg(float *vals, unsigned int vals_len)
{
	float sum = 0;

	for (int i = 0; i < vals_len; i++)
		sum += vals[i];
	
	return sum / vals_len; 
}

float
compute_median(float *vals, unsigned int vals_len)
{
	if (vals_len == 1)
		return vals[0];

	sort(vals, vals + vals_len);
	int mid = vals_len / 2;

	if (vals_len % 2)
		return vals[mid];

	return (vals[mid] + vals[mid - 1]) / 2;
}

rpc_msg_ *
login_1_svc(rpc_msg_ *argp, struct svc_req *rqstp)
{
	static rpc_msg_  result;

	req_login rl = argp->rpc_msg__u.success
		.rpc_success_u.req.body.rpc_request_body_u.r0;
	char *username = rl.username;

	if (username_exists(username)) {
		result = init_username_conflict_msg(username);
		return &result;
	}

	unsigned long new_session_key = session_sequence++;
	authorize_client(new_session_key, username);

	char *success_message = (char*) calloc(RPC_MSG_MAXLEN, sizeof(char));

	sprintf(success_message, "%s %s %s %lu", "User", username,
		"successfully logged in with session_key", new_session_key);

	result = { 
		(err_type) SUCC,
		{ { .m_type = (msg_type_) REPL, 
			{ .repl = { success_message, { (op_type) LOGIN, { new_session_key } } } } 
		} }
	};

	return &result;
}

rpc_msg_ *
logout_1_svc(rpc_msg_ *argp, struct svc_req *rqstp)
{
	static rpc_msg_  result;

	unsigned long session_key = argp->rpc_msg__u.success
		.rpc_success_u.req.session_key;

	req_logout rl = argp->rpc_msg__u.success
		.rpc_success_u.req.body.rpc_request_body_u.r1;
	
	if (!is_authorized(session_key)) {
		result = init_unauthorized_msg(session_key);
		return &result;
	}

	char *username = session_to_username[session_key];
	char *success_message = (char*) calloc(RPC_MSG_MAXLEN, sizeof(char));

	sprintf(success_message, "%s %s %s %lu %s", "user", username,
		"successfully logged out and session_key", session_key, "has been deleted.");

	logout_client(session_key, username);

	result = { 
		(err_type) SUCC,
		{ { .m_type = (msg_type_) REPL, 
			{ .repl = { success_message, { (op_type) LOGOUT, { .r1 = { 0x1 } } } } } 
		} }
	};

	return &result;
}

rpc_msg_ *
load_1_svc(rpc_msg_ *argp, struct svc_req *rqstp)
{
	static rpc_msg_  result;

	unsigned long session_key = argp->rpc_msg__u.success
		.rpc_success_u.req.session_key;

	if (!is_authorized(session_key)) {
		result = init_unauthorized_msg(session_key);
		return &result;
	}

	req_load rl = argp->rpc_msg__u.success
		.rpc_success_u.req.body.rpc_request_body_u.r2;

	int sdl_len = rl.sdl.sdl_len;
	sensor_data *sdl = rl.sdl.sdl_val;
	map<int, pair<int, float*>> client_map;

	for (int i = 0; i < sdl_len; i++) {
		int s_vals_len = sdl[i].values.values_len;
		int s_id = sdl[i].sensor_id;

		float *s_vals = (float *) calloc(s_vals_len, sizeof(float));
		memcpy(s_vals, sdl[i].values.values_val, s_vals_len * sizeof(float));

		client_map[s_id] = make_pair(s_vals_len, s_vals);
	}

	in_mem_db[session_key] = client_map;

	char *success_message = (char*) calloc(RPC_MSG_MAXLEN, sizeof(char));
	sprintf(success_message, "Successfully loaded the values!");

	result = { 
		(err_type) SUCC,
		{ { .m_type = (msg_type_) REPL, 
			{ .repl = { success_message, { (op_type) LOAD, { .r2 = { 0x2 } } } } } 
		} }
	};

	return &result;
}

rpc_msg_ *
store_1_svc(rpc_msg_ *argp, struct svc_req *rqstp)
{
	static rpc_msg_  result;

	unsigned long session_key = argp->rpc_msg__u.success
		.rpc_success_u.req.session_key;

	if (!is_authorized(session_key)) {
		result = init_unauthorized_msg(session_key);
		return &result;
	}

	sensor_data *sdl = (sensor_data*) calloc(RPC_SDL_MAXLEN, sizeof(sensor_data));
	unsigned int sdl_len = 0;

	for (const auto &kv : in_mem_db[session_key]) {
		int sensor_id = kv.first;
		int sensor_vals_len = kv.second.first;
		float* sensor_vals = kv.second.second;

		sdl[sdl_len].sensor_id = sensor_id;
		sdl[sdl_len].values.values_len = sensor_vals_len;
		sdl[sdl_len].values.values_val = (float*) calloc(sensor_vals_len, sizeof(float));

		memcpy(sdl[sdl_len].values.values_val, sensor_vals, sensor_vals_len * sizeof(float));
		sdl_len++;
	}

	char *success_message = (char*) calloc(RPC_MSG_MAXLEN, sizeof(char));
	sprintf(success_message, "Received the values from the server!");

	result = { 
		(err_type) SUCC,
		{ { .m_type = (msg_type_) REPL, 
			{ .repl = { success_message, { (op_type) STORE, { .r3 = { { sdl_len, sdl } } } } } } 
		} }
	};

	return &result;
}

rpc_msg_ *
add_1_svc(rpc_msg_ *argp, struct svc_req *rqstp)
{
	static rpc_msg_  result;

	unsigned long session_key = argp->rpc_msg__u.success
		.rpc_success_u.req.session_key;

	if (!is_authorized(session_key)) {
		result = init_unauthorized_msg(session_key);
		return &result;
	}

	req_add rl = argp->rpc_msg__u.success
		.rpc_success_u.req.body.rpc_request_body_u.r4;

	int sensor_id = rl.sd.sensor_id;
	int sd_len = rl.sd.values.values_len;
	float *sd_vals = (float*) calloc(sd_len, sizeof(float));
	memcpy(sd_vals, rl.sd.values.values_val, sd_len * sizeof(float));

	if (record_exists(sensor_id, session_key)) {
		result = init_sensor_conflict_msg(session_key);
		return &result;
	}

	in_mem_db[session_key][sensor_id] = make_pair(sd_len, sd_vals);

	char *success_message = (char*) calloc(RPC_MSG_MAXLEN, sizeof(char));
	sprintf(success_message, "Successfully added the entry!");

	result = { 
		(err_type) SUCC,
		{ { .m_type = (msg_type_) REPL, 
			{ .repl = { success_message, { (op_type) ADD, { .r4 = { 0x4 } } } } } 
		} }
	};

	return &result;
}

rpc_msg_ *
del_1_svc(rpc_msg_ *argp, struct svc_req *rqstp)
{
	static rpc_msg_  result;

	unsigned long session_key = argp->rpc_msg__u.success
		.rpc_success_u.req.session_key;

	if (!is_authorized(session_key)) {
		result = init_unauthorized_msg(session_key);
		return &result;
	}

	req_del rl = argp->rpc_msg__u.success
		.rpc_success_u.req.body.rpc_request_body_u.r5;

	int sensor_id = rl.sensor_id;

	if (!record_exists(sensor_id, session_key)) {
		result = init_record_not_found_msg(sensor_id);
		return &result;
	}

	in_mem_db[session_key].erase(sensor_id);
	
	char *success_message = (char*) calloc(RPC_MSG_MAXLEN, sizeof(char));
	sprintf(success_message, "Successfully deleted the data for sensor %d!", sensor_id);

	result = { 
		(err_type) SUCC,
		{ { .m_type = (msg_type_) REPL, 
			{ .repl = { success_message, { (op_type) DEL, { .r4 = { 0x5 } } } } } 
		} }
	};

	return &result;
}

rpc_msg_ *
update_1_svc(rpc_msg_ *argp, struct svc_req *rqstp)
{
	static rpc_msg_  result;

	unsigned long session_key = argp->rpc_msg__u.success
		.rpc_success_u.req.session_key;

	if (!is_authorized(session_key)) {
		result = init_unauthorized_msg(session_key);
		return &result;
	}

	req_update rl = argp->rpc_msg__u.success
		.rpc_success_u.req.body.rpc_request_body_u.r6;

	int sensor_id = rl.sd.sensor_id;
	int sd_len = rl.sd.values.values_len;
	float *sd_vals = (float*) calloc(sd_len, sizeof(float));
	memcpy(sd_vals, rl.sd.values.values_val, sd_len * sizeof(float));

	char *success_message = (char*) calloc(RPC_MSG_MAXLEN, sizeof(char));
	int msg_len = 0;

	if (record_exists(sensor_id, session_key)) {
		pair<int, float*> existing_record = in_mem_db[session_key][sensor_id];

		msg_len += sprintf(success_message, "Record already exists for sensor %d: ", sensor_id);
		
		int sd_len = existing_record.first;
		float *sd = existing_record.second;

		for (int i = 0; i < sd_len; i++) {
			msg_len += sprintf(success_message + msg_len, "%f ", sd[i]);
		}
	} 

	sprintf(success_message + msg_len, ". Successfully updated the entry!");

	in_mem_db[session_key][sensor_id] = make_pair(sd_len, sd_vals);

	result = { 
		(err_type) SUCC,
		{ { .m_type = (msg_type_) REPL, 
			{ .repl = { success_message, { (op_type) UPDATE, { .r6 = { 0x6 } } } } } 
		} }
	};

	return &result;
}

rpc_msg_ *
read_1_svc(rpc_msg_ *argp, struct svc_req *rqstp)
{
	static rpc_msg_  result;

	unsigned long session_key = argp->rpc_msg__u.success
		.rpc_success_u.req.session_key;

	if (!is_authorized(session_key)) {
		result = init_unauthorized_msg(session_key);
		return &result;
	}

	req_read rl = argp->rpc_msg__u.success
		.rpc_success_u.req.body.rpc_request_body_u.r7;

	int sensor_id = rl.sensor_id;

	if (!record_exists(sensor_id, session_key)) {
		result = init_record_not_found_msg(sensor_id);
		return &result;
	}

	pair<int, float*> record = in_mem_db[session_key][sensor_id];
	unsigned int sd_len = record.first;
	float *sd_vals = record.second;
	
	sensor_data sd = { sensor_id, { sd_len, sd_vals } };

	char *success_message = (char*) calloc(RPC_MSG_MAXLEN, sizeof(char));
	sprintf(success_message, "Data for sensor %d successfully received!", sensor_id);

	result = { 
		(err_type) SUCC,
		{ { .m_type = (msg_type_) REPL, 
			{ .repl = { success_message, { (op_type) READ, { .r7 = { sd } } } } } 
		} }
	};

	return &result;
}

rpc_msg_ *
get_stat_1_svc(rpc_msg_ *argp, struct svc_req *rqstp)
{
	static rpc_msg_  result;

	unsigned long session_key = argp->rpc_msg__u.success
		.rpc_success_u.req.session_key;

	if (!is_authorized(session_key)) {
		result = init_unauthorized_msg(session_key);
		return &result;
	}

	req_get_stat rl = argp->rpc_msg__u.success
		.rpc_success_u.req.body.rpc_request_body_u.r8;

	int sensor_id = rl.sensor_id;
	stat_type s_type = rl.s_type;

	if (!record_exists(sensor_id, session_key)) {
		result = init_record_not_found_msg(sensor_id);
		return &result;
	}

	pair<int, float*> record = in_mem_db[session_key][sensor_id];
	unsigned int sd_len = record.first;
	float *sd_vals = record.second;

	float stat = (s_type == MIN) ? compute_min(sd_vals, sd_len) :
				 (s_type == MAX) ? compute_max(sd_vals, sd_len) :
				 (s_type == AVG) ? compute_avg(sd_vals, sd_len) :
				 (s_type == MEDIAN) ? compute_median(sd_vals, sd_len) :
				 -1;

	char *success_message = (char*) calloc(RPC_MSG_MAXLEN, sizeof(char));
	sprintf(success_message, "Stat for sensor %d successfully received!", sensor_id);

	result = { 
		(err_type) SUCC,
		{ { .m_type = (msg_type_) REPL, 
			{ .repl = { success_message, { (op_type) GET_STAT, { .r8 = { stat } } } } } 
		} }
	};

	return &result;
}

rpc_msg_ *
get_stat_all_1_svc(rpc_msg_ *argp, struct svc_req *rqstp)
{
	static rpc_msg_ result;

	unsigned long session_key = argp->rpc_msg__u.success
		.rpc_success_u.req.session_key;

	if (!is_authorized(session_key)) {
		result = init_unauthorized_msg(session_key);
		return &result;
	}

	sensor_stats *ssl = (sensor_stats*) calloc(RPC_SDL_MAXLEN, sizeof(sensor_stats));
	unsigned int ssl_len = 0;

	for (const auto &kv : in_mem_db[session_key]) {
		int sensor_id = kv.first;
		unsigned int sensor_vals_len = kv.second.first;
		float *sensor_vals = kv.second.second;

		float *ss = (float*) calloc(RPC_NUM_STATS, sizeof(float));
		ss[MIN] 	= compute_min(sensor_vals, sensor_vals_len);
		ss[MAX] 	= compute_max(sensor_vals, sensor_vals_len);
		ss[AVG] 	= compute_avg(sensor_vals, sensor_vals_len);
		ss[MEDIAN] 	= compute_median(sensor_vals, sensor_vals_len);

		ssl[ssl_len].sensor_id = sensor_id;
		ssl[ssl_len].stats.stats_len = RPC_NUM_STATS;
		ssl[ssl_len].stats.stats_val = ss;

		ssl_len++;
	}

	char *success_message = (char*) calloc(RPC_MSG_MAXLEN, sizeof(char));
	sprintf(success_message, "Stats for sensors successfully received!");

	result = { 
		(err_type) SUCC,
		{ { .m_type = (msg_type_) REPL, 
			{ .repl = { success_message, { (op_type) GET_STAT_ALL, { .r9 = { { ssl_len, ssl } } } } } } 
		} }
	};

	return &result;
}