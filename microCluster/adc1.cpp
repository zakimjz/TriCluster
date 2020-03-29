#include "timeutil.h"
#include <math.h>
#include <cstdlib>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <algorithm>
#include <set>                                                                                 
#include <vector>
#include <map>
using namespace std;

template <class T>
inline void Copy(T& s1, T& s2){
	copy( s1.begin(), s1.end(), insert_iterator<T>(s2, s2.begin()));
}

template <class T>
inline void Intersect(T& s1, T &s2, T& r){
	T tmp;
       	set_intersection( s1.begin(), s1.end(), s2.begin(), s2.end(), insert_iterator<T> (tmp, tmp.begin()));
	r.clear();
	Copy( tmp, r );
	tmp.clear();
}

template <class T>
inline void Union(T& s1, T &s2, T& r){
       	set_union( s1.begin(), s1.end(), s2.begin(), s2.end(), insert_iterator<T> (r, r.begin()));
}

template <class T>
void rd(int fd, T* x, int num)
{
	int bytes;

	bytes = read(fd, x, num*sizeof(T));
	if( bytes != num*sizeof(T) ) {
		cout << "error file format" << endl;
		exit(1);
	}
}


class Array2D
{
	int width, height;
	float* data;

public:
	void init(int w, int h) {
        	width = w;
        	height = h;
                                                                                                                
        	data = new float[width * height];
        	if( data == NULL ) {
                	cout << "Too large data volume to be load in to the memory" << endl;
                	exit(1);
        	}
	}
 
	float& operator()(int x, int y) {
		return data[y*width + x];
	}

	float* operator()(int line) {
		return( data + line*width );
	}

	void Array2D::clear() {
		delete( data );
	}
}; 




struct Edge;
typedef set<int, less<int> >    Transet;
typedef Transet::iterator       TransetIt;
typedef vector<Edge>		Edges;		//typedef set<Edge, edge_greater> Edges;
typedef Edges::iterator         EdgesIt;


struct RangeNode{

	int attr;

        float start, end;
        Transet transet;

	int connectivity;
	int nongray_edges;
	Edges edges; 
};

 

struct node_greater
{
        bool operator()(RangeNode *pNode1, RangeNode *pNode2) const
        {
                return( pNode1->nongray_edges > pNode2->nongray_edges || (pNode1->nongray_edges == pNode2->nongray_edges && pNode1->connectivity > pNode2->connectivity) );
        }
};


struct Edge{
	bool gray;
	RangeNode* pNode;

	bool operator==(Edge e) 
	{
		return(e.pNode == pNode);
	}
};


struct edge_greater{
	bool operator()(Edge e1, Edge e2) const
	{
		if( e1.gray == e2.gray )
			return( e1.pNode->connectivity > e2.pNode->connectivity );
		else
			return( e2.gray );
	}
};


struct attVecCell{
	float	val;
	int	trans;
};


struct attVec_less{
	bool operator()(attVecCell c1, attVecCell c2) const
	{
		return(c1.val < c2.val);
	}
};


struct node_less{
	bool operator()( RangeNode* p1, RangeNode* p2 ) const
	{
		return p1 < p2;
	}
};


typedef RangeNode* Vertex;
typedef set<RangeNode*, node_less>  NodeSet;
typedef NodeSet::iterator           NodeSetIt;


class Set // for clique mining
{
public:
	NodeSet   S;
	Transet   T; //transet for C only

	Set(){
	}

        Set( NodeSet &X ){
		Copy(X, S);
	}

	void operator=(Set &X){
		S.clear();
		Copy(X.S, S);
		Copy(X.T, T);
        }
       
	void GetEdgeVertices(Vertex x){
		EdgesIt eit;
		S.clear();
               	for(eit=x->edges.begin(); eit!=x->edges.end(); eit++)
			S.insert( eit->pNode );
	}

	void clear(){
		S.clear();
	}
 
        Vertex pop(){
		Vertex x;
		if(S.empty())
			return NULL;
		x = *(S.begin());
		S.erase(S.begin());
		return x;
        }
                                                                                                                                                                                                     
        void add(Vertex x){
		S.insert(x);
	}

/*        void add_T(Vertex x){
		Transet R;

		S.insert(x);

		if(T.empty()){
			Copy(x->transet, T);
			return;
		}
		Intersect(T, x->transet, R);
		Copy(R, T);
		R.clear(); 
        }
  */                                                                                              
        void remove(Vertex x){
                S.erase(x);
        }
                                                                                                                                                                                                     
        void remove(Set &X){
		NodeSetIt it;
                for(it=X.S.begin(); it!=X.S.end(); it++)
                        S.erase(*it);
        }
                                                                                                                                                                                                     
        void intersect(Set &X, Set &R){
                R.clear();
		Intersect(X.S, S, R.S);
        }
                                                                                                                                                                                                     
        int size(){
		return S.size();
        }
                                                                                                                                                                                                     
        bool empty(){
                return S.empty();
        }


	bool contain( Set c ){
		NodeSetIt  nit;
		RangeNode* x;	
		TransetIt  tit;
		int y;

		for( tit=c.T.begin(); tit!=c.T.end(); tit++ ){
			y = *tit;
			if( y != *find(T.begin(), T.end(), y) )
				return false;
		}
		for( nit=c.S.begin(); nit!=c.S.end(); nit++ ){
			x = *nit;
			if( x != *find(S.begin(), S.end(), x) )
				return false;
		}
		return true;
	}
	
	void showCluster(Array2D &data){
		NodeSetIt it;
		vector<int> attr;
		vector<int>::iterator vit;
		TransetIt sit;
		int k;

		cout << "width x height: " << S.size() << " x " << T.size() << endl;

        	for(it=S.begin(); it!=S.end(); it++)
			attr.push_back( (*it)->attr );
		sort( attr.begin(), attr.end() );


		//cout << "        ";
        	for(vit=attr.begin(), k=0; vit!=attr.end(); vit++){
			if( k++%20 == 0 )
				cout << endl << "        ";
			printf( "%4d    ", *vit );
		}
		cout << endl;
        	for(sit=T.begin(), k=0; sit!=T.end(); sit++){
			printf( "%4d  ", *sit );
        		for(vit=attr.begin(), k=0; vit!=attr.end(); vit++){
				if( k++%20 == 0 && k != 1 )
					cout << endl << "      ";
				printf( "%6.2f  ", data(*vit, *sit));
			}
			cout << endl;
		}
		attr.clear();
	}
};


ostream& operator<<(ostream& out, RangeNode* p ){
	char buf[200];
	TransetIt it;
       	sprintf( buf, "%3d (%6.2f,%6.2f)  ", p->attr, p->start, p->end );
	out << buf;
	for(it=p->transet.begin(); it!=p->transet.end(); it++){
		sprintf(buf, "%3d ", *it );
		out << buf;
	}
	out << endl;
}


ostream& operator<<(ostream& out, NodeSet &S){
	char buf[200];
	NodeSetIt it;
	RangeNode* p;
	int k=0;
	out << "Set: ";
        for(it=S.begin(); it!=S.end(); it++){
		if( k%7==0 && k!=0 )
			cout << endl << "     ";
		k++;
		p = *it;
       		//sprintf(buf, "%3d, ", p->attr );
       		sprintf(buf, "%3d (%6.2f,%6.2f),  ", p->attr, p->start, p->end );
		out << buf;
	}
	return out;
}


typedef vector<Set>::iterator     SetsIt;


class Sets
{
public:
	vector<Set> data;

	bool contain( Set c ){
		SetsIt it;
		for(it=data.begin(); it!=data.end(); it++)
			if(it->contain(c))
				return true;
		return false;
	}

	void del_be_contained( Set c ){
		SetsIt it;

		it = data.begin(); 
		while( it != data.end() ){
			if( c.contain(*it) ){
				it->S.clear();
				it->T.clear();
				it = data.erase(it);
				//it=data.begin();
			}
			else
				it++;
		}
	}

	void showClusters(Array2D& array){
		SetsIt it;
		int i=1;
		
                cout << endl << "==================== Clusters ====================" << endl;

		for(it=data.begin(); it!=data.end(); it++){
			cout << endl << "Cluster: " << i << endl << endl;
			it->showCluster(array);
			i++;
		}
        }

	void clear(){
        	SetsIt it;
                                                                                                                                                                                                     
        	for( it=data.begin(); it!=data.end(); it++){
                	it->S.clear();
			it->T.clear();
		}
        	data.clear();
	}

	void CAL( Set &I, Set &P, Set &S );
	void A_EXPAND( Set &C, Set &P, int sup_clus, int sup_cliq );
	void shrinkClique( Set &K, int sup_clus, int sup_cliq );
	void EXPAND( Set &C, Set &I, Set &P, int sup_clus, int sup_cliq );
	void getCliques( NodeSet &nodeSet, int sup_clus, int sup_cliq );
	void getTranset( Set &clique );
	void merge( float row_overlap ); 
};




ostream& operator<<(ostream& out, Set &X){
       	NodeSetIt  it;
	TransetIt  setit;
	char buf[200];

	out << endl << "------------------ showClique ----------------" << endl;
        for(it=X.S.begin(); it!=X.S.end(); it++){
		out << *it;
	}
	out << endl << "transet: ";
	for(setit=X.T.begin(); setit!=X.T.end(); setit++){
		sprintf(buf, "%3d ", *setit );
		out << buf;
	}
	out << endl;
}

ostream& operator<<(ostream& out, Sets &S){
	SetsIt it;
	int i=1;

        out << endl << "==================== Cliques ====================" << endl;
	for(it=S.data.begin(); it!=S.data.end(); it++){
		out << endl << "clique: " << i << endl << endl;
		out << *it;
		i++;
	}
}
	
void Sets::CAL( Set &I, Set &P, Set &S )
{
       	Set S1, G;
	NodeSetIt it;
        int sz;
       	//	assert( I.size() > 0 );
        sz = INT_MAX;

       	for(it=I.S.begin(); it!=I.S.end(); it++){
		G.GetEdgeVertices(*it); // S1 = P - G(i);
       	        S1 = P;
		S1.remove(G);
               	if( S1.size() < sz ){
                       	S = S1;
       	                sz = S.size();
               	        //if( sz < alpha )
                       	//      break;
                }
       	}
	S1.clear();
	G.clear();
}



void Sets::A_EXPAND( Set &C, Set &P, int sup_clus, int sup_cliq )
{
	NodeSetIt it;
	Set C2, P2;
	Vertex x;
	
	while( !P.empty() ){                                                                                                                                         
                x = P.pop(); // P = P - i;
		P2 = P;
		C2 = C;
		C2.add(x);
                A_EXPAND( C2, P2, sup_clus, sup_cliq );
        }

        if( C.size() >= sup_cliq ) {
		getTranset( C );
		if( C.T.size() >= sup_clus ){	// 1. enought size
			if( !contain(C) ){	// 2. no overlapping
				//del_be_contained(C); // moved down to circumvent the intersection problem. coz enumerating from large attrbibute set, not need to check reversely.
				data.push_back(C);
                		//cout << "A_SUB CLIQUE " << C << endl;
			}
		}
        }
}

                                                                                                                                                                                                     
void Sets::shrinkClique( Set &K, int sup_clus, int sup_cliq )
{
	SetsIt it;
	Set C, P(K);

	A_EXPAND( C, P, sup_clus, sup_cliq );
	C.clear(); P.clear();	
}


void Sets::EXPAND( Set &C, Set &I, Set &P, int sup_clus, int sup_cliq )
{
	NodeSetIt it;
	Set S, G, C2, I2, P2;

       	if( I.empty() ){
		//cout << "Clique " << C << endl;	
		if( C.size() >= sup_cliq ){
			cout << "Here got one" << endl;
			shrinkClique( C, sup_clus, sup_cliq );
		}
       	}
       	else{
               	CAL( I, P, S );
       		for(it=S.S.begin(); it!=S.S.end(); it++){
               	        P.remove(*it); // P = P - i;
			C2 = C;
			C2.add(*it);

			G.GetEdgeVertices(*it);
			I.intersect(G, I2);
			P.intersect(G, P2);

                	EXPAND( C2, I2, P2, sup_clus, sup_cliq );
                }
		S.clear();  G.clear();
		C2.clear(); I2.clear(); P2.clear();
       	}
}

	

void Sets::getCliques( NodeSet &nodeSet, int sup_clus, int sup_cliq )
{
	Set C, I(nodeSet), P(nodeSet);
	cout << endl;
	EXPAND( C, I, P, sup_clus, sup_cliq );
	C.clear(); I.clear(); P.clear();	
}



void Sets::getTranset( Set &clique )
{
	NodeSetIt it;
        RangeNode  *p;
        int        i, j;
        Transet    set;

       	set.clear();
	clique.T.clear();

	if(!clique.S.empty()){                                                                                        
        	it = clique.S.begin();
	        p = *it;
           
		Copy(p->transet, set);
        	it++;
	        while( it!=clique.S.end() ){
        	        p = *it;
			Intersect( set, p->transet, set );
 	              	it++;
        	}
		Copy(set, clique.T);
	        set.clear();
	}
        return;
}



void Sets::merge( float row_overlap ) // column share will be achieved by shrinking columns/ increasing rows.
{
	Set      clique;
	SetsIt   it1, it2, max_it1, max_it2;
	float       max_overlap, overlap;
	

	do{
		max_overlap = 0.0;

		for(it1=data.begin(); it1!=data.end(); it1++)
		for(it2=it1, ++it2; it2!=data.end(); it2++){

               		Intersect( it1->T, it2->T, clique.T );

			cout << "size 1: " << it1->T.size() << endl;
			cout << "size 2: " << it2->T.size() << endl;
			cout << "size 3: " << clique.T.size() << endl;

			overlap = float(clique.T.size()) / (it1->T.size()+it2->T.size()-clique.T.size());

			cout << "overlap: " << overlap << endl;

			if( overlap > max_overlap ){
				max_it1 = it1;
				max_it2 = it2;
				max_overlap = overlap;
			}

			Union( max_it1->S, max_it2->S, clique.S );
			cout << clique; 
			clique.T.clear();
		}

		if( max_overlap > row_overlap ){

//              	Intersect( max_it1->clusTranset, max_it2->clusTranset, clique.clusTranset ); // union????
//			Union( max_it1->nodeSet, max_it2->nodeSet, clique.nodeSet );
			cout << clique; 
/*
			data.push_back( clique );

			max_it1->nodeSet.clear();
			max_it2->nodeSet.clear();
			max_it1->clusTranset.clear();
			max_it2->clusTranset.clear();
*/
			data.erase( max_it1 );
			data.erase( max_it2 );
/*
			clique.nodeSet.clear();
			clique.clusTranset.clear();
*/		}

	}while( max_overlap > row_overlap );
}




class Ranges 
{
	string file;
	int width, height;
	float max_value;
	
	NodeSetIt nit, nit1, nit2;

public: 
	Array2D array;
	NodeSet   nodeArray;

	Ranges(char* name) {
		file = name;
	}

	void showNodeTransets()
	{
        	TransetIt setit;
        	EdgesIt   edgeit;
                                                                                                                                                                                                     
        	cout << endl << "==================== Ranges(Vertices) and Transets ====================" << endl << endl;                                                                                                                                                                                                      
        	for(nit=nodeArray.begin(); nit!=nodeArray.end(); nit++){
			cout << *nit;
                	cout << "  Transet: ";
                	for(setit=(*nit)->transet.begin(); setit!=(*nit)->transet.end(); setit++ )
                        	printf( "%3d ", *setit );
                	cout << endl;
        	}
	}

                                                                                                                                                                                                     
	bool connect( RangeNode *pNode1, RangeNode *pNode2 ){
		RangeNode *p;
		Edge	  edge;

		if( pNode1->edges.size() < pNode2->edges.size() ){
			p = pNode1;
			edge.pNode = pNode2;
		}
		else{
			p = pNode2;
			edge.pNode = pNode1;
		}
                return( find(p->edges.begin(), p->edges.end(), edge) != p->edges.end() );
	}

	EdgesIt findInEdges( RangeNode *p1, RangeNode *p2 ){
		Edge  edge;

		edge.pNode = p2;	
		return find( p1->edges.begin(), p1->edges.end(), edge );
	}	
                                                                                                                                                                                                     
	void showConnections(){
       		int     ret;
        	EdgesIt it;
                                                                                                                                                                                                     
        	//sort(nodeArray.begin(), nodeArray.end(), node_greater());
                                                                                                                                                                                                     
        	cout << endl << "==================== Connections ====================" << endl << endl;
        	for(nit=nodeArray.begin(); nit!=nodeArray.end(); nit++)
		if( (*nit)->edges.size() > 0 ){
                	cout << *nit;
			cout << "------->\t";
                	ret = 0;
                	for(it=(*nit)->edges.begin(); it!=(*nit)->edges.end(); it++){
                        	if( ret % 5 == 0 && ret != 0 )
                                	cout << endl << "\t\t\t\t";
                        	cout << it->pNode;
                        	ret++;
                	}
                	cout << endl;
        	}
	}
         
	void showNongrayEdgesNum(){
		int ret = 0;
		cout << endl << "==================== #Nongray Edges ====================" << endl << endl;
        	for(nit=nodeArray.begin(); nit!=nodeArray.end(); nit++){
			if( ret %5 ==0 && ret != 0 )
				cout << endl;
			cout << *nit;
                	printf( "%3d,    ", (*nit)->nongray_edges );
			ret++;
		}
		cout << endl;
	}
                                                                                                                                                                                                     
	
	void Readfile(){
        	int fd, value;
                                                                                                                                                                                                     
        	fd = open( file.c_str(), O_RDONLY );
        	rd( fd, (int *)&height, 1 );
        	rd( fd, (int *)&width, 1 );
                                                                                                                                                                                                     
        	array.init( width, height );
                                                                                                                                                                                                     
        	rd( fd, array(0), width ); // skip the scope of each attribute
        	max_value = array(0, 0);
                                                                                                                                                                                                     
        	for(int i=0; i< height; i++) {
                	rd(fd, (int *)&value, 1);
                	rd(fd, array(i), width);
                	rd(fd, (int *)&value, 1);
        	}
        	close(fd);
	}

	void clear(){
        	for(nit=nodeArray.begin(); nit!=nodeArray.end(); nit++){
                	(*nit)->edges.clear();
                	(*nit)->transet.clear();
                	delete (*nit);
        	}
		nodeArray.clear();
             
		array.clear();
	}

	void showData(){
		cout << "width: " << width << ", height: " << height << endl;
       		cout << "       ";
        	for(int i=0; i<width; i++)
                	printf( "%6d  ", i );
        	cout << endl;
                                                                                                                                                                                                     
        	for(int j=0; j<height; j++) {
                	printf( "%6d  ", j );
                	for(int i=0; i<width; i++)
                        	printf( "%6.2f  ", array(i,j) );
                	cout << endl;
        	}
	}

	void getRanges( float winsz, int sup1, bool extend );
	void getConnections( int trans_sup, int sup_cliq );
};




#define EXTEND true
//#define EXTEND false

#define GET_MAX_RANGE

#ifdef GET_MAX_RANGE
void Ranges::getRanges( float winsz, int sup1, bool extend )
{
	attVecCell cell;
        vector< attVecCell > attVec;
        vector< attVecCell >::iterator it, it1, it2, it3, it4, start_it, end_it;
                                                                                                                                                                                                     
        int support, sup, max_sup;
        float win_size, val1, val2;
        RangeNode* pNode;
                                                                                                                                                                                                     
        win_size = winsz * max_value;
        support = sup1; //int(sup1 * height);

	cout << endl << endl;
	cout << "win_size: " << win_size << endl;
	cout << "max_value: " << max_value << endl;

	assert( support > 0 );

        for(int j=0; j<width; j++){

	////////////////////////////////////////
                for(int i=0; i<height; i++){
			cell.val = array(j,i);
			cell.trans = i;	
                        attVec.push_back( cell );
		}

		sort(attVec.begin(), attVec.end(), attVec_less());
	/*	
		cout << "ATT: " << j << endl;
		for( it=attVec.begin(); it!=attVec.end(); it++ )
			//printf( "(%2d, %2.2f), ", it->trans, it->val );
			printf( "%2.2f, ", it->val );
		cout << endl;
	*/	
		do{
			it1 = it2 = attVec.begin();

        	       	max_sup = 0;
                	sup = 0;

			while( it2 != attVec.end() ){  
				if( it1->trans == -1 ){
					while( it1->trans == -1 && it1 != attVec.end() )
						it1++;
                           		it2 = it1;
				}

                        	while( it2->trans != -1 && it2 != attVec.end() && it2->val - it1->val <= win_size ){
                                	it2++;
                                	sup++;
                        	}
                        	if(sup > max_sup){
					start_it = it1;
					end_it = it2;
					max_sup = sup;
				}
				if( it2->trans == -1 ){
					it1 = it2;
					sup = 0;
				}
				else{
                        		it1++;
                        		sup--;
				}
			}

			if(max_sup >= support){

				if( extend ){ // EXTEND to both directions
				/////////////////////////////////////////////////////////////////////
					it1 = it3 = start_it;
					it2 = it4 = end_it;

					///////////////////////////
					sup = max_sup;
					while( sup > support ){
						it1++;
						sup--;
					}
        	                	while( it2->trans != -1 && it2 != attVec.end() && it2->val - it1->val <= win_size ){
						it1++;
						it2++;
					}
					///////////////////////////
					sup = max_sup;
					while( sup > support ){
						it4--;
						sup--;
					}
					if( it3 != attVec.begin() ){
						it4--; // changed
						it3--;
        	                		while( it3->trans != -1 && it3 != attVec.begin() && it4->val - it3->val <= win_size ){
							it3--;
							it4--;
						}
						if( it3->trans == -1 || (it4->val - it3->val) > win_size )
							it3++;
					}
					start_it = it3;
					end_it = it2;
				}
				////////////////////////////////////////////////////////////////////

				end_it--;
                		pNode = new RangeNode;
                        	pNode->start = start_it->val;
                        	pNode->end = end_it->val;
                        	pNode->attr = j;
                        	pNode->nongray_edges = 0;
                        	pNode->connectivity = 0;
				end_it++;
                                                                                                                                                                                                     
                        	for(it=start_it; it!=end_it; it++){
                                	pNode->transet.insert( it->trans );
					it->trans = -1;
				}
                        	nodeArray.insert( pNode );
                	}

                }while( max_sup >= support );

                attVec.clear();
	///////////////////////////////////////////
        }
        // array.clear();
	cout << "Totally got " << nodeArray.size() << " ranges !" << endl;
}


#else


void Ranges::getRanges( float winsz, int support, bool extend )
{
	attVecCell cell;
        vector< attVecCell > attVec;
        vector< attVecCell >::iterator it, it1, it2;

        int sup;
	float win_size, val1, val2;
	RangeNode* pNode;

	win_size = winsz * max_value; 
                            
	assert( support > 0 );

	for(int j=0; j<width; j++) {

                for(int i=0; i<height; i++){
			cell.val = array(j,i);
			cell.trans = i;	
                        attVec.push_back( cell );
		}

		sort(attVec.begin(), attVec.end(), attVec_less());

		it1 = it2 = attVec.begin();
		sup = 0;

		cout << "attVec.size(): " << attVec.size() << endl;

		while( it2 != attVec.end() ){

 	   		while( it2 != attVec.end() && it2->val-it1->val <= win_size ) {
               			it2++;
				sup++;
			}
			if( sup >= support ){
				if( extend ){ // EXTEND
				/////////////////////////////////////////////////////////////////////
					it = it1;
					while( sup > support ) {
						it ++;
						sup--;
					}

					while( it2 != attVec.end() && it2->val - it->val <= win_size ) {
						it ++;
						it2++;
					}
				}
				/////////////////////////////////////////////////////////////////////
				it2--;
				pNode = new RangeNode;
				pNode->start = it1->val;
				pNode->end = it2->val;
				pNode->attr = j;
				pNode->nongray_edges = 0;
				pNode->connectivity = 0;
				it2++;

				for(it=it1; it!=it2; it++)
					pNode->transet.insert( it->trans );
               			
				nodeArray.insert( pNode );
	        
				it1 = it2;
				sup = 0;
			}
			else {
				it1++;
				sup--;
			}
		}
		attVec.clear();
	}
	// array.clear();
	cout << "Totally got " << nodeArray.size() << " ranges !" << endl;
}
#endif




void Ranges::getConnections( int trans_sup, int sup_cliq )
{
	Edge edge;
        Transet intersect;
	int pruned_points = 0;
	int pruned_edges = 0;
	int edges = 0;
	int k = 1;

        for(nit1=nodeArray.begin(); nit1!=nodeArray.end(); nit1++)
        for(nit2=nit1, nit2++; nit2!=nodeArray.end(); nit2++){

		Intersect( (*nit1)->transet, (*nit2)->transet, intersect );

		if( intersect.size() >= trans_sup ){
			(*nit1)->nongray_edges++;
			(*nit1)->connectivity++;
			(*nit2)->nongray_edges++;
			(*nit2)->connectivity++;

			edge.gray = false;
			edge.pNode = (*nit2);
			(*nit1)->edges.push_back( edge );
			edge.pNode = (*nit1);
			(*nit2)->edges.push_back( edge );
			edges++;
		}
		intersect.clear();
	}

	cout << "Totally got " << edges << " connections !" << endl;

#if 1 
	if( nodeArray.size() > 0 ){  // pruning

	        nit = nodeArray.begin(); 
		while( nit != nodeArray.end() ){
			if( (*nit)->connectivity < sup_cliq-1 ){
				EdgesIt it;
				Edge 	myedge;

				myedge.pNode = (*nit);
				for(it=(*nit)->edges.begin(); it!=(*nit)->edges.end(); it++){
					it->pNode->edges.erase( find(it->pNode->edges.begin(), it->pNode->edges.end(), myedge) );
					it->pNode->connectivity--;
					it->pNode->nongray_edges--;
				}
				pruned_points++;
				pruned_edges += (*nit)->edges.size();

				(*nit)->transet.clear();
				(*nit)->edges.clear();
				delete (*nit);
				nodeArray.erase(nit);
				nit = nodeArray.begin();
			}
			else
				nit++;
		}
	}
#endif
	cout << "Pruned " << pruned_points << " Points" << endl;
	cout << "Pruned " << pruned_edges  << " Edges" << endl;

}






int main(int argc, char *argv[])
{
	Sets cliques;

        if(argc < 8) {			//         1     2    3    4    5    6     7
                cout << "Usage: " << argv[0] << " file winsz sup1 sup2 minH minW overlap" << endl;
		cout << "Windows Size:       winsz      e.g. 10%"		<< endl;
		cout << "Windows Support:    sup1     > Height x winsz"		<< endl;
		cout << "2 Trans Support:    sup2     > Height x winsz^2"	<< endl;
		cout << "Cluster Support:    min_H    > Height x winsz^w"	<< endl;
		cout << "Attr.   Support:    min_W    > "			<< endl;
		cout << "Merging threshold:  overlap    e.g. 80%"		<< endl;
                exit(1);
        }
	cout << endl;
	for(int i=0; i<argc; i++)
		cout << " " << argv[i];
	cout << endl;
                                                                                 
	Timer  T("time 1");
	T.Start();
        Ranges ranges( argv[1] );
	ranges.Readfile();
//	ranges.showData();

	ranges.getRanges( atof(argv[2]), atoi(argv[3]), EXTEND );
//	ranges.showNodeTransets();

	ranges.getConnections( atoi(argv[4]), atoi(argv[6]) );
//	ranges.showConnections();

	cliques.getCliques( ranges.nodeArray, atoi(argv[5]), atoi(argv[6]) );
//	cout << cliques;

/*	cliques.merge( atof(argv[7]) );
	cout << endl << "After Merging";
	cout << cliques;
*/
	cliques.showClusters(ranges.array);

//	ranges.clear();
//	cliques.clear();
	T.Stop();

	cout << endl << T << endl;
	return 0;
}




