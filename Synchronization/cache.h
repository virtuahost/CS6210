#include<iostream>
#include<unordered_map>
#include<string>
#include<fstream>
#include<sstream>
#include<stdlib.h>
#include<time.h>
#include "url_processing.h"

using namespace std;

// *********************************************************************************************************************
// Global Variables
// *********************************************************************************************************************
int replacement_policy = 0;
unsigned int cache_size = 1024;
unsigned int access_count = 0;
unsigned int running_size = 0;
unsigned int miss_count = 0;
double miss_rate = 0;
double average_access_time = 0;

// *********************************************************************************************************************
// Data Structure
// *********************************************************************************************************************
struct Page_Info
{
	string page_data;
	int page_entry;
	int page_size;	
};

unordered_map<string, Page_Info> cache;
unordered_map<string, Page_Info> :: iterator i_cache;

// *********************************************************************************************************************
// Function Prototypes
// *********************************************************************************************************************

// Cache 
string cache_hit_miss(string);
void cache_update(string, string, int);
void cache_handler(int, string ,unsigned int& ,unsigned int& );
void print_cache(void);
void print_stats(void);

// Replacement
void replacement_algo(int);
void random_evict(void);
void lru_evict(void);
void lsf_evict(void);

// *********************************************************************************************************************
// Function Definitions
// *********************************************************************************************************************

string cache_hit_miss(string url)
{
	i_cache = cache.find(url);
	if(i_cache == cache.end())
	{
		miss_count++;
		return "not found ... ";
	}
	else 
	{
		return cache[url].page_data;
	}
}

void cache_update(string url)
{
	int size;
	cout<<"\n\n Fetching URL "<<endl;
	string url_data = fetch_url(url);
	size = url_data.length();
	cout<<"\n\n Size of Page "<<size;
	cout<<"\n\n Size of Cache "<<cache_size;
	while(cache_size < (size + running_size))
	{
		replacement_algo(replacement_policy);
		if(running_size == 0) break;
	}	
		
	if(cache_size >= (size + running_size))
	{
		running_size += size;
		Page_Info pi;
		pi.page_data = url_data;
		pi.page_entry = access_count;
		pi.page_size = size;
		cache.insert(pair<string, Page_Info>(url, pi));	
	}
}

void cache_handler(int option, string url,unsigned int& a_cnt,unsigned int& m_cnt)
{
	cout<<"\n\n Entering Handler "<<endl;
	string check = cache_hit_miss(url);
	cout<<"\n\n Cache hit "<<check;
	if(check.compare("not found ... ") == 0)
		cache_update(url);
	else
		cache[url].page_entry = access_count;

	access_count++;	
	a_cnt = access_count;
	m_cnt = miss_count;
}

// Random Selection
void random_evict()
{
	unordered_map<string, Page_Info> :: iterator it = cache.begin();
	advance(it, rand()%cache.size());
	running_size -= cache[it->first].page_size;
	cache.erase(it);
}

// Least Recently Used
void lru_evict()
{
	i_cache = cache.begin();
	unordered_map<string, Page_Info> :: iterator lru = i_cache;	
	for(i_cache = cache.begin(); i_cache != cache.end(); ++i_cache)
	{
		if(cache[i_cache->first].page_entry < cache[lru->first].page_entry) 
			lru = i_cache;
	}		
	running_size -= cache[lru->first].page_size;
	cache.erase(lru->first);
}

// Largest Size First
void lsf_evict()
{
	i_cache = cache.begin();
	unordered_map<string, Page_Info> :: iterator lsf = i_cache;	
	for(i_cache = cache.begin(); i_cache != cache.end(); ++i_cache)
	{
		if(cache[i_cache->first].page_size > cache[lsf->first].page_size) 
			lsf = i_cache;
	}		
	running_size -= cache[lsf->first].page_size;
	cache.erase(lsf->first);
}

// Arbitration
void replacement_algo(int option)
{
	switch(option)
	{
		case 1	:	random_evict();
						break;
						
		case 2	:	lru_evict();
						break;
						
		case 3	:	lsf_evict();
						break;
	
		default	:	break;
	}
}

void print_cache()
{
	unordered_map<string, Page_Info> :: iterator i = cache.begin();
	cout<<"\n ************* Cache Contents ************ \n\n";
	for(i = cache.begin(); i != cache.end(); ++i)
		cout<<" KEY : "<<i->first<<"\t VALUE : "<<cache[i->first].page_data<<"\n";	
	cout<<"\n ***************************************** \n\n";
}

void print_stats()
{
	cout<<"\n\n ****************** STATISTICS **********************\n";
	access_count -= 1; 
	miss_rate = (miss_count*1.0/access_count);
	cout<<"\n\n Total Cache Access = "<<access_count;
	cout<<"\n Cache Misses = "<<miss_count;
	cout<<"\n Cache Hits = "<<(access_count-miss_count);
	cout<<"\n Miss Rate = "<<miss_rate;
	cout<<"\n\n\t *** Assuming a cache hit time of 1us and cache miss recovery time as 100us ***";
	cout<<"\n\n\t     Average Access Time = "<<(1 + miss_rate*100)<<" us \n\n";
	
	cout<<"\n ***********************************************\n\n";	
}




