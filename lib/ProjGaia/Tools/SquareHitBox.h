#ifndef SQUAREHITBOX_H
#define SQUAREHITBOX_H

#include "Polygon.h"
#include "Coord.h"
#include "HitBox.h"
#include "Utilities.h"
namespace pg
{
class SquareHitBox: public HitBox
{
	public:


		/** Default constructor */
		SquareHitBox ( pg::Coord pos, pg::Coord size );
		bool collides(HitBox*);
		/** Default destructor */
		virtual ~SquareHitBox();

		virtual void rotate ( double angle ) ;
		virtual void rotatePoint ( double angle, pg::Coord point ) ;
		virtual void rotateOrigin (double angle );
		virtual void scale ( pg::Coord vetor );
		virtual void scaleOrigin ( pg::Coord vetor );
        virtual void move ( pg::Coord vetor );


		virtual bool pointBelongsTo ( pg::Coord pos );

        double getWidth(){return width;}
        double getHeight(){return height;}
        double getAngle(){return angle;}

		TransformationInfo transformTo(SquareHitBox* b);
        pg::Coord origin;
	protected:
	    double width, height, angle;


	private:
};
}
#endif // SQUAREHITBOX_H
