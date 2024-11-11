/**
 * @file mygrep.c
 * @brief This file implements a custom grep utility for searching keywords in files/stdin
 * @details This utility makes a line-by-line search for a specified by user keyword either
 *    	    case-sensitive or insesitive in multiple/signle files or from stdin stream
 *
 * @synopsis
 *		mygrep [-i] [-o outfile] keyword [file...]
 *
 * @param -i Perform a case-insensitive search.
 * @param -o Specify an output file to save search results
 * @param keyword The keyword to search for in each line of the files/stdin
 * @param file One or more files to search. If omitted, reads from stdin stream
 *
 * @author Volodymyr Skoryi
 * @date 2024-11-08
 *
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#define STR_SIZE (128) /**< Maximum number of characters in keyword/file_path */
#define MAX_FILES (50) /**< Maximum amount of possible input files */


/**
 * @def DEBUG
 * @brief Enables debug mode for detailed error messages
 * @details When this file is compiled with a -DDEBUG flag, 'debug' macto is going to print
 *		    debug information to stderr stream including file name and line number. In case
 *			this flag was not specified, than debug outputs are not going to be printed
 * @param fmt The format string, similar to printf
 * @param ... Additional arguments to plug in the string
 */
#ifdef DEBUG
#define debug(fmt, ...) \
    (void) fprintf(stderr, "[%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define debug(msg, ...)
#endif

// Function prototypes
void readFile_andSearch(FILE *file ,char* keyword, const char* outfile, int i_arg);
int writeLine_toFile(const char *path, char* line);


/**
 * @brief   Prints the usage information for the mygrep utility
 *
 * @details This function outputs a synopsis of the command-line arguments and options for
 *		    the utility to stderr stream, then exits the program with a failure code
 *			EXIT_FAILURE.
 *
 * @author  Volodymyr Skoryi
 * @date    2024-11-08
 */
void usage(void) {
    fprintf(stderr, "Usage mygrep [-i] [-o outfile] keyword [file...]\n");
    exit(EXIT_FAILURE);
}



int main(int argc, char* argv[]) {

    debug("Program started", NULL);
    char *outfile = NULL;
    int opt_i = 0;
    int c;

    while ( (c = getopt(argc, argv, "io:")) != -1) {
        switch (c) {
            case 'o': outfile = optarg;
                break;
            case 'i': opt_i++;
                break;
            case '?': usage();
                break;
        }
    }

    char keyword[STR_SIZE];
    if (optind < argc) {
        strcpy(keyword, argv[optind]);
        debug("Keyword: %s", keyword);
        optind++;
    } else
        usage();


    int files_amount = 0;
    char files[MAX_FILES][STR_SIZE];
    if (optind < argc) {
        files_amount = argc - optind;
        if (files_amount > MAX_FILES) {
        	debug("User specified more files than program can process. Details: Max-%d files,"
        		  " specified: %d", MAX_FILES, files_amount);
			fprintf(stderr, "Error! Too much files specified!");
        }
        debug("%d files were specified, parsing", files_amount);
        int tmp = 0;

        while (optind < argc) {
            strcpy(files[tmp], argv[optind]);
            debug("\tFile%d: %s", tmp, files[tmp]);
            optind++; tmp++;
        }
    } else {
        debug("No files were specified, reading from stdin", NULL);
        strcpy(files[0], "stdin");
        files_amount = 1;
    }


    if (opt_i > 1) {
        debug("Abnormal amount of -i arguments: %d times\n", opt_i);
        usage();
    }
    if (outfile == NULL) {
        debug("Outfile was not speicifed", NULL);
    } else
        debug("Outfile was specified: %s", outfile);

    if (opt_i == 1) {
        for (int i=0; i < strlen(keyword); i++)
            keyword[i] = tolower(keyword[i]);}
    if (files_amount > 0) {
        FILE *in;
        for (int file = 0; file < files_amount; file++) {
            debug("Reading file: %s", files[file]);
            if (strcmp(files[file], "stdin")==0)
                in = stdin;
            else
                in = fopen(files[file], "r");
            readFile_andSearch(in, keyword, outfile, opt_i);
        }
    }

    return 0;
}


/**
 * @brief reads file and searches keyword
 *
 * @details This module takes a file, searches (case sensitivity may be specified) for a
 *          keyword in each line and in case an outfile was specified is going to append
 *          it with the line containing the keyword
 *
 * @param file the FILE datatype pointer that is going to be read.
 * @param keyword keyword that is going to be searched in each line of the "*file".
 * @param outfile file path to an output file that is going to contain search
 *		          result. if not specified (or set to NULL) will outoput to stdout.
 * @param i_arg practically should be a boolean value that if set to 1 will make a
 *				case insensitive search (no matter if upper case or lower case).
 *
 * @author Volodymyr Skoryi
 * @date   2024-11-08
 *
 * @return void
 */
void readFile_andSearch(FILE *file, char* keyword, const char* outfile, int i_arg) {
    // FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    // fp = fopen(path, "r");
    if (file == NULL) {
        fprintf(stderr, "Error openening file: %s", strerror(errno));
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    // Ensure that outfile is empty
    if (outfile != NULL) {
        FILE *fp = fopen(outfile, "w");
        if (fp != NULL) fclose(fp);
        else {
            fprintf(stderr, "Failed to open outfile for clearing purpose, %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    
    while ((read = getline(&line, &len, file)) != -1) {
        char *line_dup = strdup(line);
        if (i_arg == 1) {
            for (int i = 0; i < read; i++) {
                line_dup[i] = tolower(line_dup[i]);
            }
        }

        if (strstr(line_dup, keyword) != NULL) {
            if (outfile != NULL) {

                writeLine_toFile(outfile, line);
            } else {
                printf("%s", line);
            }
        }
        free(line_dup);

    }

    fclose(file);
    if (line)
        free(line);
}


/**
 * @brief appends file with line 
 *
 * @details this module takes as input a path to the output file, after which it appends it with a string.
 *
 * @param path is the path to the output file (where line is appended)
 * @param line is the line itself that is going to be appended to the file
 *
 * @author Volodymyr Skoryi
 * @date 2024-11-08
 *
 * @return int either 1 or 0. 1 for successful execution, 0 for an error
 */
int writeLine_toFile(const char *path, char* line) {

    FILE *fp = fopen(path, "a");
    int res;

    if (fp == NULL) {
        fprintf(stderr, "Failed to open the output file, error: %s", strerror(errno));
        exit(EXIT_FAILURE);
    } else {
        res = fputs(line, fp);
        if (res == EOF) {
            fprintf(stderr, "Failed to append outfile, error: %s", strerror(errno));
            fclose(fp);
            return 0;
        } else {
            debug("Outfile succesfully appended, result: %d", res);
            debug("\tString appended: %s", line);
            fclose(fp);
            return 1;
        }
    }
}
