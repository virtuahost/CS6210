
namespace cpp urlserver

struct resp
{
	1:string doc,
	2:i32 access_count,
	3:i32 miss_count
}

service URLproxy
{
	resp request(1:string url)

	oneway void shutdown()
}