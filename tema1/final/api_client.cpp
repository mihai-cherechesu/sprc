#include "api.hpp"

void
api_prog_1(char *host)
{
	CLIENT *clnt;
	unsigned long session_key = 0UL;
	char *username;

	clnt = clnt_create (host, api_prog, api_vers, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}

	rpc_msg_ *result_1;
	rpc_msg_  login_1_arg = {
		(err_type) SUCC,
		{ { (msg_type_) REQ, { 0UL, { (op_type) LOGIN, { .r0 = { (char*) "client_1" } } } } } }
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
	}

	rpc_msg_  *result_1_1;
	rpc_msg_  login_2_arg = {
		(err_type) SUCC,
		{ { (msg_type_) REQ, { 0UL, { (op_type) LOGIN, { .r0 = { (char*) "client_1" } } } } } }
	};

	result_1_1 = login_1(&login_2_arg, clnt);
	if (result_1_1 == (rpc_msg_ *) NULL) {
		clnt_perror (clnt, "call failed");
	}

	if (result_1_1->err)
		printf("failure_message: %s, failure_code: %d\n\n",
			result_1_1->rpc_msg__u.failure.failure_message,
			result_1_1->rpc_msg__u.failure.failure_code);
	else
		printf("success_message: %s\n\n", 
			result_1_1->rpc_msg__u.success.rpc_success_u.repl.success_message);

	///////////////////////////////////////////////////////////////////////////

	rpc_msg_  *result_2;
	rpc_msg_  logout_1_arg = {
		(err_type) SUCC,
		{ { (msg_type_) REQ, { session_key, { (op_type) LOGOUT, { .r1 = { 0x1 } } } } } }
	};

	// result_2 = logout_1(&logout_1_arg, clnt);
	// if (result_2 == (rpc_msg_ *) NULL) {
	// 	clnt_perror (clnt, "call failed");
	// }

	// if (result_2->err)
	// 	printf("failure_message: %s, failure_code: %d\n",
	// 		result_2->rpc_msg__u.failure.failure_message,
	// 		result_2->rpc_msg__u.failure.failure_code);
	// else
	// 	printf("success_message: %s\n", 
	// 		result_2->rpc_msg__u.success.rpc_success_u.repl.success_message);

	///////////////////////////////////////////////////////////////////////////

	// string db_file = "client_db_" + string(username) + ".rpcdb";
	// ifstream in(db_file, ios::in | ios::binary);

	// unsigned int num_sensors;
	// unsigned int num_values;
	// int sensor_id;
	// float *values;
	// sensor_data *sdl;

	// in.read((char *) &num_sensors, sizeof num_sensors);
	// sdl = (sensor_data *) calloc(num_sensors, sizeof(sensor_data));

	// for (int i = 0; i < num_sensors; i++) {
	// 	in.read((char *) &num_values, sizeof num_values);
	// 	in.read((char *) &sensor_id, sizeof sensor_id);

	// 	values = (float *) calloc(num_values, sizeof(float));
	// 	in.read((char *) &values, sizeof values);

	// 	printf("Sensor %d with %d values: ", sensor_id, num_values);
	// 	for (int j = 0; j < num_values; j++) {
	// 		printf("%f ", values[j]);
	// 	}
	// 	printf("\n");

	// 	sdl[i] = { sensor_id, { num_values, values } };
	// }

	unsigned int num_sensors = 2;
	float float_arr_1[] = {1.1, 1.1, 1.1};
	float float_arr_2[] = {1.2, 1.2, 1.2};

	sensor_data s1 = { 1, { 3, float_arr_1 } };
	sensor_data s2 = { 2, { 3, float_arr_2 } };
	sensor_data sdl[] = {s1, s2};

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

	///////////////////////////////////////////////////////////////////////////

	float float_arr_3[] = {1.25, 7.69};
	sensor_data s3 = { 1, { 2, float_arr_3 } };

	rpc_msg_  *result_5;
	rpc_msg_  add_1_arg = {
		(err_type) SUCC,
		{ { (msg_type_) REQ, { session_key, { (op_type) ADD, { .r4 = { s3 } } } } } }
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

	sensor_data s4 = { 1111, { 2, float_arr_3 } };

	add_1_arg = {
		(err_type) SUCC,
		{ { (msg_type_) REQ, { session_key, { (op_type) ADD, { .r4 = { s4 } } } } } }
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

	///////////////////////////////////////////////////////////////////////////
	float float_arr_4[] = {0.1, 1.5, 3.555, 9.13123};
	sensor_data s5 = { 1, { 4, float_arr_4 } };

	rpc_msg_ *result_7;
	rpc_msg_  update_1_arg = {
		(err_type) SUCC,
		{ { (msg_type_) REQ, { session_key, { (op_type) UPDATE, { .r6 = { s5 } } } } } }
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

	///////////////////////////////////////////////////////////////////////////

	rpc_msg_  *result_6;
	rpc_msg_  del_1_arg = {
		(err_type) SUCC,
		{ { (msg_type_) REQ, { session_key, { (op_type) DEL, { .r5 = { 1 } } } } } }
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

	///////////////////////////////////////////////////////////////////////////

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
		
		string db_file = "client_db_" + string("client_1") + ".rpcdb";
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

		ifstream in(db_file, ios::in | ios::binary);

		unsigned int num_sensors;
		unsigned int num_values;
		float *values;

		in.read((char *) &num_sensors, sizeof num_sensors);
		sdl = (sensor_data *) calloc(num_sensors, sizeof(sensor_data));

		printf("----> Number of sensors: %d\n", num_sensors);

		for (int i = 0; i < num_sensors; i++) {
			in.read((char *) &num_values, sizeof num_values);
			in.read((char *) &sensor_id, sizeof sensor_id);

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
	}
	///////////////////////////////////////////////////////////////////////////

	rpc_msg_  *result_8;
	rpc_msg_  read_1_arg = {
		(err_type) SUCC,
		{ { (msg_type_) REQ, { session_key, { (op_type) READ, { .r7 = { 2 } } } } } }
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

	///////////////////////////////////////////////////////////////////////////
	stat_type stat = AVG;
	rpc_msg_  *result_9;
	rpc_msg_  get_stat_1_arg = {
		(err_type) SUCC,
		{ { (msg_type_) REQ, { session_key, { (op_type) GET_STAT, { .r8 = { 1111, stat } } } } } }
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
			result_9->rpc_msg__u.success.rpc_success_u.repl.body.rpc_reply_body_u.r8.stat, 2);
	}
	///////////////////////////////////////////////////////////////////////////
	
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
	///////////////////////////////////////////////////////////////////////////

	exit(0);

	clnt_destroy (clnt);
}


int
main (int argc, char *argv[])
{
	char *host;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}

	host = argv[1];
	api_prog_1 (host);
}