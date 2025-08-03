#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "include/file.h"
#include "include/exit_codes.h"
#include "include/constants.h"

char *get_file_path(const char *type)
{
	uid_t uid = getuid();
	struct passwd *pw = getpwuid(uid);

	if (pw == NULL)
	{
		fprintf(
			stderr,
			"[\033[1;31merror\033[0m] Failed to fetch home directory\n"
		);
		exit(SPARK_EXIT_GENERAL_ERROR);
	}

	char *filename = malloc(256 * sizeof(char)); // arbitrary size for development purposes, will be dynamic later
	
	if (filename == NULL)
	{
		fprintf(
			stderr,
			"[\033[1;31merror\033[0m] Failed to allocate memory\n"
		);
		exit(SPARK_EXIT_MEMORY_ALLOCATION_ERROR);
	}

	if (!strcmp(type, "alias"))
		sprintf(
			filename,
			"%s/.config/spark/aliases.json",
			pw->pw_dir
		);
	else if (!strcmp(type, "config"))
		sprintf(
			filename,
			"%s/.config/spark/spark.conf",
			pw->pw_dir
		);
	
	return filename;
}

bool file_exists(const char *filename)
{
	FILE *fptr = fopen(filename, "r");
	bool file_exists = false;
	
	if (fptr != NULL)
	{
		fclose(fptr);
		file_exists = true;
	}

	return file_exists;
}

char **read_file(const char *filename, int *lines_read)
{
	int line_count = 0;
	int array_size = 16;

	char **line_array = malloc(array_size * sizeof(char*));
	if (line_array == NULL)
	{
		fprintf(
			stderr,
			"[\033[1;31merror\033[0m] Failed to allocate memory\n"
		);
		exit(SPARK_EXIT_MEMORY_ALLOCATION_ERROR);
	}

	FILE *fptr = fopen(filename, "r");

	if (fptr == NULL)
	{
		fprintf(
			stderr,
			"[\033[1;31merror\033[0m] File %s was not found\n",
			filename
		);
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
				fprintf(
					stderr,
					"[\033[1;31merror\033[0m] Failed to reallocate memory\n"
				);
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
	uid_t uid = getuid();
	struct passwd *pw = getpwuid(uid);

	struct stat st;
	char path[256];

	snprintf(
		path,
		sizeof(path),
		"%s/.config/spark",
		pw->pw_dir
	);

	if (stat(path, &st) == -1)
	{
		if (mkdir(path, 0755) == -1)
		{
			fprintf(
				stderr,
				"[\033[1;31merror\033[0m] Failed to create directory '%s'\n",
				path
			);
			exit(SPARK_EXIT_GENERAL_ERROR);
		}
	}

	FILE *fptr = fopen(filename, mode);

	fprintf(fptr, content);

	fclose(fptr);

	return SPARK_EXIT_SUCCESS;
}