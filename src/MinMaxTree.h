
#ifndef MINMAXTREE_H_
#define MINMAXTREE_H_
#include <vector>
#include <unordered_map>
#include <list>
#include <iostream>
#include <memory>
#include <time.h>

typedef unsigned char uchar;

namespace wag
{
///just a base structure

struct Game {
	Game() = default;
	virtual ~Game()
	{
	}

	virtual bool move ( uchar t )
	{
		return 0;
	}
	;
	virtual int getFitness()
	{
		return 0;
	}
	;
	virtual bool getPlayer()
	{
		return 0;
	}
	;
	Game* clone()
	{
		return new Game();
	}
};


struct TreeConfig {
    unsigned long limit=100000;
    int threads=1;
};

template<class T, class TYPE=uchar>
class MinMaxTree
{
	public:
		using GAME = std::shared_ptr<T>;
		static GAME game;

		unsigned short level = 0;
		static const bool MIN = false;
		static const bool MAX = true;
		static unsigned long long cont;


		TreeConfig config;
		struct Node : public std::enable_shared_from_this<Node> {
			std::unordered_map<TYPE, std::shared_ptr<Node> > sons;
			std::weak_ptr<Node> dad;
			bool type = 0;
			short fitness = 0;
			short trueFitness = 0;
			TYPE mov;
			TYPE root;
			TYPE transition;

			Node()
			{
				cont++;
			}
			Node ( Node* d , bool type  , int fit, TYPE transition  ) :
				type ( type ), fitness ( fit ), trueFitness ( fit ), root ( d->root ),
				transition( transition )
			{
				dad = d->shared_from_this();
				cont++;
			}
			~Node()
			{
				cont--;

			}
			bool operator<(const Node& n )const{
                return this->fitness < n.fitness;
			}
			void acquireMoves(std::list<TYPE>& allMoves){
			    if(!dad.expired()){
                    auto alce = dad.lock();
                    allMoves.push_front(transition);
                    alce->acquireMoves(allMoves);
			    }
			}
			GAME createGame(){
			    T* alce = static_cast<T*> ( game->clone() );
                GAME future = GAME ( alce );
                std::list<TYPE> moveList;
                acquireMoves(moveList);
                for(auto x: moveList){
                    future->move(x);
                }
                return future;
			}
			inline void updateFitness()
			{
				if ( sons.size() == 0 ) {
					return;
				};
				auto it = sons.begin();

				fitness = it->second->fitness;
				mov = it->first;
				for ( auto& it : sons ) {
					if ( type == MAX ) {
						if ( it.second->fitness > fitness ) {
							fitness = it.second->fitness;
							mov = it.first;
						} else if ( it.second->fitness == fitness && it.second->trueFitness > fitness ) {
							mov = it.first;
						}
					} else {
						if ( it.second->fitness < fitness ) {
							fitness = it.second->fitness;
							mov = it.first;
						} else if ( it.second->fitness == fitness && it.second->trueFitness < fitness ) {
							mov = it.first;
						}
					}
				}

				if ( !dad.expired() ) {
					auto alce = dad.lock();
					alce->updateFitness();
				}

			}


			void createSons ( std::list<std::shared_ptr<Node> >& next )
			{
			    GAME tnow = createGame();
				std::list<TYPE> moves = tnow->getMoves ( type );
				for ( auto movement : moves ) {
                    T* future = static_cast<T*> ( tnow->clone() );
                   // GAME future = GAME ( alce );
					future->move ( movement );

					int fit = future->getFitness();

					std::shared_ptr<Node> novoson = std::shared_ptr<Node> ( new Node ( this , !type , fit, movement ) );

					sons[movement] = novoson;
					next.push_back ( novoson );
					delete(future);
				}
				if ( sons.size() > 0 ) {
					updateFitness();
				}

			}
			void firstUpdate ( std::unordered_map<TYPE, std::list<std::shared_ptr<Node>>>& next )
			{

				GAME tnow = createGame();
				std::list<TYPE> moves = tnow->getMoves ( type );
				for ( auto movement : moves ) {
                    T* future = static_cast<T*> ( tnow->clone() );

					future->move ( movement );

					int fit = future->getFitness();

					std::shared_ptr<Node> novoson = std::shared_ptr<Node> ( new Node ( this , !type  , fit, movement ) );
					novoson->root = movement;
					sons[movement] = novoson;
					next[movement].push_back ( novoson );
					delete(future);
				}
				if ( sons.size() > 0 ) {updateFitness();}//TODO :novo update

			}
			void updateRoot ( TYPE novoRoot, bool first = 0 )
			{
				root = novoRoot;
				if ( !first ) {
					for ( auto& it : sons ) {
						it.second->updateRoot ( novoRoot );
					}
				} else { ///first
					for ( auto& it : sons ) {
						it.second->updateRoot ( it.first );
					}
				}
			}

		};
		using UpdateMap = std::unordered_map< TYPE , std::list<std::shared_ptr<Node>> >;

		std::shared_ptr<Node> start;
		//std::list<std::shared_ptr<Node> > nextUpdate;
		UpdateMap nextUpdate;

	public:

		MinMaxTree()
		{
		    config.limit=-1;
		    config.threads = 1;
		}

		MinMaxTree ( TreeConfig config ) :
			config ( config )
		{
		}
		void setGame ( T* g, bool xo=0 )
		{
			nextUpdate.clear();
			start =  std::shared_ptr<Node> ( new Node() );
			T* alce = static_cast<T*> ( g->clone() );
			game =  GAME ( alce );
			start->type = !g->getPlayer() ^xo;
			start->firstUpdate ( nextUpdate );
			// start->updateRoot(0,true );
		}
		int update ()
		{
		    if(cont > config.limit)return 1;
			for ( auto& it : nextUpdate ) {
				std::list<std::shared_ptr<Node> >& next = it.second;
				std::list<std::shared_ptr<Node> > novo;
				for ( auto& node : next ) {
					node->createSons ( novo );
				}
				next.swap ( novo );
				if( cont > config.limit)return 1;
			}
            return 0;
		}
		int update (unsigned long int time)
		{
            if(cont > config.limit)return 1;
		    auto tnow = clock();

		    while(true){
                if(nextUpdate.size()==0){
                   return 1;
                }
                for ( auto& it : nextUpdate ) {
                    std::list<std::shared_ptr<Node> >& next = it.second; //proximo nodos a atualizar
                    std::list<std::shared_ptr<Node> > novo;
                    auto beg = next.begin();
                    while(beg != next.end() ){//iterating over updates
                        auto n = *beg;
                        n->createSons(novo);
                        auto now = beg;
                        beg++;
                        next.erase(now);
                        if( clock() - tnow >= time || cont > config.limit ){
                            next.splice(next.end(), novo);
                            return 1;
                        }
                    }
                     next.swap ( novo );
                }
                 if( clock() - tnow >= time ){
                     return 1;
                 }

		    }
		    return 0;

		}

		TYPE getMovement()
		{

            std::cout<< "Fitness:"<<start->fitness<<" cont:"<<cont <<std::endl;
			return start->mov;
		}
		void sendCommand ( TYPE m )
		{

			std::shared_ptr<Node> next;

			if ( start->sons.count ( m ) ) {
				next = start->sons[m];
			} else {
				throw "FUUU";
			}

			auto& lista = nextUpdate[m];

			UpdateMap novoUpdate;
            if ( next->sons.size() ) { //has sons
				next->updateRoot ( 0, true );
				for ( auto& node : lista ) {
					novoUpdate[ node->root ].push_back ( node );
				}

			} else {
				next->firstUpdate ( novoUpdate );
			}
			game->move( m );


			nextUpdate = novoUpdate;
			start = next;
		}
};
template<class T,class U>
unsigned long long MinMaxTree<T,U>::cont = 0;

template<class T,class U>
std::shared_ptr<T> MinMaxTree<T,U>::game=std::shared_ptr<T>();


} /* namespace wag */

#endif /* MINMAXTREE_H_ */
