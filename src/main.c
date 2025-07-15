#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/operations.h"
#include "include/exit_codes.h"

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		printf("spark - a lightweight linux alias manager\n\n");
		printf("USAGE:\n");
		printf(" --> to create a new alias:\tspark --create <name> <command>\n");
		printf(" --> to execute an alias:\tspark <name>\n");
		printf(" --> to remove an alias:\tspark --remove <name>\n");
		return SPARK_EXIT_SUCCESS;
	}

	if (argc == 2 && strcmp(argv[1], "--list"))
	{
		// call function to run alias here
		return SPARK_EXIT_SUCCESS;
	}

	// CREATE ALIAS
	if (!strcmp(argv[1], "--create"))
	{
		if (argc < 4) // 4 args in total: spark --create <name> <command>
		{
			switch (argc)
			{
				case 2:
					fprintf(stderr, "[\033[1;31merror\033[0m] Failed to create a new alias: missing parameters <name> and <command>\n");
					break;
				case 3:
					fprintf(stderr, "[\033[1;31merror\033[0m] Failed to create a new alias: missing parameter <command>\n");
					break;
			}
			exit(SPARK_EXIT_INVALID_ARGUMENTS);
		}

		char *alias_name = argv[2];
		char *alias_command = argv[3];
		
		create_alias(alias_name, alias_command);
	}
	// REMOVE ALIAS
	else if (!strcmp(argv[1], "--remove"))
	{
		if (argc < 3) // 3 args in total: spark --remove <name>
		{
			fprintf(stderr, "[\033[1;31merror\033[0m] Failed to remove alias: missing parameter <name>\n");
			exit(SPARK_EXIT_INVALID_ARGUMENTS);
		}

		char *alias_name = argv[2];
		
		remove_alias(alias_name);
	}
	// LIST ALIASES
	else if (!strcmp(argv[1], "--list"))
	{
		list_aliases();
	}
	// OPTION NOT RECOGNIZED
	else
	{
		fprintf(stderr, "[\033[1;31merror\033[0m] Unknown argument passed, exiting...\n");
		exit(SPARK_EXIT_INVALID_ARGUMENTS);
	}

	return SPARK_EXIT_SUCCESS;
}