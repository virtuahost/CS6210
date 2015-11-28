#include "rvm.h"
#include <errno.h>
#include <fstream>
#include <iostream>
#include <dirent.h>

using namespace fstream;

//************************************** ALL UTILITY FUNCTIONS****************************************************//
int isSegmentMapped(rvm_t *obj_name, const char* segname)
{
 	return (obj_name.segments.find(segname)!=obj_name.segments.end());
}

int write_log(char *log_data, int size, const char *file_name, fstream &iodata)
{
	fstream fs(file_name,in|out|binary);

	if(fs)
	{
		char* offset;
		size_t length;
		bool done = false;
	
		log_file.seekg(0,ios_base::beg);
	
		while(log_file.read(offset,sizeof(int)))
		{
			done = true;
			log_file.read((char *)&length,sizeof(size_t));
			log_file.read(log_data + offset,length);
		}

		log_file.close();

		if(done)
		{
			iodata.seekp(0,ios_base::beg);
			iodata.write((char *)log_data,size);

			log_file.open(log_file_name,in|out|binary|trunc);
			log_file.close();
		}
	}
}

const char * find_name(rvm_t *obj_name,void * segbase)
{
	for(map<const char *, void *> >::iterator iter = obj_name.segments.begin(); iter != obj_name.segments.end(); iter++)
	{
		if(iter->second.data == segbase)
		{
			return iter->first;
		}
	}
	return NULL;
}

//************************************** ALL REQUIRED FUNCTIONS***************************************************//
rvm_t rvm_init(const char *directory);
{
	if(directory == NULL)
	{
		return (rvm_t)NULL; //Handle null directory by sending back a null rvm_t
	}
	rvm_t new_obj;
	int ret_val = mkdir(directory, 0755);
	if(ret_val == -1 && errno != EEXIST)
	{
		return (rvm_t)NULL;
	}
	strcpy(new_obj.directory,directory);
	return new_obj;
}

void *rvm_map(rvm_t rvm, const char *segname, int size_to_create);
{
	int seg_size = 0;
	string segment_file;
	char * seg_data;
	if(rvm == (rvm_t)NULL || segname == NULL || size_to_create < 0)
	{
		return (void *)-1;	//Error code -1 for input error.
	}
	
	segment_file = rvm.directory + "/" + segname + ".seg";

	if(isSegmentMapped(rvm,segname) == 0)
	{
		return (void *)-2;	//Error code -2 for already mapped segment.
	}
	
	fstream iodata;
	iodata.open(segment_file.c_str(),binary|in|out);

	iodata.seekg(0,ios_base::end);
	seg_size = iodata.tellg();

	if(size_to_create != 0 && size_to_create > seg_size)
	{
		if(!(iodata.seekp(size_to_create-1,ios_base::beg)))
		{
			iodata.close();
			return (void *)-3; //Error code 3 if segment size cannot be set
		}
	}
	else
	{
		size_to_create = seg_size;
	}

	if(!(iodata.seekg(0,ios_base::beg)))
	{
		iodata.close();
		return (void *)-3;	//Error code 3 if segment size cannot be set
	}

	seg_data = new char[size_to_create];

	if(seg_data == NULL)
	{
		iodata.close();
		return (void *)-3;	//Error code 3 if segment size cannot be set
	}

	if(!(iodata.read((char *)seg_data,size_to_create)))
	{
		iodata.close();
		delete [] seg_data;
		return (void *)-3;	//Error code 3 if segment size cannot be set
	}
	//Write to redo log
	segment_file.append("_l");
	write_log(seg_data,size_to_create,segment_file.c_str(),iodata);
	iodata.close();
	rvm_seg inseg;
	inseg.data = seg_data;
	inseg.size = size_to_create;
	rvm.segments.insert(make_pair(seg_name,inseg));
	return (void *)0;	//Success
}

void rvm_unmap(rvm_t rvm, void *segbase);
{
	if(rvm == (rvm_t)NULL || rvm == (rvm_t)-1 || rvm == (rvm_t)-2 || rvm == (rvm_t)-3)
	{
		return;
	}
	
	rvm.segments.erase(rvm.find_name(segbase));
}

void rvm_destroy(rvm_t rvm, const char *segname);
{
	string segment_file;
	if(isSegmentMapped(rvm,segname))
	{
		return;
	}
	segment_file = rvm.directory + "/" + segname + ".seg";
	unlink(segment_file.c_str());
	segment_file.append("_l");
	unlink(segment_file.c_str());
}

trans_t rvm_begin_trans(rvm_t rvm, int numsegs, void **segbases);
{
	int i = 0;
	trans_t trans;
	string fname;
	if(rvm == (rvm_t)NULL || numsegs < 0 || segbases == NULL)
	{
		return (trans_t)-1; //Error code for input errors
	}
	//Create transaction
	trans.trans_context = rvm;
	for(i = 0; i < numsegs;i++)
	{
		map<void *, const char * >::iterator iter = rvm.segments.find(segbases[i]);

		if( iter == rvm.segments.end())
		{
			delete trans;
			return (trans_t)-1;	//Error code for segment not found
		}

		if( rvm.inTrans.find(segbases[i]) != rvm.inTras.end())
		{
			delete trans;
			return (trans_t)-1; //Error code for segment already in transaction
		}

		fname = rvm.directory + "/" + iter->first + ".seg_l";

		list<rvm_logdata> list_of_changes;

		trans.log_data_list.insert(make_pair(segbases[i],list_of_changes));

		rvm.inTrans.insert(segbases[i]);

	}
	return trans;
}

void rvm_about_to_modify(trans_t tid, void *segbase, int offset, int size);
{
	if(tid == (trans_t)NULL || tid == (trans_t)-1 || offset < 0 || size < 0)
	{
		return;
	}
	map<void *, list<rvm_logdata> >::iterator iter = tid.log_data_list.find(segbase);
	if(iter == tid.log_data_list.end())
	{
		return;
	}
	rvm_logdata seg_log;
	seg_log.data = segbase;
	seg_log.offset = offset;
	seg_log.length = size;
	iter->push_back(seg_log);
}

void rvm_commit_trans(trans_t tid);
{
	string log_seg_name;
	ofstream fls;
	int retval = 0;
	if(tid == (trans_t)NULL || tid == (trans_t)-1)
	{
		return;
	}

	rvm_t curr_rvm = tid.trans_context;
	for(map<void *, list<rvm_logdata> >::iterator iter = trans.log_data_list.begin(); iter != trans.log_data_list.end(); iter++)
	{
		log_seg_name = curr_rvm.directory + "/" + iter + ".seg_l";

		retval = fls.open(log_seg_name.c_str(),out|binary|trunc);
		for(list<rvm_logdata>::iterator seg_iter = iter->second.begin(); seg_iter != iter->second.end(); seg_iter++)
		{
			fls.write((char *)&(seg_iter->offset),sizeof(int));
			fls.write((char *)&(seg_iter->length),sizeof(int));
			fls.write((char *)iter->first + seg_iter->offset,length);
		}
		fls.close();
		curr_rvm.inTrans.erase(iter->first);
	}
	delete tid;
}

void rvm_abort_trans(trans_t tid);
{
	if(tid == (trans_t)NULL || tid == (trans_t)-1)
	{
		return;
	}
	rvm_t curr_rvm = tid.trans_context;
	for(map<void *, list<rvm_logdata> >::iterator iter = trans.log_data_list.begin(); iter != trans.log_data_list.end(); iter++)
	{
		for(list<rvm_logdata>::iterator seg_iter = iter->second.begin(); seg_iter != iter->second.end(); seg_iter++)
		{
			memcpy((char *)iter->first + seg_iter->offset, seg_iter->data, seg_iter->length);
		}
		curr_rvm.inTrans.erase(iter->first);
	}
}

void rvm_truncate_log(rvm_t rvm);
{
	string fname, segname, segnamel;
	int found, size;
	if(rvm == (rvm_t)NULL || rvm == (rvm_t)-1 || rvm == (rvm_t)-2 || rvm == (rvm_t)-3)
	{
		return;
	}
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (rvm.directory.c_str())) != NULL) {
	  while ((ent = readdir (dir)) != NULL) {
	    fname = ent->d_name;
	    found = fname.rfind(".seg_l");
	    if(found != string::npos)
	    {
	    	segname = fname.substr(0,found);
	    	segnamel = rvm.directory + "/" + file;
	    	if(isSegmentMapped(segname.c_str()))
	    	{
	    		map<const char *, void *>::const_iterator iter = rvm.segments.find(segname.c_str());
				segname = rvm.directory + "/" + segname + ".seg";
				fstream fs(segname.c_str(),in|out|binary|trunc);
				write_log(iter->first,iter->second,segnamel.c_str(),fs);
				fs.close();
	    	}
	    	else
	    	{
	    		void *addr = rvm_map(rvm, segname.c_str() ,0);
				if(addr != (void *) -1)
				{
					rvm_unmap(rvm,addr);
				}
	    	}
	    	unlink(segnamel.c_str());
	    }
	  }
	  closedir (dir);
	} else {
	  /* could not open directory */
	  return;
	}
}