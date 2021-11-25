#include "api.hpp"

map<string, op_type> op_stoi = {
	{ "login", 			(op_type) LOGIN },
	{ "logout", 		(op_type) LOGOUT },
	{ "load", 			(op_type) LOAD },
	{ "store", 			(op_type) STORE },
	{ "add", 			(op_type) ADD },
	{ "del", 			(op_type) DEL },
	{ "update", 		(op_type) UPDATE },
	{ "read", 			(op_type) READ },
	{ "get_stat", 		(op_type) GET_STAT },
	{ "get_stat_all", 	(op_type) GET_STAT_ALL }
};

unsigned long session_key;
char *username;

void
handle_login(string uname, CLIENT *clnt)
{
	cout << "Executing login " << uname << "...\n";

	rpc_msg_ *result_1;
	rpc_msg_  login_1_arg = {
		(err_type) SUCC,
		{ { (msg_type_) REQ, { 0UL, { (op_type) LOGIN, { .r0 = { &uname[0] } } } } } }
	};

	result_1 = login_1(&login_1_arg, clnt);
	if (result_1 == (rpc_msg_ *) NULL) {
		clnt_perror (clnt, "call failed");
	}

	if (result_1->err)
		printf("failure_message: %s, failure_code: %d\n\n",
			result_1->rpc_msg__u.failure.failure_message,
			result_1->rpc_msg__u.failure.failure_code);
	else {
		printf("success_message: %s\n\n", 
			result_1->rpc_msg__u.success.rpc_success_u.repl.success_message);
		
		session_key = result_1->rpc_msg__u.success
			.rpc_success_u.repl.body
			.rpc_reply_body_u.r0.new_session_key;

		username = (char *) calloc(uname.size(), sizeof(char));
		strcpy(username, &uname[0]);
	}
}

void
handle_logout(CLIENT *clnt)
{
	printf("Executing logout...\n");

	rpc_msg_  *result_2;
	rpc_msg_  logout_1_arg = {
		(err_type) SUCC,
		{ { (msg_type_) REQ, { session_key, { (op_type) LOGOUT, { .r1 = { 0x1 } } } } } }
	};

	result_2 = logout_1(&logout_1_arg, clnt);
	if (result_2 == (rpc_msg_ *) NULL) {
		clnt_perror (clnt, "call failed");
	}

	if (result_2->err)
		printf("failure_message: %s, failure_code: %d\n",
			result_2->rpc_msg__u.failure.failure_message,
			result_2->rpc_msg__u.failure.failure_code);
	else
		printf("success_message: %s\n", 
			result_2->rpc_msg__u.success.rpc_success_u.repl.success_message);
	return;	
}

bool file_is_empty(ifstream& file)
{
    return file.peek() == ifstream::traits_type::eof();
}

void
handle_load(CLIENT *clnt)
{
	printf("Executing load...\n");

	string db_file = "client_db_" + string(username) + ".rpcdb";
	ifstream in(db_file, ios::in | ios::binary);

	if (!in) {
		cout << "[FATAL]: Could not open file " << db_file << "!\n";
		exit(1);
	}

	if (file_is_empty(in)) {
		cout << "File " << db_file << " has been opened and initialized!\n";
		in.close();

		return;
	}

	cout << "Opening file: " << db_file << "\n";

	unsigned int num_sensors;
	unsigned int num_values;
	int sensor_id;
	float *values;
	sensor_data *sdl;

	in.read((char *) &num_sensors, sizeof num_sensors);
	sdl = (sensor_data *) calloc(num_sensors, sizeof(sensor_data));

	printf("----> Number of sensors: %d\n", num_sensors);

	for (int i = 0; i < num_sensors; i++) {
		in.read((char *) &num_values, sizeof num_values);
		printf("Here1!\n");
		in.read((char *) &sensor_id, sizeof sensor_id);
		printf("Here2!\n");
		values = (float *) calloc(num_values, sizeof(float));
		in.read((char *) &values, num_values * sizeof(float));

		printf("\tSensor %d with %d values: (", sensor_id, num_values);

		for (int j = 0; j < num_values; j++) {
			printf("%f%s", values[j], j == num_values - 1 ? ")\n" : ", ");
		}

		sdl[i] = { sensor_id, { num_values, values } };
	}

	printf("\n");
	in.close();

	rpc_msg_  *result_3;
	rpc_msg_  load_1_arg = {
		(err_type) SUCC,
		{ { (msg_type_) REQ, { session_key, { (op_type) LOAD, { .r2 = { num_sensors, sdl } } } } } }
	};

	result_3 = load_1(&load_1_arg, clnt);
	if (result_3 == (rpc_msg_ *) NULL) {
		clnt_perror (clnt, "call failed");
	}

	if (result_3->err)
		printf("failure_message: %s, failure_code: %d\n\n",
			result_3->rpc_msg__u.failure.failure_message,
			result_3->rpc_msg__u.failure.failure_code);
	else
		printf("success_message: %s\n\n", 
			result_3->rpc_msg__u.success.rpc_success_u.repl.success_message);
}

void
handle_store(CLIENT *clnt)
{
	printf("Executing store...\n");

	rpc_msg_  *result_4;
	rpc_msg_  store_1_arg = {
		(err_type) SUCC,
		{ { (msg_type_) REQ, { session_key, { (op_type) STORE, { .r3 = { 0x3 } } } } } }
	};

	result_4 = store_1(&store_1_arg, clnt);
	if (result_4 == (rpc_msg_ *) NULL) {
		clnt_perror (clnt, "call failed");
	}

	if (result_4->err)
		printf("failure_message: %s, failure_code: %d\n\n",
			result_4->rpc_msg__u.failure.failure_message,
			result_4->rpc_msg__u.failure.failure_code);
	else {
		printf("success_message: %s\n", 
			result_4->rpc_msg__u.success.rpc_success_u.repl.success_message);
		
		repl_store rs = result_4->rpc_msg__u.success
			.rpc_success_u.repl.body
			.rpc_reply_body_u.r3;
		
		string db_file = "client_db_" + string(username) + ".rpcdb";
		ofstream out(db_file, ios::out | ios::binary);

		if (!out) {
			cout << "[FATAL]: Could not open file: " << db_file << "!\n";
			exit(1);
		}

		sensor_data *sdl = rs.sdl.sdl_val;
		unsigned int sdl_len = rs.sdl.sdl_len;
		unsigned int sd_len;
		float *sd_vals;
		int sensor_id;

		out.write((char *) &sdl_len, sizeof sdl_len);

		printf("----> The data to be persisted: \n");
		for (int i = 0; i < sdl_len; i++) {
			sensor_id = sdl[i].sensor_id;
			sd_len = sdl[i].values.values_len;
			sd_vals = sdl[i].values.values_val;

			printf("\tSensor %d [%u values]: (", sensor_id, sd_len);

			out.write((char *) &sd_len, sizeof sd_len);
			out.write((char *) &sensor_id, sizeof sensor_id);
			out.write((char *) &sd_vals, sd_len * sizeof(float));

			for (int j = 0; j < sd_len; j++) {
				printf("%f%s", sd_vals[j], j == sd_len - 1 ? ")\n" : ", ");
			}
		}

		printf("\n");
		out.close();
	}
}

void
handle_add(int sensor_id, int data_len, float *data, CLIENT *clnt)
{
	printf("Executing add %d %d [vals]...\n", sensor_id, data_len);

	sensor_data sd = { sensor_id, { (unsigned int) data_len, data } };

	rpc_msg_  *result_5;
	rpc_msg_  add_1_arg = {
		(err_type) SUCC,
		{ { (msg_type_) REQ, { session_key, { (op_type) ADD, { .r4 = { sd } } } } } }
	};

	result_5 = add_1(&add_1_arg, clnt);
	if (result_5 == (rpc_msg_ *) NULL) {
		clnt_perror (clnt, "call failed");
	}

	if (result_5->err)
		printf("failure_message: %s, failure_code: %d\n\n",
			result_5->rpc_msg__u.failure.failure_message,
			result_5->rpc_msg__u.failure.failure_code);
	else
		printf("success_message: %s\n\n", 
			result_5->rpc_msg__u.success.rpc_success_u.repl.success_message);
}

void
handle_del(int sensor_id, CLIENT *clnt)
{
	printf("Executing del %d...\n", sensor_id);

	rpc_msg_  *result_6;
	rpc_msg_  del_1_arg = {
		(err_type) SUCC,
		{ { (msg_type_) REQ, { session_key, { (op_type) DEL, { .r5 = { sensor_id } } } } } }
	};

	result_6 = del_1(&del_1_arg, clnt);
	if (result_6 == (rpc_msg_ *) NULL) {
		clnt_perror (clnt, "call failed");
	}

	if (result_6->err)
		printf("failure_message: %s, failure_code: %d\n\n",
			result_6->rpc_msg__u.failure.failure_message,
			result_6->rpc_msg__u.failure.failure_code);
	else
		printf("success_message: %s\n\n", 
			result_6->rpc_msg__u.success.rpc_success_u.repl.success_message);
}

void
handle_update(int sensor_id, int data_len, float *data, CLIENT *clnt)
{
	printf("Executing update %d %d [vals]...\n", sensor_id, data_len);

	sensor_data sd = { sensor_id, { (unsigned int) data_len, data } };

	rpc_msg_ *result_7;
	rpc_msg_  update_1_arg = {
		(err_type) SUCC,
		{ { (msg_type_) REQ, { session_key, { (op_type) UPDATE, { .r6 = { sd } } } } } }
	};

	result_7 = update_1(&update_1_arg, clnt);
	if (result_7 == (rpc_msg_ *) NULL) {
		clnt_perror (clnt, "call failed");
	}

	if (result_7->err)
		printf("failure_message: %s, failure_code: %d\n\n",
			result_7->rpc_msg__u.failure.failure_message,
			result_7->rpc_msg__u.failure.failure_code);
	else
		printf("success_message: %s\n\n", 
			result_7->rpc_msg__u.success.rpc_success_u.repl.success_message);
}

void
handle_read(int sensor_id, CLIENT *clnt)
{
	printf("Executing read %d...\n", sensor_id);

	rpc_msg_  *result_8;
	rpc_msg_  read_1_arg = {
		(err_type) SUCC,
		{ { (msg_type_) REQ, { session_key, { (op_type) READ, { .r7 = { sensor_id } } } } } }
	};

	result_8 = read_1(&read_1_arg, clnt);
	if (result_8 == (rpc_msg_ *) NULL) {
		clnt_perror (clnt, "call failed");
	}

	if (result_8->err)
		printf("failure_message: %s, failure_code: %d\n\n",
			result_8->rpc_msg__u.failure.failure_message,
			result_8->rpc_msg__u.failure.failure_code);
	else {
		printf("success_message: %s\n", 
			result_8->rpc_msg__u.success.rpc_success_u.repl.success_message);

		sensor_data sd = result_8->rpc_msg__u.success
			.rpc_success_u.repl.body
			.rpc_reply_body_u.r7.sd;

		printf("----> The data received for sensor %d: ", sd.sensor_id);

		int sd_len = sd.values.values_len;
		float *sd_vals = sd.values.values_val;

		for (int i = 0; i < sd_len; i++) {
			printf("%f%s", sd_vals[i], i == sd_len - 1 ? ")\n" : ", ");
		}
		printf("\n");
	}
}

void
handle_get_stat(int sensor_id, stat_type stat, CLIENT *clnt)
{
	printf("Executing get_stat %d...\n", sensor_id);

	rpc_msg_  *result_9;
	rpc_msg_  get_stat_1_arg = {
		(err_type) SUCC,
		{ { (msg_type_) REQ, { session_key, { (op_type) GET_STAT, { .r8 = { sensor_id, stat } } } } } }
	};

	result_9 = get_stat_1(&get_stat_1_arg, clnt);
	if (result_9 == (rpc_msg_ *) NULL) {
		clnt_perror (clnt, "call failed");
	}

	if (result_9->err)
		printf("failure_message: %s, failure_code: %d\n\n",
			result_9->rpc_msg__u.failure.failure_message,
			result_9->rpc_msg__u.failure.failure_code);
	else {
		printf("success_message: %s\n", 
			result_9->rpc_msg__u.success.rpc_success_u.repl.success_message);
		printf("----> The stat %s is %f for sensor %d\n\n",
			stat == MIN ? "MIN" :
			stat == MAX ? "MAX" :
			stat == AVG ? "AVG" :
			stat == MEDIAN ? "MEDIAN" :
			"NONE",
			result_9->rpc_msg__u.success.rpc_success_u.repl.body.rpc_reply_body_u.r8.stat, sensor_id);
	}
}

void
handle_get_stat_all(CLIENT *clnt)
{
	printf("Executing get_stat_all...\n");

	rpc_msg_  *result_10;
	rpc_msg_  get_stat_all_1_arg = {
		(err_type) SUCC,
		{ { (msg_type_) REQ, { session_key, { (op_type) GET_STAT_ALL, { .r9 = { 0x9 } } } } } }
	};

	result_10 = get_stat_all_1(&get_stat_all_1_arg, clnt);
	if (result_10 == (rpc_msg_ *) NULL) {
		clnt_perror (clnt, "call failed");
	}

	if (result_10->err)
		printf("failure_message: %s, failure_code: %d\n\n",
			result_10->rpc_msg__u.failure.failure_message,
			result_10->rpc_msg__u.failure.failure_code);
	else {
		printf("success_message: %s\n", 
			result_10->rpc_msg__u.success.rpc_success_u.repl.success_message);
		
		repl_get_stat_all rall = result_10->rpc_msg__u.success
			.rpc_success_u.repl.body
			.rpc_reply_body_u.r9;

		unsigned int ssl_len = rall.ssl.ssl_len;
		sensor_stats *ssl = rall.ssl.ssl_val;

		printf("----> The received stats for all sensors: \n");
		for (int i = 0; i < ssl_len; i++) {
			printf("\tsensor %d: (min, %f) (max, %f) (avg, %f) (median, %f)\n", 
				ssl[i].sensor_id,
				ssl[i].stats.stats_val[MIN],
				ssl[i].stats.stats_val[MAX],
				ssl[i].stats.stats_val[AVG],
				ssl[i].stats.stats_val[MEDIAN]);
		}

		printf("\n");
	}
}

void
execute_input(string input_file, CLIENT *clnt)
{
	ifstream in(input_file);
	string line, token;

	if (!in) {
		cout << "[FATAL]: Unable to open input file " << input_file << "!\n";
		exit(1);
	}

	while (getline(in, line)) {
		stringstream ss(line);
		vector<string> tokens;

		while (getline(ss, token, ' ')) {
			tokens.push_back(token);
		}

		switch(op_stoi[tokens[TKN_CMD]]) {
			case LOGIN: {
				string username = tokens[TKN_USERNAME];
				handle_login(username, clnt);
				break;
			}

			case LOGOUT: {	
				handle_logout(clnt);
				break;
			}
			
			case LOAD: {
				handle_load(clnt);
				break;
			}

			case STORE: {
				handle_store(clnt);
				break;
			}

			case ADD: {
				int sensor_id = stoi(tokens[TKN_SID]);
				int data_len = stoi(tokens[TKN_SDATA_LEN]);
				float *data = (float *) calloc(data_len, sizeof(float));

				for (int i = 0; i < data_len; i++) {
					data[i] = stof(tokens[TKN_SDATA_VAL + i]);
				}

				handle_add(sensor_id, data_len, data, clnt);
				break;
			}

			case DEL: {
				int sensor_id = stoi(tokens[TKN_SID]);
				handle_del(sensor_id, clnt);
				break;
			}

			case UPDATE: {
				int sensor_id = stoi(tokens[TKN_SID]);
				int data_len = stoi(tokens[TKN_SDATA_LEN]);
				float *data = (float *) calloc(data_len, sizeof(float));

				for (int i = 0; i < data_len; i++) {
					data[i] = stof(tokens[TKN_SDATA_VAL + i]);
				}

				handle_update(sensor_id, data_len, data, clnt);
				break;
			}

			case READ: {
				int sensor_id = stoi(tokens[TKN_SID]);
				handle_read(sensor_id, clnt);
				break;
			}

			case GET_STAT: {
				int sensor_id = stoi(tokens[TKN_SID]);
				stat_type stat = AVG;
				handle_get_stat(sensor_id, stat, clnt);
				break;
			}

			case GET_STAT_ALL: {
				handle_get_stat_all(clnt);
				break;
			}
			default: 
				cout << "[ERROR]: Cannot execute command " << tokens[TKN_CMD] << "!\n";
    	}
	}

	in.close();
}

void
api_prog_1(char *host, char *input_file)
{
	CLIENT *clnt;

	clnt = clnt_create (host, api_prog, api_vers, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}

	execute_input(string(input_file), clnt);
	clnt_destroy (clnt);
}


int
main (int argc, char *argv[])
{
	char *host;
	char *input_file;

	if (argc < 3) {
		printf ("usage: %s server_host input_file\n", argv[0]);
		exit (1);
	}

	host = argv[1];
	input_file = argv[2];

	api_prog_1 (host, input_file);
}