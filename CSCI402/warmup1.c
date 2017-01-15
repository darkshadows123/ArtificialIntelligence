#ifndef _WARMUP1_H_
#define _WARMUP1_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "my402list.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#define MAXLINELENGTH 1024
extern int errno;

typedef struct transaction_record
{
	char transaction_type;
	long int transaction_time;
	unsigned long transaction_amount;
	char * 	transaction_description;
} My402Transaction;

FILE* ValidateArguments(int argc, char const *argv[]) {
	if (argc < 2 || argc > 3 || strcmp(argv[1],"sort") != 0 ) {
		fprintf(stderr, "%s\n", "Malformed command!!! \n 	Usage : program_name sort input_file(optional argument)\n");
		exit(0);
	}
	FILE *fp = NULL;
	int errnum;
	if (argc == 2) {
		fp = stdin;
	} else {
		struct stat path_stat;
		if ( (stat(argv[2], &path_stat) == 0) && (path_stat.st_mode & S_IFDIR)) {
			fprintf(stderr, "input file %s is a directory\n", argv[2]);
			exit(0);
		} else if (stat(argv[2], &path_stat) != 0 ) {
			fprintf(stderr, "input file %s does not exist\n", argv[2]);
			exit(0);
		}
		fp = fopen(argv[2],"r");
		if (fp == NULL) {
			errnum = errno;
			fprintf(stderr, "Error opening file %s: %s\n", argv[2], strerror( errnum ));
			exit(0);
		}
	}
	return fp;
}

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
	
My402Transaction* ValidateTokens(char* tokens[4]) {
	My402Transaction* my402t = (My402Transaction*) malloc(sizeof(My402Transaction));
	
	//check transaction type
	if (strlen(tokens[0]) != 1 && (strcmp(tokens[0],"+") != 0 || strcmp(tokens[0],"-") != 0) ) {
		fprintf(stderr, "%s\n", "transaction type should be of only 1 character and it be either + or i");
		exit(0);
	}

	printf("Transaction type = %s\n", tokens[0]);
	my402t->transaction_type = tokens[0][0];
	//check transaction time
	if (strlen(tokens[1]) >= 11) {
		fprintf(stderr, "%s\n", "Invalid transaction time. Too large");
		exit(0);
	}
	if (atol(tokens[1]) >= 0 && atol(tokens[1]) <= time(NULL)) {}
	else {
		fprintf(stderr, "%s\n", "Invalid transaction time. Should be between 0 and current time");
		exit(0);
	}
	printf("Transaction time = %ld\n", atol(tokens[1]));
	my402t->transaction_time = atol(tokens[1]);
	// check transaction amount
	int d ;
	int i = 0;
	unsigned long amount = 0;
	int beforeDecimalLen = 0;
	// printf("Transaction amount = %s\n", tokens[2]);
	while (tokens[2][i] != '.') {
		d = tokens[2][i];
		// printf("%d\n", isdigit(d));
		i++;
		if (isdigit(d)) {
			amount = amount * 10 + (d-'0');
			beforeDecimalLen++; 
		} else {

		// printf("%c\n", (d));
			fprintf(stderr, "%s\n", "Digits in transaction amount should be numeric (0-9)");
			exit(0);
		}
	}
	if (beforeDecimalLen > 7 || beforeDecimalLen == 0) {
		fprintf(stderr, "%s\n", "Maximum transaction amount should be less than 10000000 and greater than 0");
		exit(0);
	}
	i++;
	int afterDecimalLen = 0;
	while (tokens[2][i] != '\0') {
		d = tokens[2][i];
		i++;
		if (isdigit(d)) {
			amount = amount * 10 + (d-'0');
			afterDecimalLen++; 
		} else {
			fprintf(stderr, "%s\n", "Digits in transaction amount should be numeric (0-9)");
			exit(0);
		}
	}
	if (afterDecimalLen != 2) {
		fprintf(stderr, "%s\n", "Digits after decimal should be excatly 2");
		exit(0);
	}
	my402t->transaction_amount = amount;
	printf("Transaction amount = %s %ld\n", tokens[2], amount);

	char* tmp = strdup(tokens[3]);
	tmp = trimwhitespace(tmp); // need to modify this function

	if (strlen(tmp) > 0) {
		my402t->transaction_description = strdup(tmp);
	} else {
		fprintf(stderr, "%s\n", "Transaction description should not be empty");
		exit(0);
	}
	printf("Transaction description = %s\n", tmp);
	free(tmp);

	return my402t;
}

void CreateTransactionObj(char* line, My402List* my402list) {
	char* tmp = strdup(line);
	tmp = trimwhitespace(tmp); // need to modify this function
	line = NULL;
    line = (char *) malloc(strlen(tmp)*sizeof(char));
	strncpy(line, tmp, strlen(tmp));
	line[strlen(tmp)] = '\0';
	free(tmp);

	
	tmp = strdup(line);
	char *tokens[4];
	char *chunk;
	chunk = strtok(tmp, "\t");
	int count = 0 ;
	while( chunk != NULL ) {
	  tokens[count] = strdup(chunk);
	  printf( " %s\n", chunk );
	  chunk = strtok(NULL, "\t");
	  count++;
	}
	free(chunk);
	free(tmp);
	if (count != 4) {
		fprintf(stderr, "%s\n", "Each row of transaction should have four columns");
		exit(0);
	}

	My402Transaction* my402t = NULL;
	my402t = ValidateTokens(tokens);
	My402ListAppend(my402list, (void*) my402t);
}

void CreateTransactionList(FILE* fp, My402List* my402list) {
	char buf[1026];
	while (fgets (buf, sizeof(buf), fp) != NULL) {
		if (strcmp(buf,"\n") != 0) {
			if (strlen(buf) > MAXLINELENGTH) {
				fprintf(stderr, "%s\n", "Error !!! Length of the line is too big (should be less than 1024).. ");
				exit(0);
			}		
			CreateTransactionObj(buf, my402list);
		}
	}
}

void SortTransactionList(My402List* my402list) {
	void* tempObj;
	My402ListElem* my402elem =  My402ListFirst(my402list);
	My402ListElem* my402elemnext = NULL;
	while (my402elem != NULL) {
		my402elemnext = My402ListNext(my402list, my402elem);
		while(my402elemnext != NULL) {
			if (((My402Transaction*)my402elem->obj)->transaction_time 
					== ((My402Transaction*)my402elemnext->obj)->transaction_time) {
				fprintf(stderr, "%s\n", "Timestamp of two transactions cannot be same");
				exit(0);
			} else if (((My402Transaction*)my402elem->obj)->transaction_time 
					> ((My402Transaction*)my402elemnext->obj)->transaction_time) {
				tempObj = my402elem->obj;
				my402elem->obj = my402elemnext->obj;
				my402elemnext->obj = tempObj;
			}
			my402elemnext=My402ListNext(my402list, my402elemnext);
		}
		my402elem = My402ListNext(my402list, my402elem);
	}
	// my402elem =  My402ListFirst(my402list);
	// while(my402elem != NULL) {
	// 	printf("%ld\n", ((My402Transaction*)my402elem->obj)->transaction_time);
	// 	my402elem = My402ListNext(my402list,my402elem);
	// }
}

void printColumnNames() {
	printf("+-----------------+--------------------------+----------------+----------------+\n");
	printf("|       Date      | Description              |         Amount |        Balance |\n");
	printf("+-----------------+--------------------------+----------------+----------------+\n");
}

void printDate(long int t) {
	time_t tr_timestamp = (time_t)t;
	
	char *trans_timestamp = ctime(&tr_timestamp);
	char *final_time = (char*) malloc(16 * sizeof(char));
	snprintf(final_time,11,"%s",trans_timestamp);
	snprintf(final_time + 10,6,"%s",trans_timestamp + 19);
	printf("| %s |", final_time);
}

void printDescription(char* desc) {

}

void PrintTransactionList(My402List* my402list) {
	printColumnNames();
	My402ListElem * my402listelem = NULL;
	for (my402listelem = My402ListFirst(my402list); my402listelem != NULL; my402listelem = My402ListNext(my402list, my402listelem)) {
		printDate(((My402Transaction*) my402listelem->obj)->transaction_time);
		printDescription(((My402Transaction*) my402listelem->obj)->transaction_description);
	}
}

int main(int argc, char const *argv[]) {
	FILE *fp = NULL;
	fp = ValidateArguments(argc, argv);

	My402List my402list;
	(void)My402ListInit(&my402list);
	(void)CreateTransactionList(fp,&my402list);
	(void)SortTransactionList(&my402list);
	(void)PrintTransactionList(&my402list);
	//printf("%d\n", My402ListLength(&my402list));
	return 0;
}

#endif /*_WARMUP1_H_*/