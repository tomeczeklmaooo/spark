#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "include/operations.h"
#include "include/file.h"
#include "include/json.h"
#include "include/exit_codes.h"
#include "include/constants.h"

static char *fold_json(char **src, int lines)
{
	size_t total_size = 0;

	for (int i = 0; i < lines; i++)
	{
		total_size += strlen(src[i]);
	}

	char *json = malloc(total_size * sizeof(char) + 1);

	if (json == NULL)
	{
		fprintf(stderr, "[\033[1;31merror\033[0m] Failed to allocate memory\n");
		exit(SPARK_EXIT_MEMORY_ALLOCATION_ERROR);
	}

	json[0] = '\0';

	for (int i = 0; i < lines; i++)
	{
		strcat(json, src[i]);
	}

	return json;
}

int create_alias(const char *name, const char *command)
{
	printf("alias to add: '%s' with command '%s'\n", name, command);

	if (strlen(name) > MAX_ALIAS_NAME_LENGTH)
	{
		fprintf(stderr, "[\033[1;31merror\033[0m] Parameter <name> is too long! Make sure your alias name does not exceed %d characters\n", MAX_ALIAS_NAME_LENGTH);
		exit(SPARK_EXIT_GENERAL_ERROR);
	}
	else if (strlen(command) > MAX_ALIAS_COMMAND_LENGTH)
	{
		fprintf(stderr, "[\033[1;31merror\033[0m] Parameter <command> is too long! Make sure your alias command does not exceed %d characters\n", MAX_ALIAS_COMMAND_LENGTH);
		exit(SPARK_EXIT_GENERAL_ERROR);
	}
	else if (strlen(name) > MAX_ALIAS_NAME_LENGTH && strlen(command) > MAX_ALIAS_COMMAND_LENGTH)
	{
		fprintf(stderr, "[\033[1;31merror\033[0m] Parameters <name> and <command> are too long! Make sure the parameters do not exceed %d and %d characters respectively\n", MAX_ALIAS_NAME_LENGTH, MAX_ALIAS_COMMAND_LENGTH);
		exit(SPARK_EXIT_GENERAL_ERROR);
	}

	int line_count = 0;
	// int last_alias_id = 0;

	if (!file_exists(get_file_path("alias")))
	{
		char json_str_single[16384 * 16]; // arbitrary size because idk
		sprintf(json_str_single, "{\n\t\"%d\": {\n\t\t\"name\": \"%s\",\n\t\t\"command\": \"%s\"\n\t}\n}", 0, name, command);
		write_file(get_file_path("alias"), json_str_single, "w");

		return SPARK_EXIT_SUCCESS;
	}

	char **file_buffer = read_file(get_file_path("alias"), &line_count);

	if (file_buffer == NULL)
	{
		fprintf(stderr, "[\033[1;31merror\033[0m] Failed to read file\n");
		exit(SPARK_EXIT_GENERAL_ERROR);
	}

	char *json_str = fold_json(file_buffer, line_count);
	struct json json = json_parse(json_str);
	struct json child = json_first(json);

	int json_objects_length = 0;

	while (json_exists(child))
	{
		child = json_next(child);
		if (json_exists(child))
			json_objects_length++;
		child = json_next(child);
	}
	
	printf("json_objects_length: %d\n", json_objects_length);

	int json_str_length = strlen(json_str);

	char *end = json_str + json_str_length - 1;

	while (end > json_str && (*end == '\n' || *end == '\r' || *end == ' ' || *end == '\t'))
	{
		*--end = '\0';
	}

	if (*end != '}')
	{
		fprintf(stderr, "[\033[1;31merror\033[0m] Invalid JSON structure: missing closing brace\n");
		exit(SPARK_EXIT_GENERAL_ERROR);
	}
	end--;
	*end = '\0';

	strcat(json_str, ",\n");

	char temp_buf[16384 * 16]; // arbitrary size because idk
	snprintf(temp_buf, sizeof(temp_buf), "%s\t\"%d\": {\n\t\t\"name\": \"%s\",\n\t\t\"command\": \"%s\"\n\t}\n}", json_str, json_objects_length, name, command);

	write_file(get_file_path("alias"), temp_buf, "w");

	free(file_buffer);
	free(json_str);

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

	int space_amount = 16;

	FILE *fptr = fopen(get_file_path("alias"), "r");

	if (fptr == NULL)
	{
		fprintf(stderr, "[\033[1;31merror\033[0m] File %s was not found\n", get_file_path("alias"));
		exit(SPARK_EXIT_FILE_NOT_FOUND);
	}

	int line_count = 0;

	char **file_buffer = read_file(get_file_path("alias"), &line_count);
	
	if (file_buffer == NULL)
	{
		fprintf(stderr, "[\033[1;31merror\033[0m] Failed to read file\n");
		exit(SPARK_EXIT_GENERAL_ERROR);
	}

	char *json_str = fold_json(file_buffer, line_count);
	struct json json = json_parse(json_str);
	struct json child = json_first(json);

	int json_objects_length = 0;

	while (json_exists(child))
	{
		child = json_next(child);
		if (json_exists(child))
			json_objects_length++;
		child = json_next(child);
	}

	for (int i = 0; i < json_objects_length; i++)
	{
		char name_path[32] = "";
		sprintf(name_path, "%d.name", i);
		struct json value = json_get(json_str, name_path);
		int alias_name_length = json_string_length(value) + 1;
		char alias_name[alias_name_length];
		json_string_copy(value, alias_name, sizeof(alias_name));

		char command_path[32] = "";
		sprintf(command_path, "%d.command", i);
		value = json_get(json_str, command_path);
		char alias_command[json_string_length(value) + 1];
		json_string_copy(value, alias_command, sizeof(alias_command));

		// this is so stupid but it works, memcpy might have been better here probably lol
		char spaces[space_amount - alias_name_length + 1];
		for (int i = 0; i <= (int)sizeof(spaces); i++)
		{
			spaces[i] = ' ';
		}
		spaces[sizeof(spaces)] = '\0';

		printf("%s%s%s\n", alias_name, spaces, alias_command);
	}

	free(file_buffer);
	free(json_str);
	
	fclose(fptr);

	return SPARK_EXIT_SUCCESS;
}