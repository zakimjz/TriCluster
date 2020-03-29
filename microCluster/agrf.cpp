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
#include <cstring>

//#define CLEAR

using namespace std;

template <class T>
inline void Switch(T &x, T &y)
{
        static T f;
        f = x; x = y; y = f;
}
                                                                                                                                                                                                     

class Region 
{
	double Seed;
	double *col_val;
	double *row_val;
public:
	int x, y, row, col;

	void init( int x0, int y0, int r, int c ){
		x = x0; y = y0; row = r; col = c;
		col_val = new double[c];
		row_val = new double[r];
		for(int i=0; i<c; i++)
			col_val[i] = -1.0;
		for(int i=0; i<r; i++)
			row_val[i] = -1.0;
		Seed = -1.0;
			
	}

	void clear(){
		delete col_val;
		delete row_val;
	}

	double get_row_val( int r ){
		assert( r < row );
		return row_val[r];	
	}

	double get_col_val( int c ){
		assert( c < col );
		return col_val[c];	
	}

	double set_row_val( int row, double val ){
		row_val[row] = val;
	}
	
	double set_col_val( int col, double val ){
		col_val[col] = val;
	}
	
	double get_seed(){
		return Seed;
	}
	
	double set_seed( double val ){
		Seed = val;
	}
	
	bool In( int px, int py ){
		if( px >= x && px < x+col && py >= y && py < y+row )
			return true;
		return false;
	}

	bool In( int px, int py, double &r_val, double &c_val, double &seed ){
		if( px >= x && px < x+col && py >= y && py < y+row ){
			r_val = row_val[ py-y ];
			c_val = col_val[ px-x ];
			seed = Seed;	
			return true;
		}
		return false;
	}
	bool InRegion( Region re ){
		double r, c, s;
		return re.In(x, y, r, c, s) && In(x+col-1, y+row-1, r, c, s) && In(x, y+row-1, r, c, s) && In( x+col-1, y, r, c, s); 
	}
		
	bool InRegions( vector<Region> regions ){
		vector<Region>::iterator it;
		for(it=regions.begin(); it!=regions.end(); it++)
			if( InRegion(*it) )
				return true;
		return false;
	} 

	bool Overlap( Region re ){
		double r, c, s;
		return In(re.x, re.y, r, c, s) || In(re.x+re.col-1, re.y+re.row-1, r, c, s) || In(re.x+re.col-1, re.y, r, c, s) || In(re.x, re.y+re.row-1, r, c, s) 
			|| re.In(x, y, r, c, s) || re.In(x+col-1, y+row-1, r, c, s) || re.In(x+col-1, y, r, c, s) || re.In(x, y+row-1, r, c, s);
			// Any of 4 angler points in the squeare
	}
	bool RowOverlap( Region r ){
		return (r.y >= y && r.y < y+row) || ((r.y+r.row-1) >= y && (r.y+r.row-1) < y+row) || (y >= r.y && y < r.y+r.row) || ((y+row-1) >= r.y && (y+row-1) < r.y+r.row); 
	}

	bool Overlaps( vector<Region> regions ){
		vector<Region>::iterator it;
		for(it=regions.begin(); it!=regions.end(); it++)
			if( Overlap(*it) )
				return true;
		return false;
	}
        bool RowOverlaps( vector<Region> regions ){
                vector<Region>::iterator it;
                for(it=regions.begin(); it!=regions.end(); it++)
                        if( RowOverlap(*it) )
                                return true;
                return false;
        }
};


 
class Matrix
{
	int	row, col;
	double	max_val;
	vector<Region> regions;
	int eSum, eCover, cNum;
public:
	double	*data;
	double   *data1;	
	double   *data2;	

	void init( int width, int height, double max ){
        	row = height;
        	col = width;
        	max_val = max;
                                                                                                                
        	srand( time(NULL) );
        	data = new double[row * col];
        	data1= new double[row * col];
        	data2= new double[row * col];
        	for(int i=0; i<row*col; i++){
                	data[i] = -1.0; // be blank initially
		}
	}

	void clear(){
                vector<Region>::iterator it;
                for( it=regions.begin(); it!=regions.end(); it++ )
                        it->clear();
		regions.clear();
		delete data;
		delete data1;
		delete data2;
	}


	bool setNonregionVal( int x, int y, double val ){
		int index = y*col + x;
		if( data[index] < 0.0 ){
			data[index] = val;
			return true;
		}
		return false;
	}

	double frandom( double value ){
        	return (double)rand()/RAND_MAX * value;
	}

	int irandom( int value ){
		return rand() % value;
	}
                                                                                                                                                                                                     
	void random_switch(void)
	{
        	int i, j, k1, k2;
                
	        for( i=0; i<col; i++ )
        	{
                	k1 = rand() % col;
	                k2 = rand() % col;
        	        for( j=0; j<row; j++ ){
                	        Switch( data1[j*col+k1], data1[j*col+k2] );
                	        Switch( data2[j*col+k1], data2[j*col+k2] );
			}
        	}
	        for( i=0; i<row; i++ )
        	{
                	k1 = rand() % row;
	                k2 = rand() % row;
        	        for( j=0; j<col; j++ ){
                	        Switch( data1[k1*col+j], data1[k2*col+j] );
                	        Switch( data2[k1*col+j], data2[k2*col+j] );
			}
	        }
	}

	bool InRegions( int x, int y, double &r_val, double &c_val, double &seed ){
        	vector<Region>::iterator it;
                                                                                                                
        	for( it=regions.begin(); it!=regions.end(); it++ )
                	if( it->In(x, y, r_val, c_val, seed ) )
                        	return true;
        	return false;
	}

	void FillRandom(void){
        	double r, c, s;
                                                                                                                
        	for(int i=0; i<row; i++)
                for(int j=0; j<col; j++){
                        if( ! InRegions(j, i, r, c, s) )
                                data[i*col + j] = frandom( max_val );
		}
		memcpy(data2, data, row*col*sizeof(double));                                                                                  
        }

	void showClusterInfo( ostream& out )
	{
		char buf[1000];

	        out << "Clusters#:\t" << cNum << endl << endl;
        	if(cNum !=0 ){
                	out << "Elements#:\t" << eSum << endl;
                	out << "Ave Elements#:\t" << eSum / cNum << endl << endl;
                	out << "Cover:   \t" << eCover << endl;
                	out << "Ave Cover:\t" << eCover / cNum << endl << endl;
                	sprintf( buf, "Overlap%%:\t%2.2f%\n\n", double(eSum-eCover) / eCover * 100.0 );
			out << buf;
        	}
	}

	bool RandEmbed( int W, int H, int minWidth, int maxWidth, int minHeight, int maxHeight, int numClus, double eps, double overlap_in );
	bool embed( int x, int y, int width, int height, double eps );
        void writeTabFile( double *dat, ostream& out );
	void show( bool clear, double *dat, ostream& out );
	void writeFile( string file, int width, int height, double *dat );
};



#define TOO_STUFF	1000000

bool Matrix::RandEmbed( int W, int H, int minWidth, int maxWidth, int minHeight, int maxHeight, int numClus, double eps, double overlap_in )
{
	int x, y, width, height, width_span, height_span, LoopTimes=0;
	bool first = true;
	Region region;
	
	width_span = maxWidth - minWidth;
	height_span = maxHeight - minHeight;

	for(int i=0; i<numClus; i++){

                width = minWidth + irandom( width_span + 1 );
                height = minHeight + irandom( height_span + 1 );

		if( first || frandom(1.0) > overlap_in ){	// no overlapping cluster
			do{
                        	x = irandom( W - width );
                        	y = irandom( H - height );

				region.init(x, y, height, width);
				region.clear();
				//cout << "Here 1" << endl;
				LoopTimes++;

			}while( region.Overlaps(regions) && LoopTimes < TOO_STUFF );
			//}while( region.RowOverlaps(regions) && LoopTimes < TOO_STUFF );
			first = false;
		}
		else{
		/*	if( frandom(1.0) > 0.5 )		// row overlap
				do{
					x = irandom( W - width );
					y = irandom( H - height );

					region.init(x, y, height, width);
					region.clear();
					//cout << "Here 2" << endl;
					LoopTimes++;

				}while( !(region.RowOverlaps(regions) && !region.Overlaps(regions)) && LoopTimes < TOO_STUFF );
			else	*/				// column overlap
                                do{
                                        x = irandom( W - width );
                                        y = irandom( H - height );
                                                                                                                                                                                                     
                                        region.init(x, y, height, width);
                                        region.clear();
                                        //cout << "Here 3" << endl;
                                                                                                                                                                                                     
                                }while( !region.Overlaps(regions) );

		}
		if( LoopTimes >= TOO_STUFF ){
			cout << "Too many clusters (" << numClus << " x " << "(" << minWidth << "->" << maxWidth << " x " << minHeight << "->" << maxHeight <<  ")) in " 
							<< H << " lines under this overlapping: " << overlap_in << endl;
			cout << "Suggest Cluster number: <= " << i << " or try running more times" << endl;
			return false;
		}
		else if( !embed( x, y, width, height, eps )){	// overlaped by all existed clusters
			//cout << "Here fail embedding" << i << endl;
			i--;
		}
	}
////////////////////////// element#, cover ////////////////////////////
	vector<Region>::iterator rit;
	eCover = 0;
	eSum = 0;
	cNum = regions.size();
		
	for(int i=0; i<W; i++)
	for(int j=0; j<H; j++){
		for(rit=regions.begin(); rit!=regions.end(); rit++){
			if( rit->In( i, j )){
				eCover++;
				break;
			}
		}
		for(rit=regions.begin(); rit!=regions.end(); rit++){
			if( rit->In( i, j ))
				eSum++;
		}
	}
///////////////////////////////////////////////////////////////////////
#ifndef CLEAR
	double maxv=0;		// normalization 
	for(int i=0; i<row*col; i++)
		if( data[i] > maxv )
			maxv = data[i];
	
	for(int i=0; i<row*col; i++)
		data[i] *= max_val / maxv;
#endif
	memcpy(data1, data, row*col*sizeof(double));  
	return true;
}


bool Matrix::embed( int x, int y, int w, int h, double eps )
{
	Region region;
	bool in;
	double val;
        int overlap=0;
       	double r, c, s;


        assert( x+w <= col && y+h <= row );
        
	region.init(x, y, h, w );

	for(int i=0; i<w; i++) // column by column check if in other region
        for(int j=0; j<h; j++)
	if( InRegions(i+x, j+y, r, c, s) ){
		region.set_row_val( j, r );
		region.set_col_val( i, c );
		if( region.get_seed() < 0.0 )
			region.set_seed( s );
	}

	for(int i=0; i<w; i++) // column by column check if in other region
	if( region.get_col_val(i) < 0.0 )
#ifndef CLEAR
		region.set_col_val(i, 1.0+frandom(3.0) );
#else
		region.set_col_val(i, 1.0+i );
#endif

	for(int i=0; i<h; i++) // column by column check if in other region
	if( region.get_row_val(i) < 0.0 )
#ifndef CLEAR
		region.set_row_val(i, 1.0+frandom(3.0) );
#else
		region.set_row_val(i, 1.0+i );
#endif

	if( region.get_seed() < 0.0 )
#ifndef CLEAR
		region.set_seed( 3.0 + frandom(1.0));
#else
		region.set_seed( 1.0 + irandom(2));
#endif

        for( int i=0; i<h; i++)
        for( int j=0; j<w; j++)
#ifndef CLEAR
        	if( !setNonregionVal(x+j, y+i, (1.0+frandom(eps))*region.get_seed()* /*region.get_col_val(j)**/region.get_row_val(i)))
#else
        	if( !setNonregionVal(x+j, y+i, region.get_seed()* /*region.get_col_val(j)*/ region.get_row_val(i)))
#endif
			overlap++;
/*
	cout << "col: ";
	for(int i=0; i<w; i++)
		printf( ", %2.2f", region.get_col_val(i) );
	cout << endl;
	cout << "row: ";
	for(int i=0; i<h; i++)
		printf( ", %2.2f", region.get_row_val(i) );
	cout << endl;
	cout << "seed: " << region.get_seed() << endl;
*/
	if( overlap == w*h )
		return false;

        regions.push_back( region );
	return true;
}


void Matrix::writeTabFile( double *dat, ostream& out )
{
        float val;
        char buf[1000];
                                                                                                                                                                                                     
        sprintf ( buf, "ID\tNAME" );
        out << buf;
                                                                                                                                                                                                     
        for(int i=0; i<col; i++){
                sprintf( buf, "\tCOL-%d", i+1 );
                out << buf;
        }
        out << endl;
                                                                                                                                                                                                     
        for(int i=0; i<row; i++)
        {
                sprintf( buf, "%d\tROW-%d", i+1, i+1 );
                out << buf;
                                                                                                                                                                                                     
                for(int j=0; j<col; j++){
                                sprintf ( buf, "\t%2.2f", dat[i*col + j] );
                                out << buf;
                        }
                out << endl;
        }
        out << endl;
}




void Matrix::show( bool clear, double *dat, ostream& out )
{
	double val;
	char buf[1000];

	//printf( "%6d  ", row );
        sprintf ( buf, "%6d  ", row );
	out << buf;

        for(int i=0; i<col; i++){
		sprintf( buf, "%6d  ", i );
		out << buf;
	}
	//	printf( "%6d  ", i );
	out << endl << endl;

        for(int i=0; i<row; i++)
        {
		sprintf( buf, "%6d  ", i );
		out << buf;
		//printf( "%6d  ", i );
                for(int j=0; j<col; j++)
			if( !clear || (dat[i*col+j]+1.0) > 0.0000001 ){ //InRegions(j, i, val) )
				sprintf ( buf, "%6.2f  ", dat[i*col + j] ); 
				out << buf;
			}
			else
                       		out << "--------";
                out << endl; 
        }
	out << endl;
}
                                                                                                                


void Matrix::writeFile( string file, int width, int height, double *datt )
{
	float *dat;
	float tmp;
	int fd, neg_one = -1;
	ofstream txtFile;

	dat = new float[width*height];

	for(int i=0; i<width*height; i++)
		dat[i] = datt[i];

        fd = open(file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 511);
        assert(fd != -1);

        write(fd, &height, sizeof(int));
        write(fd, &width, sizeof(int));
                  
	tmp = (float) max_val;                                                                                              
        for(int i=0; i<width; i++)
                write(fd, &tmp, sizeof(float));
                                                                                                                
        for(int i=0; i<height; i++) {
                write(fd, &i, sizeof(int));
		write(fd, dat + width*i, width*sizeof(float));
                write(fd, &neg_one, sizeof(int));
        }
	close(fd);

	///////////////// for pCluster input //////////////////////////
             /*     
	printf( "%d %d\n", height, width );                                                                                                                                                                                   
        for(int i=0; i<height; i++){
		for(int j=0; j<width; j++)
				printf( " %3d", int(dat[i*width+j]) );  
		printf( "\n" );                                                                                
	}*/
	///////////////////////////////////////////////////////////////

	delete dat;
}

///////////////////////////// main process //////////////////////////////                                                                                                                

//#define MULTI_OUTPUT

int main(int argc, char **argv)
{
	string file, file1, file2;
	ofstream txtFile;
	ofstream tabFile;
	double maxValue, eps, overlap_in;
	int Width, Height, minWidth, maxWidth, minHeight, maxHeight, numClus;
	Matrix matrix;

	if(argc < 12){				//  1      2      3        4         5         6         7          8          9     10       11
		cout << "Usage: " << argv[0] << "  FILE  Width  Height  maxValue  minWidth  maxWidth  minHeight  maxHeight  numClus  eps  overlap_in" << endl;
		exit(1);
	}

	file = file1 = file2 = argv[1];
	Width 	   = atoi( argv[2] );
	Height 	   = atoi( argv[3] );
	maxValue   = atof( argv[4] );
	minWidth   = atoi( argv[5] );
	maxWidth   = atoi( argv[6] );
	minHeight  = atoi( argv[7] );
	maxHeight  = atoi( argv[8] );
	numClus    = atoi( argv[9] );
	eps 	   = atof( argv[10] );
	overlap_in = atof( argv[11] );

	matrix.init(Width, Height, maxValue );

	if( matrix.RandEmbed( Width, Height, minWidth, maxWidth, minHeight, maxHeight, numClus, eps, overlap_in )){
#ifdef MULTI_OUTPUT
		file += ".txt";
    		txtFile.open(file.c_str());
                file2 += ".tab";
                tabFile.open(file2.c_str());

		matrix.show( true, matrix.data, cout );			// clear to screen
		matrix.showClusterInfo( txtFile );
		matrix.show( true, matrix.data, txtFile );		// clear to txt file
#endif
		matrix.FillRandom();

#ifdef MULTI_OUTPUT
		matrix.show( false, matrix.data, cout );		// to screen
		matrix.show( false, matrix.data, txtFile );		// to txt file

		txtFile.close();

//		matrix.writeFile( file1, Width, Height, matrix.data );	// binary to file
                                                                                                                                                                                                     
                matrix.writeTabFile( matrix.data, tabFile );
                tabFile.close();
#endif
		///////////////////////////////////////////////////////////////////////////
		matrix.random_switch();					// random switch
#ifdef MULTI_OUTPUT
		file1 += "1";
		matrix.writeFile( file1, Width, Height, matrix.data2 );// binary to file
#endif
		file2 = file1;


                file2 += ".tab";
                tabFile.open(file2.c_str());

#ifdef MULTI_OUTPUT
		file1 += ".txt";
    		txtFile.open(file1.c_str());

		matrix.show( true, matrix.data1, cout );		// clear to screen 
		matrix.show( true, matrix.data1, txtFile );		// clear to txt file

		matrix.show( false, matrix.data2, cout );		// to screen
		matrix.show( false, matrix.data2, txtFile );		// to txt file

		txtFile.close();
#endif
                matrix.writeTabFile( matrix.data2, tabFile );
                tabFile.close();

	}
	matrix.clear();

        return 0;
}










