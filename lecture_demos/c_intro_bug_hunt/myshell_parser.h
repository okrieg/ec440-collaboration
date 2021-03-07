#ifndef MYSHELL_PARSER_H
#define MYSHELL_PARSER_H
#include <stdbool.h>

/*
 * Max line length is specified in the homework prompt.
 */
#define MAX_LINE_LENGTH 512

/*
 * The longest argv would happen on a line with all 1-character words, plus the
 * NULL terminating pointer.
 */
#define MAX_ARGV_LENGTH (MAX_LINE_LENGTH / 2 + 1)

/*
 * Represents a single command in a pipeline.
 */
struct pipeline_command {
	char *command_args[MAX_ARGV_LENGTH]; /* List of pointers to each
						argument for a command. The
						first entry is the command
						name. The last entry is NULL.
						E.g., input "ls -al" is
						equivalent to ["ls", "-al",
						NULL] */
	char *redirect_in_path; /* Name of a file to redirect in from, or NULL
				   if there is no stdin redirect */
	char *redirect_out_path; /* Name of a file to redirect out to, or NULL if
				    there is no stdout redirect */
	struct pipeline_command *next; /* Pointer to the next command in the
					  pipeline. NULL if this is the last
					  command in the pipeline */
};

/*
 * Represents a collection of commands that are connected through a pipeline.
 */
struct pipeline {
	struct pipeline_command *commands; /* Pointer to the first command in
					      the pipeline*/
	bool is_background; /* True if this pipeline should execute in the
			       background */
};

/*
 * Frees a pipeline structure that was created with pipeline_build().
 *
 * Arguments:
 * pipeline  Pipeline structure to be freed.
 */
void pipeline_free(struct pipeline *pipeline);

/*
 * Create a pipeline structure that represents the given command line.
 * The created structure must be freed by pipeline_free().
 *
 * Arguments:
 * command_line  Command line that is to be parsed.
 *
 * For example, given input:
 *   pipeline_build("ls -al < infile | wc -l | cat > outfile &")
 * The returned struct should look like:
 *   {
 *     ->commands = {
 *        ->command_args = { "ls",  "-al", NULL, ...undefined-values... },
 *        ->redirect_in_path = "infile",
 *        ->redirect_out_path = NULL,
 *        ->next = {
 *          ->command_args = { "wc", "-l", NULL, ...undefined-values...  },
 *          ->redirect_in_path = NULL,
 *          ->redirect_out_path = NULL,
 *          ->next = {
 *            ->command_args = { "cat", NULL, ...undefined-values...  },
 *            ->redirect_in_path = NULL,
 *            ->redirect_out_path = "outfile",
 *            ->next = NULL
 *          }
 *        }
 *     },
 *     ->is_background = true
 *   }
 *
 */
struct pipeline *pipeline_build(const char *command_line);

#endif /* MYSHELL_PARSER_H */
