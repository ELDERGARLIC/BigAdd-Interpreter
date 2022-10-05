// libraries
#include <stdio.h>
#include <string.h>

// constants
#define ALPHABET "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define DIGITS "0123456789"
#define MINUS "-"
#define NON_STATE_CHARS ",.][" // I DEFINED SUCH CONSTANT BECAUSE THEY HAVE SIMILAR BEHAVIOR PATTERN
#define SEPERATOR ","
#define ENDOFLINE "."
#define OPENBLOCK "["
#define CLOSEBLOCK "]"
#define COMMENT_START "{"
#define COMMENT_END "}"
#define QUOTATION "\""
#define UNDERSCORE "_"
#define SPACE " "
#define KEYWORDS "int move to loop times out newline add sub from"

/**
Instead of initializing a state string variable and modifying it every time;
I defined a STATE struct with a name member.
I change the state when needed and access their name property to find the source_state(current state)
**/
typedef struct {
	char* name;
} STATE;

STATE default_state = {"default"};
STATE key_iden_state = {"keyword and identifier"};
STATE str_const_state = {"string constant"};
STATE comment_state = {"comment"};
STATE int_const_state = {"integer constant"};


void main(int argc, char *argv[]) {
    // handle given filename argument
	char readFilename[256];
	char writeFilename[256];
	snprintf(readFilename, sizeof(readFilename), "%s.ba", argv[1]);
	snprintf(writeFilename, sizeof(writeFilename), "%s.lx", argv[1]);


	STATE source_state = default_state; // FSA starts from default state

	char token_stack[100]; // tokens are written to file from this string
	token_stack[0] = '\0'; // empty string

	FILE *readFilePointer = fopen(readFilename, "r"); // read files from here
	FILE *writeFilePointer = fopen(writeFilename, "w"); // write to this file
	char ch;

	if (readFilePointer == NULL) // check if file exists
	{
		printf("ERROR: File is not available\n");
	}
	else
	{
		// iterate number of characters in file, assign current char to ch
		while ((ch = fgetc(readFilePointer)) != EOF)
		{
			// check if char assigned to ch is valid
            // (ascii -126) used in code
            // (ascii 126-) may only appear in a comment or string
			if (ch<0 || ch>125) {
                if (source_state.name == "comment" || source_state.name == "string constant") {

                }
                else {
                    printf("An unrecognized character is detected in code!\n", ch);
				    break;
                }
			}

			char ch_str[2] = {ch, '\0'}; // create string from ch to use it in string.h functions 

			// printf("%s | %s -> ", ch_str, source_state.name);

			/**
			NESTED IF ELSE STATEMENTS TO CATCH EVERY POSSIBLE STATE-INPUT COMBINATION;
			APPLY THE OPERATIONS DEFINED FOR THAT PARTICULAR STATE-INPUT COMBINATION;

			POSSIBLE OPERATIONS
			-add char to token_stack --> strcat(token_stack, ch_str);
			-empty token_stack --> token_stack[0] = '\0';
			-change state --> source_state = ..._state;
			-write to file --> fprintf(writeFilePointer, "...");
			-print error message and break loop --> printf("..."); break;
 			**/
			if (source_state.name == "default") { // CHECK 
				if (strstr(ALPHABET, ch_str)) { // DEFAULT_STATE-ALPHABET_INPUT COMBINATION
					strcat(token_stack, ch_str); 
					source_state = key_iden_state;
				}
				else if (strstr(DIGITS MINUS, ch_str)) {
					strcat(token_stack, ch_str);
					source_state = int_const_state;
				}
				else if (strstr(NON_STATE_CHARS, ch_str)) { // ] [ , .
					if (strstr(ENDOFLINE, ch_str)) {
						fprintf(writeFilePointer, "EndOfLine\n");
					}
					else if (strstr(SEPERATOR, ch_str)) {
						fprintf(writeFilePointer, "Seperator\n");
					}
					else if (strstr(OPENBLOCK, ch_str)) {
						fprintf(writeFilePointer, "OpenBlock\n");
					}
					else if (strstr(CLOSEBLOCK, ch_str)) {
						fprintf(writeFilePointer, "CloseBlock\n");
					}
				}
				else if (strstr(COMMENT_START, ch_str)) {
					source_state = comment_state;
				}
				else if (strstr(COMMENT_END UNDERSCORE, ch_str)) {
					printf("ERROR: Invalid use of token: %s\n", ch_str);
					break;
				}
				else if (strstr(QUOTATION, ch_str)) {
					strcat(token_stack, ch_str);
					source_state = str_const_state;
				}
				else if (strstr(SPACE, ch_str)) {
					continue;
				}
			}	
			else if (source_state.name == "keyword and identifier") {
				// check if identifier in token_stack is out of bound
				if (strlen(token_stack) > 20 ) {
					printf("Length of variable name is out of bound\n");
					break;
				}

				// check inputs 
				if (strstr(ALPHABET DIGITS UNDERSCORE, ch_str)) {
					strcat(token_stack, ch_str);
				}
				else if (strstr(NON_STATE_CHARS SPACE, ch_str)) { // ] [ , . or space
					
					if (strstr(KEYWORDS, token_stack)) {
						fprintf(writeFilePointer, "Keyword %s\n", token_stack);
					} 
					else {
						fprintf(writeFilePointer, "Identifier %s\n", token_stack);
					}

					token_stack[0] = '\0';

					if (strstr(ENDOFLINE, ch_str)) {
						fprintf(writeFilePointer, "EndOfLine\n");
					}
					else if (strstr(SEPERATOR, ch_str)) {
						fprintf(writeFilePointer, "Seperator\n");
					}
					else if (strstr(OPENBLOCK, ch_str)) {
						fprintf(writeFilePointer, "OpenBlock\n");
					}
					else if (strstr(CLOSEBLOCK, ch_str)) {
						fprintf(writeFilePointer, "CloseBlock\n");
					}
					source_state = default_state;
				}
				else if (strstr(COMMENT_START, ch_str)) {
					source_state = comment_state;
				}
				else if (strstr(QUOTATION MINUS COMMENT_END, ch_str)) {
					printf("ERROR: Invalid use of token: %s\n", ch_str);
					break;
				}
				// else if (strstr(SPACE, ch_str)) {
				// 	if (strstr(KEYWORDS, token_stack)) {
				// 		fprintf(writeFilePointer, "Keyword %s\n", token_stack);
				// 	} 
				// 	else {
				// 		fprintf(writeFilePointer, "Identifier %s\n", token_stack);
				// 	}
				// 	token_stack[0] = '\0';
				// 	source_state = default_state;
				// }
			}
			else if (source_state.name == "string constant") {
				// check inputs 
				if (strstr(QUOTATION, ch_str)) {
					strcat(token_stack, ch_str);
					fprintf(writeFilePointer, "StringConstant %s\n", token_stack);
					token_stack[0] = '\0';
					source_state = default_state;
				}
				else {
					strcat(token_stack, ch_str);
				}
			}
			else if (source_state.name == "comment") {
				// check inputs 
				if (strstr(COMMENT_END, ch_str)) {
					token_stack[0] = '\0';
					source_state = default_state;
				}
				else {
					// do nothing
				}
			}
			else if (source_state.name == "integer constant") {
				// check if integer constant is out of bound
				if (strlen(token_stack) > 100 ) {
					printf("Length of integer constant is out of bound\n");
					break;
				}

				// check inputs 
				if (strstr(ALPHABET QUOTATION COMMENT_END MINUS UNDERSCORE, ch_str)) {
					printf("ERROR: Invalid use of token: %s\n", ch_str);
					break;
				}
				else if (strstr(DIGITS, ch_str)) {
					strcat(token_stack, ch_str);
				}
				else if (strstr(NON_STATE_CHARS SPACE, ch_str)) { // ] [ , . or space
					fprintf(writeFilePointer, "IntConstant %s\n", token_stack);

					token_stack[0] = '\0';

					if (strstr(ENDOFLINE, ch_str)) {
						fprintf(writeFilePointer, "EndOfLine\n");
					}
					else if (strstr(SEPERATOR, ch_str)) {
						fprintf(writeFilePointer, "Seperator\n");
					}
					else if (strstr(OPENBLOCK, ch_str)) {
						fprintf(writeFilePointer, "OpenBlock\n");
					}
					else if (strstr(CLOSEBLOCK, ch_str)) {
						fprintf(writeFilePointer, "CloseBlock\n");
					}
					source_state = default_state;

				}
				else if (strstr(COMMENT_START, ch_str)) {
					source_state = comment_state;
				}
				// else if (strstr(SPACE, ch_str)) {
				// 	fprintf(writeFilePointer, "IntConstant %s\n", token_stack);
				// 	token_stack[0] = '\0';
				// 	source_state = default_state;
				// }
			}

			// else {
			// 	printf("state error!\n"); // should never go in here
			// }

			// printf("%s\n", source_state.name);

		}
		// end of file
		// check if file ended in a comment or string
		if (source_state.name == "comment" || source_state.name == "string constant") {
			printf("ERROR: A big lexeme is just left open\n");
		}
	}
	
	// close files
	fclose(readFilePointer);
	fclose(writeFilePointer);
}
