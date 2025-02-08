#ifndef MDCONV_H
#define MDCONV_H

#define MAX_PATH 80
#define MAX_LINE 256

// Function prototypes
void processFile(const char *inFile, const char *outFile);
void convertToHtml(FILE *input, FILE *output);
void processLine(const char *line, FILE *output);
int isHeading(const char *line);
void processHeading(const char *line, FILE *output);
void processTableLine(const char *line, FILE *output);
int isTableLine(const char *line);
void processFormatting(const char *line, FILE *output);

#endif 