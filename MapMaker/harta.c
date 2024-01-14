#include <cstdio>
#include <iostream>
#include<unistd.h>
#include <time.h>
#include <cstring>
#include <string>
#include <sqlite3.h> 
#include <algorithm>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <cmath>
#define MAX_SIZE 300
using namespace std;
ifstream fin("nodes_positions.txt");
ifstream gin("edges.txt");

sqlite3* DB; 
struct intersection{
	int x, y;
	int id;
};/*
struct street{
	intersection left, right;
	int distance;
	int speed_limit;
	int name;
	int id;
};*/
struct map{
	vector<pair<int,string>> List[510];
	vector<intersection> Nodes;
	int mat[510][510];
}M;

intersection Data_Nodes[500];
int no_nodes=0;
int no_streets=0;
int codex[500], codey[500];
void string_to_charptr(string A, char *P) {
  char mess[250]="\0";
  
  for(int i=0; i<A.size(); ++i)
  {
    mess[i]=A[i];
    mess[i+1]='\0';
  }
  strcpy(P,mess);
}
int callback(void *NotUsed, int argc, char **argv, 
                    char **azColName);
void addedge();
void add_edge(int x, int y, string A);
void addnode();
void add_node(int x, int y);
void listnodes();
void listedges();
void viewmap();
void insertintodatabase();
int parse(char *p);
int distanta(int x1, int y1, int x2, int y2) {
  return floor(sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)));

}
void map_init() {
  int Ax, Ay;
  while(fin>>Ax>>Ay) {
    add_node(Ax,Ay);
  }
  string str;
  while(gin>>Ax>>Ay) {
    getline(gin,str);
    add_edge(Ax,Ay,str);
  }
}
int MapLeed[510][510];
int main() {

	char input[MAX_SIZE+10];
	map_init();
	printf("[Map maker]\n\n");
	printf("Enter command [1 - view map; 2 - add street(edge); 3 - add intersection(node); 4 - exit]\n");
	int END=0;
	while(cin.getline(input,MAX_SIZE))
	{
		int code=parse(input);
		if(code==0) {
			printf("Wrong command, type.\n");
			continue;
		}		
		
		switch(code) {
			case 1: viewmap(); break;
			case 2: addedge(); break;
			case 3: addnode(); break;
			case 4: END=1; break;
		}
		if(END) {
                  printf("Map finished... Printing map and modifying database!\n");
                  sleep(1);
                  viewmap();
                  insertintodatabase();
                  return 0;
                }
	printf("Enter command [1 - view map; 2 - add street(edge); 3 - add intersection(node); 4 - exit]]\n");
	}	

	return 0;
}
int parse(char *p)
{
  int lg=strlen(p);
  int cd=-1;
  for(int i=0; i<lg; ++i)
    if(p[i]>='0' && p[i]<='9') {
      if(cd!=-1)
        return 0;
      cd=p[i]-'0';
    }
    else if(p[i]!=' ' || p[i]!='\t' || p[i]!='\n')
      return 0;
  return cd;
}

void listnodes() {
    
	printf("The %d nodes are:\n",M.Nodes.size());
	for(int i=0; i<M.Nodes.size(); ++i) 
		printf("Node %d has position (%d,%d)\n", M.Nodes[i].id,M.Nodes[i].x, M.Nodes[i].y);
}
void listedges() {
	printf("The edges are:\n");
	for(int i=1; i<=no_nodes; ++i) {
		printf("For node [%d]>", i);
		for(auto it: M.List[i]) {
		  cout<<it.first<<"("<<it.second<<");";
		}
		printf("\n");
	}

}

void addnode() {
	printf("<For adding a node> Enter the position (x,y):");
	int nodex,nodey;
	scanf("%d%d",&nodex,&nodey);
	no_nodes++;
	intersection Inter={nodex,nodey,no_nodes};
	Data_Nodes[no_nodes]=Inter;
	M.Nodes.push_back(Inter);
}
void add_node(int nodex, int nodey) {

	no_nodes++;
	intersection Inter={nodex,nodey,no_nodes};
	Data_Nodes[no_nodes]=Inter;
	M.Nodes.push_back(Inter);
}

void addedge() {
	printf("<For adding an edge> Enter the id of the nodes:");
	listnodes();
	printf("Select two nodes (two integers >=1):");
	int node1,node2 ;
	scanf("%d%d",&node1,&node2);
	while(node1>no_nodes || node2>no_nodes || node1<1 || node2<1) {
	printf("Select two nodes (two integers >=1):");
	
	scanf("%d%d",&node1,&node2);
	}
	cout<<"Name the street: ";
	string A;
        cin>>A;
	add_edge(node1,node2,A);
}
void add_edge(int node1, int node2, string A) {
	M.List[node1].push_back({node2, A});
	M.List[node2].push_back({node1,A});
}/*
void colormap() {
  for(int i=1; i<=no_nodes; ++i) {
      lee(codex[i], codey[i]);
        
    }
}*/

void viewmap() {
	if(M.Nodes.size()==0) {
		printf("No intersections, map empty\n");
		return ;
	}
	
	vector<int> Sorted_Nodes_x;
	vector<int> Sorted_Nodes_y;
	Sorted_Nodes_x.clear();
	Sorted_Nodes_y.clear();
	for(auto it: M.Nodes) {
		pair<int,int> element;
		element.first=it.x;
		element.second=it.y;
		Sorted_Nodes_x.push_back(element.first);
		Sorted_Nodes_y.push_back(element.second);
	}
	sort(Sorted_Nodes_x.begin(), Sorted_Nodes_x.end());
	sort(Sorted_Nodes_y.begin(), Sorted_Nodes_y.end());
	
	int cx, cy; cx=cy=0;
	for(int i=0; i<Sorted_Nodes_x.size(); ++i)
	{
		if(i==0 || Sorted_Nodes_x[i-1]!=Sorted_Nodes_x[i])
			cx++;
		codex[Sorted_Nodes_x[i]]=cx;
	}
	for(int i=0; i<Sorted_Nodes_y.size(); ++i)
	{
		if(i==0 || Sorted_Nodes_y[i-1]!=Sorted_Nodes_y[i])
			cy++;
		codey[Sorted_Nodes_y[i]]=cy;
	}
	for(int i=0; i<cx+3; ++i) 
	    for(int j=0; j<cy+3; ++j) 
	        M.mat[i][j]=0;
	for(auto it: M.Nodes) {
		pair<int,int> element;
		element.first=codex[it.x];
		element.second=codey[it.y];
		M.mat[element.first][element.second]=it.id;
	}
	listedges();
	
	for(int i=0; i<cx+3; ++i) {
		for(int j=0; j<cy+3; ++j) {
			if(M.mat[i][j] && M.mat[i][j]<10)
				printf(" %d ", M.mat[i][j]);
			else if(M.mat[i][j] && M.mat[i][j]>=10)
				printf("%d ", M.mat[i][j]);
			else printf("   ");
		}
		printf("\n");
	}
}


int callback(void *NotUsed, int argc, char **argv, 
                    char **azColName) {
    
    NotUsed = 0;
    
    for (int i = 0; i < argc; i++) {

        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    
    printf("\n");
    
    return 0;
}
void insertintodatabase() {
  char* messaggeError;
  int exitC = 0; 
  exitC = sqlite3_open("Iashington.db", &DB); 
  if (exitC) { 
        printf("Error open DB %d\n",sqlite3_errmsg(DB));
  }
  else
        printf("Opened Database Successfully!\n"); 
  messaggeError=nullptr;
  string sql1 = "DROP TABLE IF EXISTS Streets;" 
                "CREATE TABLE Streets("
                "ID INT PRIMARY KEY     NOT NULL, "
                "NAME           VARCHAR(50)    NOT NULL, "
                "IDLEFT         INT,"
                "IDRIGHT         INT,"
                "SPEEDLIMIT         INT,"
                 "DISTANCE         INT );";
    exitC = sqlite3_exec(DB, sql1.c_str(), NULL, 0, &messaggeError); 
    if (exitC != SQLITE_OK) { 
        printf("Error Create\n"); 
        sqlite3_free(messaggeError); 
    } 
    else
        printf("Table created Successfully!\n");
    int no_streets=0;
    srand(time(NULL));
    for(int i=1; i<=no_nodes; ++i) {
		for(auto it: M.List[i]) 
		if(i<=it.first) {
		  char query[250]; no_streets++;
		  int speed=(rand())%70+30;
		  int dist=(distanta(Data_Nodes[i].x, Data_Nodes[i].y, Data_Nodes[it.first].x, Data_Nodes[it.first].y))*500;
		  char opa[250];
		  string_to_charptr(it.second,opa);
		  sprintf(query,"INSERT INTO Streets VALUES(%d,'%s',%d,%d,%d,%d);",no_streets, opa, i, it.first, speed, dist);
		  exitC = sqlite3_exec(DB, query, NULL, 0,  &messaggeError);
                    if (exitC != SQLITE_OK) {

                    printf("Error in SQL statement: %s\n",  messaggeError);
                    sqlite3_free(messaggeError);
                    return ;
                    }
                  }
    
		}
    string query = "SELECT * FROM Streets;"; 
    int rc = sqlite3_exec(DB, query.c_str(), callback, 0, &messaggeError);
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", messaggeError);

        sqlite3_free(messaggeError);
    } 
    sqlite3_close(DB); 
}
