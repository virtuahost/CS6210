#include "cache.h"
#include "../url_processing.h"

extern int replacement_policy;
extern unsigned int cache_size;
extern unsigned int access_count;
extern unsigned int running_size;
extern map<string, Page_Info> cache;
extern map<string, Page_Info> :: iterator i_cache;

string cache_hit_miss(string url)
{
	i_cache = cache.find(url);
	if(i_cache == cache.end())
		return "not found ... ";
	else 
	{
		return cache[url].page_data;
	}
}

void cache_update(string url, string& url_data, int size)
{
	while(cache_size < (size + running_size))
	{
		cout<<"\n shudnt be here ... \n";
		replacement_algo(replacement_policy);
	}
	if(cache_size >= (size + running_size))
	{
		running_size += size;
		fetch_url(url, url_data);
		Page_Info pi;
		pi.page_data = *url_data;
		pi.page_entry = access_count;
		pi.page_size = size;
		cache.insert(pair<string, Page_Info>(url, pi));	
	}
}

void cache_handler(int option, string url, string url_data, int size,int& a_cnt, int& m_cnt)
{
	
	string check = cache_hit_miss(url);
	if(check.compare("not found ... ") == 0)
	{
		cout<<"\n Access "<<access_count<<" missed in cache ...";
		cache_update(url, url_data, size);
	}
	else
	{
		cache[url].page_entry = access_count;
	}
	access_count++;	
	a_cnt = access_count;
	m_cnt = miss_count;
}


