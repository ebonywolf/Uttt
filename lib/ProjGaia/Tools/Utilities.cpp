#include "Utilities.h"

#include "Angle.h"
#include "../Graphics/Sprite.h"
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>      // std::stringstream
#include <fstream>
#include "HitBox3d.h"

#ifdef __linux__
#include <time.h>
#endif // __linux__

using namespace std;

namespace pg
{
#ifdef __linux__


void sleep ( long mili )
{
	struct timespec time;

	time.tv_nsec = mili * 1000000;
	nanosleep ( &time, 0 );


}
#endif // __linux__



bool pointInTriangle(Coord point , std::vector<Coord> tri ){
   double area = ( -tri[1].y*tri[2].x + tri[0].y*(-tri[1].x + tri[2].x) + tri[0].x*(tri[1].y - tri[2].y) + tri[1].x*tri[2].y)/2;
   double s = 1/(2*area)*(tri[0].y*tri[2].x - tri[0].x*tri[2].y + (tri[2].y - tri[0].y)*point.x + (tri[0].x - tri[2].x)*point.y);

   double t = 1/(2*area)*(tri[0].x*tri[1].y - tri[0].y*tri[1].x + (tri[0].y - tri[1].y)*point.x + (tri[1].x - tri[0].x)*point.y);

   if(s<0)return false;
   if(t<0)return false;
   if(1-s-t <0)return false;

   return true;

}

pg::Polygon TORNADO()
{
	int size = 300;

	double insidemov = 1.3;
	double outsidemov = 15;

	double insidextra = 0.00;
	double outsidextra = 0.10;

	int insideP = 700;
	int outsideP = 45;

	Polygon p = Polygon::create_regular_polygon ( Coord ( 0, 0 ), Coord ( 300, 300 ), 9 );
	vector<vector<Coord>> inside;
	vector<vector<Coord>> outside;

	auto generatePoint = [] ( vector<Coord>& p, double speed, double ang ) {
		auto novo = p;
		for ( int i = 0 ; i <  p.size() ; i++ ) {
			Coord next = p[ ( i + 1 ) % p.size()  ];
			double angle = Angle::findangle ( novo[i], next );
			angle += ang;
			novo[i].move ( angle, speed );
		}
		return novo;

	};
	auto linhas = p.getPoints();
	auto novalinhas = generatePoint ( linhas, insidemov , insidextra );

	inside.push_back ( novalinhas );
	for ( int i = 0 ; i < insideP ; i++ ) {
		auto linhas = inside.back();
		auto novalinhas = generatePoint ( linhas, insidemov, insidextra );
		inside.push_back ( novalinhas );
	}


	novalinhas = generatePoint ( linhas, outsidemov , outsidextra );
	//outside.push_back ( novalinhas );
	Coord sum = {0,0};
	int count = 0;
	for ( int i = 0 ; i < inside.size() ; i++ ) {
		for ( int j = 0 ; j < inside[i].size() ; j++ ) {
			count++;
			sum += inside[i][j];
		}
	}
	sum /= ( double ) count;
    outside=inside;
	for ( int i = 0 ; i < inside.size() ; i++ ) {
		for ( int j = 0 ; j < inside[i].size() ; j++ ) {
			outside[i][j].rotate(sum, -0.35);
		}
	}
	vector<Coord> outerCircle=linhas;
    for ( auto& x : outerCircle ){
        x.rotate(sum,-.16);
    }

	vector<Coord> finalist;

	for ( int i = 0 ; i < linhas.size() ; i++ ) {
            finalist.push_back(outerCircle[i]);
		finalist.push_back ( linhas[i] );



		for ( int j = 0 ; j < inside.size()  ; j++ ) {
			finalist.push_back ( inside[j][i] );
		}

		for ( int j = outside.size() - 1 ; j >= 0  ; j-- ) {
			int d = ( i + 1 ) % outside[j].size();

			finalist.push_back ( outside[ j ] [ d ] );
			//finalist.push_back(outside[j][i]  );
		}
	}
	return Polygon ( finalist );
}

pg::Polygon applySquareTransformation ( Polygon p, TransformationInfo info )
{
	p.rotate ( info.angle, info.cameraOrigin );

    p.move ( info.offset );

	p.scale ( info.proportion,info.canvasOrigin);


	return p;

}

Sprite* readWaveFront ( std::string name )
{
	vector<Coord> pontos;
	ifstream file ( name, std::ifstream::in );

	auto getPoint = [] ( ifstream & file )->Coord {
		Coord c;
		string line;
		getline ( file, line );
		stringstream lines;
		lines<<line;

		lines >> c.x;
		lines >> c.y;
		lines >> c.z;
		return c;
	};
	auto getFace = []( ifstream & file ){
        vector<int> c;
		string line;
		getline ( file, line );
		stringstream lines;
		lines<<line;
		while( lines>> line){
             stringstream word;
            word<<line;

            int a;
            word>>a;
            c.push_back(a);
		}
		return c;

	};

	vector<Coord> points;
   vector<vector<int>> faces;
	string cmd = "";

	if ( !file.is_open() ) {
		throw ( "Unable to open " + name );
	} else {
		while ( file >> cmd ) {
			if ( cmd == "#" || cmd[0] == '#' ) { ///skip this line
				getline ( file, cmd );
				continue;
			}
			if ( cmd == "v" ) { ///starting polygon
				points.push_back(getPoint ( file ));

				///skip the rest of the line;

				continue;
			}
			if( cmd == "f"){
                faces.push_back(getFace(file ));
			}
			if ( cmd == "" ) {
				continue;
			}

		}
		file.close();
	}
	vector<Polygon> polies;
	for ( auto face : faces ){

        vector<Coord> cords;
        for ( auto cordnum : face  ){
             cords.push_back( points[cordnum-1] );

        }
        polies.push_back(cords );

	}
	HitBox3d* novo = new HitBox3d(polies);
	return new Sprite(novo);
}

void writeWaveFront ( std::string name , const  Sprite* s )
{
	ofstream file ( name );
	if ( !file.is_open() ) { throw ( "unable to open " + name ); }
	auto faces = s->getHitBox()->getFaces();
    vector<string> towrite;
    int i=1;
	for ( auto face : faces ) {
		auto points = face.getPoints();
		stringstream temp;
		temp<< "f";
		//
		for ( auto point : points  ){
            file<<"v ";
            file<<point.x << " "<<point.y<<" "<<point.z<<"\n";
            temp<<" "<<i;
            i++;
		}
		temp<<"\n";
		towrite.push_back( temp.str());
	}

	for ( auto x : towrite ){
        file<<x;
	}

	file.close();
}

std::string basename ( std::string const& pathname )
{
	return std::string (
	           std::find_if ( pathname.rbegin(), pathname.rend(), MatchPathSeparator() ).base(),
	           pathname.end() );
}

std::string removeExtension ( std::string const& filename )
{
	std::string::const_reverse_iterator
	pivot
	    = std::find ( filename.rbegin(), filename.rend(), '.' );
	return pivot == filename.rend()
	       ? filename
	       : std::string ( filename.begin(), pivot.base() - 1 );
}


}
