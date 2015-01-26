struct input_args_t {
	char* search_method;
	char* key_file_name;
	char* seed_file_name;
};
typedef struct input_args_t* InputArgs;

struct array_data_t {
	int * array;
	int size;
};
typedef struct array_data_t* Array;

InputArgs process_input_parameters(int argc, char *argv[])
{
	if(argc != 4) {
		fprintf(stderr, "Usage: %s <search_mode> <name_of_key_file> <name_of_seek_file>\n", argv[0]);
		exit(1);
	}

	InputArgs input_args = malloc(sizeof(struct input_args_t));
	input_args -> search_method = strdup(argv[1]);
	input_args -> key_file_name = strdup(argv[2]);
	input_args -> seed_file_name = strdup(argv[3]);

	FILE* seek_file = fopen(input_args -> key_file_name, "rb");
	if(seek_file == NULL) {
		printf("%s does not exist.\n", input_args->key_file_name);
		exit(-1);
	}

	seek_file = fopen(input_args->seed_file_name, "rb");
	if(seek_file == NULL) {
		printf("%s does not exist.\n", input_args->seed_file_name);
		exit(-1);
	}

	return input_args;
}