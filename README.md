# BigAdd-Interpreter

### Input Sets

```
● A = {SET OF ALPHABET}
● D= {SET OF DIGITS}
● MINUS = { - }
● I = { ,. [ ] } // non_state_chars in code
● O = {SPACE}
● {
● }
● "
● _
● E = {ALL POSSIBLE INPUTS}
```
### Statements

```
● Assignment Statement
o move <int_const> to <variable>.
● Addition statement:
o add < int_const | variable> to <variable>.
● Subtraction statement:
o sub < int_const | variable> from <variable>.
● Output Statement:
o out <out_list>.
```
▪ <out_list>→<out_list>,<list_element>|<list_element>

▪ <list_element>→<int_value>|<string>| newline

```
● Loop Statement:
o loop <int_value> times <line>
o loop <int_value> times <code_block>
```

### Code Explanation

- **Firstpartofthecodeisthelexicalanalyzer,whichisverysimilartotheonewedevelopedin**

**project1. Itreadstheinputfileandgeneratestokensforallthesymbolsandkeywords.It**

**stores the tokens in a .lx file.**

- **OurlexicalanalyzerworksbasedontheFiniteStateMachinewhichweaddedinthefigure**

**downbelow.Thisisjustanabstractionanditdoesnotcoverallthefeaturesoflexicalanalyzer**

**such as outputs, a big lexeme left open error andthe out of bound error.**

- **Wecatchallthementionederrorsinthefirstprojectinourlexicalanalyzerandprintitinthe**

**output.**


- **Secondpartoftheprojectiscreatinganinterpreterbasedonthelexicalanalyzerwecreatedin**

**thelastproject.Theinterpreterrunstheprogramdefinedinthefileanditcatchesalltypesof**

**error which violates BigAdd syntax and semantics.**

**● Usingfunctionsandmodulesinthesecondpartwecanunderstandwhichoperationweare**

**dealingwithandiftheoperationisstructuredcorrectlyornot.Eachfunctionisresponsiblefor**

**one specificstatementand evaluate (char*statement) function is thecontrol point and**

**executor of each statement**

**● Functions are like the list below**

**o evaluate (char *statement) function**

▪ **Evaluates and executes the statement.**

**o declaration (char *tokens[]) function**

▪ **Declares a new variable in the array and sets thedefault 0 to it.**

**o move (char *tokens[]) function**

▪ **Assigns an int to an already declared variable.**

**o out (char *tokens[]) function**

▪ **Prints the statement as it explained in the Statementpart of the report.**

**o add (char *tokens[]) function**

▪ **Performstheadditionoperatorasitexplainedinthestatementpartofthe**

**report.**

**o sub (char *tokens[]) function**

▪ **Performsthesubtractionoperatorasitexplainedinthestatementpartofthe**

**report.**

**o loop (char *tokens[], int token_size, char*type) function**

▪ **It performs the loop operation in the correct type(block and oneline).**


### Demonstration

## Error Messages

● Double Declaration

● Unvalid Declaration

● Non-ending line

● Unvalid Keyword

● Out Invalid Syntax


● Loop Invalid Syntax

● Add Invalid Syntax

● Sub Invalid Syntax

## Source Code

_// libraries_
#include<stdbool.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<assert.h>

_// constants_
#defineALPHABET"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#defineDIGITS"0123456789"


#defineMINUS"-"
#defineNON_STATE_CHARS",.]["
#defineSEPERATOR","
#defineENDOFLINE"."
#defineOPENBLOCK"["
#defineCLOSEBLOCK"]"
#defineCOMMENT_START"{"
#defineCOMMENT_END"}"
#defineQUOTATION"\""
#defineUNDERSCORE"_"
#defineSPACE" "
charKEYWORDS[ 20 ][ 20 ]={"int","move","to","loop","times","out","newline","add","sub","from","newline"};

_/**
Instead of initializing a state string variable andmodifying it every time;
I defined a STATE struct with a name member.
I change the state when needed and access their nameproperty to find the source_state(current state)
**/_

_// State struct_
typedefstruct{
char*name;
} STATE;

_// States_
STATE default_state={"default"};
STATE key_iden_state={"keyword and identifier"};
STATE str_const_state={"string constant"};
STATE comment_state={"comment"};
STATE int_const_state={"integer constant"};

_// Variables_
char*variables[ 1000 ];
intvariableIndex= 0 ;
intvalues[ 1000 ]={ 0 };
intline= 1 ;

_/**
PROTOTYPES
**/_

_// Utilities_
intisKeyword(char* _str_ );
char*substring(char* _string_ ,int _position_ ,int _length_ );
boolvariableExists(char* _variable_ );
voidundeclarationError(char _identifier_ []);
voidsyntaxError(char _type_ [],char _expected_ [],char _previous_ []);
intgetIndex(char* _name_ );

_// Operations_
voidmove(char* _token_ []);
voidout(char* _tokens_ [],int _tokens_size_ );
voiddeclaration(char* _tokens_ []);
voidadd(char* _token_ []);
voidsub(char* _tokens_ []);
voidloop(char* _tokens_ [],int _tokens_size_ ,char* _type_ );

_// Execution prototype_
voidevaluate(char* _statement_ );


_// Main and Lexical Analyzer_
voidmain(int _argc_ ,char* _argv_ []) {
charreadFilename[ 256 ];
charwriteFilename[ 256 ];
snprintf(readFilename,sizeof(readFilename),"%s.ba",argv[ 1 ]);
snprintf(writeFilename,sizeof(writeFilename),"%s.lx",argv[ 1 ]);

```
STATE source_state=default_state; // FSA startsfrom default state
```
```
chartoken_stack[ 100 ]; // tokens are written tofile from this string
token_stack[ 0 ]='\0'; // empty string
```
```
charstatement_stack[ 1000 ];
statement_stack[ 0 ]='\0';
```
```
charstatements_arr[ 100 ][ 1000 ]={};
intstatements_arr_len= 0 ;
```
```
intloop_detector= 0 ;
```
```
FILE*readFilePointer=fopen(readFilename,"r"); // read files from here
FILE*writeFilePointer=fopen(writeFilename,"w"); // write to this file
charch;
```
```
if(readFilePointer==NULL) // check if fileexists
{
printf("ERROR: File is not available\n");
}
else
{
// iterate number of characters in file, assigncurrent char to ch
while((ch=fgetc(readFilePointer))!=EOF)
{
if(ch=='\n') {
line++;
}
// check if char assigned to ch is valid
// (ascii -126) used in code
// (ascii 126-) may only appear in a commentor string
if(ch< 0 ||ch> 125 ) {
if(source_state.name=="comment"||source_state.name=="string constant") {
```
```
}
else{
printf("An unrecognized characteris detected in code!\n");
break;
}
}
```
```
charch_str[ 2 ]={ch,'\0'}; // createstring from ch to use it in string.h functions
```
```
/**
NESTED IF ELSE STATEMENTS TO CATCH EVERYPOSSIBLE STATE-INPUT COMBINATION;
APPLY THE OPERATIONS DEFINED FOR THATPARTICULAR STATE-INPUT COMBINATION;
POSSIBLE OPERATIONS
-add char to token_stack --> strcat(token_stack,ch_str);
-empty token_stack --> token_stack[0]= '\0';
-change state --> source_state = ..._state;
```

_-write to file --> fprintf(writeFilePointer, "...");
-print error message and break loop -->printf("..."); break;
**/_
if(source_state.name=="default") { _// CHECK_
if(strstr(ALPHABET, ch_str)) { _//DEFAULT_STATE-ALPHABET_INPUT COMBINATION_
strcat(token_stack, ch_str);
source_state=key_iden_state;
}
elseif(strstr(DIGITS MINUS, ch_str)){
strcat(token_stack, ch_str);
source_state=int_const_state;
}
elseif(strstr(NON_STATE_CHARS, ch_str)){ _// ] [ ,._
if(strstr(ENDOFLINE, ch_str)){
fprintf(writeFilePointer,"EndOfLine\n");
strcat(statement_stack,"EndOfLine");
if(loop_detector== 0 ) {
strcpy(statements_arr[statements_arr_len],statement_stack);
evaluate(statements_arr[statements_arr_len]);
statements_arr_len++;
statement_stack[ 0 ]='\0';
}
}
elseif(strstr(SEPERATOR, ch_str)){
fprintf(writeFilePointer,"Seperator\n");
strcat(statement_stack,"Seperator");
}
elseif(strstr(OPENBLOCK, ch_str)){
fprintf(writeFilePointer,"OpenBlock\n");
strcat(statement_stack,"OpenBlock");
loop_detector++;
}
elseif(strstr(CLOSEBLOCK, ch_str)){
fprintf(writeFilePointer,"CloseBlock\n");
strcat(statement_stack,"CloseBlock");
loop_detector--;
if(loop_detector== 0 ) {
strcpy(statements_arr[statements_arr_len],statement_stack);
evaluate(statements_arr[statements_arr_len]);
statements_arr_len++;
statement_stack[ 0 ]='\0';
}
}
}
elseif(strstr(COMMENT_START, ch_str)){
source_state=comment_state;
}
elseif(strstr(COMMENT_END UNDERSCORE,ch_str)) {
printf("ERROR: Invalid use oftoken:%s\n", ch_str);
break;
}
elseif(strstr(QUOTATION, ch_str)){
strcat(token_stack, ch_str);
source_state=str_const_state;
}
elseif(strstr(SPACE, ch_str)) {
continue;
}
elseif(ch=='\n'){


#### //

#### }

else{
printf("ERROR: Invalid token:%s\n", ch_str);
break;
}
}
elseif(source_state.name=="keywordand identifier") {
_// check if identifier in token_stackis out of bound_
if(strlen(token_stack)> 20 ) {
printf("Length of variable nameis out of bound\n");
break;
}

```
// check inputs
if(strstr(ALPHABET DIGITS UNDERSCORE,ch_str)) {
strcat(token_stack, ch_str);
}
elseif(strstr(NON_STATE_CHARS SPACE,ch_str)) { // ] [ ,. or space
```
```
if(isKeyword(token_stack)) {
fprintf(writeFilePointer,"Keyword%s\n", token_stack);
strcat(statement_stack,"Keyword");
strcat(statement_stack, token_stack);
strcat(statement_stack,"");
}
else{
fprintf(writeFilePointer,"Identifier%s\n", token_stack);
strcat(statement_stack,"Identifier");
strcat(statement_stack, token_stack);
strcat(statement_stack,"");
}
```
```
token_stack[ 0 ]='\0';
```
```
if(strstr(ENDOFLINE, ch_str)){
fprintf(writeFilePointer,"EndOfLine\n");
strcat(statement_stack,"EndOfLine");
if(loop_detector== 0 ) {
strcpy(statements_arr[statements_arr_len],statement_stack);
evaluate(statements_arr[statements_arr_len]);
statements_arr_len++;
statement_stack[ 0 ]='\0';
}
}
elseif(strstr(SEPERATOR, ch_str)){
fprintf(writeFilePointer,"Seperator\n");
strcat(statement_stack,"Seperator");
}
elseif(strstr(OPENBLOCK, ch_str)){
fprintf(writeFilePointer,"OpenBlock\n");
strcat(statement_stack,"OpenBlock");
loop_detector++;
}
elseif(strstr(CLOSEBLOCK, ch_str)){
fprintf(writeFilePointer,"CloseBlock\n");
strcat(statement_stack,"CloseBlock");
loop_detector--;
if(loop_detector== 0 ) {
```

strcpy(statements_arr[statements_arr_len], statement_stack);
evaluate(statements_arr[statements_arr_len]);
statements_arr_len++;
statement_stack[ 0 ]='\0';
}
}
source_state=default_state;
}
elseif(strstr(COMMENT_START, ch_str)){
source_state=comment_state;
}
elseif(strstr(QUOTATION MINUS COMMENT_END,ch_str)) {
printf("ERROR: Invalid use oftoken:%s\n", ch_str);
break;
}
elseif(ch=='\n'){
_//_
}
else{
printf("ERROR: Invalid token:%s\n", ch_str);
break;
}
}
elseif(source_state.name=="stringconstant") {
_// check inputs_
if(strstr(QUOTATION, ch_str)) {
strcat(token_stack, ch_str);
fprintf(writeFilePointer,"StringConstant%s\n", token_stack);
strcat(statement_stack,"StringConstant");
strcat(statement_stack, token_stack);
strcat(statement_stack," ");
token_stack[ 0 ]='\0';
source_state=default_state;
}
else{
strcat(token_stack, ch_str);
}
}
elseif(source_state.name=="comment"){
_// check inputs_
if(strstr(COMMENT_END, ch_str)) {
token_stack[ 0 ]='\0';
source_state=default_state;
}
else{
_// do nothing_
}
}
elseif(source_state.name=="integerconstant") {
_// check if integer constant is outof bound_
if(strlen(token_stack)> 100 ) {
printf("Length of integer constantis out of bound\n");
break;
}

```
// check inputs
if(strstr(ALPHABET QUOTATION COMMENT_ENDMINUS UNDERSCORE, ch_str)) {
printf("ERROR: Invalid use oftoken:%s\n", ch_str);
break;
```

#### }

```
elseif(strstr(DIGITS, ch_str)) {
strcat(token_stack, ch_str);
}
elseif(strstr(NON_STATE_CHARS SPACE,ch_str)) { // ] [ ,. or space
fprintf(writeFilePointer,"IntConstant%s\n", token_stack);
strcat(statement_stack,"IntConstant");
strcat(statement_stack, token_stack);
strcat(statement_stack," ");
```
```
///// CHECK IF
```
```
token_stack[ 0 ]='\0';
```
if(strstr(ENDOFLINE, ch_str)){
fprintf(writeFilePointer,"EndOfLine\n");
strcat(statement_stack,"EndOfLine");
if(loop_detector== 0 ) {
strcpy(statements_arr[statements_arr_len],statement_stack);
evaluate(statements_arr[statements_arr_len]);
statements_arr_len++;
statement_stack[ 0 ]='\0';
}
}
elseif(strstr(SEPERATOR, ch_str)){
fprintf(writeFilePointer,"Seperator\n");
strcat(statement_stack,"Seperator");
}
elseif(strstr(OPENBLOCK, ch_str)){
fprintf(writeFilePointer,"OpenBlock\n");
strcat(statement_stack,"OpenBlock");
loop_detector++;
}
elseif(strstr(CLOSEBLOCK, ch_str)){
fprintf(writeFilePointer,"CloseBlock\n");
strcat(statement_stack,"CloseBlock");
loop_detector--;
if(loop_detector== 0 ) {
strcpy(statements_arr[statements_arr_len],statement_stack);
evaluate(statements_arr[statements_arr_len]);
statements_arr_len++;
statement_stack[ 0 ]='\0';
}
}
source_state=default_state;
}
elseif(strstr(COMMENT_START, ch_str)){
source_state=comment_state;
}
elseif(ch=='\n'){
_//_
}
else{
printf("ERROR: Invalid token:%s\n", ch_str);
break;
}
}
}
_// end of file_


```
// check if file ended in a comment or string
if(source_state.name=="comment"||source_state.name=="string constant") {
printf("ERROR: A big lexeme is just leftopen\n");
}
}
```
fclose(readFilePointer);
fclose(writeFilePointer);
}

_/**
FUNCTION BODY
**/_

intisKeyword(char* _str_ ) {
intkeywords_len= 11 ;
for(inti= 0 ; i<keywords_len; i++) {
if(!strcmp(KEYWORDS[i], str)) {
returntrue;
}
}
returnfalse;
}

char*substring(char* _string_ ,int _position_ ,int _length_ )
{
char*p;
intc;

```
p=malloc(length+ 1 );
```
```
if(p==NULL)
{
printf("Unable to allocate memory.\n");
exit( 1 );
}
```
```
for(c= 0 ; c<length; c++)
{
*(p+c)=*(string+position- 1 );
string++;
}
```
```
*(p+c)='\0';
```
returnp;
}

boolvariableExists(char* _variable_ ){
if(variableIndex== 0 )
returnfalse;
for(inti= 0 ; i<(variableIndex); i++)
if(!strcmp(variable,variables[i]))
returntrue;
returnfalse;
}

voidundeclarationError(char _identifier_ []) {
printf("ERROR: line:%d| Identifier%scalledbefore declaration!\n", line, identifier);


exit( 0 );
}

voidsyntaxError(char _type_ [],char _expected_ [],char _previous_ []) {
printf("ERROR: line:%d| Unvalid%sstatement=>%sexpected after%s.\n", line, type, expected,previous);
exit( 0 );
}

_// Getting the index of a variable by the name ofit_
intgetIndex(char* _name_ )
{
if(variableIndex== 0 )
return- 1 ;
for(inti= 0 ; i<(variableIndex); i++)
if(!strcmp(name,variables[i]))
returni;
return- 1 ;
}

_// Assignment functionality_
voidmove(char* _token_ [])
{
intindex=getIndex(token[ 7 ]);
if(!strcmp(token[ 2 ],"Identifier"))
values[index]=values[getIndex(token[ 3 ])];

```
if(!strcmp(token[ 2 ],"IntConstant"))
values[index]=atoi(token[ 3 ]);
```
_// printf("Moved %d to %s\n", values[index], variables[index]);_
}

_// Print functionality_
voidout(char* _tokens_ [],int _tokens_size_ )
{
charnew_tokens[ 1000 ][ 1000 ];
intnew_tokens_len= 0 ;

```
charstring_holder[ 1000 ];
intholder_len= 0 ;
```
```
intstr= 0 ;
```
```
for(inti= 0 ; i<tokens_size; i++) {
```
```
if(str== 0 ) {
if(strstr(tokens[i],"\"")) {
strcat(string_holder,tokens[i]);
holder_len++;
str++;
}
else{
strcpy(new_tokens[new_tokens_len],tokens[i]);
}
}
elseif(str== 1 ) {
if(strstr(tokens[i],"\"")) {
strcat(string_holder,tokens[i]);
strcpy(new_tokens[new_tokens_len],string_holder);
```

string_holder[ 0 ]='\0';
holder_len= 0 ;
str--;
}
else{
strcat(string_holder,tokens[i]);
}
}
else{
printf("never should have come here");
}
}
intcounter= 0 ;
_// Looping through the array and execute all thepossible executables_
while(tokens[counter]!=NULL)
{
if(!strcmp(tokens[counter],"Seperator"))
{
counter++;
continue;
}
elseif(!strcmp(tokens[counter],"Identifier"))
printf("%d",values[getIndex(tokens[counter+ 1 ])]);
elseif(!strcmp(tokens[counter],"IntConstant"))
printf("%s",tokens[counter+ 1 ]);
elseif(!strcmp(tokens[counter],"newline"))
printf("\n");
elseif(!strcmp(tokens[counter],"StringConstant"))
{
while(tokens[counter]!=NULL&&strcmp(tokens[counter+ 1 ],"Seperator"))
{
counter++;
if(!strcmp(tokens[counter],"EndOfLine"))
break;
if(holder_len== 1 &&(!strcmp(tokens[counter+ 1 ],"Seperator")||!strcmp(tokens[counter],
"EndOfLine")))
{
char*temp=substring(tokens[counter], 2 ,strlen(tokens[counter])- 2 );
printf("%s",temp);
temp="";
}

```
else
{
if(counter== 6 )
{
char*temp=substring(tokens[counter], 2 ,strlen(tokens[counter]));
printf("%s",temp);
}
elseif((!strcmp(tokens[counter+ 1 ],"Seperator")||!strcmp(tokens[counter],"EndOfLine")))
{
char*temp=substring(tokens[counter], 1 ,strlen(tokens[counter])- 1 );
printf("%s",temp);
}
else
{
printf("%s",tokens[counter]);
}
}
```

#### }

#### }

counter++;
}
}

_// Declaration functionality_
voiddeclaration(char* _tokens_ [])
{
variables[variableIndex]=tokens[ 3 ]; _// 0 whenusing, then 1_
values[variableIndex]= 0 ; _// 1_
variableIndex++;
}

_// Addition functionality_
voidadd(char* _token_ [])
{
intindex=getIndex(token[ 7 ]);
if(!strcmp(token[ 2 ],"Identifier"))
values[index]+=values[getIndex(token[ 3 ])];

if(!strcmp(token[ 2 ],"IntConstant"))
values[index]+=atoi(token[ 3 ]);
}

_// Subtraction functionality_
voidsub(char* _tokens_ [])
{
intindex=getIndex(tokens[ 7 ]);

```
if(!strcmp(tokens[ 2 ],"Identifier"))
values[index]-=values[getIndex(tokens[ 3 ])];
```
if(!strcmp(tokens[ 2 ],"IntConstant"))
values[index]-=atoi(tokens[ 3 ]);
}

_// Loop functionality_
voidloop(char* _tokens_ [],int _tokens_size_ ,char* _type_ )
{
charinside_loop_matris[ 100 ][ 1000 ];
intlines= 0 ;
inttimes= 0 ;

```
charidentifier[ 21 ];
intidentifier_index;
```
```
// Finding the valid time for the specific command
if(!strcmp(tokens[ 2 ],"Identifier")) {
strcpy(identifier,tokens[ 3 ]);
identifier_index=getIndex(tokens[ 3 ]);
times=values[identifier_index];
}
elseif(!strcmp(tokens[ 2 ],"IntConstant"))
times=atoi(tokens[ 3 ]);
```
```
if(!strcmp("oneline", type)) {
for(inti= 6 ; i<tokens_size; i++) {
```

```
strcat(inside_loop_matris[ 0 ],tokens[i]);
if(i!=tokens_size- 1 ) {
strcat(inside_loop_matris[ 0 ]," ");
}
}
lines= 1 ;
}
elseif(!strcmp("block", type)) {
intlooper= 0 ;
for(inti= 7 ; i<tokens_size- 1 ; i++)
{
strcat(inside_loop_matris[lines],tokens[i]);
if(i!= 6 ) {
strcat(inside_loop_matris[lines]," ");
}
```
```
if(!strcmp(tokens[i],"OpenBlock")){
looper++;
}
elseif(!strcmp(tokens[i],"CloseBlock")){
looper--;
lines++;
}
elseif(!strcmp("EndOfLine",tokens[i])&&looper== 0 ){
lines++;
}
}
}
```
```
charrun[ 1000 ][ 1000 ];
intrun_len=times*lines;
```
```
for(inti= 0 ; i<run_len;) {
for(intj= 0 ; j<lines; j++) {
strcat(run[i],inside_loop_matris[j]);
i++;
}
}
```
for(inti= 0 ; i<run_len; i++) {
evaluate(run[i]);
if(!strcmp(tokens[ 2 ],"Identifier")&&i%(lines)==lines- 1 ) {
values[identifier_index]--;
}
}
}

_// Evaluation and Execution_
voidevaluate(char* _statement_ ) {
intstrconst= 0 ;
char*tokens[ 1000 ]={'\0'};
inttokens_size= 0 ;
intwordCount= 0 ;
char*splitedWords=strtok(statement," ");
while(splitedWords!=NULL) {
tokens[wordCount++]=splitedWords;
splitedWords=strtok(NULL," ");
tokens_size++;


#### }

chartype[ 20 ]={"\0"};

if(!strcmp(tokens[ 0 ],"Keyword")){
_// declaration_
if(!strcmp(tokens[ 1 ],"int")) {
strcpy(type,"Declaration");
if(!strcmp(tokens[ 2 ],"Identifier")){
if(variableExists(tokens[ 3 ])) {
printf("ERROR at line%d: Doubledeclaration of variable%s\n", line,tokens[ 3 ]);
exit( 0 );
}
if(!strcmp(tokens[ 4 ],"EndOfLine")){
declaration(tokens);
}else{
syntaxError(type,"EndOfLine",tokens[ 3 ]);
}
}
else{
syntaxError(type,"Identifier",tokens[ 1 ]);
}
}
_// assignment_
elseif(!strcmp(tokens[ 1 ],"move")) {
strcpy(type,"Assignment");
if(!strcmp(tokens[ 2 ],"Identifier")||!strcmp(tokens[ 2 ],"IntConstant")) {
if(!strcmp(tokens[ 2 ],"Identifier")&&!variableExists(tokens[ 3 ]) ) {
undeclarationError(tokens[ 3 ]);
}
if(!strcmp(tokens[ 4 ],"Keyword")&&!strcmp(tokens[ 5 ],"to")) {
if(!strcmp(tokens[ 6 ],"Identifier")){
if(!variableExists(tokens[ 7 ])){
undeclarationError(tokens[ 7 ]);
}
if(!strcmp(tokens[ 8 ],"EndOfLine")){
move(tokens);
}else{
syntaxError(type,"EndOfLine",tokens[ 7 ]);
}
}else{
syntaxError(type,"Identifier",tokens[ 5 ]);
}
}else{
syntaxError(type,"Keyword to",tokens[ 2 ]);
}
}else{
syntaxError(type,"Identifier or IntConstant",tokens[ 1 ]);
}
}
_// addition_
elseif(!strcmp(tokens[ 1 ],"add")) {
strcpy(type,"Addition");
if(!strcmp(tokens[ 2 ],"Identifier")||!strcmp(tokens[ 2 ],"IntConstant")) {
if(!strcmp(tokens[ 2 ],"Identifier")&&!variableExists(tokens[ 3 ]) ) {
undeclarationError(tokens[ 3 ]);
}
if(!strcmp(tokens[ 4 ],"Keyword")&&!strcmp(tokens[ 5 ],"to")) {
if(!strcmp(tokens[ 6 ],"Identifier")){


if(!variableExists(tokens[ 7 ])) {
undeclarationError(tokens[ 7 ]);
}
if(!strcmp(tokens[ 8 ],"EndOfLine")){
add(tokens);
}else{
syntaxError(type,"EndOfLine",tokens[ 7 ]);
}
}else{
syntaxError(type,"Identifier",tokens[ 5 ]);
}
}else{
syntaxError(type,"Keyword to",tokens[ 3 ]);
}
}else{
syntaxError(type,"Identifier or IntConstant",tokens[ 1 ]);
}
}
_// subtraction_
elseif(!strcmp(tokens[ 1 ],"sub")) {
strcpy(type,"Subtraction");
if(!strcmp(tokens[ 2 ],"Identifier")||!strcmp(tokens[ 2 ],"IntConstant")) {
if(!strcmp(tokens[ 2 ],"Identifier")&&!variableExists(tokens[ 3 ]) ) {
undeclarationError(tokens[ 3 ]);
}
if(!strcmp(tokens[ 4 ],"Keyword")&&!strcmp(tokens[ 5 ],"from")) {
if(!strcmp(tokens[ 6 ],"Identifier")){
if(!variableExists(tokens[ 7 ])){
undeclarationError(tokens[ 7 ]);
}
if(!strcmp(tokens[ 8 ],"EndOfLine")){
sub(tokens);
}else{
syntaxError(type,"EndOfLine",tokens[ 7 ]);
}
}else{
syntaxError(type,"Identifier",tokens[ 5 ]);
}
}else{
syntaxError(type,"Keyword from",tokens[ 3 ]);
}
}else{
syntaxError(type,"Identifier or IntConstant",tokens[ 1 ]);
}
}
_// output_
elseif(!strcmp(tokens[ 1 ],"out")) {
intcounter= 2 ;
while(tokens[counter]!=NULL)
{
if(!strcmp(tokens[counter],"Keyword"))
{
if(!strcmp(tokens[counter+ 1 ],"newline"))
{
if(!strcmp(tokens[counter+ 2 ],"EndOfLine")||!strcmp(tokens[counter+ 2 ],"Seperator"))
{
counter+= 2 ;
continue;
}


```
else
{
printf("ERROR: line:%d|Invalid out syntax\n", line);
exit( 0 );
}
```
}
else
{
printf("ERROR: line:%d| Invalidout syntax\n", line);
exit( 0 );
}
}
elseif(!strcmp(tokens[counter],"IntConstant"))
{
if(atoi(tokens[counter+ 1 ])!=NULL)
{
if(!strcmp(tokens[counter+ 2 ],"EndOfLine")||!strcmp(tokens[counter+ 2 ],"Seperator"))
{
counter+= 2 ;
continue;
}
else
{
printf("ERROR: line:%d|Invalid out syntax\n", line);
exit( 0 );
}
}
else
{
printf("ERROR: line:%d| Invalidout syntax\n", line);
exit( 0 );
}

}
elseif(!strcmp(tokens[counter],"Identifier"))
{
if(variableExists(tokens[counter+ 1 ]))
{
if(!strcmp(tokens[counter+ 2 ],"EndOfLine")||!strcmp(tokens[counter+ 2 ],"Seperator"))
{
counter+= 2 ;
continue;
}
else
{
printf("ERROR: line:%d|Invalid out syntax\n", line);
exit( 0 );
}
}
else
{
printf("ERROR: line:%d| Variabledoes not existed!\n", line);
exit( 0 );
}
}
elseif(!strcmp(tokens[counter],"StringConstant"))
{
counter++;


```
while(strcmp(tokens[counter],"EndOfLine"))
{
```
```
if(!strcmp(tokens[counter],"Seperator"))
{
counter++;
break;
}
counter++;
```
```
}
if(!strcmp(tokens[counter],"Identifier")||!strcmp(tokens[counter],"Keyword")||
!strcmp(tokens[counter],"IntConstant")||!strcmp(tokens[counter],"StringConstant")||
!strcmp(tokens[counter],"EndOfLine")||!strcmp(tokens[counter],"Seperator"))
{
continue;
}
else
{
printf("ERROR: line:%d| Sepraterdoes not exist!\n", line);
exit( 0 );
}
}
counter++;
}
```
```
if(!strcmp(tokens[counter- 1 ],"EndOfLine")&&strcmp(tokens[counter- 2 ],"Seperator"))
out(tokens, tokens_size);
```
else
printf("ERROR: line:%d| Invalid out syntax\n",line);
}
_// loop_
elseif(!strcmp(tokens[ 1 ],"loop")) {
if(!strcmp(tokens[ 2 ],"Identifier")||!strcmp(tokens[ 2 ],"IntConstant")) {
if(!strcmp(tokens[ 2 ],"Identifier")&&!variableExists(tokens[ 3 ]) )
undeclarationError(tokens[ 3 ]);

```
if(!strcmp(tokens[ 5 ],"times")) {
if(!strcmp(tokens[ 6 ],"OpenBlock")){
loop(tokens, tokens_size,"block");
}
elseif(!strcmp(tokens[ 6 ],"Keyword")){
loop(tokens, tokens_size,"oneline");
}
else
syntaxError(type,"OpenBlockor a starting Keyword",tokens[ 5 ]);
}
else
syntaxError(type,"Keyword times",tokens[ 4 ]);
```
}
else{
syntaxError(type,"Identifier or IntConstant",tokens[ 1 ]);
}
}
}
else{
printf("ERROR at line%d: Statement shouldstart with a valid keyword!\n", line);


exit( 0 );
}
}


