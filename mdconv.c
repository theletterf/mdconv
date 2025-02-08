#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <dos.h>
#include "mdconv.h"

// Add memory model specification for Watcom
#pragma pack(1)

typedef struct {
    char *markdown;
    int length;
} MarkdownLine;

// Function prototypes
void processFile(const char *inFile, const char *outFile);
void convertToHtml(FILE *input, FILE *output);
void processLine(const char *line, FILE *output);
int isHeading(const char *line);
void processHeading(const char *line, FILE *output);
void processTableLine(const char *line, FILE *output);
int isTableLine(const char *line);
void processFormatting(const char *line, FILE *output);

int main(int argc, char *argv[]) {
    struct find_t fileinfo;
    char inputPath[MAX_PATH];
    char outputPath[MAX_PATH];
    unsigned result;

    if (argc != 3) {
        printf("Usage: MDCONV.EXE <input_dir> <output_dir>\n");
        return 1;
    }

    strcpy(inputPath, argv[1]);
    strcpy(outputPath, argv[2]);
    
    // Add trailing backslash if needed
    if (inputPath[strlen(inputPath) - 1] != '\\') {
        strcat(inputPath, "\\");
    }
    if (outputPath[strlen(outputPath) - 1] != '\\') {
        strcat(outputPath, "\\");
    }

    // Create search path
    strcat(inputPath, "*.md");
    
    // Start searching for files
    result = _dos_findfirst(inputPath, _A_NORMAL, &fileinfo);
    if (result != 0) {
        printf("No .md files found in directory: %s\n", argv[1]);
        return 1;
    }

    while (result == 0) {
        char inFile[MAX_PATH];
        char outFile[MAX_PATH];
        char *dot;
        
        // Construct input filename
        strcpy(inFile, argv[1]);
        if (inFile[strlen(inFile) - 1] != '\\') {
            strcat(inFile, "\\");
        }
        strcat(inFile, fileinfo.name);
        
        // Construct output filename
        strcpy(outFile, argv[2]);
        if (outFile[strlen(outFile) - 1] != '\\') {
            strcat(outFile, "\\");
        }
        strcat(outFile, fileinfo.name);
        
        // Change extension to .html
        dot = strrchr(outFile, '.');
        if (dot != NULL) {
            strcpy(dot, ".html");
        }
        
        processFile(inFile, outFile);
        result = _dos_findnext(&fileinfo);
    }

    return 0;
}

void processFile(const char *inFile, const char *outFile) {
    FILE *input, *output;
    
    input = fopen(inFile, "r");
    if (input == NULL) {
        printf("Error opening input file: %s\n", inFile);
        return;
    }
    
    output = fopen(outFile, "w");
    if (output == NULL) {
        printf("Error opening output file: %s\n", outFile);
        fclose(input);
        return;
    }
    
    // Write HTML header
    fprintf(output, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2//EN\">\n");
    fprintf(output, "<html>\n<head>\n");
    fprintf(output, "<title>%s</title>\n", inFile);
    fprintf(output, "</head>\n<body>\n");
    
    convertToHtml(input, output);
    
    // Write HTML footer
    fprintf(output, "</body>\n</html>\n");
    
    fclose(input);
    fclose(output);
} 