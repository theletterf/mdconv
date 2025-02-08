#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mdconv.h"

void convertToHtml(FILE *input, FILE *output) {
    char line[MAX_LINE];
    int inTable = 0;
    
    while (fgets(line, MAX_LINE, input) != NULL) {
        // Remove trailing newline
        line[strcspn(line, "\n")] = 0;
        
        if (isHeading(line)) {
            processHeading(line, output);
        }
        else if (isTableLine(line)) {
            if (!inTable) {
                fprintf(output, "<table border=\"1\">\n");
                inTable = 1;
            }
            processTableLine(line, output);
        }
        else if (inTable && line[0] == '\0') {
            fprintf(output, "</table>\n");
            inTable = 0;
        }
        else {
            processFormatting(line, output);
            fprintf(output, "<br>\n");
        }
    }
    
    if (inTable) {
        fprintf(output, "</table>\n");
    }
}

int isHeading(const char *line) {
    int i;
    for (i = 0; line[i] == '#'; i++);
    return i > 0 && i <= 6 && (line[i] == ' ' || line[i] == '\t');
}

void processHeading(const char *line, FILE *output) {
    int level = 0;
    const char *text = line;
    
    while (*text == '#') {
        level++;
        text++;
    }
    
    while (*text == ' ' || *text == '\t') {
        text++;
    }
    
    fprintf(output, "<h%d>%s</h%d>\n", level, text, level);
}

int isTableLine(const char *line) {
    return strchr(line, '|') != NULL;
}

void processTableLine(const char *line, FILE *output) {
    char *token;
    char lineCopy[MAX_LINE];
    
    strcpy(lineCopy, line);
    
    // Check if this is a separator line
    if (strstr(line, "|-") != NULL) {
        return;
    }
    
    fprintf(output, "<tr>\n");
    
    token = strtok(lineCopy, "|");
    while (token != NULL) {
        while (*token == ' ') token++;
        fprintf(output, "  <td>%s</td>\n", token);
        token = strtok(NULL, "|");
    }
    
    fprintf(output, "</tr>\n");
}

void processFormatting(const char *line, FILE *output) {
    int i = 0;
    int inBold = 0;
    int inItalic = 0;
    int inCode = 0;
    
    while (line[i]) {
        if (line[i] == '*' && line[i+1] == '*') {
            fprintf(output, inBold ? "</strong>" : "<strong>");
            inBold = !inBold;
            i += 2;
        }
        else if (line[i] == '*') {
            fprintf(output, inItalic ? "</em>" : "<em>");
            inItalic = !inItalic;
            i++;
        }
        else if (line[i] == '`') {
            fprintf(output, inCode ? "</code>" : "<code>");
            inCode = !inCode;
            i++;
        }
        else if (line[i] == '[') {
            char linkText[MAX_LINE];
            char linkUrl[MAX_LINE];
            int j = 0;
            i++;
            
            while (line[i] && line[i] != ']') {
                linkText[j++] = line[i++];
            }
            linkText[j] = '\0';
            
            if (line[i] == ']' && line[i+1] == '(') {
                i += 2;
                j = 0;
                while (line[i] && line[i] != ')') {
                    linkUrl[j++] = line[i++];
                }
                linkUrl[j] = '\0';
                fprintf(output, "<a href=\"%s\">%s</a>", linkUrl, linkText);
            }
            i++;
        }
        else {
            fputc(line[i++], output);
        }
    }
} 