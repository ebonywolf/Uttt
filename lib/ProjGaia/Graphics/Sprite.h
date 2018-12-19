#ifndef SPRITE_H
#define SPRITE_H
#include "../Tools/Coord.h"
#include "DrawableType.h"
#include "../Tools/HitBox.h"
#include "../Tools/NeedsUpdate.h"
#include "../Tools/HasCollison.h"

#include <vector>
namespace pg
{
class Sprite : public DrawableType, public NeedsUpdate, HasCollison
{
	public:
       Sprite( const pg::Sprite&);
		Sprite ( std::vector<Coord> lista );

		Sprite ( HitBox* );
		/** Default constructor */
		Sprite ( pg::Coord position, pg::Coord scale, float height );


	virtual	 void rotate ( double angle ) {  hitbox->rotate ( angle);}
    virtual     void rotatePoint ( double angle, pg::Coord point ) {  hitbox->rotatePoint ( angle, point );}
	virtual	 void scale ( pg::Coord vetor ) {hitbox->scale ( vetor);}
	virtual	 void setScale ( pg::Coord vetor ) {hitbox->setScale ( vetor);}
//		 void setOrigin ( pg::Coord vetor ) {hitbox->setOrigin(vetor) ;}
	virtual	 void move ( pg::Coord vetor ) { hitbox->move ( vetor );  }

    virtual void update(void*){};

		/** Default destructor */
		virtual ~Sprite();
		pg::HitBox* getHitBox() const {return hitbox;}


		virtual Sprite* getSprite()
		{
			return this;
		}
		bool _deleteHitbox=true;
	protected:

		pg::HitBox* hitbox;

	private:
};
}
#endif // SPRITE_H
