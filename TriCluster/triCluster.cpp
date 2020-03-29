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
////////////////// FOR DEBUG //////////////////
#define R(x) cout << "Run Here: " << x << endl;
///////////////////////////////////////////////
bool BRIEF_OUT = false;

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

inline bool fZero( float f ){
	return( fabs(f) < 0.0000001 );
}

#define NAME_LEN	50
struct Name{
	char str[NAME_LEN];
};

struct Ratio{
        float   val;
        int     trans;
	bool	f1Neg; // when val < 0, and the denominator > 0, it is true.
};

struct RatioLess{
        bool operator()(Ratio r1, Ratio r2) const{
                return(r1.val < r2.val);
	}
};

typedef vector<float> Ivn; // InvalidValue
typedef vector<float>::iterator IvnIt;

typedef set<int, less<int> >  Dset;
typedef Dset::iterator        DsetIt;




class Array3D
{
	float	*data;
public:
	Name	*tName, *sName, *gName;
	int	T, S, G;

        void init( int t, int s, int g ){
                T=t; G=g; S=s;
                data = new float[T*S*G];  assert( data != NULL );
                tName = new Name[T];  assert( tName != NULL );
                sName = new Name[S];  assert( sName != NULL );
                gName = new Name[G];  assert( gName != NULL );
        }
                                                                                                                                                                                                     
        float& dat( int t, int s, int g ){
                return data[t*(S*G)+s*G+g];
        }

	float* get2d( int t ){ 
                return data + t*(S*G);
	}

        float& operator()( int t, int s, int g ){
                return data[t*(S*G)+s*G+g];
        }

	void clear(){
		delete   data;
		delete[] tName;
		delete[] sName;
		delete[] gName;
	}

        void show(){
                cout << endl; 
                cout << "Total Times:\t"   << T << endl;
                cout << "Total Samples:\t" << S << endl;
                cout << "Total Genes:\t"   << G << endl;
                                                                                                                                                                                                     
                for(int i=0; i<T; i++){
                        cout << "Time\t" << i << endl;
                        cout << "ID\tNAME";
                        for(int k=0; k<S; k++)
                                cout << "\tS-" << k;
                        cout << endl;
                                                                                                                                                                                                     
                        for(int j=0; j<G; j++){
                                cout << j << "\tG-" << j;
                                for(int k=0; k<S; k++)
                                        printf( "\t%2.2f", dat(i,k,j) );
                                cout << endl;
                        }
                }
                cout << endl;
        }
}; 




class TabInput
{
	#define MAX_LINE	100000
	#define MAX_WORD	1000
        char	buf[MAX_LINE], sub[MAX_WORD];
        int	len, k;
        ifstream infile;
	
public:
	void open(char *file){
	        infile.open(file);
	}
	
	void close(){
		infile.close();
	}

	void getLine(){
                infile.getline(buf, MAX_LINE, '\n');
		len = strlen( buf );
		assert(len <= MAX_LINE);
		k=0;
	}

        char* get2tab(){
                int p=0;
                                                                                                    
                if(buf[k] == '\t' || buf[k]=='\n'){
                        sub[0] = '\0';
			if(buf[k] == '\t')
                        	k++;
                }
                else{                                                                                    
                	do{                                                                                                     
                        	sub[p++] = buf[k++];
                	}while( buf[k]!='\t' && buf[k]!=0xd && k<len ); //oxd for ^M
			k++;
                	sub[p] = '\0';
                	//assert(p <= MAX_WORD);
                	assert(p <= NAME_LEN);
		}
                return (char *)sub;
        }
};




class Input 
{
        char file[100], *p;
	Array3D* pArray3d;
	TabInput in;
public:
        Input( char* name, Array3D& array ){
                strcpy( file, name );
		pArray3d = (Array3D*) &array;
        }

        void readTabFile(){
                int T, S, G;

		in.open( file );

                in.getLine();  in.get2tab();  p=in.get2tab(); // T 
                pArray3d->T = T = atoi( p );

                in.getLine();  in.get2tab();  p=in.get2tab(); // S
                pArray3d->S = S = atoi( p );

                in.getLine();  in.get2tab();  p=in.get2tab(); // G 
                pArray3d->G = G = atoi( p );

                pArray3d->init( T, S, G );

		for(int i=0; i<T; i++){
                	in.getLine();  in.get2tab();  p=in.get2tab(); 
			strcpy(pArray3d->tName[i].str, p);  

                	in.getLine(); 

			if( i==0 ){
                		in.get2tab();  in.get2tab(); // ID and NAME
				for(int k=0; k<S; k++){
                			p = in.get2tab();
					strcpy(pArray3d->sName[k].str, p);  
				}
			}

			for(int j=0; j<G; j++){
                		in.getLine();  in.get2tab();  p=in.get2tab(); // ID and NAME
				if( i==0 )
					strcpy(pArray3d->gName[j].str, p); 
				
				for(int k=0; k<S; k++){
					p = in.get2tab();
					// assert p is a float;
					pArray3d->dat(i,k,j) = atof(p);
					// TMP times 10
					//if( k!= 0 );
					//	cout << "\t";
					//printf( "%2d", int(atof(p)*100) );
				}
				// TMP times 100
				// cout << endl;	
                        }
                }
                in.close();
        }
};
                



class Rect{
public:
	Dset S, G;
	bool deleted;

	Rect(){
		deleted = false;
	}

	void clear(){
		S.clear();
		G.clear();
	}

        bool contain( Rect r ){
                Dset U;

                if( S.size() < r.S.size() || G.size() < r.G.size() )
                        return false;
                Union( S, r.S, U );
                if( U.size() > S.size() )
                        return false;
                Union( G, r.G, U );
                if( U.size() > G.size() )
                        return false;
                return true;
	}

        bool containPoint( int s, int g )
        {
                if( s != *find(S.begin(), S.end(), s) )
                        return false;
                if( g != *find(G.begin(), G.end(), g) )
                        return false;
                return true;
        }

        int elementNum(void)
        {
                return S.size() * G.size();
        }
	
	void show(Array3D &array3d, int T ){
		#define data(s, g)	array3d(T,s,g)
        	DsetIt its, itg;
		int width = array3d.S;

                cout << "|S| x |G|: " << S.size() << " x " << G.size() << endl;
		cout << "                              ";
		for(its=S.begin(); its!=S.end(); its++)
			printf( "%14s", array3d.sName[*its].str );
                cout << endl;
                for(itg=G.begin(); itg!=G.end(); itg++){
			printf( "%30s\t", array3d.gName[*itg].str );
                        for(its=S.begin(); its!=S.end(); its++)
                                printf( "      %6.2f  ", data(*its, *itg));
                        cout << endl;
                }
        }
};




class Rects
{
public:
	vector<Rect>  rectVec;

	void clear(){
		vector<Rect>::iterator  it;	
                for(it=rectVec.begin(); it!=rectVec.end(); it++)
			it->clear();
		rectVec.clear();
	}

        bool contain( Rect &r ){
		vector<Rect>::iterator  it;	

                for(it=rectVec.begin(); it!=rectVec.end(); it++)
                        if(it->contain(r))
                                return true;
                return false;
        }
                                                                                                                                                                                                     
        void del_becontained( Rect &r ){
		vector<Rect>::iterator  it;	

                it = rectVec.begin();
                while( it != rectVec.end() ){
                        if( r.contain(*it) ){
                                it->S.clear();
                                it->G.clear();
                                it = rectVec.erase(it);
                        }
                        else	it++;
                }
        }

	void addIfMax( Rect &r ){
		if( !contain(r) ){
			del_becontained(r);
			rectVec.push_back(r);
		}
	}

	void show(Array3D &array3d, int T ){
		vector<Rect>::iterator it;
		int i=0;

		cout << endl << "================================================ Time: " << T << " =================================================" << endl;
		for(it=rectVec.begin(); it!=rectVec.end(); it++)
			if( !it->deleted ){
				cout << endl << "Rect " << i++ <<": ";
				it->show(array3d, T);
			}
	}

        int validNum(){
                int num=0;
		vector<Rect>::iterator it;
                for(it=rectVec.begin(); it!=rectVec.end(); it++)
                        if( !it->deleted )
                                num++;
                return num;
        }
};




class Cube{
public:
	Dset T, S, G;
	bool deleted;

	Cube(){
		deleted = false;
	}

	void clear(){
		T.clear();
		S.clear();
		G.clear();
	}

        bool contain( Cube c ){
                Dset U;

                if( T.size() < c.T.size() || S.size() < c.S.size() || G.size() < c.G.size() )
                        return false;
                Union( T, c.T, U );
                if( U.size() > T.size() )
                        return false;
                Union( S, c.S, U );
                if( U.size() > S.size() )
                        return false;
                Union( G, c.G, U );
                if( U.size() > G.size() )
                        return false;
                return true;
	}

        bool containPoint( int t, int s, int g )
        {
                if( t != *find(T.begin(), T.end(), t) )
                        return false;
                if( s != *find(S.begin(), S.end(), s) )
                        return false;
                if( g != *find(G.begin(), G.end(), g) )
                        return false;
                return true;
        }

        int elementNum(void)
        {
                return T.size() * S.size() * G.size();
        }

// Output for GNUPLOT
#if 0	
	void show( Array3D &array3d, ostream& out ){
        	DsetIt itt, its, itg;
		char buf[1000];
		int width = array3d.S;
		int number;
               	out << endl << "|T|x|S|x|G|: " << T.size() << "x" << S.size() << "x" << G.size() << endl;
		/*
		// [Xaxis x Curve x Fig] = GxSxT
		for(itt=T.begin(); itt!=T.end(); itt++){
			{ sprintf( buf, "#Time: %s\n", array3d.tName[*itt].str ); out << buf; }
			out << "#Sample:";
			for(its=S.begin(); its!=S.end(); its++)
				{ sprintf(buf, "\t%s", array3d.sName[*its].str ); out << buf; }
                	out << endl;
			number = 0;
                	for(itg=G.begin(); itg!=G.end(); itg++){
				out << number++;
                        	for(its=S.begin(); its!=S.end(); its++)
                                	{ sprintf( buf, "%5.1f", array3d(*itt, *its, *itg)); out << buf; }
                        	out << endl;
                	}
			out << endl;
		}
		*/
		/*
		// [Xaxis x Curve x Fig] = GxTxS
		for(its=S.begin(); its!=S.end(); its++){
			{ sprintf( buf, "#Sample: %s\n", array3d.sName[*its].str ); out << buf; }
			out << "#Time:";
			for(itt=T.begin(); itt!=T.end(); itt++)
				{ sprintf(buf, "\t%s", array3d.tName[*itt].str ); out << buf; }
                	out << endl;
			number = 0;
                	for(itg=G.begin(); itg!=G.end(); itg++){
				out << number++;
				for(itt=T.begin(); itt!=T.end(); itt++)
                                	{ sprintf( buf, "\t%5.1f", array3d(*itt, *its, *itg)); out << buf; }
                        	out << endl;
                	}
			out << endl;
		}
		*/
		/*	
		// [Xaxis x Curve x Fig] = TxGxS
		for(its=S.begin(); its!=S.end(); its++){
			{ sprintf( buf, "#Sample: %s\n", array3d.sName[*its].str ); out << buf; }
			out << "#Gene:";
                	for(itg=G.begin(); itg!=G.end(); itg++)
				{ sprintf(buf, "\t%s", array3d.gName[*itg].str ); out << buf; }
                	out << endl;
			for(itt=T.begin(); itt!=T.end(); itt++){
				{ sprintf(buf, "\%s", array3d.tName[*itt].str ); out << buf; }
                		for(itg=G.begin(); itg!=G.end(); itg++)
                                	{ sprintf( buf, "\t%5.1f", array3d(*itt, *its, *itg)); out << buf; }
                        	out << endl;
                	}
			out << endl;
		}
		*/
		// 3D:[Xaxis x Yaxis x Fig] = GxTxS
		for(its=S.begin(); its!=S.end(); its++){
			{ sprintf( buf, "#Sample: %s\n", array3d.sName[*its].str ); out << buf; }
                	for(itg=G.begin(); itg!=G.end(); itg++){
				for(itt=T.begin(); itt!=T.end(); itt++)
                                	{ sprintf( buf, "%5.1f\t", array3d(*itt, *its, *itg)); out << buf; }
                        	out << endl;
                	}
			out << endl;
		}
		
        }
#else
	void show( Array3D &array3d, ostream& out ){
        	DsetIt itt, its, itg;
		char buf[1000];
		int width = array3d.S;

               	out << endl << "|T|x|S|x|G|: " << T.size() << "x" << S.size() << "x" << G.size() << endl;
		for(itt=T.begin(); itt!=T.end(); itt++){
			{ sprintf( buf, "    Time: %s\n", array3d.tName[*itt].str ); out << buf; }
			out << "                              ";
			for(its=S.begin(); its!=S.end(); its++)
				{ sprintf(buf, "%14s", array3d.sName[*its].str ); out << buf; }
                	out << endl;
                	for(itg=G.begin(); itg!=G.end(); itg++){
				{ sprintf(buf, "%30s\t", array3d.gName[*itg].str ); out << buf; }
                        	for(its=S.begin(); its!=S.end(); its++)
                                	{ sprintf( buf, "      %6.2f  ", array3d(*itt, *its, *itg)); out << buf; }
                        	out << endl;
                	}
			out << endl;
		}
	}
#endif

	float variance( char dimension, Array3D& array3d )
	{
       		double	ave = 0.0;
	       	double	sum = 0.0;
	       	double	SUM = 0.0;
		DsetIt	it1, it2, it3, itS1, itS2, itS3, itE1, itE2, itE3;

		switch( dimension ){
			case 'T': itS1=S.begin();  itE1=S.end();  itS2=G.begin();  itE2=G.end();  itS3=T.begin();  itE3=T.end();  break;
			case 'S': itS1=T.begin();  itE1=T.end();  itS2=G.begin();  itE2=G.end();  itS3=S.begin();  itE3=S.end();  break;
			case 'G': itS1=T.begin();  itE1=T.end();  itS2=S.begin();  itE2=S.end();  itS3=G.begin();  itE3=G.end();  break;
		}
	        for(it1=itS1; it1!=itE1; it1++)
	        for(it2=itS2; it2!=itE2; it2++){
			ave = 0.0;
	        	for(it3=itS3; it3!=itE3; it3++)
			switch( dimension ){
                       		case 'T': ave += array3d(*it3, *it1, *it2);  break;
                       		case 'S': ave += array3d(*it1, *it3, *it2);  break;
                       		case 'G': ave += array3d(*it1, *it2, *it3);  break;
			}
			switch( dimension ){
				case 'T': ave /= T.size();  break;
				case 'S': ave /= S.size();  break;
				case 'G': ave /= G.size();  break;
			}
			sum = 0.0;
	        	for(it3=itS3; it3!=itE3; it3++)
			switch( dimension ){
                       		case 'T': sum += fabs(array3d(*it3, *it1, *it2) - ave);  break;
                       		case 'S': sum += fabs(array3d(*it1, *it3, *it2) - ave);  break;
                       		case 'G': sum += fabs(array3d(*it1, *it2, *it3) - ave);  break;
			}
			switch( dimension ){
               			case 'T': sum /= T.size();  break;
               			case 'S': sum /= S.size();  break;
               			case 'G': sum /= G.size();  break;
			}
			SUM += sum;
		}
		switch( dimension ){
               		case 'T': SUM /= S.size() * G.size();  break;
               		case 'S': SUM /= T.size() * G.size();  break;
               		case 'G': SUM /= T.size() * S.size();  break;
		}
		return (float) SUM;
	}
};




class Cubes
{
public:
	vector<Cube>  cubeVec;

	void clear(){
		vector<Cube>::iterator  it;	
                for(it=cubeVec.begin(); it!=cubeVec.end(); it++)
			it->clear();
		cubeVec.clear();
	}
		
        bool contain( Cube &c ){
		vector<Cube>::iterator  it;	

                for(it=cubeVec.begin(); it!=cubeVec.end(); it++)
                        if(it->contain(c))
                                return true;
                return false;
        }
                                                                                                                                                                                                     
        void del_becontained( Cube &c ){
		vector<Cube>::iterator  it;	

                it = cubeVec.begin();
                while( it != cubeVec.end() ){
                        if( c.contain(*it) ){
                                it->T.clear();
                                it->S.clear();
                                it->G.clear();
                                it = cubeVec.erase(it);
                        }
                        else	it++;
                }
        }

	void addIfMax( Cube &c ){
		if( !contain(c) ){
			del_becontained(c);
			cubeVec.push_back(c);
		}
	}

	void show( Array3D &array3d, ostream& out ){
		vector<Cube>::iterator it;
		int i=0;

		out << endl << "===================================================== Clusters ======================================================" << endl;
		for(it=cubeVec.begin(); it!=cubeVec.end(); it++)
			if( !it->deleted ){
				out << endl << "Cluster " << i++ <<": " << endl;
				out << "=====================================================================================================================" << endl;
				it->show( array3d, out );
			}
	}
	
        int validNum(){
                int num=0;
		vector<Cube>::iterator it;
                for(it=cubeVec.begin(); it!=cubeVec.end(); it++)
                        if( !it->deleted )
                                num++;
                return num;
        }

	void delet( float overlaped ){
		vector<Cube>::iterator  it1, it2;
        	DsetIt  itt, its, itg;
        	int     in, total;
                                                                                                                                                                                                     
        	for( it1=cubeVec.begin(); it1!=cubeVec.end(); it1++ ){
                	in = 0;
                	total= 0;

                	for( itt=it1->T.begin(); itt!=it1->T.end(); itt++)
                	for( its=it1->S.begin(); its!=it1->S.end(); its++)
                	for( itg=it1->G.begin(); itg!=it1->G.end(); itg++){ // loop every point
                        	for( it2=cubeVec.begin(); it2!=cubeVec.end(); it2++)
                               	if( it1!=it2 && !it2->deleted )
                                if( it2->containPoint(*itt, *its, *itg)){
                                        in++;
                                         break;
                                }
                        	total++;
                	}
                	if( (double)in / total >= overlaped )
                        	it1->deleted = true;
        	}
	}

	void merge( float overlaped ) 
	{
		vector<Cube>::iterator  it1, it2, it3;
       		DsetIt  itt, its, itg;
        	Cube	cube;
        	bool	noMerge;
        	int	in, total;
                                                                                                                                                                                                     
	start:
		it3 = cubeVec.end();
        	noMerge = true;
	        for(it1=cubeVec.begin(); it1!=it3; it1++) // cubeVec.end() changes as times
        	for(it2=it1, ++it2; it2!=it3; it2++)
	        if( !it1->deleted && !it2->deleted ){
        	        in = 0;
                	total = 0;
	                Union( it1->T, it2->T, cube.T );
        	        Union( it1->S, it2->S, cube.S );
                	Union( it1->G, it2->G, cube.G );

	               	for( itt=cube.T.begin(); itt!=cube.T.end(); itt++)
        	       	for( its=cube.S.begin(); its!=cube.S.end(); its++)
               		for( itg=cube.G.begin(); itg!=cube.G.end(); itg++){ // loop every point
                        	if( it2->containPoint(*itt, *its, *itg) || it1->containPoint(*itt, *its, *itg) )
                                	in++;
	                        total++;
        	        }
	                if( (double)in / total >= overlaped ){
	                        it1->deleted = true;
        	                it2->deleted = true;
                	        cubeVec.push_back( cube );
                        	noMerge = false;
	                }
        	}
	        if( !noMerge )
        	        goto start;
	}


	void showQuality( Array3D& array3d, ostream& out ){
		vector<Cube>::iterator  it;
       		DsetIt  itt, its, itg;
                int eSum, cNum, eCover;
                double tFluc=0;
                double sFluc=0;
                double gFluc=0;
                map<int, int>   Cover;
		char buf[1000];

                out << endl << "====================================================== Quality ======================================================" << endl;
                cNum = 0;
                eSum = 0;
                for(it=cubeVec.begin(); it!=cubeVec.end(); it++)
                if( !it->deleted ){
                        cNum++;
                        eSum += it->elementNum();

                        tFluc += it->variance('T', array3d) * it->elementNum();
                        sFluc += it->variance('S', array3d) * it->elementNum();
                        gFluc += it->variance('G', array3d) * it->elementNum();

                        for(itt=it->T.begin(); itt!=it->T.end(); itt++)
                        for(its=it->S.begin(); its!=it->S.end(); its++)
                        for(itg=it->G.begin(); itg!=it->G.end(); itg++)
                		Cover[(*itt)*(array3d.S*array3d.G)+(*its)*array3d.G+(*itg)] = 1;
                }
                tFluc /= eSum;
                sFluc /= eSum;
                gFluc /= eSum;
                eCover = Cover.size();
                Cover.clear();

                out << "Clusters#:\t" << cNum << endl;
                if(cNum !=0 ){
                        out << "Elements#:\t" << eSum << endl;
                        out << "Cover:   \t" << eCover << endl;
                        sprintf( buf, "Overlap%%:\t%2.2f%\n", double(eSum-eCover) / eCover * 100.0 );  out << buf;
                        sprintf( buf, "Variance:\tT:%2.2f    S:%2.2f    G:%2.2f\n", tFluc, sFluc, gFluc );  out << buf;
                }
        }
};




class Cluster
{
	#define EDGE(i, j)      edgesMatrix[i*width+j]
	vector<Dset>		*edgesMatrix;
	Rects			*pRects; // times T
	Cubes			*pCubes; // only one
	int			width, height;
	Array3D*		pArray3d;
public:
	Cluster( Array3D& array3d, Cubes& cubes ){
		pArray3d = (Array3D*) &array3d;
		width  = array3d.S;
		height = array3d.G;
 		edgesMatrix = new vector<Dset> [width*width];
 		pRects = new Rects[array3d.T];
		pCubes = (Cubes *) &cubes;
	}

	void clear(){
		delete[] edgesMatrix;
		for(int i=0; i<pArray3d->T; i++)
			pRects[i].clear();
		delete[] pRects;
	}

	void getRects( int T, Rects &rects, float winsz, int* support, float* eDelta, Ivn &ivn )
	{
		Rect r;
		Dset P;
		int  width = pArray3d->S;
		int  edges = 0;

		getRanges( T, winsz, support[2], ivn );

        	for(int i=0; i<width-1; i++)
        	for(int j=i+1; j<width; j++)
			edges += EDGE(i,j).size();
		//cout << "Time " << T << ": Got " << edges << " edges, Average edges: " << edges/(width*width) << endl;
		cout << "Time " << pArray3d->tName[T].str << ": Got " << edges << " edges, Average edges: " << edges/(width*width) << endl;

		for( int i=0; i<width; i++ )
			P.insert(i);
		EXPAND( rects, r, P, support, eDelta );

        	for(int i=0; i<width-1; i++)
        	for(int j=i+1; j<width; j++)
			EDGE(i,j).clear();
	}

	void getCubes( float winsz, int* support, float* eDelta, Ivn &ivn )
	{
		Cube c;
		Dset P;
		int  width = pArray3d->T;

		for(int i=0; i<width; i++){
			getRects(i, pRects[i], winsz, support, eDelta, ivn);
			cout << "Biclusters got at this time: " << pRects[i].validNum() << endl;
			//pRects[i].show( *pArray3d, i );
		}
		for(int i=0; i<width; i++)
			P.insert(i);
		EXPAND_T( *pCubes, c, P, support, eDelta );
	}

	void EXPAND( Rects &rects, Rect r, Dset P, int* support, float* eDelta );
	void EXPAND_T( Cubes &cubes, Cube c, Dset P, int* support, float* eDelta );
	void getRanges( int T, float winsz, int gSup, Ivn &ivn );
};




void Cluster::EXPAND_T( Cubes &cubes, Cube c, Dset P, int* support, float* eDelta )
{
        int	x;
        Cube	c1;
	vector<Rect>::iterator  it;
	vector<Cube>::iterator	cit;
	DsetIt	it1;
	int minT = support[0];
	int minS = support[1];
	int minG = support[2];

	if( c.T.size() >= minT && c.S.size() >= minS && c.G.size() >= minG ){
		if((eDelta[0] < 0.0 || c.variance('T', *pArray3d) <= eDelta[0]) &&
		   (eDelta[1] < 0.0 || c.variance('S', *pArray3d) <= eDelta[1]) &&
		   (eDelta[2] < 0.0 || c.variance('G', *pArray3d) <= eDelta[2]) )
			cubes.addIfMax( c );
        }
        while( !P.empty() ){
                x = *(P.begin());
                c1 = c;
                c1.T.insert(x);
                P.erase(x);

                if( c.T.empty() )
			cout << "Processing Time: " << x << endl;

                for(it=pRects[x].rectVec.begin(); it!=pRects[x].rectVec.end(); it++)
                if( !it->deleted ){
        		if( c.T.empty() ){
                		Copy( it->S, c1.S );
                		Copy( it->G, c1.G );
			}
			else{
                               	Intersect( c.S, it->S, c1.S );
                               	Intersect( c.G, it->G, c1.G );
			}
                        if( c1.S.size() >= minS && c1.G.size() >= minG )
                                EXPAND_T( cubes, c1, P, support, eDelta );
                }
        }
}




void Cluster::EXPAND( Rects &rects, Rect r, Dset P, int* support, float* eDelta )
{
        int	x, y;
        Rect	r1;
        vector<Dset>::iterator	it;
	vector<Rect>::iterator	rit;
	DsetIt	it1;
	int minS = support[1];
	int minG = support[2];

	if( r.S.size() >= minS && r.G.size() >= minG )
			rects.addIfMax( r );
        
        while( !P.empty() ){
                x = *(P.begin());
                r1 = r;
                r1.S.insert(x);
                P.erase(x);
                if( r.S.empty() ){
			if(!BRIEF_OUT)
				cout << "Processing Sample: " << x << endl;
                        EXPAND( rects, r1, P, support, eDelta ); // first time
		}
		else{
                        it1 = r.S.end();
			it1--;
			y = *it1;
                        for(it=EDGE(y,x).begin(); it!=EDGE(y,x).end(); it++){ 
                                if( r.S.size() == 1 ) // at the beginning
                                        Copy( *it, r1.G );
                                else
                                        Intersect( r.G, *it, r1.G );
                                if( r1.G.size() >= minG )
                                        EXPAND( rects, r1, P, support, eDelta );
                        }
                }
        }
}




void Cluster::getRanges( int T, float winsz, int gSup, Ivn &ivn )
{
	#define array(x,y)	(*pArray3d)(T,x,y)
        vector<Ratio>		ratioVec;
        vector<Ratio>::iterator	it, it1, it2, it3, it4, start_it, end_it;

        Ratio	ratio;
        Dset	sSet, sSet1;
	IvnIt	iit;
        int	sup, max_sup;
        float	win_size, val1, val2;

        win_size = 1.0 + winsz;

        for(int i=0; i<width-1; i++)  // proof: 10 * 1.2 --> 12  <==>  1/12 * 1.2 --> 1/10
        for(int j=i+1; j<width; j++){ // loop every pair of attributes

                ratioVec.clear();
                for(int k=0; k<height; k++){

                        if(fZero(array(i,k)) || fZero(array(j,k))) // avoid divided by zero
				goto SKIP;
                        for(iit=ivn.begin(); iit!=ivn.end(); iit++)
                        if( fZero(*iit-array(i,k)) || fZero(*iit-array(j,k)) ) // undesired values
                                goto SKIP;

                        ratio.val = array(j,k) / array(i,k);
			if( ratio.val < 0 ){
				if( array(i,k) > 0 )
					ratio.f1Neg = true;
				else
					ratio.f1Neg = false;
			}
                        ratio.trans = k;
                       	ratioVec.push_back( ratio );
			SKIP:;
                }

                sort(ratioVec.begin(), ratioVec.end(), RatioLess());
		//////////////////////////////////////////////////////// to keep positive ??????
		float vv = ratioVec.begin()->val;
		if( vv < 0 )
		for( it=ratioVec.begin(); it!=ratioVec.end(); it++ )
			it->val -= vv;
		////////////////////////////////////////////////////////////////////////
                do{
                        it1 = it2 = ratioVec.begin();
                        max_sup = sup = 0;

                        while( it2 != ratioVec.end() ){
                                if( it1->trans == -1 ){
                                        while( it1 != ratioVec.end() && it1->trans == -1 )
                                                it1++;
                                        it2 = it1;
                                }
                                while( it2 != ratioVec.end() && it2->trans != -1 && it2->val <= it1->val * win_size ) // positive values are required 
                                        { it2++;  sup++; }
                                if(sup > max_sup)
                                        { start_it = it1;  end_it = it2;  max_sup = sup; }
                                if( it2->trans == -1 )
                                        { it1 = it2;  sup = 0; }
                                else{	it1++;  sup--; }
                        }
                        if(max_sup >= gSup){
                                if( true ){ // EXTEND to both directions
                                        it1 = it3 = start_it;
                                        it2 = it4 = end_it;
                                        sup = max_sup;
                                        while( sup > gSup )
                                                { it1++;  sup--; }
                                        while( it2->trans != -1 && it2 != ratioVec.end() && it2->val <= it1->val * win_size )
                                                { it1++;  it2++; }
                                        sup = max_sup;
                                        while( sup > gSup )
                                                { it4--;  sup--; }
                                        if( it3 != ratioVec.begin() ){
                                                it4--;  it3--;
                                                while( it3->trans != -1 && it3 != ratioVec.begin() && it4->val <= it3->val * win_size )
                                                        { it3--;  it4--; }
                                                if( it3->trans == -1 || it4->val > it3->val * win_size ) 
                                                        it3++;
                                        }
                                        start_it = it3;
                                        end_it = it2;
                                }
                                for(it=start_it; it!=end_it; it++){
					if( it->val > 0 || it->f1Neg ) // for -a/a problem
                                        	sSet.insert( it->trans );
					else // if( it->val < 0 && it->f1Neg==false )
                                        	sSet1.insert( it->trans );
                                        it->trans = -1;
                                }
                                if( sSet.size() >= gSup )
                                        EDGE(i,j).push_back( sSet );
                                if( sSet1.size() >= gSup )
                                        EDGE(i,j).push_back( sSet1 );
                                sSet.clear();
                                sSet1.clear();
                        }
                }while( max_sup >= gSup );
                ratioVec.clear();
        }
}




int main(int argc, char *argv[])
{
        char	filename[100];		// -fdatafile.txt
        int	support[3];		// 0:T, 1:S, 2:G
        bool	showClusters[3];	// 0:T, 1:S, 2:G
        float	eDelta[3];		// 0:T, 1:S, 2:G
        Ivn	invalidNum;
        float	win_size = 0.03;        // -w0.01
        float	del_overlap = 1.00;     // -d0.97
        float	mer_overlap = 1.00;     // -m0.95
        char	*p, str[100];
	int	k, len;
        char	outfile[100];		// -frecord.txt
	bool	ifout = false;
	ofstream out;

        if(argc < 3) {
                cout << endl << "Usage: " << argv[0] << " -fFile -s[Times,Samples,Genes] [other options]" << endl << endl;
                cout << "==================== OPTIONS =====================" << endl << endl;
                cout << "File Name:           -fString	/*File name"         << endl;
                cout << "Minimum Size:        -s[Integer,Interge,Integer]   /*T, S, G*/"      << endl;
                cout << "Range Window Size:   -wFloat   /*0.03 by default*/"    << endl;
                cout << "Deletion Threshold:  -dFloat   /*1.00 by default*/"    << endl;
                cout << "Merging  Threshold:  -mFloat   /*1.00 by default*/"    << endl;
                cout << "Unrelated Numbers:   -uFloat   /*mciroCluster will not consider the values refered by this option*/" << endl;
                cout << "delta-T:             -etFloat  /*when trying to get close Time values only*/"   << endl;
                cout << "delta-S:             -esFloat  /*when trying to get close Sample values only*/" << endl;
                cout << "delta-G:             -egFloat  /*when trying to get close Gene values only*/"   << endl;
                cout << "Record to file:      -rString  /*Output the result to a file */"   << endl;
                cout << "output in brief:     -b        /*Output the current status in brief*/"   << endl;
                cout << "Output Clusters:     -o123     /*1:Original clusters,  2:Clusters  after deletion, 3:Clusters  after merging*/" << endl << endl;
                exit(1);
        }
	
	for(int i=0; i<3; i++){
		showClusters[i] = false;
		eDelta[i] = -1.0;
	}

        for(int i=1; i<argc; i++){
                if( argv[i][0] == '-' )
                        switch( argv[i][1] ){
                                case 'f': strcpy( filename, argv[i]+2 );        break;
                                case 'r': strcpy( outfile, argv[i]+2 ); ifout=true;        break;
                                case 's': strcpy( str, argv[i]+3 );
                                          p = str;
					  k = 0;
                                          len = strlen(str);
                                          for(int j=0; j<len; j++){
                                          	if( str[j]==',' || str[j]==']' ){
                                                	str[j] = '\0'; 
							support[k] = atoi(p); 
							p = str+j+1; 
							k++;
					  	} 
                                          	else if( !isdigit(str[j]) && str[j]!=' ' )
                                                	{ cout << "Error Input Options: " << argv[i] << endl; exit(0); }
					  }
                                case 'w': win_size =    atof( argv[i]+2 );      break;
                                case 'd': del_overlap = atof( argv[i]+2 );      break;
                                case 'm': mer_overlap = atof( argv[i]+2 );      break;
                                case 'u': invalidNum.push_back( atof(argv[i]+2) );      break;
                                case 'b': BRIEF_OUT = true;      break;
                                case 'e': switch( argv[i][2] ){
                                          	case 't': eDelta[0] = atof( argv[i]+3 ); break;
                                          	case 's': eDelta[1] = atof( argv[i]+3 ); break;
                                          	case 'g': eDelta[2] = atof( argv[i]+3 ); break;
                                                default: cout << "Error Input Options: " << argv[i] << endl; exit(0); break;
					  }
					  break;
                                case 'o': for(int k=2; k<strlen(argv[i]); k++)
                                                switch( argv[i][k] ){
                                                        case '1': showClusters[0] = true; break;
                                                        case '2': showClusters[1] = true; break;
                                                        case '3': showClusters[2] = true; break;
                                                        default: cout << "Error Input Options: " << argv[i] << endl; exit(0); break;
                                                }
                                          break;
                                default: cout << "Error Input Options: " << argv[i] << endl; exit(0); break;
                        }
                else{	cout << "Error Input Options: " << argv[i] << endl;  exit(0); }
        }
	cout << endl;
	for(int i=0; i<argc; i++)
		cout << argv[i] << " ";
	cout << endl << endl;
                                                                                                                                                                                                     
        cout << "Input File Name:\t" << filename << endl;
        cout << "Cluster Minimum Size:\tT:" << support[0] << ", S:" << support[1] << ", G:" << support[2] << endl;
        cout << "Ratio Window Size:\t" << win_size << endl;
        cout << "Deletion Threshold:\t" << del_overlap << endl;
        cout << "Merging  Threshold:\t" << mer_overlap << endl;
        if( !fZero(eDelta[0]+1.0) )	cout << "delta-T:\t\t" << eDelta[0] << endl;
        if( !fZero(eDelta[1]+1.0) )	cout << "delta-S:\t\t" << eDelta[1] << endl;
        if( !fZero(eDelta[2]+1.0) )	cout << "delta-G:\t\t" << eDelta[2] << endl;
        cout << endl << endl;

        Timer  T("Time ");  T.Start();
        ///////////////////////////////////////////////////////////////////

	Array3D	array3d;
	Cubes	cubes;
	Input	in(filename, array3d);
	in.readTabFile(); //array3d.show();
	Cluster	cluster( array3d, cubes );



	if(ifout)  out.open(outfile);

	if(true){
		cluster.getCubes( win_size, support, eDelta, invalidNum );
		cout << endl << "Original clusters: " << cubes.validNum();
		if(ifout)  out << endl << "Original clusters: " << cubes.validNum();
        	if(showClusters[0]){
			if(ifout)  cubes.show( array3d, out );
			cubes.show( array3d, cout );
		}
		if(ifout)  cubes.showQuality( array3d, out );
		cubes.showQuality( array3d, cout );
	}

	T.Stop();  

	if(!fZero(del_overlap - 1.0)){
		cubes.delet( del_overlap );
       		cout << endl << "Clusters after deletion: " << cubes.validNum();
       		if(ifout)  out << endl << "Clusters after deletion: " << cubes.validNum();
        	if(showClusters[1]){	
			if(ifout)  cubes.show( array3d, out );
			cubes.show( array3d, cout );
		}
		if(ifout)  cubes.showQuality( array3d, out );
		cubes.showQuality( array3d, cout );
	}
        
	if(!fZero(mer_overlap - 1.0)){
		cubes.merge( mer_overlap );
        	cout << endl << "Clusters after merging: " << cubes.validNum();
        	if(ifout)  out << endl << "Clusters after merging: " << cubes.validNum();
        	if(showClusters[2]){
			if(ifout)  cubes.show( array3d, out );
			cubes.show( array3d, cout );
		}
		if(ifout)  cubes.showQuality( array3d, out );
		cubes.showQuality( array3d, cout );
	}

        ///////////////////////////////////////////////////////////////////
        out << endl << T << endl;  cout << endl << T << endl;
	if(ifout)  out.close();
	cluster.clear();
	array3d.clear();
	invalidNum.clear();
	cubes.clear();
        return 0;
}





