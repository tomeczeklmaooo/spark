#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "include/operations.h"
#include "include/file.h"
#include "include/json.h"
#include "include/exit_codes.h"
#include "include/constants.h"

static char *fold_json(char **src, unsigned long lines)
{
	size_t total_size = 0;

	for (unsigned int i = 0; i < lines; i++)
	{
		total_size += strlen(src[i]);
	}

	char *json = malloc(total_size * sizeof(char) + 1);

	if (json == NULL)
	{
		fprintf(
			stderr,
			"[\033[1;31merror\033[0m] Failed to allocate memory\n"
		);
		exit(SPARK_EXIT_MEMORY_ALLOCATION_ERROR);
	}

	json[0] = '\0';

	for (unsigned int i = 0; i < lines; i++)
	{
		strcat(json, src[i]);
	}

	return json;
}

int execute_alias(const char *name)
{
	printf("[\033[1;34minfo\033[0m] Attempting to execute alias '%s'...\n", name);

	char *alias_file_path = get_file_path("alias");

	unsigned long line_count = 0;

	if (!file_exists(alias_file_path))
	{
		printf("[\033[1;34minfo\033[0m] There is nothing to execute (file '%s' does not exist).\n", alias_file_path);
		return SPARK_EXIT_SUCCESS;
	}

	char **file_buffer = read_file(alias_file_path, &line_count);

	if (file_buffer == NULL)
	{
		fprintf(
			stderr,
			"[\033[1;31merror\033[0m] Failed to read file\n"
		);
		exit(SPARK_EXIT_FILE_READ_FAILURE);
	}

	char *json_str = fold_json(file_buffer, line_count);
	struct json json = json_parse(json_str);

	size_t json_objects_length = json_array_count(json);
	size_t alias_exit_code = 0;

	char command_buf[MAX_ALIAS_COMMAND_LENGTH];

	for (size_t i = 0; i < json_objects_length; i++)
	{
		struct json json_obj = json_array_get(json, i);
		struct json json_name = json_object_get(json_obj, "name");
		struct json json_command = json_object_get(json_obj, "command");

		size_t len = json_string_copy(json_command, command_buf, sizeof(command_buf));
		if (len > sizeof(command_buf) - 1)
		{
			fprintf(
				stderr,
				"[\033[1;31merror\033[0m] Failed to copy json string to C string buffer\n"
			);
			exit(SPARK_EXIT_GENERAL_ERROR);
		}

		if (!json_string_compare(json_name, name)) break;
		command_buf[0] = '\0';
	}

	if (strlen(command_buf) == 0)
	{
		fprintf(
			stderr,
			"[\033[1;31merror\033[0m] There is no alias with name '%s'. Did you spell it correctly?\n",
			name
		);
		exit(SPARK_EXIT_GENERAL_ERROR);
	}

	char *args[] = {"/bin/sh", "-c", command_buf, NULL};
	execv(args[0], args);
	
	// leaving the rest here for now, clang sanitizer does not seem to care about it though
	for (size_t i = 0; i < line_count; i++)
	{
		free(file_buffer[i]);
	}
	free(file_buffer);
	free(json_str);
	free(alias_file_path);

	printf(
		"[\033[1;32msuccess\033[0m] Executed alias '%s' (exited with code %zu)\n",
		name,
		alias_exit_code
	);

	return SPARK_EXIT_SUCCESS;
}

int create_alias(const char *name, const char *command)
{
	char *alias_file_path = get_file_path("alias");

	if (strlen(name) > MAX_ALIAS_NAME_LENGTH)
	{
		fprintf(
			stderr,
			"[\033[1;31merror\033[0m] Parameter <name> is too long! Make sure your alias name does not exceed %d characters\n",
			MAX_ALIAS_NAME_LENGTH
		);
		exit(SPARK_EXIT_GENERAL_ERROR);
	}
	else if (strlen(command) > MAX_ALIAS_COMMAND_LENGTH)
	{
		fprintf(
			stderr,
			"[\033[1;31merror\033[0m] Parameter <command> is too long! Make sure your alias command does not exceed %d characters\n",
			MAX_ALIAS_COMMAND_LENGTH
		);
		exit(SPARK_EXIT_GENERAL_ERROR);
	}
	else if (strlen(name) > MAX_ALIAS_NAME_LENGTH && strlen(command) > MAX_ALIAS_COMMAND_LENGTH)
	{
		fprintf(
			stderr,
			"[\033[1;31merror\033[0m] Parameters <name> and <command> are too long! Make sure the parameters do not exceed %d and %d characters respectively\n",
			MAX_ALIAS_NAME_LENGTH,
			MAX_ALIAS_COMMAND_LENGTH
		);
		exit(SPARK_EXIT_GENERAL_ERROR);
	}

	unsigned long line_count = 0;

	if (!file_exists(alias_file_path))
	{
		char json_str_single[16384 * 16]; // arbitrary size because idk
		sprintf(
			json_str_single,
			"[\n\t{ \"name\": \"%s\", \"command\": \"%s\" }\n]",
			name,
			command
		);
		write_file(alias_file_path, json_str_single, "w");

		free(alias_file_path);

		printf(
			"[\033[1;32msuccess\033[0m] Added command '%s' to alias list as '%s'\n",
			command,
			name
		);

		return SPARK_EXIT_SUCCESS;
	}

	char **file_buffer = read_file(alias_file_path, &line_count);

	if (file_buffer == NULL)
	{
		fprintf(
			stderr,
			"[\033[1;31merror\033[0m] Failed to read file\n"
		);
		exit(SPARK_EXIT_FILE_READ_FAILURE);
	}

	char *json_str = fold_json(file_buffer, line_count);

	unsigned long json_str_length = strlen(json_str);

	char *end = json_str + json_str_length - 1;

	while (end > json_str && (*end == '\n' || *end == '\r' || *end == ' ' || *end == '\t'))
	{
		*--end = '\0';
	}

	if (*end != ']')
	{
		fprintf(
			stderr,
			"[\033[1;31merror\033[0m] Invalid JSON structure: missing closing square bracket\n"
		);
		exit(SPARK_EXIT_GENERAL_ERROR);
	}
	end--;
	*end = '\0';

	strcat(json_str, ",\n");

	char temp_buf[16384 * 16]; // arbitrary size because idk
	snprintf(
		temp_buf,
		sizeof(temp_buf),
		"%s\t{ \"name\": \"%s\", \"command\": \"%s\" }\n]",
		json_str,
		name,
		command
	);

	write_file(alias_file_path, temp_buf, "w");

	for (size_t i = 0; i < line_count; i++)
	{
		free(file_buffer[i]);
	}
	free(file_buffer);
	free(json_str);
	free(alias_file_path);

	printf(
		"[\033[1;32msuccess\033[0m] Added command '%s' to alias list as '%s'\n",
		command,
		name
	);

	return SPARK_EXIT_SUCCESS;
}

int remove_alias(const char *name)
{
	char *alias_file_path = get_file_path("alias");

	unsigned long line_count = 0;

	if (!file_exists(alias_file_path))
	{
		printf("[\033[1;34minfo\033[0m] There is nothing to remove.\n");
		return SPARK_EXIT_SUCCESS;
	}

	char **file_buffer = read_file(alias_file_path, &line_count);

	if (file_buffer == NULL)
	{
		fprintf(
			stderr,
			"[\033[1;31merror\033[0m] Failed to read file\n"
		);
		exit(SPARK_EXIT_FILE_READ_FAILURE);
	}

	char *json_str = fold_json(file_buffer, line_count);
	struct json json = json_parse(json_str);

	size_t json_objects_length = json_array_count(json);
	size_t alias_index = 0;

	for (size_t i = 0; i < json_objects_length; i++)
	{
		struct json json_obj = json_array_get(json, i);
		struct json json_name = json_object_get(json_obj, "name");

		if (json_string_compare(json_name, name)) continue;
		alias_index = i + 1; // add 1 because line at i = 0 is a singluar opening square bracket
		break;
	}

	size_t total_size = 0;

	for (unsigned int i = 0; i < line_count; i++)
	{
		total_size += strlen(file_buffer[i]);
	}

	char *buffer = malloc(total_size * sizeof(char) + 1);

	if (buffer == NULL)
	{
		fprintf(
			stderr,
			"[\033[1;31merror\033[0m] Failed to allocate memory\n"
		);
		exit(SPARK_EXIT_MEMORY_ALLOCATION_ERROR);
	}

	buffer[0] = '\0';
	bool is_last_alias = false;

	for (unsigned int i = 0; i < line_count; i++)
	{
		if (alias_index == line_count - 2 && i == line_count - 2) is_last_alias = true;
		if (i == alias_index) continue;
		strcat(buffer, file_buffer[i]);
	}

	if (is_last_alias)
	{
		buffer[strlen(buffer) - 3] = '\n';
		buffer[strlen(buffer) - 2] = ']';
		buffer[strlen(buffer) - 1] = '\0';
	}

	if (json_objects_length != 1)
	{
		write_file(alias_file_path, buffer, "w");
		goto success;
	}
	else
	{
		if (remove(alias_file_path) == 0) goto success;
	}

success:
	for (size_t i = 0; i < line_count; i++)
	{
		free(file_buffer[i]);
	}
	free(buffer);
	free(file_buffer);
	free(json_str);
	free(alias_file_path);

	printf(
		"[\033[1;32msuccess\033[0m] Removed alias '%s' from alias list\n",
		name
	);

	return SPARK_EXIT_SUCCESS;
}

int list_aliases()
{
	printf("\033[1mID\tName\t\tCommand\033[0m\n");

	char *alias_file_path = get_file_path("alias");
	unsigned long space_amount = 16;

	FILE *fptr = fopen(alias_file_path, "r");

	if (fptr == NULL)
	{
		fprintf(
			stderr,
			"[\033[1;31merror\033[0m] File %s was not found\n",
			alias_file_path
		);
		exit(SPARK_EXIT_FILE_NOT_FOUND);
	}

	fclose(fptr);

	unsigned long line_count = 0;

	char **file_buffer = read_file(alias_file_path, &line_count);
	
	if (file_buffer == NULL)
	{
		fprintf(
			stderr,
			"[\033[1;31merror\033[0m] Failed to read file\n"
		);
		exit(SPARK_EXIT_FILE_READ_FAILURE);
	}

	char *json_str = fold_json(file_buffer, line_count);
	struct json json = json_parse(json_str);

	size_t json_objects_length = json_array_count(json);

	for (size_t i = 0; i < json_objects_length; i++)
	{
		struct json json_obj = json_array_get(json, i);
		struct json name = json_object_get(json_obj, "name");
		struct json command = json_object_get(json_obj, "command");

		if (!json_exists(name) || !json_exists(command))
			continue;
		
		char alias_name[json_string_length(name) + 1];
		json_string_copy(name, alias_name, sizeof(alias_name));
		
		char alias_command[json_string_length(command) + 1];
		json_string_copy(command, alias_command, sizeof(alias_command));

		unsigned long alias_name_length = strlen(alias_name);
		size_t space_padding_amount = space_amount > alias_name_length ? space_amount - alias_name_length : 1;

		char spaces[space_padding_amount + 1];
		memset(spaces, ' ', space_padding_amount);
		spaces[space_padding_amount] = '\0';

		printf(
			"%zu\t%s%s%s\n",
			i,
			alias_name,
			spaces,
			alias_command
		);
	}

	for (size_t i = 0; i < line_count; i++)
	{
		free(file_buffer[i]);
	}
	free(file_buffer);
	free(json_str);
	free(alias_file_path);

	return SPARK_EXIT_SUCCESS;
}
