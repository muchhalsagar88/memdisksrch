#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include <sys/stat.h>
#include <sys/time.h>

InputArgs input_params;

off_t file_size(const char *filename) {
    struct stat st;

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1;
}

Array read_file_in_memory(const char* file_name)
{
	off_t size_of_seeds_file = file_size(file_name);
	int *seeds = (int *)malloc(sizeof(int) * (size_of_seeds_file/4));
	FILE* seek_file = fopen(file_name, "rb");

	int i = 0, loop_count = size_of_seeds_file/4;
	for (; i < loop_count; ++i)
	{
		fread(&seeds[i], sizeof(int), 1, seek_file);
		fseek(seek_file, (i+1) * sizeof(int), SEEK_SET);
	}
	fclose(seek_file);

	Array array = (Array) malloc(sizeof(struct array_data_t));
	array -> array = seeds;
	array -> size = (size_of_seeds_file/4);

	return array;
}

void calculate_time_diff(struct timeval time_after, struct timeval time_bef, struct timeval* time_total)
{
	struct timeval time_diff;
	timersub(&time_after, &time_bef, &time_diff);
	timeradd(&time_diff, time_total, time_total);
}

void print_results(Array seeds, int* hits, struct timeval time_total)
{
	int i = 0;

	for(; i < seeds->size; ++i)
	{
		if(hits[i] == 1)
			printf("%12d: Yes\n", seeds->array[i]);
		else
			printf("%12d: No\n", seeds->array[i]);
	}
	printf("Time: %ld.%06ld\n", time_total.tv_sec, time_total.tv_usec);
}

int bin_search(int* keys, int seed, int start_position, int end_position)
{
	int srch_position = (start_position + end_position)/2;

	if(keys[srch_position] == seed)
		return 1;

	if(start_position >= end_position)
		return 0;

	if(seed < keys[srch_position])
		bin_search(keys, seed, 0, srch_position - 1);
	else
		bin_search(keys, seed, srch_position+1, end_position);
}

int file_bin_search(FILE* keys_file_stream, int seed, int start_position, int end_position)
{
	int srch_position = (start_position + end_position)/2;

	int current_key = 0;
	fseek(keys_file_stream, srch_position * sizeof(int), SEEK_SET);
	fread(&current_key, sizeof(int), 1, keys_file_stream);

	if(current_key == seed)
		return 1;

	if(start_position >= end_position)
		return 0;

	if(seed < current_key)
		file_bin_search(keys_file_stream, seed, 0, srch_position - 1);
	else
		file_bin_search(keys_file_stream, seed, srch_position+1, end_position);
}

void in_memory_sequential_search(InputArgs input_params)
{
	struct timeval tm_before, tm_after, tm_total;
	tm_total.tv_sec = 0;tm_total.tv_usec=0;
	int seeds_loop_count = 0, keys_loop_count = 0;

	Array seeds_array = read_file_in_memory(input_params->seed_file_name);

	gettimeofday(&tm_before, NULL);
	Array keys_array = read_file_in_memory(input_params->key_file_name);
	gettimeofday(&tm_after, NULL);
	calculate_time_diff(tm_after, tm_before, &tm_total);

	int *hits = (int *)malloc(sizeof(int) * seeds_array->size);

	gettimeofday(&tm_before, NULL);
	for(; seeds_loop_count < seeds_array->size; ++seeds_loop_count)
	{
		keys_loop_count = 0;
		hits[seeds_loop_count] = 0;
		for(; keys_loop_count < keys_array->size; ++keys_loop_count)
		{
			if(keys_array->array[keys_loop_count] == seeds_array->array[seeds_loop_count])
			{
				hits[seeds_loop_count] = 1;
				break;
			}
		}
	}
	gettimeofday(&tm_after, NULL);
	calculate_time_diff(tm_after, tm_before, &tm_total);
	free(keys_array->array);free(keys_array);

	print_results(seeds_array, hits, tm_total);
	free(seeds_array->array);
}

void in_memory_binary_search(InputArgs input_params)
{
	struct timeval tm_before, tm_after, tm_total;
	tm_total.tv_sec = 0;tm_total.tv_usec=0;
	int seeds_loop_count = 0, keys_loop_count = 0;

	Array seeds_array = read_file_in_memory(input_params->seed_file_name);

	gettimeofday(&tm_before, NULL);
	Array keys_array = read_file_in_memory(input_params->key_file_name);
	gettimeofday(&tm_after, NULL);
	calculate_time_diff(tm_after, tm_before, &tm_total);

	int *hits = (int *)malloc(sizeof(int) * seeds_array->size);

	gettimeofday(&tm_before, NULL);
	for(; seeds_loop_count < seeds_array->size; ++seeds_loop_count)
	{
		int res = bin_search(keys_array->array, seeds_array->array[seeds_loop_count], 0, (keys_array->size)-1);
		(res == 1)?(hits[seeds_loop_count] = 1):(hits[seeds_loop_count] = 0);

	}
	gettimeofday(&tm_after, NULL);
	calculate_time_diff(tm_after, tm_before, &tm_total);

	print_results(seeds_array, hits, tm_total);
}

void on_disk_sequential_search(InputArgs input_params)
{
	struct timeval tm_before, tm_after, tm_total;
	tm_total.tv_sec = 0;tm_total.tv_usec=0;
	int seeds_loop_count = 0, keys_loop_count = 0;

	Array seeds_array = read_file_in_memory(input_params->seed_file_name);

	int *hits = (int *)malloc(sizeof(int) * seeds_array->size);

	off_t sizeof_keys_file = file_size(input_params->key_file_name);
	FILE* seek_file = fopen(input_params->key_file_name, "rb");

	for(; seeds_loop_count < seeds_array->size; ++seeds_loop_count)
	{
		keys_loop_count = 0;
		hits[seeds_loop_count] = 0;

		int i = 0, loop_count = sizeof_keys_file/4;
		int current_key = 0;

		gettimeofday(&tm_before, NULL);
		for (; i < loop_count; ++i)
		{
			fread(&current_key, sizeof(int), 1, seek_file);
			if(current_key == seeds_array->array[seeds_loop_count])
			{
				hits[seeds_loop_count] = 1;
				break;
			}
			fseek(seek_file, (i+1) * sizeof(int), SEEK_SET);
		}
		fseek(seek_file, 0, SEEK_SET);
		gettimeofday(&tm_after, NULL);
		calculate_time_diff(tm_after, tm_before, &tm_total);
	}

	fclose(seek_file);
	print_results(seeds_array, hits, tm_total);
}

void on_disk_binary_search(InputArgs input_params)
{
	struct timeval tm_before, tm_after, tm_total;
	tm_total.tv_sec = 0;tm_total.tv_usec=0;
	int seeds_loop_count = 0, keys_loop_count = 0;

	Array seeds_array = read_file_in_memory(input_params->seed_file_name);

	int *hits = (int *)malloc(sizeof(int) * seeds_array->size);

	off_t sizeof_keys_file = file_size(input_params->key_file_name);
	for(; seeds_loop_count < seeds_array->size; ++seeds_loop_count)
	{
		keys_loop_count = 0;
		hits[seeds_loop_count] = 0;

		FILE* seek_file = fopen(input_params->key_file_name, "rb");

		int i = 0, loop_count = sizeof_keys_file/4;
		int current_key = 0;

		gettimeofday(&tm_before, NULL);
		int res = file_bin_search(seek_file, seeds_array->array[seeds_loop_count], 0, loop_count-1);
		gettimeofday(&tm_after, NULL);

		(res == 1)?(hits[seeds_loop_count] = 1):(hits[seeds_loop_count] = 0);
		calculate_time_diff(tm_after, tm_before, &tm_total);

		fclose(seek_file);
	}
	print_results(seeds_array, hits, tm_total);
}

void select_search(InputArgs input_params)
{
	char* search_method = input_params->search_method;

	if(strcmp(search_method, "--mem-seq") == 0)
		in_memory_sequential_search(input_params);
	else if(strcmp(search_method, "--mem-bin") == 0)
		in_memory_binary_search(input_params);
	else if(strcmp(search_method, "--sec-seq") == 0)
		on_disk_sequential_search(input_params);
	else if(strcmp(search_method, "--sec-bin") == 0)
		on_disk_binary_search(input_params);
	else
		printf("Unrecognized search method. Enter:\n"
			"--mem-seq for In memory Linear search\n"
			"--mem-bin for In memory Binary search\n"
			"--sec-seq for Disk-based Linear search\n"
			"--sec-bin for Disk-based Binary search\n");
}

int main(int argc, char *argv[])
{
	input_params = process_input_parameters(argc,argv);
	select_search(input_params);
	exit(1);
}