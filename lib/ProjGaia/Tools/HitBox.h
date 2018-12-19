#ifndef HITBOX_H
#define HITBOX_H
#include "Coord.h"
#include <list>
#include "Coord.h"
#include "Polygon.h"
#include "Utilities.h"
//TODO :Better collision detection
namespace pg
{
class HitBox
{


	public:
		/** Default constructor */
		//	HitBox();
		HitBox ( std::vector<pg::Coord> lista );
		//	HitBox ( pg::Coord pos ) : position ( pos ) {}
		HitBox ( pg::Polygon pos );
		virtual ~HitBox();

		virtual void rotate ( double angle ) {  polygon.rotate ( angle, position ); position=polygon.getCenter(); }
        virtual void rotatePoint ( double angle, pg::Coord point ) {  polygon.rotate ( angle, point );  position=polygon.getCenter();}
		virtual void scale ( pg::Coord vetor ) {polygon.scale ( vetor,position ); position=polygon.getCenter(); _scale*=vetor;  }
		//virtual void setOrigin ( pg::Coord vetor ) {position=vetor ;}
		virtual void setScale(pg::Coord vetor){
            Coord mod = vetor/_scale;
            scale(mod);
		}
		virtual void move ( pg::Coord vetor ) { polygon.move ( vetor ); position+=vetor;   }



        virtual bool circularCollision(HitBox*) const;
		virtual bool collides ( HitBox* b );
		virtual bool pointBelongsTo ( pg::Coord coord ) {return polygon.pointBelongsTo ( coord );}

        virtual bool isClosed(){return closed;}
        virtual pg::Polygon getClippedPolygon(HitBox*);


        virtual pg::Polygon getPolygon() {return polygon;}
        virtual double getRadius()const {return polygon.getRadius();}
        virtual std::vector<Polygon> getFaces(){std::vector<Polygon> faces = {getPolygon()}; return faces;}

       Coord position;
       const Coord getScale() const{return _scale;}

		int z = 1;
	protected:
       Coord _scale;

       bool closed=true;
       HitBox(){};
       pg::Polygon polygon;
	private:


};
}
#endif // HITBOX_H
