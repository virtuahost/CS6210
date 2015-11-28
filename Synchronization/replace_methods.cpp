#include"cache.h"

extern int replacement_policy;
extern unsigned int cache_size;
extern unsigned int access_count;
extern unsigned int running_size;
extern map<string, Page_Info> cache;
extern map<string, Page_Info> :: iterator i_cache;

// Random Selection
void random_evict()
{
	map<string, Page_Info> :: iterator it = cache.begin();
	advance(it, rand()%cache.size());
	cout<<"\n Size before eviction : "<<cache.size()<<" ...";
	cout<<"\n Evicting data with key : "<<it->first<<" ... ";
	cache.erase(it->first);
	cout<<"\n Size after eviction : "<<cache.size()<<" ...";
	running_size -= cache[it->first].page_size;
}

// Least Recently Used
void lru_evict()
{
	i_cache = cache.begin();
	map<string, Page_Info> :: iterator lru = i_cache;	
	for(i_cache = cache.begin(); i_cache != cache.end(); ++i_cache)
	{
		if(cache[i_cache->first].page_entry < cache[lru->first].page_entry) 
			lru = i_cache;
	}		
	cout<<"\n Evicting data with key : "<<lru->first<<" ... ";
	cache.erase(lru->first);
	cout<<"\n Size after eviction : "<<cache.size()<<" ...";
	running_size -= cache[lru->first].page_size;
}

// Largest Size First
void lsf_evict()
{
	i_cache = cache.begin();
	map<string, Page_Info> :: iterator lsf = i_cache;	
	for(i_cache = cache.begin(); i_cache != cache.end(); ++i_cache)
	{
		if(cache[i_cache->first].page_size > cache[lsf->first].page_size) 
			lsf = i_cache;
	}		
	cout<<"\n Evicting data with key : "<<lsf->first<<" ... ";
	cache.erase(lsf->first);
	cout<<"\n Size after eviction : "<<cache.size()<<" ...";
	running_size -= cache[lsf->first].page_size;
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

