#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/file.h"
#include "include/exit_codes.h"
#include "include/constants.h"

char *get_file_path(const char *type)
{
	// this function will not look like this, just for development :)
	if (!strcmp(type, "alias"))
		return "test.json";
	else if (!strcmp(type, "config"))
		return "spark.conf";
	else
		return "";
}

char **read_file(const char *filename, int *lines_read)
{
	int line_count = 0;
	int array_size = 16;

	char **line_array = malloc(array_size * sizeof(char*));
	if (line_array == NULL)
	{
		fprintf(stderr, "[\033[1;31merror\033[0m] Failed to allocate memory\n");
		exit(SPARK_EXIT_MEMORY_ALLOCATION_ERROR);
	}

	FILE *fptr = fopen(filename, "r");

	if (fptr == NULL)
	{
		fprintf(stderr, "[\033[1;31merror\033[0m] File %s was not found\n", filename);
		exit(SPARK_EXIT_FILE_NOT_FOUND);
	}

	char line[MAX_FILE_LINE_LENGTH];

	while (fgets(line, MAX_FILE_LINE_LENGTH, fptr) != NULL)
	{
		line_array[line_count] = malloc(MAX_FILE_LINE_LENGTH * sizeof(char));
		strncpy(line_array[line_count], line, MAX_FILE_LINE_LENGTH);
		line_count++;

		if (line_count >= array_size)
		{
			array_size *= 2;
			char **temp = realloc(line_array, array_size * sizeof(char*));

			if (temp == NULL)
			{
				fprintf(stderr, "[\033[1;31merror\033[0m] Failed to reallocate memory\n");
				exit(SPARK_EXIT_MEMORY_ALLOCATION_ERROR);
			}

			line_array = temp;
		}
	}

	fclose(fptr);

	*lines_read = line_count;
	return line_array;
}

int write_file(const char *filename, const char *content, const char* mode)
{
	FILE *fptr = fopen(filename, mode);

	fprintf(fptr, content);

	fclose(fptr);

	return SPARK_EXIT_SUCCESS;
}