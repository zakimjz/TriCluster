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

#include "timeutil.h"

using namespace std;

char rowName[100000][30];
char colName[100000][30];
bool showColName=false;
bool showRowName=false;

template <class T>
inline void Copy(T& s1, T& s2){
        s2.clear();
        copy( s1.begin(), s1.end(), insert_iterator<T>(s2, s2.begin()));
}
                                                                                                                                                                                                     
template <class T>
inline void Intersect(T& s1, T &s2, T& r){
        T tmp;
        set_intersection( s1.begin(), s1.end(), s2.begin(), s2.end(), insert_iterator<T> (tmp, tmp.begin()));
        Copy( tmp, r );
        tmp.clear();
}
                                                                                                                                                                                                     
template <class T>
inline void Union(T& s1, T &s2, T& r){
        T tmp;
        set_union( s1.begin(), s1.end(), s2.begin(), s2.end(), insert_iterator<T> (tmp, tmp.begin()));
        Copy( tmp, r );
        tmp.clear();
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

	void clear() {
		delete( data );
	}
}; 




struct Edge;
typedef set<int, less<int> >    Transet;
typedef Transet::iterator       TransetIt;
typedef vector<Edge>		Edges;		
typedef Edges::iterator         EdgesIt;
#define	Attrset		Transet
#define AttrsetIt	TransetIt
#define EDGE(i, j)	edgesMatrix[i*width+j]
typedef vector<float> Ivn;      // InvalidValue
typedef vector<float>::iterator IvnIt;


vector<Transet> *edgesMatrix;


class Cluster{
public:
	Transet T;
	Attrset A;
	bool deleted;

	Cluster(){
		deleted = false;
	}

	bool ColEqual( float delta, Array2D& array )
	{
        	float ave = 0.0;
        	float sum = 0.0;
		TransetIt tit;
		AttrsetIt ait;

		if( delta < 0 )
			return true;

		for( ait=A.begin(); ait!=A.end(); ait++ ){
			ave = 0.0;
                	for(tit=T.begin(); tit!=T.end(); tit++)
                        	ave += array(*ait, *tit);
                	ave /= T.size();
                	/////////////////////////////////
			sum = 0.0;
                	for(tit=T.begin(); tit!=T.end(); tit++)
                        	sum += fabs( array(*ait, *tit) - ave );
                	sum /= T.size();

                	if( sum > delta )
				return false;
		}
		return true;
	}

	bool RowEqual( float delta, Array2D& array )
	{
        	float ave = 0.0;
        	float sum = 0.0;
		TransetIt tit;
		AttrsetIt ait;

		if( delta < 0 )
			return true;

		for( tit=T.begin(); tit!=T.end(); tit++ ){
			ave = 0.0;
                	for(ait=A.begin(); ait!=A.end(); ait++)
                        	ave += array(*ait, *tit);
                	ave /= A.size();
                	/////////////////////////////////
			sum = 0.0;
                	for(ait=A.begin(); ait!=A.end(); ait++)
                        	sum += fabs( array(*ait, *tit) - ave );
                	sum /= A.size();

               	 	if( sum > delta )
				return false;
		}
		return true;
	}

        bool contain( Cluster c ){
                Transet U;
                int x;
                                                                                                                                                                                                     
                if( T.size() < c.T.size() || A.size() < c.A.size() )
                        return false;
                                                                                                                                                                                                     
                Union( T, c.T, U );
                if( U.size() > T.size() )
                        return false;
                                                                                                                                                                                                     
                Union( A, c.A, U );
                if( U.size() > A.size() )
                        return false;
                                                                                                                                                                                                     
                return true;
	}

        bool ContainPoint( int t, int a )
        {
                if( t != *find(T.begin(), T.end(), t) )
                        return false;
                if( a != *find(A.begin(), A.end(), a) )
                        return false;
                return true;
        }

        int elementNum(void)
        {
                return T.size() * A.size();
        }

	void show(Array2D &data){
        	TransetIt it, it1;
                int k;
                                                                                                                                                                                                     
                cout << "width x height: " << A.size() << " x " << T.size() << endl;


                for(it=A.begin(), k=0; it!=A.end(); it++){
                        if( k++%20 == 0 ){
                                cout << endl << "        ";
				if(showColName)
                        		cout << endl << "                              ";
			}
			if(showColName){
				printf( "%8s", colName[*it] );
			}
			else
                        	printf( "%4d    ", *it );
                }
                cout << endl;

                for(it=T.begin(), k=0; it!=T.end(); it++){
			if(showRowName)
				printf( "%30s\t", rowName[*it] );
			else
                        	printf( "%4d  ", *it );

                        for(it1=A.begin(), k=0; it1!=A.end(); it1++){
                                if( k++%20 == 0 && k != 1 )
                                        cout << endl << "      ";
                                printf( "%6.2f  ", data(*it1, *it));
                        }
                        cout << endl;
                }
        }
};


inline void TranSub(Transet s1, Transet &s2){
	TransetIt it;
	for(it=s2.begin(); it!=s2.end(); it++)
		s1.erase( find(s1.begin(), s1.end(), *it) );
}



class Clusters
{
	int width;
public:
	vector<Cluster>  CVec;

        bool contain( Cluster &c ){
		vector<Cluster>::iterator  it;	

                for(it=CVec.begin(); it!=CVec.end(); it++)
                        if(it->contain(c))
                                return true;
                return false;
        }
                                                                                                                                                                                                     
        void del_becontained( Cluster &c ){
		vector<Cluster>::iterator  it;	

                it = CVec.begin();
                while( it != CVec.end() ){
                        if( c.contain(*it) ){
                                it->A.clear();
                                it->T.clear();
                                it = CVec.erase(it);
                                //it=CVec.begin();
                        }
                        else
                                it++;
                }
        }

	void show(Array2D &data){
		vector<Cluster>::iterator it;
		int i=1;

		cout << endl <<"=========================== Clusters =========================" << endl;
		for(it=CVec.begin(); it!=CVec.end(); it++)
			if( !it->deleted ){
				cout << endl << "Cluster " << i++ <<": ";
				it->show(data);
			}
	}

        int validNum(){
                int num=0;
		vector<Cluster>::iterator it;
                                                                                                                                                                                                     
                for(it=CVec.begin(); it!=CVec.end(); it++)
                        if( !it->deleted )
                                num++;
                return num;
        }

        void showQuality(Array2D& array){
		vector<Cluster>::iterator it;
                TransetIt tit;
                AttrsetIt ait;
                int eSum, cNum, eCover;
                double Fluctuation, clusFluc, rowFluc, rowAve;
                map<int, int>   Cover;
                                                                                                                                                                                                     
                cout << endl << "==================== Quality ====================" << endl;
                                                                                                                                                                                                     
                cNum = 0;
                eSum = 0;
                Fluctuation = 0.0;
                                                                                                                                                                                                     
                for(it=CVec.begin(); it!=CVec.end(); it++)
                if( !it->deleted ){
                        cNum++;
                        eSum += it->elementNum();
                                                                                                                                                                                                     
                        for(ait=it->A.begin(); ait!=it->A.end(); ait++)
                        for(tit=it->T.begin(); tit!=it->T.end(); tit++)
                                Cover[(*tit) * width + (*ait)] = 1;
                                //Cover[(*tit)*array.width + (*ait)] = 1;
                                                                                                                                                                                                     
                        clusFluc = 0.0;
                        for(tit=it->T.begin(); tit!=it->T.end(); tit++){
                                /////////////////////////////////
                                rowAve = 0.0;
                                for(ait=it->A.begin(); ait!=it->A.end(); ait++)
                                        rowAve += array(*ait, *tit);
                                rowAve /= it->A.size();
                                /////////////////////////////////
                                rowFluc = 0.0;
                                for(ait=it->A.begin(); ait!=it->A.end(); ait++)
                                        rowFluc += fabs( array(*ait, *tit) - rowAve );
                                rowFluc /= it->A.size();
                                if( rowAve > 0.0000000001 )
					rowFluc /= rowAve; // percentage
                                /////////////////////////////////
                                clusFluc += rowFluc;
                        }
                        clusFluc /= it->T.size();
                        Fluctuation += clusFluc * it->elementNum();
                }
                Fluctuation /= eSum;
                eCover = Cover.size();
                Cover.clear();
                                                                                                                                                                                                     
                cout << "Clusters#:\t" << cNum << endl << endl;
                if(cNum !=0 ){
                        cout << "Elements#:\t" << eSum << endl;
                        cout << "Ave Elements#:\t" << eSum / cNum << endl << endl;
                        cout << "Cover:   \t" << eCover << endl;
                        cout << "Ave Cover:\t" << eCover / cNum << endl << endl;
                        printf( "Overlap%%:\t%2.2f%\n\n", double(eSum-eCover) / eCover * 100.0 );
                        printf( "Fluctuation:\t%2.2f%\n\n", Fluctuation*100.0 );
                }
        }

	void EXPAND( Cluster C, Attrset P, int minW, int minH, float deltaR, float deltaC, Array2D& array);
	void getClusters( int minW, int minH, int w, float deltaR, float deltaC, Array2D& array);
	void delet( float overlaped );
	void merge( float overlaped ); 
};




void Clusters::EXPAND( Cluster C, Attrset P, int minW, int minH, float deltaR, float deltaC, Array2D& array)
{
        int x, y;
        Cluster C1;
        vector<Transet>::iterator it;
	AttrsetIt it1;
	vector<Cluster>::iterator cit;
	bool new_clus;

	if( C.A.size() >= minW && C.T.size() >= minH ){
		/*
		cout << "A set: ";
		cout << C.A << endl;
		cout << "T set: ";
		cout << C.T << endl;
		*/
		if( C.RowEqual(deltaR, array) && C.ColEqual(deltaC, array) ){
			if( !contain(C)){
				del_becontained( C );
				CVec.push_back(C);
			}
		}
        }

        while( !P.empty() ){
                                                                                                                                                                                                     
                x = *(P.begin());

                C1 = C;
                C1.A.insert(x);
                P.erase(x);
                //////////////////////////////////////////////
                if( C.A.empty() ){
			cout << "Processing Attribute: " << x << endl;
                        EXPAND( C1, P, minW, minH, deltaR, deltaC, array ); // first time
		}
		else{
                        //y = *(C.A.begin());
                        it1 = C.A.end();
			it1--;
			y = *it1;

                        for(it=EDGE(y,x).begin(); it!=EDGE(y,x).end(); it++){ // continue to extend
                                                                                                                                                                                                     
                                if( C.A.size() == 1 ) // at the beginning
                                        Copy( *it, C1.T );
                                else
                                        Intersect( C.T, *it, C1.T );

                                if( C1.T.size() >= minH )
                                        EXPAND( C1, P, minW, minH, deltaR, deltaC, array );
                        }
                }
                //////////////////////////////////////////////
        }
}



void Clusters::getClusters( int minW, int minH, int w, float deltaR, float deltaC, Array2D& array )
{
	Cluster C;
	Attrset P;
	int Edges=0;
	width = w;

	for(int i=0; i<width; i++)
	for(int j=0; j<width; j++)
		Edges += EDGE(i,j).size();
	
	cout << "Got " << Edges << " edges, Average edges: " << Edges/(width*width) << endl;

	for( int i=0; i<width; i++ )
		P.insert(i);
	EXPAND( C, P, minW, minH, deltaR, deltaC, array);
}


void Clusters::delet( float overlaped )
{
	vector<Cluster>::iterator  it1, it2, it3;
        TransetIt  tit;
        AttrsetIt  ait;
        int        in, total;
                                                                                                                                                                                                     
                                                                                                                                                                                                     
        for( it1=CVec.begin(); it1!=CVec.end(); it1++ ){
                in = 0;
                total= 0;
                                                                                                                                                                                                     
                for( tit=it1->T.begin(); tit!=it1->T.end(); tit++)
                for( ait=it1->A.begin(); ait!=it1->A.end(); ait++){ // loop every point
                                                                                                                                                                                                     
                        for( it2=CVec.begin(); it2!=CVec.end(); it2++)
                                if( it1 != it2 && ! it2->deleted ){
                                        if( it2->ContainPoint(*tit, *ait)){
                                                in++;
                                                break;
                                        }
                                }
                        total++;
                }
	        //cout << "in: " << in << ", total: " << total << endl;
                if( (double)in / total >= overlaped ){
                        it1->deleted = true;
                }
        }
}



void Clusters::merge( float overlaped ) // column share will be achieved by shrinking columns/ increasing rows.
{
	vector<Cluster>::iterator  it1, it2, it3;
        Transet         TU;
        Attrset         AU;
        TransetIt       tit;
        AttrsetIt       ait;
        Cluster         mycluster;
        bool            noMerge;
        int             in, total;
                                                                                                                                                                                                     
start:
        noMerge = true;
        it3 = CVec.end();
                                                                                                                                                                                                     
        for( it1=CVec.begin(); it1!=it3; it1++)
        for( it2=it1, ++it2; it2!=it3; it2++)
        if( !it1->deleted && !it2->deleted ){
                                                                                                                                                                                                     
                in = 0;
                total = 0;
                                                                                                                                                                                                     
                Intersect( it1->A, it2->A, AU );
                //cout << AU.size() << " " << it1->A.size() << " " << it2->A.size() << endl;
                                                                                                                                                                                                     
                if( AU.size() < (it1->A.size() + it2->A.size()) / 3 )
                        continue;
                Intersect( it1->T, it2->T, TU );
                if( TU.size() == 0 )
                        continue;
                                                                                                                                                                                                     
                Union( it1->T, it2->T, TU );
                Union( it1->A, it2->A, AU );
                                                                                                                                                                                                     
                for( tit=TU.begin(); tit!=TU.end(); tit++)
                for( ait=AU.begin(); ait!=AU.end(); ait++){ // loop every point
                        if( it2->ContainPoint(*tit, *ait) || it1->ContainPoint(*tit, *ait) )
                                in++;
                        total++;
                }
                                                                                                                                                                                                     
                //cout << "in: " << in << ", total: " << total << endl;
                                                                                                                                                                                                     
                if( (double)in / total >= overlaped ){
                        Copy( TU, mycluster.T );
                        Copy( AU, mycluster.A );
                                                                                                                                                                                                     
                        it1->deleted = true;
                        it2->deleted = true;
                                                                                                                                                                                                     
                        CVec.push_back( mycluster );
                        noMerge = false;
                }
        }
        if( !noMerge ){
                //cout << "go start" << endl;
                goto start;
        }
}



	
class Ranges
{
        string file;
        float max_value;

	vector<Transet> *pM;
public:
        Array2D array;

        int width, height;
                                                                                                                                                                                                     
        Ranges(char* name) {
                file = name;
        }


        void GetStr2Tab( int &pos, int len, char *str, char*sub ){
                int p=0;
                                                                                                                                                                                                     
                while( str[pos]!= '\t' && pos < len ){
                        if( str[pos] != 0 ){
                                sub[p] = str[pos];
                                p++;
                        }
                        pos++;
                }
                sub[p] = '\0';
                                                                                                                                                                                                     
                while( str[pos] == '\t' )
                        pos++;
                                                                                                                                                                                                     
                assert( p < 1000 );
                assert( len < 100000 );
        }
                                                                                                                                                                                                     
                                                                                                                                                                                                     
        void GetTabFileSize( bool show ){
                char buf[100000], sub[1000];
                float cell;
                int w, h, len, k, w1;
                ifstream in;
                                                                                                                                                                                                     
                in.open(file.c_str());
                                                                                                                                                                                                     
                w = h = 0;
                in.getline(buf, 100000, '\n');
                len = strlen(buf);
                                                                                                                                                                                                     
                k = 0;
                while( k < len ){
                        GetStr2Tab( k, len, buf, sub );         if(show) cout << sub << '\t';
                        w++;
                }                                               if(show) cout << endl;
                w -= 2;
                                                                                                                                                                                                     
                //                                              if(show) cout << "width: " << w << endl;
                in.getline(buf, 100000, '\n');
                do{
                        int tmp=0;
                        for(int i=0; i<strlen(buf); i++){
                                if( buf[i] == '\t' )
                                        tmp++;
                        }
                        //cout << endl << "tab number: " << tmp << endl;
                                                                                                                                                                                                     
                        len = strlen(buf);
                        k = 0;
                        GetStr2Tab( k, len, buf, sub );         if(show) cout << sub;           // ID 
                        GetStr2Tab( k, len, buf, sub );         if(show) cout << '\t' << sub;   // NAME
                        if( strlen(sub) == 0 )
                                break;
                                                                                                                                                                                                     
                        h++;
                        w1 = 0;
                        while( k < len ){

                                GetStr2Tab( k, len, buf, sub ); if(show) cout << '\t' << sub;
                                w1++;
                                                                                                                                                                                                     
                                if( sub[0] == ' ' ){
                                        for(int i=1; i<strlen(sub); i++)
                                        if( sub[i] != ' ' ){
                                                cout << endl << "error input file, row: " << h << " not a blank" << endl;
                                                exit(0);
                                        }
                                }
                                else if( isdigit( sub[0]) || sub[0] == '-' ){
                                        for(int i=1; i<strlen(sub); i++)
                                        if( !isdigit(sub[i]) && sub[i]!='.' ){
                                                cout << endl << "error input file, row: " << h << " not a number" << endl;
                                                exit(0);
                                        }
                                }
                                else{
                                        cout << endl << "error input file, row: " << h << " first character: " << sub[0];
                                        printf( ", i.e. 0x%x\n", sub[0] );
                                        exit(0);
                                }
                                                                if(show) cout << "\t" << sub;
                        }                                       if(show) cout << endl;
                        if( w1 < w ){
                                cout << endl << "error input file, row: " << h << " col: " << w1 << " missing data" << endl;
                                exit(0);
                        }
                        if( w1 > w ){
                                cout << endl << "error input file, row: " << h << " col: " << w1 << " extra data" << endl;
                                exit(0);
                        }
                                                                                                                                                                                                     
                        in.getline(buf, 100000, '\n');
                }while( !in.eof() );
                                                                                                                                                                                                     
                width = w;
                height = h;
                in.close();
                cout << endl << "width: " << w << endl;
                cout << "height: " << h << endl;
        }
                                                                                                                                                                                                     
        /////////////////////////////////////////

        void ReadTabFile(){
                                                                                                                                                                                                     
                char buf[100000], sub[1000];
                int len, k;
                ifstream in;
                                                                                                                                                                                                     
                array.init( width, height );
                in.open(file.c_str());
                                                                                                                                                                                                     
                in.getline(buf, 100000, '\n');
                len = strlen(buf);
                k=0;
                GetStr2Tab( k, len, buf, sub ); // ID
                GetStr2Tab( k, len, buf, sub ); // NAME

		for(int i=0; i<width; i++){
                        GetStr2Tab( k, len, buf, sub );
			assert( strlen(sub) <=30 );
			strcpy( colName[i], sub );
		}	
                                                                                                                                                                                                     
                for(int j=0; j<height; j++){
                        in.getline(buf, 100000, '\n');
                        len = strlen(buf);
                        k=0;
                        GetStr2Tab( k, len, buf, sub ); // ID
                        GetStr2Tab( k, len, buf, sub ); // NAME
			assert( strlen(sub) <=30 );
			strcpy( rowName[j], sub );

                        for(int i=0; i<width; i++){
                                GetStr2Tab( k, len, buf, sub );
                                if( sub[0] == ' ' )
                                        array(i,j) = -1.0; // no value there
                                else
                                        array(i,j) = atof( sub );
                                                                                                                                                                                                     
                                if( array(i,j) > max_value )
                                        max_value = array(i,j);
                        }
                }
                in.close();
		edgesMatrix = new vector<Transet> [width*width];
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

		edgesMatrix = new vector<Transet> [width*width];
        }
                                                                                                                                                                                                     
        void clear(){
                array.clear();
		for(int i=0; i<width*width; i++)
			edgesMatrix[i].clear();
		delete[] edgesMatrix;
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
                                                                                                                                                                                                     
        void getRanges( float winsz, int minH, bool extend, Ivn &ivn, bool onlyRatioOne );
	void showRanges();
};
                                                                                                                                                                                                     


struct attVecCell{
        float   val;
        int     trans;
};
                                                                                                                                                                                                     
                                                                                                                                                                                                     
struct attVec_less{
        bool operator()(attVecCell c1, attVecCell c2) const
        {
                return(c1.val < c2.val);
	}
};



void Ranges::getRanges( float winsz, int minH, bool extend, Ivn &ivn, bool onlyRatioOne )
{
        attVecCell cell;
	IvnIt iit;
        vector<attVecCell> attVec;
        vector<attVecCell>::iterator it, it1, it2, it3, it4, start_it, end_it;
        int support, sup, max_sup;
                                                                                                                                                                                                     
        float win_size, val1, val2;
        Transet transet;
                                                                                                                                                                                                     
        win_size = 1.0 + winsz;
        support = minH;

        for(int i=0; i<width-1; i++)  // proof: 10 * 1.2 --> 12  <==>  1/12 * 1.2 --> 1/10
        for(int j=i+1; j<width; j++){ // loop every pair of attributes

//cout << "H21" << endl;

                attVec.clear();
		//cout << i << " " << j << endl;
                for(int k=0; k<height; k++){

                        if( fabs(array(i,k)) < 0.0000001 || fabs(array(j,k)) < 0.0000001 ){ // avoid divided by zero and chain broken by zero
                        	if( fabs(array(i,k)) < 0.0000001 && fabs(array(j,k)) < 0.0000001 ){ // both are zero
                        		for(iit=ivn.begin(); iit!=ivn.end(); iit++)
                                		if( fabs(*iit) < 0.0000001 ) // zero is undesired values
                                        		goto SKIP;
                        		cell.val = 1.0;
                        		cell.trans = k;
                        		attVec.push_back( cell );
				}
                                goto SKIP;
			}
//cout << "H22" << endl;

                        for(iit=ivn.begin(); iit!=ivn.end(); iit++){
                                if( fabs(*iit - array(i,k)) < 0.00001 || fabs(*iit - array(j,k)) < 0.00001 ) // undesired values
                                        goto SKIP;
                        }

                        cell.val = array(j,k) / array(i,k);
                        cell.trans = k;
			if( onlyRatioOne ){
				if( 1.0/win_size < cell.val && cell.val < win_size )
                        		attVec.push_back( cell );
			}
			else
                        	attVec.push_back( cell );
			SKIP:;
                }

                                                                                                  
                sort(attVec.begin(), attVec.end(), attVec_less());
		//////////////////////////////////////////////////////// to keep positive
		float vv = attVec.begin()->val;
		if( vv < 0 )
		for( it1=attVec.begin(); it1!=attVec.end(); it1++ )
			it1->val -= vv;
		////////////////////////////////////////////////////////////////////////
                do{
                        it1 = it2 = attVec.begin();
                                                                                                                                                                                                     
                        max_sup = 0;
                        sup = 0;

                        while( it2 != attVec.end() ){

                                if( it1->trans == -1 ){
                                        while( it1 != attVec.end() && it1->trans == -1 )
                                                it1++;
                                        it2 = it1;
                                }

                                while( it2 != attVec.end() && it2->trans != -1 && it2->val <= it1->val * win_size ){ // positive values are required 
                                        it2++;
                                        sup++;
                                }

                                if(sup > max_sup){
                                        start_it = it1;
                                        end_it = it2;
                                        max_sup = sup;
                                }

                                if( it2->trans == -1 ){
                                //if( it2->trans == -1 || it2 != attVec.end() ){
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
                                        while( it2->trans != -1 && it2 != attVec.end() && it2->val <= it1->val * win_size ){ // t2->val - it1->val <= win_size ){
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
                                                while( it3->trans != -1 && it3 != attVec.begin() && it4->val <= it3->val * win_size ){// t4->val - it3->val <= win_size ){
                                                        it3--;
                                                        it4--;
                                                }
                                                if( it3->trans == -1 || it4->val > it3->val * win_size ) //it4->val - it3->val > win_size )
                                                        it3++;
                                        }
                                        start_it = it3;
                                        end_it = it2;
                                }
                                ////////////////////////////////////////////////////////////////////
                                                                                                                                                                                                     
                                for(it=start_it; it!=end_it; it++){
                                        transet.insert( it->trans );
                                        it->trans = -1;
                                }

                                if( transet.size() >= support )
                                        EDGE(i,j).push_back( transet );

                                transet.clear();
                        }
                                                                                                                                                                                                     
                }while( max_sup >= support );
                                                                                                                                                                                                     
                attVec.clear();
        ///////////////////////////////////////////
        }
        // array.clear();
}


ostream& operator<<(ostream& out, Transet &t){
        char buf[200];
        TransetIt it;
        for(it=t.begin(); it!=t.end(); it++){
                sprintf(buf, "%3d ", *it );
                out << buf;
        }
        out << endl;
}



void Ranges::showRanges()
{
	vector<Transet>::iterator  it;

	for(int i=0; i<width-1; i++)  
	for(int j=i+1; j<width; j++){ // loop every pair of attributes

		if( !EDGE(i,j).empty() ){
			cout << "========================= (" << i << ", " << j << ") ==========================" << endl;
			for(it=EDGE(i,j).begin(); it!=EDGE(i,j).end(); it++)
				cout << *it;
		}
	}
}

/*for shifting patterns, this program is sensitive to small values, so add a constant first*/
int main(int argc, char *argv[])
{
	Clusters  clusters;
                                                                                                                                                                                                     
        /////////// default values /////////
                                                                                                                                                                                                     
        char  filename[100];            // -fdatafile.txt
        int   row_sup;                  // -r10
        int   col_sup;                  // -c5
                                                                                                                                                                                                     
        float win_size = 0.01;          // -w0.01
        float del_overlap = 1.00;       // -d0.97
        float mer_overlap = 1.00;       // -m0.95
        Ivn   InvalidNum;
        bool  showClusters1 = false;
        bool  showClusters2 = false;
        bool  showClusters3 = false;
        bool  showQuality1 = false;
        bool  showQuality2 = false;
        bool  showQuality3 = false;
	bool  onlyRatioOne  = false;
        float deltaR = -1.0;                                                                                  
        float deltaC = -1.0;                                                                                  
        ////////////////////////////////////
                                                                                                                                                                                                     
        if(argc < 4) {
                cout << endl << "Usage: " << argv[0] << " -fFile -rRow -cColumn [other options]" << endl << endl;
                cout << "==================== OPTIONS =====================" << endl << endl;
                cout << "File Name:           -fString"         << endl;
                cout << "Minimum Rows:        -rInteger"        << endl;
                cout << "Minimum Columns:     -cInteger"        << endl;
                cout << "Range Window Size:   -wFloat   /*0.01 by default*/"    << endl;
                cout << "Deletion Threshold:  -dFloat   /*1.00 by default*/"    << endl;
                cout << "Merging  Threshold:  -mFloat   /*1.00 by default*/"    << endl;
                cout << "Unrelated Numbers:   -uFloat   /*mciroCluster will not consider the values refered by this option*/" << endl;
                cout << "Only Ratio 1.0:      -e        /*when trying to get the rows with close values only*/" << endl;
                cout << "delta-Row:           -erFloat  /*when trying to let the row values be around equal*/" << endl;
                cout << "delta-Column:        -ecFloat  /*when trying to let the column values be around equal*/" << endl;
                cout << "Show Qualities:      -q123     /*1:Original qualities, 2:Qualities after deletion, 3:Qualities after merging*/" << endl;
                cout << "Output Clusters:     -o123     /*1:Original clusters, 2:Clusters after deletion, 3:Clusters after merging*/" << endl;
                cout << "Output Names:        -nrc      /*r: row names c: column names*/" << endl << endl;
                exit(1);
        }
                                                                                                                                                                                                     
        for(int i=1; i<argc; i++){
                if( argv[i][0] == '-' ){
                        switch( argv[i][1] ){
                                case 'f': strcpy( filename, argv[i]+2 );        break;
                                case 'r': row_sup =     atoi( argv[i]+2 );      break;
                                case 'c': col_sup =     atoi( argv[i]+2 );      break;
                                case 'w': win_size =    atof( argv[i]+2 );      break;
                                case 'd': del_overlap = atof( argv[i]+2 );      break;
                                case 'm': mer_overlap = atof( argv[i]+2 );      break;
                                case 'u': InvalidNum.push_back( atof(argv[i]+2) );      break;
                                case 'e': if( strlen(argv[i]) == 2 ) 
						onlyRatioOne = true;
					  else{
                                                switch( argv[i][2] ){
                                                        case 'r': deltaR = atof( argv[i]+3 ); break;
                                                        case 'c': deltaC = atof( argv[i]+3 ); break;
                                                        default: cout << "Error Input Options: " << argv[i] << endl; exit(0); break;
                                                }
					  }
					  break;

                                case 'q': for(int k=2; k<strlen(argv[i]); k++)
                                                switch( argv[i][k] ){
                                                        case '1': showQuality1 = true; break;
                                                        case '2': showQuality2 = true; break;
                                                        case '3': showQuality3 = true; break;
                                                        default: cout << "Error Input Options: " << argv[i] << endl; exit(0); break;
                                                }
                                          break;
                                case 'o': for(int k=2; k<strlen(argv[i]); k++)
                                                switch( argv[i][k] ){
                                                        case '1': showClusters1 = true; break;
                                                        case '2': showClusters2 = true; break;
                                                        case '3': showClusters3 = true; break;
                                                        default: cout << "Error Input Options: " << argv[i] << endl; exit(0); break;
                                                }
                                          break;
                                case 'n': for(int k=2; k<strlen(argv[i]); k++)
                                                switch( argv[i][k] ){
                                                        case 'c': showRowName = true; break;
                                                        case 'r': showColName = true; break;
                                                        default: cout << "Error Input Options: " << argv[i] << endl; exit(0); break;
                                                }
                                          break;
                                default: cout << "Error Input Options: " << argv[i] << endl; exit(0); break;
                        }
                }
                else{
                        cout << "Error Input Options: " << argv[i] << endl;
                        exit(0);
                }
        }

	cout << endl;
	for(int i=0; i<argc; i++)
		cout << argv[i] << " ";
	cout << endl << endl;
                                                                                                                                                                                                     
        cout << "File:\t" << filename << endl;
        cout << "MinRow:\t" << row_sup << endl;
        cout << "MinCol:\t" << col_sup << endl;
        cout << "Win Size:" << win_size << endl;
        cout << "Deletion Threshold: " << del_overlap << endl;
        cout << "Merging  Threshold: " << mer_overlap << endl;
        if(deltaR>0)                                                                                                                                             
        	cout << "delta-Row: " << deltaR << endl;
        if(deltaC>0)                                                                                                                                             
        	cout << "delta-Column:: " << deltaC << endl;

        Timer  T("Time ");  T.Start();

        ///////////////////////////////////////////////////////////////////
        Ranges ranges( filename );
        ranges.GetTabFileSize(false);   // show data or not
        ranges.ReadTabFile();
        //ranges.showData();

        ranges.getRanges( win_size, row_sup, true, InvalidNum, onlyRatioOne ); // extend = true
	//ranges.showRanges();

        clusters.getClusters( col_sup, row_sup, ranges.width, deltaR, deltaC, ranges.array);
                                                                                                  
        ///////////////////////////////////////////////////////////////////
	cout << endl << "Original clusters: " << clusters.validNum();
        if( showClusters1 )
        	clusters.show(ranges.array);

        if(showQuality1) 
		clusters.showQuality(ranges.array);

        ///////////////////////////////////////////////////////////////////
	
	if( fabs(del_overlap - 1.0) > 0.00001 ){
        	clusters.delet( del_overlap );
       		cout << endl << "Clusters after deletion: " << clusters.validNum();
        	if( showClusters2 )
        		clusters.show(ranges.array);
	}

       	if(showQuality2) 
		clusters.showQuality(ranges.array);

        ///////////////////////////////////////////////////////////////////
        
	if( fabs(mer_overlap - 1.0) > 0.00001 ){
		clusters.merge( mer_overlap );
        	cout << endl << "Clusters after merging: " << clusters.validNum();
        	if( showClusters3 )
        		clusters.show(ranges.array);
	}

       	if(showQuality3) 
		clusters.showQuality(ranges.array);

        ///////////////////////////////////////////////////////////////////
                                                                                           
        T.Stop();  cout << endl << T << endl;
                                                                                                                                                                                                     
        ranges.clear();
        InvalidNum.clear();
        return 0;
}

