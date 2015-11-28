// #include "cache.h"

#include "gen-cpp/URLproxy.h"
#include <string>       
#include <map>
#include <iostream>     
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
// using namespace apache::thrift::server;

using boost::shared_ptr;

using namespace::urlserver;
using namespace::std;

map<int,string> URL;
map<int,string> :: iterator i_URL;
int URL_mode;
int rand10_array[10];

void populate_URL_map()
{
	URL.insert(pair<int,string>(8,"google.com"));
	URL.insert(pair<int,string>(2,"yahoo.com"));
	URL.insert(pair<int,string>(3,"amazon.com"));
	URL.insert(pair<int,string>(4,"hotmail.com"));
	URL.insert(pair<int,string>(5,"gmail.com"));
	URL.insert(pair<int,string>(6,"mit.edu"));
	URL.insert(pair<int,string>(7,"yelp.com"));
	URL.insert(pair<int,string>(1,"youtube.com"));
	URL.insert(pair<int,string>(9,"ebay.com"));
	URL.insert(pair<int,string>(10,"espn.com"));
}

string generate_URL(int mode)
{
	static int i, j = 0;
	static int pattern_flag = 0;	
	if(mode == 1)
	{
		// random generator
		i = rand()%10 + 1;
		return URL[i];
	}
	else if(mode == 2)
	{
		// a-c-e-g-i- - - - 
		int x = (i%7) + 1;
		i += 1;
		if(rand()%10 > 4) return URL[rand()%10 + 1];
		return URL[x];
	}
	else 
	{
		if(pattern_flag == 0)
		{
			for(int k=0; k<10; k++)
			{
				rand10_array[k] =  rand()%10 + 1;	
				
			}
			pattern_flag = 1;
		}
		int index = (j%10);
		j++;	
		return URL[rand10_array[index]]; 	
	}

}

void print_end_stats(int miss_count_l,int access_count_l)
{
	cout<<"\n\n ****************** STATISTICS **********************\n";
	access_count_l -= 1; 
	int miss_rate = (miss_count_l*1.0/access_count_l);
	cout<<"\n\n Total Cache Access = "<<access_count_l;
	cout<<"\n Cache Misses = "<<miss_count_l;
	cout<<"\n Cache Hits = "<<(access_count_l-miss_count_l);
	cout<<"\n Miss Rate = "<<miss_rate;
	cout<<"\n\n\t *** Assuming a cache hit time of 1us and cache miss recovery time as 100us ***";
	cout<<"\n\n\t     Average Access Time = "<<(1 + miss_rate*100)<<" us \n\n";
	
	cout<<"\n ***********************************************\n\n";	
}


int main(int argc, char **argv) {
  boost::shared_ptr<TSocket> socket(new TSocket("localhost", 9090));
  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
  resp serverResponse;
  URLproxyClient client(protocol);

  string url;
  // unsigned int access_count_l = 0;
  // unsigned int miss_count_l = 0;

	srand(time(NULL));
	URL_mode = atoi(argv[1]);
	populate_URL_map();
	for(int z=0; z<100; z++)
	{
		url = generate_URL(URL_mode);

		try {  
			transport->open();
			client.request(serverResponse, url);
			// access_count_l = serverResponse.access_count;
			// miss_count_l = serverResponse.miss_count;
	        
	    	} catch(TException tx) {
		    std::cout << "ERROR: " << tx.what() <<std::endl;
		  }
		  // client.shutdown();
		  transport->close();
	}
	
	// print_end_stats(access_count_l,miss_count_l);
  return 0;
}
