// Depth first search using vectors of c++

#include<bits/stdc++.h>

// This program works only in c++ 11 compiler.

using namespace std;
int main()
{
	int n,e;
	cin>>n>>e;
	vector<int> adj[n+1];
//	vector<int> *adj = new vector<int>[n];
	for(int i=0;i<e;i++)
	{
		int a,b;
		cin>>a>>b;
		adj[a].push_back(b);
		adj[b].push_back(a);
	}
	for(int i=0;i<6;i++)
	{
		cout<<i<<" -> ";
//		for( j=adj.begin();j!=adj.end();j++)
		for(auto j:adj[i])
		{
			cout<<j<<" ";
		}
		cout<<endl;
	}
}
