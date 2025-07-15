#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/operations.h"
#include "include/file.h"
#include "include/json.h"
#include "include/exit_codes.h"
#include "include/constants.h"

int create_alias(const char *name, const char *command)
{
	printf("alias to add: '%s' with command '%s'\n", name, command);

	if (strlen(name) > MAX_ALIAS_NAME_LENGTH)
	{
		fprintf(stderr, "[\033[1;31merror\033[0m] Parameter <name> is too long! Make sure your alias name does not exceed %d characters\n", MAX_ALIAS_NAME_LENGTH);
		exit(SPARK_EXIT_GENERAL_ERROR);
	}

	FILE *fptr = fopen(get_file_path("alias"), "a");

	int line_count = 0;
	// int last_alias_id = 0;

	char **file_buffer = read_file(get_file_path("alias"), &line_count);

	if (file_buffer == NULL)
	{
		fprintf(stderr, "[\033[1;31merror\033[0m] Failed to read file\n");
		exit(SPARK_EXIT_GENERAL_ERROR);
	}
	
	for (int i = 0; i < line_count; i++)
	{
		printf("%s", file_buffer[i]);
	}

	free(file_buffer);

	fclose(fptr);

	return SPARK_EXIT_SUCCESS;
}

int remove_alias(const char *name)
{
	printf("alias to remove: '%s'\n", name);
	return SPARK_EXIT_SUCCESS;
}

int list_aliases()
{
	printf("\033[1mName\t\tCommand\033[0m\n");

	// int space_amount = 16;

	FILE *fptr = fopen(get_file_path("alias"), "r");

	if (fptr == NULL)
	{
		fprintf(stderr, "[\033[1;31merror\033[0m] File %s was not found\n", get_file_path("alias"));
		exit(SPARK_EXIT_FILE_NOT_FOUND);
	}
	
	fclose(fptr);

	return SPARK_EXIT_SUCCESS;
}