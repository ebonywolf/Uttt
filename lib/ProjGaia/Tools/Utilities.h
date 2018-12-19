#ifndef UTILITIES_H
#define UTILITIES_H
#include "Coord.h"
#include <iostream>
#include <vector>

namespace pg{
class Polygon;
class Sprite;
class DrawableGtk;

void sleep(long mili);

 struct TransformationInfo{
            pg::Coord offset, proportion, cameraPos,cameraOrigin,canvasOrigin;
            double angle;
 };

 struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
        return ch == '/';
    }
};
pg::Polygon TORNADO();


pg::Polygon applySquareTransformation(Polygon, TransformationInfo);
Sprite*  readWaveFront(std::string file );
void writeWaveFront(std::string file ,const  Sprite* s);
std::string basename( std::string const& pathname );
std::string removeExtension( std::string const& filename );

bool pointInTriangle(Coord, std::vector<Coord>);



}

#endif // UTILITIES_H
