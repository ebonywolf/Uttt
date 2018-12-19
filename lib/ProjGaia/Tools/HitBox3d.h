#ifndef HITBOX3D_H
#define HITBOX3D_H
#include "HitBox.h"
#include <vector>
#include "LineSeg.h"
#include "Polygon.h"
namespace pg{

class HitBox3d : public HitBox
{
    public:
        HitBox3d(std::vector<Polygon> faces);
        HitBox3d(std::vector<LineSeg> lines);
        HitBox3d(Polygon p);


        virtual void rotate ( double angle, Coord::AXIS axis= Coord::XY ) ;
        virtual void rotatePoint ( double angle, pg::Coord point,Coord::AXIS axis= Coord::XY ) ;
		virtual void scale ( pg::Coord vetor ) ;
       virtual void move ( pg::Coord vetor ) ;

        virtual double getRadius()const;
        virtual std::vector<Polygon> getFaces(){if(usingFaces)return faces; else return HitBox::getFaces();}

        pg::Polygon getClippedPolygon(HitBox* hb);
        virtual ~HitBox3d(){};
    protected:
        std::vector<Polygon> faces;
        bool usingFaces=0;
        void updatePosition();
    private:
};

}
#endif // HITBOX3D_H
