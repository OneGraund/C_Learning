#ifndef MYGREP_H 
#define MYGREP_H

void readFile_andSearch(FILE *file, char* keyword, const char* outfile, int i_arg);
int writeLine_toFile(const char *path, char* line);

#endif
