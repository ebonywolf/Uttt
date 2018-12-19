#ifndef CAMERA_H
#define CAMERA_H
#include "../Tools/Coord.h"
//TODO :Camera field of view
#include "../Tools/SquareHitBox.h"
#include "../Tools/HasCollison.h"
namespace pg
{
class Camera: public HasCollison{
	public:
		Camera(pg::Coord size, pg::Coord position=Coord(0,0),double rotation=0);
		Camera(SquareHitBox* hb):hb(hb){};
		virtual ~Camera();

		void zoom(double z){hb->scale({z,z}); }
		void rotate(double z){hb->rotate(z); _rotation+=z; }
		void move(pg::Coord cord);
		void moveFixed(pg::Coord cord){hb->move(cord);}
		Coord getZoom()const {return hb->getScale();}



		pg::Coord origin;
        pg::SquareHitBox* hb;

        pg::HitBox* getHitBox()const {return hb;}
    double _rotation=0;
	protected:

	private:
};

}
#endif // CAMERA_H
