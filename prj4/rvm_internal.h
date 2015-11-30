#ifndef _RVM_INTERNAL_H
#define _RVM_INTERNAL_H

#include <string>
#include <map>
#include <list>

using namespace std;


//Structure to handle segment information for list in rvm_t
typedef struct _rvm_segment
{
	void * data;
	int size;
} rvm_seg;

//Structure to handle all recoverable memory data
typedef struct _rvm_t
{
	const char* directory; //Contains the directory to be used for writing logs.
	map<const char *, rvm_seg *> segments; //Stores name of every segment to be written.
	list<void *> inTrans; //To track segments in transaction.
} rvm_t;

//Structure to handle changes in logs
typedef struct _rvm_logdata
{
	void* data;	//Data of each log entry.
	int offset;		//Offset amount for writing in file.
	int length;

} rvm_logdata;

//Structure to handle all recoverable memory transactions
typedef struct _trans_t
{
	rvm_t *trans_context;	//rvm pointer for the transaction.
	map<void *, list<rvm_logdata> > log_data_list; //List of log for segments.
} trans_t;

#endif