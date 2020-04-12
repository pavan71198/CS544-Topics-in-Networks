#include <bits/stdc++.h>
#include <iostream>
#include <fstream>

using namespace std;

bool failure;
bool connect;
vector<int> fpath;
int fdist;
vector<int> gdist;

int mindist(vector<int> dist, vector<int> Set, int N) 
{      
    int min = INT_MAX, min_in;   
    for (int in = 0; in < N; in++)
    { 
        if (Set[in] == 0 && dist[in] <= min)
        {
            min = dist[in];
            min_in = in;
        }         
    }
    return min_in; 
} 
  
vector<int> min_path(vector<vector<int>> graph, int src,int dest) 
{ 
    int N = graph[0].size();
    vector<int> dist;  
    vector<int> Set; 
    vector<int> parent; 
    for (int i = 0; i < N; i++) 
    { 
        parent.push_back(-1); 
        dist.push_back(INT_MAX); 
        Set.push_back(0); 
    } 
    dist[src] = 0;
    int u; 
    for (int count = 0; count < N - 1; count++) 
    { 
        u = mindist(dist, Set, N); 
        Set[u] = true; 
        for (int in = 0; in < N; in++)
            if (Set[in] ==0 && graph[u][in]!=0 && dist[u] + graph[u][in] < dist[in]) 
            { 
                parent[in] = u; 
                dist[in] = dist[u] + graph[u][in]; 
            }  
    } 
    fdist = dist[dest];
    for(int i=0;i<N;i++)
    {
        gdist.push_back(dist[i]);
    }
    return parent;
}



vector<vector<float>> connection(int src,int dest, vector<vector<float>> Bandwidth, float b, vector<vector<int>> graph)
{
    connect = 1;
    failure = 0;
    vector<int> par;

    par = min_path(graph,src,0);
    vector<vector<float>> Band = Bandwidth;
    while(par[dest]!=-1)
    {
        Band[dest][par[dest]] = Band[dest][par[dest]] - b;
        Band[par[dest]][dest] = Band[par[dest]][dest] - b;
        if(Band[par[dest]][dest] < 0)
        {
            failure = 1;
            return Bandwidth;
        }
        dest = par[dest];
    } 
    Bandwidth = Band;
    connect = 0;
    return Bandwidth;
}

int main(int argc, char **argv)
{
    int N,E,R,x=0,p;
    FILE* f = fopen(argv[1],"r");
    fscanf(f,"%d %d",&N,&E);
    vector<vector<int>> graph;
    vector<vector<float>> Bandwidth;
    graph.resize(N);
    Bandwidth.resize(N);
    for (int i = 0; i < N; ++i)
    {
        graph[i].resize(N);
        Bandwidth[i].resize(N);
    }
        
    for(int i=0;i<N;i++)
    {
        for(int j=0;j<N;j++)
        {
            graph[i][j] = 0;
            Bandwidth[i][j]= 0;
        }
    }
    for(int i=0;i<E;i++)
    {
        int node_1,node_2,d;
        float T,dust;
        fscanf(f,"%d %d %d %f %f",&node_1,&node_2,&d,&T,&dust);
        graph[node_1][node_2] = d;
        graph[node_2][node_1] = d;
        Bandwidth[node_1][node_2]= T;
        Bandwidth[node_2][node_1]= T;   
    }
    
    fclose(f);
    FILE *fi = fopen(argv[2],"r");
    FILE* O_connect = fopen(argv[6],"w");
    fscanf(f,"%d",&R);        
    if(strcmp(argv[3],"1")==0)
    {
       x=1;
    }
    p=0;
    for(int i=0;i<R;i++)
    {
        int node_1,node_2;
        float b1,b2,b3;
        fscanf(fi,"%d %d %f %f %f",&node_1,&node_2,&b1,&b2,&b3);
        float b4 = b2 + 0.25*(b3 - b1);
        if(b3 < b4)
            b4 = b3;
       if(x=1)
           Bandwidth = connection(node_1, node_2,Bandwidth,b4,graph);
        else
           Bandwidth = connection(node_1,node_2,Bandwidth,b3,graph);
        if(failure == 0)
        {
            p++;
            fprintf(O_connect,"CID: %d Src: %d Dest: %d ",p,node_1,node_2);
            fprintf(O_connect,"Path:");
            fpath.clear();
            fpath = min_path(graph,node_1,node_2);
            while(fpath[node_2]!=-1)
            {
                fprintf(O_connect,"%d ",node_2);
                node_2 = fpath[node_2];
            }
            fprintf(O_connect,"%d Cost: %d\n",node_1,fdist);
        }
    } 
    fclose(O_connect);
    fclose(fi); 
    int index;
    connect = 0;
    
    FILE *o_path = fopen(argv[4],"w");

    for(int i=0;i<N;i++)
    {
        fpath.clear();
        gdist.clear();
        fpath = min_path(graph,i,i);
        fprintf(o_path,"Source: %d\n",i);
        for(int j=0;j<N;j++)
        {   int k = j;
            fprintf(o_path,"Dest: %d Path: ",k);
            while(fpath[k]!=-1)
            {
                fprintf(o_path,"%d ",k);
                k = fpath[k];
            }
            fprintf(o_path,"%d Cost: %d\n",i,gdist[j]);
        }
    }
    fclose(o_path);
    FILE* o_con = fopen(argv[5],"w");
    fprintf(o_con,"%d %d",R,p);
    fclose(o_con);

    return 0;

}