#include <iostream>
#include <fstream>
#include <math.h>
#include <cstdlib>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <set>                                                                                 
#include <vector>
#include <map>
#include <cstring>
using namespace std;

char colName[20000][30];
char rowName[20000][30];


class Array2D
{
public:
	int width, height;
	float* data;

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




typedef set<int, less<int> >    Transet;
typedef Transet::iterator       TransetIt;

typedef	map<int, int>		Area;
typedef	map<int, int>::iterator AreaIt;

#define Attrset    Transet
#define AttrsetIt  TransetIt


class Set 
{
public:
	Transet   T;
	Attrset   A;	

	Set(){
	}

	int elementNum(void)
	{
		return T.size() * A.size();
	}

	
	void showCluster(Array2D &data){
		vector<int>::iterator vit;
		TransetIt sit;
		AttrsetIt ait;
		int k;

		cout << "width x height: " << A.size() << " x " << T.size() << endl;



        	for(sit=A.begin(), k=0; sit!=A.end(); sit++){
			if( k++%20 == 0 )
				cout << endl << "        ";
			printf( "%4d    ", *sit );
		}
		cout << endl;

        	for(sit=T.begin(), k=0; sit!=T.end(); sit++){
			printf( "%4d  ", *sit );
        		for(ait=A.begin(), k=0; ait!=A.end(); ait++){
				if( k++%20 == 0 && k != 1 )
					cout << endl << "      ";
				printf( "%6.2f  ", data(*ait, *sit));
			}
			cout << endl;
		}
	}
};



typedef vector<Set>::iterator     SetsIt;


class Sets
{
public:
	vector<Set> data;

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
	
	void showQuality(Array2D& array){
		SetsIt it;
		TransetIt tit;
		AttrsetIt ait;
		int eSum, cNum, eCover;
		double Fluctuation, clusFluc, colFluc, colAve;
		map<int, int>	Cover;
		
                cout << endl << "==================== Quality ====================" << endl;

		cNum = 0;
		eSum = 0;
		Fluctuation = 0.0;

		for(it=data.begin(); it!=data.end(); it++){
			cNum++;
			eSum += it->elementNum();

        		for(ait=it->A.begin(); ait!=it->A.end(); ait++)
        		for(tit=it->T.begin(); tit!=it->T.end(); tit++)
				Cover[(*tit)*array.width + (*ait)] = 1;

			clusFluc = 0.0;
        		for(ait=it->A.begin(); ait!=it->A.end(); ait++){
				/////////////////////////////////
				colAve = 0.0;
        			for(tit=it->T.begin(); tit!=it->T.end(); tit++)
					colAve += array(*ait, *tit);
				colAve /= it->T.size();
				/////////////////////////////////
				colFluc = 0.0;
        			for(tit=it->T.begin(); tit!=it->T.end(); tit++)
					colFluc += fabs( array(*ait, *tit) - colAve );
				colFluc /= it->T.size();
				colFluc /= colAve; // percentage
				/////////////////////////////////
				clusFluc += colFluc;
			}
			clusFluc /= it->A.size();
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

	int  getNum( char *p, int &k ){
		int i;
		char str[100];

		while( !isdigit(p[k]) )
			k++;

		for(i=0; i<100; i++){
			str[i] = p[k];
			k++;
			if( !isdigit(p[k]) )
				break;
		}
		str[i+1] = '\0';
		return atoi( str );
	}

	void GetClustersFile( char *file ){
		char buf[100000];
		int len, k, length, element;
		ifstream in;
		Set c;

                in.open( file );
		in.getline(buf, 100000, '\n');
		do{
			len = strlen(buf);
			/////////////////////////////
			k = 0;
			while( k < len ){
				if( buf[k]=='O' && buf[k+1]=='b' && buf[k+2]=='j' && buf[k+3]=='s' && buf[k+4]=='(' ){
					length = getNum(buf, k);
					for(int i=0; i<length; i++)
						c.T.insert( getNum(buf, k) );
					break; 
				}
				if( buf[k]=='C' && buf[k+1]=='o' && buf[k+2]=='l' && buf[k+3]=='(' ){
					length = getNum(buf, k);
					for(int i=0; i<length; i++)
						c.A.insert( getNum(buf, k) );
					data.push_back(c);
					c.T.clear();
					c.A.clear();
					break;				 
				}
				else
					k++;
			}
			/////////////////////////////	
			in.getline(buf, 100000, '\n');
		}while( !in.eof() ); 
		in.close();
	}
};





class Ranges 
{
public: 
	string file;
	int width, height;
	float max_value;

	Array2D array;

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
			GetStr2Tab( k, len, buf, sub );		if(show) cout << sub << '\t';
			w++;
		}						if(show) cout << endl;
		w -= 2;

		//						if(show) cout << "width: " << w << endl;
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
			GetStr2Tab( k, len, buf, sub );		if(show) cout << sub;		// ID
			GetStr2Tab( k, len, buf, sub ); 	if(show) cout << '\t' << sub;	// NAME
			if( strlen(sub) == 0 )
				break;

			h++;
			w1 = 0;
			while( k < len ){
				GetStr2Tab( k, len, buf, sub );	if(show) cout << '\t' << sub;
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
			}					if(show) cout << endl;
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
        float frandom( float value ){
                return (double)rand()/RAND_MAX * value;
        }

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
					array(i,j) = frandom(10000.0); //-1.0; // no value there, but pCluster can not deal with space+tab
				else
					array(i,j) = atof( sub );

				if( array(i,j) > max_value )
					max_value = array(i,j);
			}
		}
                in.close();
	}

	void showData(){
		int k=0;

		cout << "width: " << width << ", height: " << height << endl;
	
		k = 0;	
        	for(int i=0; i<width; i++){
                	printf( "\t%d", i );
			if( ++k % 23 == 0 )
				cout << endl;
		}
			
        	cout << endl;
                                                                                                                                                                                                     
        	for(int j=0; j<height; j++) {
                	printf( "%d", j );
			k = 0;
                	for(int i=0; i<width; i++){
                        	printf( "\t%2.2f", array(i,j) );
				if( ++k % 23 == 0 )
					cout << endl;
			}
                	cout << endl;
        	}
	}
};



int main(int argc, char *argv[])
{
	Sets clusters;
	ofstream  out;
	char filename[100];

        if(argc < 2) {		
                cout << endl << "Usage: " << argv[0] << " File.tab" << endl << endl;
                exit(1);
        }
        Ranges ranges( argv[1] );

	ranges.GetTabFileSize(false);	// show data or not
	ranges.ReadTabFile(); 
	//ranges.showData();

	strcpy( filename, argv[1] );
	strcat( filename, ".flp" );
	out.open( filename );
	out << "ID\tNAME";
        for(int i=0; i<ranges.height; i++)
                out << "\t" << rowName[i];
	out << endl;
	for(int i=0; i<ranges.width; i++){
		out << i+1 << "\t" << colName[i];
		for(int j=0; j<ranges.height; j++)
			out << "\t" << ranges.array(i, j);
		out << endl;
	}
	out.close();

	return 0;
}




