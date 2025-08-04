#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

	for (unsigned int i = 0; i < lines; i++)
	{
		free(src[i]);
	}

	return json;
}

static int count_json_objects(const char *json_str)
{
	int length = 0;
	struct json json = json_parse(json_str);
	struct json child = json_first(json);

	while (json_exists(child))
	{
		char path[32] = "";
		sprintf(
			path,
			"%d.name",
			length
		);
		struct json name_temp = json_get(json_str, path);

		if (!json_exists(name_temp)) break;

		length++;
		child = json_next(child);
	}

	return length;
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
			"{\n\t\"%d\": {\n\t\t\"name\": \"%s\",\n\t\t\"command\": \"%s\"\n\t}\n}",
			0,
			name,
			command
		);
		write_file(alias_file_path, json_str_single, "w");

		free(alias_file_path);

		printf(
			"Added command '%s' to alias list as '%s'\n",
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
		exit(SPARK_EXIT_GENERAL_ERROR);
	}

	char *json_str = fold_json(file_buffer, line_count);

	int json_objects_length = count_json_objects(json_str);

	unsigned long json_str_length = strlen(json_str);

	char *end = json_str + json_str_length - 1;

	while (end > json_str && (*end == '\n' || *end == '\r' || *end == ' ' || *end == '\t'))
	{
		*--end = '\0';
	}

	if (*end != '}')
	{
		fprintf(
			stderr,
			"[\033[1;31merror\033[0m] Invalid JSON structure: missing closing brace\n"
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
		"%s\t\"%d\": {\n\t\t\"name\": \"%s\",\n\t\t\"command\": \"%s\"\n\t}\n}",
		json_str,
		json_objects_length,
		name,
		command
	);

	write_file(alias_file_path, temp_buf, "w");

	free(file_buffer);
	free(json_str);
	free(alias_file_path);

	printf(
		"Added command '%s' to alias list as '%s'\n",
		command,
		name
	);

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
		exit(SPARK_EXIT_GENERAL_ERROR);
	}

	char *json_str = fold_json(file_buffer, line_count);

	int json_objects_length = count_json_objects(json_str);

	for (int i = 0; i < json_objects_length; i++)
	{
		char name_path[32] = "";
		char command_path[32] = "";

		sprintf(
			name_path,
			"%d.name",
			i
		);

		sprintf(
			command_path,
			"%d.command",
			i
		);

		struct json name = json_get(json_str, name_path);
		struct json command = json_get(json_str, command_path);

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
			"%s%s%s\n",
			alias_name,
			spaces,
			alias_command
		);
	}

	free(file_buffer);
	free(json_str);
	free(alias_file_path);

	return SPARK_EXIT_SUCCESS;
}
