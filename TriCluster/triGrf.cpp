#include <iostream>
#include <assert.h>
#include <fstream>
#include <math.h>
#include <cstdlib>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <vector>
#include <time.h>

using namespace std;

///////////// FOR DEBUG /////////////////
#define R(x) cout << "Run Here: " << x << endl;
/////////////////////////////////////////

#define TOO_STUFF	1000000

double	 gMAX_VAL;
double	 sMAX_VAL;
double	 tMAX_VAL;

template <class T>
inline void Switch(T &x, T &y)
{
        static T f;
        f = x; x = y; y = f;
}

double frandom( double value ){
       	return (double)rand()/RAND_MAX * value;
}

int irandom( int value ){
	return rand() % value;
}



class Cube 
{
public:
	double *gVal, *sVal, *tVal;	// gene, sample, time
	int g[2], s[2], t[2];		// start, end
	int gn, sn, tn;			// width 

	void setSize( int gg, int ss, int tt, int gN, int sN, int tN ){
		g[0]=gg; s[0]=ss; t[0]=tt; 
		gn=gN;  sn=sN;  tn=tN;	// number

		g[1]=g[0]+gn-1; 
		s[1]=s[0]+sn-1; 
		t[1]=t[0]+tn-1;
	}

	void init(void){
		gVal = new double[gn];
		sVal = new double[sn];
		tVal = new double[tn];

		for(int i=0; i<gn; i++)
			gVal[i] = -1.0;
		for(int i=0; i<sn; i++)
			sVal[i] = -1.0;
		for(int i=0; i<tn; i++)
			tVal[i] = -1.0;
	}

	void randUnusedGST(){
		for(int i=0; i<gn; i++)
		if( fabs(gVal[i]+1.0) < 0.000001 )
			gVal[i] = 1.0 + frandom( gMAX_VAL );
	
		for(int i=0; i<sn; i++)
		if( fabs(sVal[i]+1.0) < 0.000001 )
			sVal[i] = 1.0 + frandom( sMAX_VAL );

		for(int i=0; i<tn; i++)
		if( fabs(tVal[i]+1.0) < 0.000001 )
			tVal[i] = 1.0 + frandom( tMAX_VAL );
	}

	void clear(){
		delete gVal;
		delete sVal;
		delete tVal;
	}

	bool contain( int gg, int ss, int tt ){
		if( gg >= g[0] && gg <= g[1] 
		 && ss >= s[0] && ss <= s[1] 
		 && tt >= t[0] && tt <= t[1] ) 
			return true;
		return false;
	}

	bool contain( Cube& c ){
		for(int i=0; i<2; i++)
		for(int j=0; j<2; j++)
		for(int k=0; k<2; k++)
			if( !contain(c.g[i], c.s[j], c.t[k]) )
				return false;
		return true;
	}
	
	void gstVal( int gg, int ss, int tt, double& gv, double& sv, double& tv ){
		assert( contain(gg,ss,tt) );
		gv = gVal[ gg-g[0] ];
		sv = sVal[ ss-s[0] ];
		tv = tVal[ tt-t[0] ];
	}

	bool gValSet( int i, double val ){
		assert( i>=0 && i<gn );
		if( fabs(gVal[i]-val) < 0.000001 || fabs(gVal[i]+1.0) < 0.000001 ){
			gVal[i] = val;
			return true;
		}
		return false;
	}
	
	bool sValSet( int i, double val ){
		assert( i>=0 && i<sn );
		if( fabs(sVal[i]-val) < 0.000001 || fabs(sVal[i]+1.0) < 0.000001 ){
			sVal[i] = val;
			return true;
		}
		return false;
	}
	
	bool tValSet( int i, double val ){
		assert( i>=0 && i<tn );
		if( fabs(tVal[i]-val) < 0.000001 || fabs(tVal[i]+1.0) < 0.000001 ){
			tVal[i] = val;
			return true;
		}
		return false;
	}
	
	bool inCubes( vector<Cube>& cs ){
		vector<Cube>::iterator it;
		for(it=cs.begin(); it!=cs.end(); it++)
			if( it->contain(*this) )
				return true;
		return false;
	} 

	bool overlap( Cube &c ){
		if( contain(c) || c.contain(*this) )
			return false;
		for(int i=c.g[0]; i<=c.g[1]; i++)
		for(int j=c.s[0]; j<=c.s[1]; j++)
		for(int k=c.t[0]; k<=c.t[1]; k++)
			if( contain(i, j, k) )
				return true;
		return false;
	}

	bool overlap( vector<Cube>& cs ){
		vector<Cube>::iterator it;
		for(it=cs.begin(); it!=cs.end(); it++)
			if( overlap(*it) )
				return true;
		return false;
	}
};


 
class Array3D
{
	int	G, S, T;
	vector<Cube> cubes;
public:
	double	*data;

	void init( int gN, int sN, int tN ){
        	G=gN; S=sN; T=tN;
        	srand( time(NULL) );
        	data = new double[G*S*T];
	}

	double& dat(int g, int s, int t){
		return data[g*(S*T)+s*T+t];
	}

	void clear(){
                vector<Cube>::iterator it;
                for( it=cubes.begin(); it!=cubes.end(); it++ )
                        it->clear();
		cubes.clear();
		delete data;
	}
	
	void randomSwitch(void)
	{
        	int i, j, k, m, n;
                
	        for( i=0; i<G; i++ )
        	{
                	m = rand() % G;
	                n = rand() % G;
        	        for( j=0; j<S; j++ )
        	        for( k=0; k<T; k++ )
                	        Switch( dat(m,j,k), dat(n,j,k) );
        	}
	        for( j=0; j<S; j++ )
        	{
                	m = rand() % S;
	                n = rand() % S;
        	        for( i=0; i<G; i++ )
        	        for( k=0; k<T; k++ )
                	        Switch( dat(i,m,k), dat(i,n,k) );
        	}
	        for( k=0; k<T; k++ )
        	{
                	m = rand() % T;
	                n = rand() % T;
        	        for( j=0; j<S; j++ )
        	        for( i=0; i<G; i++ )
                	        Switch( dat(i,j,m), dat(i,j,n) );
        	}
	}
	
	bool getCubes( int gBegin, int gEnd, int sBegin, int sEnd, int tBegin, int tEnd, int numClus, double overlap ){
		int g0, s0, t0, gn, sn, tn, gSpan, sSpan, tSpan, loops=0;
		bool first = true;
		Cube cube;
		bool overlapped;

		gSpan = gEnd - gBegin + 1;
		sSpan = sEnd - sBegin + 1;
		tSpan = tEnd - tBegin + 1;

		for(int i=0; i<numClus; i++){

                	gn = gBegin + irandom( gSpan );
                	sn = sBegin + irandom( sSpan );
                	tn = tBegin + irandom( tSpan );

			if( first || frandom(1.0) > overlap ){	// no overlapping cluster
				do{
                        		g0 = irandom( G - gn );
                        		s0 = irandom( S - sn );
                        		t0 = irandom( T - tn );
					cube.setSize( g0, s0, t0, gn, sn, tn );

				}while( cube.overlap(cubes) && loops++ < TOO_STUFF );
				overlapped = false;
			}
			else{
				do{
                        		g0 = irandom( G - gn );
                        		s0 = irandom( S - sn );
                        		t0 = irandom( T - tn );
					cube.setSize( g0, s0, t0, gn, sn, tn );

                        	}while( !cube.overlap(cubes) );
				overlapped = true;
			}

			if( loops >= TOO_STUFF ){
				printf( "Too many clusters: %dx[%d->%d][%d->%d][%d->%d] in %dx%dx%d with overlapping: %2.2f\n", 
						numClus, gBegin, gEnd, sBegin, sEnd, tBegin, tEnd, G, S, T, overlap );
				cout << "Suggest Cluster number: <= " << i << " or try running more times" << endl;
				return false;
			}

			if( first || !overlapped ){ 
				first = false;
				cube.init();
				cube.randUnusedGST();
				cubes.push_back(cube);	//c.clear(); should not clear here for the *gVal, *sVal, *tVal.
			}
			else if( overlapped && handleOverlap(cube) ){ // overlaped by existed clusters
				cubes.push_back(cube);	
			}
			else // overlapped and can not handle
				i--;
		}
		return true;
	}

	bool handleOverlap( Cube& c )
	{
		vector<Cube>::iterator it;
		double gv, sv, tv;

		c.init();

		for(int i=c.g[0]; i<=c.g[1]; i++)
		for(int j=c.s[0]; j<=c.s[1]; j++)
		for(int k=c.t[0]; k<=c.t[1]; k++)
		for(it=cubes.begin(); it!=cubes.end(); it++)
		if( it->contain(i,j,k) ){
			it->gstVal(i, j, k, gv, sv, tv);
			if( !c.gValSet(i-c.g[0],gv) || !c.sValSet(j-c.s[0],sv) || !c.tValSet(k-c.t[0],tv) ){
				c.clear();
				return false;
			}
		}
		c.randUnusedGST();
		return true;
	}

	void fillArray( double var )
	{
		vector<Cube>::iterator it;
	
       		for(int i=0; i<G*S*T; i++)
               		data[i] = -1.0; // be blank initially

		for(it=cubes.begin(); it!=cubes.end(); it++)
		for(int i=0; i<it->gn; i++)
		for(int j=0; j<it->sn; j++)
		for(int k=0; k<it->tn; k++)
			dat(i+it->g[0], j+it->s[0], k+it->t[0]) = it->gVal[i] * it->sVal[j] * it->tVal[k] * (1.0+frandom(var));
	}

	void fillRandom(){
		double MAX = gMAX_VAL * sMAX_VAL * tMAX_VAL;

		for(int i=0; i<G*S*T; i++)
		if( fabs(data[i]+1.0) < 0.000001 )
			data[i] = frandom(MAX);
	}

	void writeFile( char *file, double var, bool bTxt )
	{
		vector<Cube>::iterator it;
        	double val;
	        char buf[1000], extFile[30];
		ofstream out;
		int q=0;
 
		fillArray(var); 

		if( bTxt ){
			strcpy( extFile, file );
        		strcat( extFile, ".txt" );
        		out.open( extFile );
	
			for(it=cubes.begin(); it!=cubes.end(); it++){
				sprintf( buf, "cluster %d (%dx%dx%d): \tT[%2d-%2d]\tS[%2d-%2d]\tG[%4d-%4d]\n", 
					q++, it->tn, it->sn, it->gn, it->t[0], it->t[1], it->s[0], it->s[1], it->g[0], it->g[1] );
				cout << buf;
				out << buf;
			}
			////////////////////////////////////////////////////////////////////////////////////////
	       		sprintf ( buf, "\nTotal Times:\t%d\n", T );	out << buf;
	       		sprintf ( buf, "Total Samples:\t%d\n", S );	out << buf;
	       		sprintf ( buf, "Total Genes:\t%d\n", G );	out << buf;

			for(int k=0; k<T; k++){
       				sprintf ( buf, "\nTime\t%d\n", k );	out << buf;
				sprintf ( buf, "ID\tNAME" );		out << buf;
        			for(int i=0; i<S; i++)
					{ sprintf( buf, "\tS-%d", i );	out << buf; }
        			out << endl;
                                                                                                                                                                                                     
	        		for(int i=0; i<G; i++){
                			sprintf( buf, "%d\tG-%d", i, i ); out << buf;
	               	 		for(int j=0; j<S; j++){
						if( fabs(dat(i,j,k)+1.0) < 0.000001 ){
							sprintf ( buf, "        ");	out << buf; 
						}
						else{
							sprintf ( buf, "\t%2.2f", dat(i,j,k) );	out << buf; 
						}
					}
                			out << endl;
        			}
			}
        		out << endl;
			////////////////////////////////////////////////////////////////////////////////////////
        		out.close();
		}

		fillRandom();
//		randomSwitch();

		strcpy( extFile, file );
        	strcat( extFile, ".tab" );
        	out.open( extFile );
		////////////////////////////////////////////////////////////////////////////////////////
	       	sprintf ( buf, "Total Times:\t%d\n", T );	out << buf;
	      	sprintf ( buf, "Total Samples:\t%d\n", S );	out << buf;
	       	sprintf ( buf, "Total Genes:\t%d\n", G );	out << buf;

		for(int k=0; k<T; k++){
       			sprintf ( buf, "Time\t%d\n", k );	out << buf;
			sprintf ( buf, "ID\tNAME" );		out << buf;
        		for(int i=0; i<S; i++)
				{ sprintf( buf, "\tS-%d", i );	out << buf; }
        		out << endl;
                                                                                                                                                                                                     
	        	for(int i=0; i<G; i++){
                		sprintf( buf, "%d\tG-%d", i, i ); out << buf;
	                	for(int j=0; j<S; j++)
					{ sprintf ( buf, "\t%2.2f", dat(i,j,k) );	out << buf; }
                		out << endl;
        		}
		}
        	out << endl;
		////////////////////////////////////////////////////////////////////////////////////////
        	out.close();
	}	
};


///////////////////////////// main process //////////////////////////////                                                                                                                

int main(int argc, char **argv)
{
	char str[100], *p, file[30];  
	int numClus, number, minimum, maximum, len;
	int numGene, minGene, maxGene, numSample, minSample, maxSample, numTime, minTime, maxTime;
	double var, overlap;
	Array3D  array3d;

        if(argc < 8) {
                cout << endl << "Usage: " << argv[0] << " -fFile -gGene[min,max] -sSample[min,max] -tTime[min,max] -nCluster# -vVariation -oOverlap" << endl << endl;
                cout << "==================== OPTIONS =====================" << endl << endl;
                cout << "File Name:           -fString"         << endl;
                cout << "Number of Genes:     -gInteger"        << endl;
                cout << "Number of Samples:   -sInteger"        << endl;
                cout << "Number of Times:     -tInteger"        << endl;
                cout << "Cluster Size Range:  [Integer,Integer]"        << endl;
                cout << "Cluster Number:      -nInteger"        << endl;
                cout << "Variale Noises(%):   -vFloat"          << endl;
                cout << "Overlap Ratio(%):    -oFloat"          << endl << endl;
                exit(1);
        }

        for(int i=1; i<argc; i++){
                if( argv[i][0] == '-' ){
                        switch( argv[i][1] ){
                                case 'f': strcpy( file, argv[i]+2 ); break;
                                case 'g': 
                                case 's':
                                case 't': strcpy( str, argv[i]+2 ); 
					  p = str; 
					  len = strlen(str);
					  for(int j=0; j<len; j++)
                                          switch( str[j] ){
						case '[': str[j] = '\0'; number  = atoi( p ); p = str+j+1; break;
						case ',': str[j] = '\0'; minimum = atoi( p ); p = str+j+1; break;
						case ']': str[j] = '\0'; maximum = atoi( p );              break;
						default:  if( isdigit(str[j]) )	break;
							  cout << "Error Input Options: " << argv[i] << endl; exit(0); 
					  }
					  switch( argv[i][1] ){
						case 'g': numGene = number;	minGene = minimum;	maxGene = maximum;	gMAX_VAL = maximum;	break; 
						case 's': numSample = number;	minSample = minimum;	maxSample = maximum;	sMAX_VAL = maximum;	break; 
						case 't': numTime = number;	minTime = minimum;	maxTime = maximum;	tMAX_VAL = maximum;	break;
					  } 
					  break;
                                case 'n': numClus = atoi( argv[i]+2 );	break;
                                case 'v': var = atof( argv[i]+2 );	break;
                                case 'o': overlap = atof( argv[i]+2 );	break;
                                default : cout << "Error Input Options: " << argv[i] << endl; exit(0); break;
                        }
                }
                else{ cout << "Error Input Options: " << argv[i] << endl; exit(0); }
        }
	////////////////////////////////////////////////////////////////////////

	printf("\nFilename: %s, G:%d[%d,%d], S:%d[%d,%d], T:%d[%d,%d], C#:%d, Var:%2.2f, Overlap:%2.2f\n", 
		file, numGene, minGene, maxGene, numSample, minSample, maxSample, numTime, minTime, maxTime, numClus, var, overlap );

	array3d.init(numGene, numSample, numTime);
	if( array3d.getCubes( minGene, maxGene, minSample, maxSample, minTime, maxTime, numClus, overlap) )
                array3d.writeFile( file, var, true ); // write clear txt file or not
	array3d.clear();
        return 0;
}












