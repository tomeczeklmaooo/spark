#include <stdio.h>

#include "include/operations.h"
#include "include/json.h"
#include "include/exit_codes.h"

int create_alias(char *name, char *command)
{
	// open JSON file in write mode -> clear last line (the closing curly brace) -> append the formatted entry -> append the closing curly brace -> close file
	// this might not be the best way to do it though
	printf("alias to add: '%s' with command '%s'\n", name, command);
	return SPARK_EXIT_SUCCESS;
}

int remove_alias(char *name)
{
	// open JSON file in write mode -> find the alias to remove -> erase the lines -> decrement key values by 1 (if alias was at "10": {...}, then after removal the one that was at "11" will be at "10" (does that make sense???)) -> close file
	printf("alias to remove: '%s'\n", name);
	return SPARK_EXIT_SUCCESS;
}

int list_aliases()
{
	// open JSON file in read mode -> use the JSON parser to get the alias name and command -> print -> close file
	printf("\033[1mName\t\tCommand\033[0m\n");
	return SPARK_EXIT_SUCCESS;
}