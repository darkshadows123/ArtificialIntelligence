#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/*http://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way*/
char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

int main() {
    char* line = "- 1230728833  45.33 Phone bill";

    char* tmp = (char *) malloc(60*sizeof(char));
    tmp = strdup(line);
    // tmp = trimwhitespace(tmp); // need to modify this function
    // line = NULL;
    line = (char *) malloc(strlen(tmp)*sizeof(char));
    strncpy(line, tmp, strlen(tmp));
    while (*line != '\0') {
      line++;
      printf("%c\n", *line);
    }
    printf("%d\n", strlen(tmp));
    printf("%d\n", strlen(line));
  //   = (char *) malloc(60*sizeof(char));
  //   //tmp = strdup(line);
  // strcpy(tmp, line);/// strlen(tmp));
  // printf("%s\n",tmp);
  // printf("asasasa\n");
  // free(tmp);
//  My402Transaction* my402t = (My402Transaction*) malloc(sizeof(my402t));

  tmp = strdup(line);
  char *tokens[4];
  char *chunk;
  char * del = " ";
  chunk = strtok(tmp, del);
  printf("asasasa\n");
  int count = 0 ;
  while( chunk != NULL ) {
    tokens[count] = strdup(chunk);
    printf( " %s\n", chunk );
    chunk = strtok(NULL, del);
    count++;
  }
  free(chunk);
  free(tmp);
return 0;

}
