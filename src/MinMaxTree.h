
#ifndef MINMAXTREE_H_
#define MINMAXTREE_H_
#include <vector>
#include <unordered_map>
#include <list>
#include <iostream>
#include <memory>
#include <time.h>
#include <set>
#include <functional>
#include <sstream>
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
	unsigned long limit = 100000;
	int minimalScore = 0xf0000000;
	float chosenMovePercent = 1;
	int threads = 1;
};

template<class T, class TYPE = uchar>
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
			using Nodeptr = std::shared_ptr<Node>;
			std::weak_ptr<Node> dad;
			bool type = 0;
			bool dead=0;
			short fitness = 0;
			short trueFitness = 0;
			TYPE mov;
			TYPE root;
			TYPE transition;
			int poda;
			static TreeConfig config;

            TYPE getRoot(){
                if( !dad.expired() ){
                    //auto pt = dad.lock();
                    return dad->getRoot();
                }else{
                    return transition;
                }
            }

			Node()
			{
				cont++;
			}
			Node ( Node* d , bool type  , int fit, TYPE transition ) :
				type ( type ), fitness ( fit ), trueFitness ( fit ), root ( d->root ),
				transition ( transition )
			{
				dad = d->shared_from_this();
				cont++;
			}
			~Node()
			{
				cont--;

			}
			bool operator< ( const Node& n ) const
			{
				return this->fitness < n.fitness;
			}
			void acquireMoves ( std::list<TYPE>& allMoves )
			{
				if ( !dad.expired() ) {
					auto alce = dad.lock();
					allMoves.push_front ( transition );
					alce->acquireMoves ( allMoves );
				}
			}
			void setAlive(bool val=0){
                dead = !val;
                for(auto& x: sons){
                    auto& s = x.second;
                    s->setAlive(val);
                }
			}
			void printTree()
			{
				using listCallback = std::function< decltype ( sons ) ( Nodeptr ) >;
				using stringCallback  = std::function< std::string ( Nodeptr ) > ;
				using voidCallback = std::function<void ( std::string ) > ;

				static std::string PREFIXES[2][2] = { { " \xC3\xC4", " \xB3  " }, { " \xC0 ", "    " } };

				std::function < void ( Nodeptr node, std::string , listCallback ,
				                       stringCallback , voidCallback ) >
				dumpRecursive = [&PREFIXES, &dumpRecursive] ( Nodeptr node, std::string parentPrefix,
				                listCallback breeder,
				stringCallback stringizer, voidCallback emitter ) {
					auto lista = breeder ( node );

					for ( auto iterator = lista.begin(); iterator != lista.end(); iterator++ ) {
						auto nextIt = iterator;
						//nextIt++;
						auto semi = nextIt;
						if ( nextIt == lista.end() ) { break; }
						semi++;


						Nodeptr child = ( nextIt != lista.end()  ) ?  nextIt->second : 0;
						std::string* prefixes = PREFIXES[ ( nextIt != lista.end()  ) ? 0 : 1  ];
						emitter ( parentPrefix + prefixes[0] + stringizer ( child ) );
						dumpRecursive ( child, parentPrefix + prefixes[1], breeder, stringizer, emitter );
					}
				};
				auto dump = [ &dumpRecursive] ( Nodeptr node, listCallback breeder,
				stringCallback stringizer, voidCallback emitter ) {
					emitter ( stringizer ( node ) );
					dumpRecursive ( node, "", breeder, stringizer, emitter );
				};
				auto getSons = [] ( Nodeptr p ) {
					if ( p.get() == 0 ) { return decltype ( p->sons ) (); }
					return p->sons;
				};
				auto toString = [] ( Nodeptr p ) {
					if ( p.get() == 0 ) { return std::string ( "" ); }
					std::stringstream ss;
					std::string type = p->type == MAX ? "MAX" : "MIN";
					ss << " trans:" << ( int ) ( p->transition ) << " Fit:" << p->fitness << " " << type << " poda:" << p->poda<<" tru:"<<p->trueFitness;
					return ss.str();
				};
				auto printer = [] ( std::string s ) {
					std::cout << s << std::endl;
				};

				dump ( this->shared_from_this() , getSons, toString, printer );


			}
			GAME createGame()
			{
				T* alce = static_cast<T*> ( game->clone() );
				GAME future = GAME ( alce );
				std::list<TYPE> moveList;
				acquireMoves ( moveList );
				for ( auto x : moveList ) {
					future->move ( x );
				}
				return future;
			}
			int getDepth() const
			{
				int highest = 0;
				for ( auto& x : sons ) {
					auto& s = x.second;
					int alce = s->getDepth();
					if ( alce > highest ) {
						highest = alce;
					}
				}
				return highest + 1;
			}
			inline void updateFitness(std::list<std::shared_ptr<Node> >* next, Nodeptr son=0, TYPE newmov=0)
			{
				if ( sons.size() == 0 ) {
					return;
				};

                auto it = sons.begin();
                fitness = it->second->fitness;
                mov = it->first;
              //  if(config.chosenMovePercent==1){
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
               // }


                if(config.chosenMovePercent!=1 && next!=0 && son.get()!=0){
                    if(MAX && son->fitness < poda ){
                        Nodeptr chosen = son;
                        for(auto& it: sons){
                            auto& novoson = it.second;
                            if( novoson->fitness <= poda && novoson->fitness > chosen->fitness ){
                                chosen = novoson;
                            }
                        }
                        poda = chosen->fitness;
                        if(chosen.get() != son.get()){
                            son->setAlive(0);
                            chosen->setAlive(1);
                        }
                        next->push_back(chosen);
                    }
                     if(MIN && son->fitness > poda ){
                        Nodeptr chosen = son;
                        for(auto& it: sons){
                            auto& novoson = it.second;
                            if( novoson->fitness >= poda && novoson->fitness > chosen->fitness ){
                                chosen = novoson;
                            }
                        }
                        poda = chosen->fitness;
                        if(chosen.get() != son.get()){
                            son->setAlive(0);
                            chosen->setAlive(1);
                        }
                        next->push_back(chosen);
                    }

                }
				if ( !dad.expired() ) {
					auto alce = dad.lock();
					alce->updateFitness(next, this->shared_from_this(), transition );
				}

			}


			void createSons ( std::list<std::shared_ptr<Node> >& next )
			{
				GAME tnow = createGame();
				std::vector<TYPE> moves = tnow->getMoves ( type );
				if ( config.chosenMovePercent == 1 ) {
					for ( auto movement : moves ) {
						T* future = static_cast<T*> ( tnow->clone() );
						// GAME future = GAME ( alce );
						future->move ( movement );
						int fit = future->getFitness();

						std::shared_ptr<Node> novoson = std::shared_ptr<Node> ( new Node ( this , !type , fit, movement ) );
						sons[movement] = novoson;
						next.push_back ( novoson );
						delete ( future );
					}
				} else {
				    if(dead)return;

					using meutipo = std::function<bool ( const std::shared_ptr<Node>&, const std::shared_ptr<Node>& ) > ;
					static meutipo comp = [] ( const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b ) {
						if ( a->fitness == b->fitness ) { return a < b; }
						else { return a->fitness < b->fitness; }
					};
					static meutipo invcomp = [] ( const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b ) {
						if ( a->fitness == b->fitness ) { return a < b; }
						else { return a->fitness > b->fitness; }
					};
					int moveAmount = ( float ) moves.size() * config.chosenMovePercent + 1;
					decltype ( comp ) * chosen; // =// type == MIN ? &comp : &invcomp;
					if ( type == MIN ) { chosen = &comp; }
					else { chosen = &invcomp; }
					std::set< std::shared_ptr<Node>, decltype ( comp )  > ordered ( *chosen );
					for ( auto movement : moves ) {
						T* future = static_cast<T*> ( tnow->clone() );

						future->move ( movement );

						int fit = future->getFitness();

						std::shared_ptr<Node> novoson = std::shared_ptr<Node> ( new Node ( this , !type , fit, movement ) );
						sons[movement] = novoson;
						if ( type == MIN && fit < config.minimalScore ) continue;

						if ( type == MAX && fit > config.minimalScore * -1 ) continue;
						delete ( future );
						ordered.insert ( novoson );
					}
					int i = 0;
					for ( auto& x : ordered ) {
						next.push_back ( x );
						if ( i >= moveAmount ) {
					        poda = (x)->fitness;
							break;
						}
						i++;
					}
				}
				if ( sons.size() > 0 ) {
					updateFitness(&next);
				}
			}
			void firstUpdate ( std::unordered_map<TYPE, std::list<std::shared_ptr<Node>>>& next )
			{
				GAME tnow = createGame();
				std::vector<TYPE> moves = tnow->getMoves ( type );
				for ( auto movement : moves ) {
					T* future = static_cast<T*> ( tnow->clone() );
					future->move ( movement );
					int fit = future->getFitness();
					std::shared_ptr<Node> novoson = std::shared_ptr<Node> ( new Node ( this , !type  , fit, movement ) );
					novoson->root = movement;
					sons[movement] = novoson;
					next[movement].push_back ( novoson );
					delete ( future );
				}
				if ( sons.size() > 0 ) {updateFitness(0);}//TODO :novo update

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
			config.limit = -1;
			config.threads = 1;
		}

		MinMaxTree ( TreeConfig config ) :
			config ( config )
		{
			Node::config = config;
		}
		void setGame ( T* g, bool xo = 0 )
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
			if ( cont > config.limit ) { return 1; }
			for ( auto& it : nextUpdate ) {
				std::list<std::shared_ptr<Node> >& next = it.second;
				std::list<std::shared_ptr<Node> > novo;

				for ( auto& node : next ) {
					node->createSons ( novo );
				}
				next.swap ( novo );
				if ( cont > config.limit ) { return 1; }
			}
			return 0;
		}
		int update ( unsigned long int time )
		{
			if ( cont > config.limit ) { return 1; }
			auto tnow = clock();

			while ( true ) {
				if ( nextUpdate.size() == 0 ) {
					return 1;
				}
				for ( auto& it : nextUpdate ) {
					std::list<std::shared_ptr<Node> >& next = it.second; //proximo nodos a atualizar
					std::list<std::shared_ptr<Node> > novo;
					//std::cout << "next:" << next.size() << std::endl;
					auto beg = next.begin();
					while ( beg != next.end() ) { //iterating over updates
						auto n = *beg;
						n->createSons ( novo );
						auto now = beg;
						beg++;
						next.erase ( now );
						if ( clock() - tnow >= time || cont > config.limit ) {
							next.splice ( next.end(), novo );
							return 1;
						}
					}
					next.swap ( novo );
				}
				if ( clock() - tnow >= time ) {
					return 1;
				}

			}
			return 0;

		}

		TYPE getMovement()
		{
			std::cout << "Fitness:" << start->fitness << " cont:" << cont << " depth:" << start->getDepth() << std::endl;
			//start->printTree();
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
			game->move ( m );


			nextUpdate = novoUpdate;
			start = next;
			std::cout << "New Depth:" << start->getDepth() << std::endl;

		}
};


template<class T, class U>
unsigned long long MinMaxTree<T, U>::cont = 0;

template<class T, class U>
std::shared_ptr<T> MinMaxTree<T, U>::game = std::shared_ptr<T>();

template<class T, class U>
TreeConfig MinMaxTree<T, U>::Node::config = TreeConfig();


} /* namespace wag */

#endif /* MINMAXTREE_H_ */
