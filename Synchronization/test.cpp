#include<iostream>
#include<map>

using namespace std;

map<int,int> a;
map<int,int> :: iterator i;

void print()
{
	cout<<"\n ";
	for(i = a.begin(); i != a.end(); ++i)
		cout<<"  "<<a[i->first];	
	cout<<"\n";
}

int main()
{
	i = a.begin();
	a.insert(pair<int,int>(1,9));
	a.insert(pair<int,int>(2,10));
	a.insert(pair<int,int>(3,11));
	a.insert(pair<int,int>(4,12));
	print();
//	advance(i, );
	a.erase(i->first);
	print();
	
	
	return 0;
}


