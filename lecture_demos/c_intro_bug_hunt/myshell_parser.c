/* This is an intentionally buggy soltuion to challenge 0.
 * It contains a bug that manifests when running:
 *    make check CFLAGS=-DMULTIPLE_PARSES
 * Use tools to help detect the source of the bug.
 */
#include "myshell_parser.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WHITESPACE_CHARS "\n\t "
#define SPECIAL_CHARS "<>|&"

/*
 * Max word length is specified in the homework prompt.
 */
#define MAX_WORD_LENGTH 32

static struct pipeline_command *pipeline_command_alloc()
{
	struct pipeline_command *command =
		malloc(sizeof(struct pipeline_command));
	if (command) {
		command->next = NULL;
		command->command_args[0] = NULL;
		command->redirect_in_path = NULL;
		command->redirect_out_path = NULL;
	}
	return command;
}

enum token_type {
	TT_NONE, /* No token, nul byte */
	TT_AMPERSAND, /* '&' character */
	TT_PIPE, /* '|' character */
	TT_LEFT_ANGLE, /* '<' character */
	TT_RIGHT_ANGLE, /* '>' character */
	TT_WORD, /* A sequence of any OTHER non-whitespace characters */
};

/*
 * Get the next non-whitespace character at or after a given position.
 *
 * Args:
 * token [in,out] Precondition: Points to the start location of the search.
 *                Postcondition: Points to the next token. If there are no more
 *                               tokens, points to the string's terminator.
 * token_length [out]: Length of the detected token, in bytes.
 *
 * Returns the type of the detected token pointed to by 'token', TT_NONE if no
 * tokens remain.
 */
static enum token_type next_token(const char **token, size_t *token_length)
{
	*token += strspn(*token, WHITESPACE_CHARS);
	switch (**token) {
	case '&':
		*token_length = 1;
		return TT_AMPERSAND;
	case '|':
		*token_length = 1;
		return TT_PIPE;
	case '<':
		*token_length = 1;
		return TT_LEFT_ANGLE;
	case '>':
		*token_length = 1;
		return TT_RIGHT_ANGLE;
	case '\0':
		*token_length = 0;
		return TT_NONE;
	default:
		*token_length = strcspn(*token, WHITESPACE_CHARS SPECIAL_CHARS);
		return TT_WORD;
	}
	return TT_NONE;
}

/*
 * Parse 0 or 1 background indicators.
 *
 * Args:
 * token     Precondition: Points to the start of a candidate background
 *                         indicator in a string.
 *          Postcondition: Points to the start of the first token after the
 *                         background indicator.
 * pipeline Pipeline structure to which background state is stored.
 *
 * Returns the count of acknowledged background indicators (0 or 1).
 */
static int parse_background(const char **start, struct pipeline *pipeline)
{
	size_t token_length = 0;
	enum token_type token_type = next_token(start, &token_length);

	if (token_type != TT_AMPERSAND) {
		/* This isn't a background indicator. Ignore the token. */
		return 0;
	}

	pipeline->is_background = true;
	*start = *start + token_length;
	return 1;
}

/*
 * Parse 0 or 1 stdout redirects.
 *
 * Args:
 * token     Precondition: Points to the start of a candidate redirect in a
 *                         string.
 *          Postcondition: Points to the start of the first token after the
 *                         redirect.
 * command  Command structure in which a parsed command is stored.
 *
 * Returns:
 *   -1: Encountered an error.
 *    0: No error, but no stdout redirect was recorded.
 *    1: One stdout redirect was recorded.
 */
static int parse_redirect_out(const char **token,
			      struct pipeline_command *command)
{
	size_t token_length = 0;
	enum token_type token_type = next_token(token, &token_length);

	if (token_type != TT_RIGHT_ANGLE) {
		/* This isn't a redirect out. Ignore the token. */
		return 0;
	}
	*token = *token + token_length;

	token_type = next_token(token, &token_length);
	if (token_type != TT_WORD) {
		fprintf(stderr, "ERROR: Expected a redirect source\n");
		return -1;
	}

	if (token_length > MAX_WORD_LENGTH) {
		fprintf(stderr,
			"ERROR: Encountered a word longer than %d characters\n",
			MAX_WORD_LENGTH);
		return -1;
	}

	command->redirect_out_path = strndup(*token, token_length);

	*token = *token + token_length;
	return 1;
}

/*
 * Parse 0 or 1 stdin redirect.
 *
 * Args:
 * token     Precondition: Points to the start of a candidate redirect in a
 *                         string.
 *          Postcondition: Points to the start of the first token after the
 *                         redirect.
 * command  Command structure in which a parsed command is stored.
 *
 * Returns:
 *   -1: Encountered an error.
 *    0: No error, but no stdin redirect was recorded.
 *    1: One stdout redirect was recorded.
 */
static int parse_redirect_in(const char **token,
			     struct pipeline_command *command)
{
	size_t token_length = 0;
	enum token_type token_type = next_token(token, &token_length);

	if (token_type != TT_LEFT_ANGLE) {
		/* This isn't a redirect in. Ignore the token. */
		return 0;
	}
	*token = *token + token_length;

	token_type = next_token(token, &token_length);
	if (token_type != TT_WORD) {
		fprintf(stderr, "ERROR: Expected a redirect source\n");
		return -1;
	}

	if (token_length > MAX_WORD_LENGTH) {
		fprintf(stderr,
			"ERROR: Encountered a word longer than %d characters\n",
			MAX_WORD_LENGTH);
		return -1;
	}

	command->redirect_in_path = strndup(*token, token_length);

	*token = *token + token_length;
	return 1;
}

/*
 * Parse 1 or more arguments for a command.
 *
 * Args:
 * token     Precondition: Points to the start of a candidate arg list in a
 *                         string.
 *          Postcondition: Points to the start of the first token after the
 *                         args.
 * command  Command to which parsed arguments are allocated and added.
 *          Allocated memory must be freed by pipeline_command_free_args(command).
 *
 * Returns:
 *   -1: Encountered an error.
 * else: Number of args processed (0 or more)
 */
static int parse_args(const char **token, struct pipeline_command *command)
{
	// args  : WORD
	//       | WORD args
	size_t token_length = 0;
	enum token_type token_type = next_token(token, &token_length);
	int arg_count = 0;

	while ((token_type = next_token(token, &token_length)) == TT_WORD) {
		if (token_length > MAX_WORD_LENGTH) {
			fprintf(stderr,
				"ERROR: Encountered a word longer than %d characters\n",
				MAX_WORD_LENGTH);
			return -1;
		}

		command->command_args[arg_count] =
			strndup(*token, token_length);

		*token = *token + token_length;
		arg_count += 1;
	}

	return arg_count;
}

/*
 * Parse a command at a given starting location in an input string.
 *
 * Args:
 * token     Precondition: Points to the start of a command in a string.
 *          Postcondition: Points to the start of the first token after the
 *                         command.
 * command  Command structure in which a parsed command is stored.
 *
 * Returns:
 *   -1: an error occurred
 *    0: 0 commands were parsed
 *    1: 1 command was parsed
 */
static int parse_command(const char **token, struct pipeline_command *command)
{
	// command  : args
	//          | command '<' WORD
	//          | command '>' WORD
	//          | command '&'
	int args_parsed = parse_args(token, command);
	if (args_parsed < 1) {
		/* Return on 0 args or on error. Not much to do with 0 args. */
		return args_parsed;
	}

	/*
	 * Handle in/out in a loop since they could be presented in either order
	 */
	int redirect_in_ret = 0;
	int redirect_out_ret = 0;
	do {
		redirect_in_ret = parse_redirect_in(token, command);
		redirect_out_ret = parse_redirect_out(token, command);
	} while (redirect_in_ret > 0 && redirect_out_ret > 0);
	if (redirect_in_ret == -1 || redirect_out_ret == -1) {
		return -1;
	}

	return 1;
}

/*
 * Top level of the parser. Breaks a line into separate components of a command
 * pipeline, separated by the '|' token.
 *
 * Args:
 * token:    Token pointing to the start of the pipeline
 * pipeline: Pipeline to which background state and command lists are saved.
 *           Each pipeline command must have its args freed by
 *           pipeline_command_free_args(). That is handled by calling
 *           pipeline_free() on the pipeline struct.
 *
 * Returns:
 *   -1: an error occurred
 * else: Number of pipes processed (0 or more)
 */
static int parse_pipes(const char **token, struct pipeline *pipeline)
{
	// pipeline  : command
	//           | command '|' pipeline
	int last_cmd_parse = 0;
	int pipe_count = 0;
	enum token_type token_type;
	size_t token_length = 0;

	pipeline->is_background = false;

	pipeline->commands = pipeline_command_alloc();
	last_cmd_parse = parse_command(token, pipeline->commands);
	if (last_cmd_parse == -1) {
		return -1;
	}
	struct pipeline_command *current_command = pipeline->commands;

	while ((token_type = next_token(token, &token_length)) == TT_PIPE) {
		if (last_cmd_parse != 1) {
			fprintf(stderr,
				"ERROR: Unexpected pipe is not preceded by a command.\n");
			return -1;
		}
		*token = *token + token_length;
		current_command->next = pipeline_command_alloc();
		last_cmd_parse = parse_command(token, current_command->next);
		if (last_cmd_parse == -1) {
			return -1;
		}
		current_command = current_command->next;
		pipe_count += 1;
	}

	if (parse_background(token, pipeline) == -1) {
		return -1;
	}
	current_command = current_command->next;

	if ((token_type = next_token(token, &token_length)) != TT_NONE) {
		fprintf(stderr,
			"ERROR: Unexpected token encountered at the end of a pipeline.\n");
		return -1;
	}

	return pipe_count;
}

struct pipeline *pipeline_build(const char *command_line)
{
	const char *token = command_line;
	struct pipeline *pipeline = malloc(sizeof(struct pipeline));
	int pipes_parsed = parse_pipes(&token, pipeline);

	if (pipes_parsed == -1) {
		fprintf(stderr, "===> %s", command_line);
		fprintf(stderr, "===> %*s^-- Stopped parsing here\n",
			(int)(token - command_line), "");
		pipeline_free(pipeline);
		return NULL;
	}

	return pipeline;
}

/*
 * Frees the args in a pipeline command.
 */
static void pipeline_command_free_args(struct pipeline_command *command)
{
	for (char **arg = command->command_args;
	     arg != NULL && *arg != NULL &&
	     arg < (command->command_args + MAX_ARGV_LENGTH);
	     ++arg) {
		free(*arg);
	}
}

void pipeline_free(struct pipeline *pipeline)
{
	if (pipeline)
	{
		struct pipeline_command *command_to_free = pipeline->commands;
		while (command_to_free != NULL) {
			struct pipeline_command *next_command = command_to_free->next;
			pipeline_command_free_args(command_to_free);
			free(command_to_free->redirect_in_path);
			free(command_to_free->redirect_out_path);
			free(command_to_free);
			command_to_free = next_command;
		}
		free(pipeline);
	}
}
