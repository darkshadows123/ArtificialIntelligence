#ifndef _WARMUP1_H_
#define _WARMUP1_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define MAXLINELENGTH 1024
extern int errno;
typedef struct transaction_record
{
	char transaction_type;
	char * transaction_time;
	char * transaction_amount;
	char * 	transaction_description;
} transaction;

#endif /*_WARMUP1_H_*/