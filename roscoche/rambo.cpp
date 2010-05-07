// A simple player that makes a little fort and hides behind it.
//
// Feel free to use this as a starting point for your own player.
//
// ICPC Challenge
// Sturgill, Baylor University

#include "Const.h"
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>

using namespace std;

/** Simple representation for a child in the Game. */
struct Child {
  // Location of the child.
  int x, y;

  // True if  the child is standing.
  bool standing;

  // Side the child is on.
  int color;

  // What's the child holding.
  int holding;

  // How many more turns this child is dazed.
  int dazed;

  // Quantos turnos em fuga.
  int fuga;

  int fugaX;
  int fugaY;
};

// X displacement for each walk direction.
int dispX[] = {
  1,
  0,
  -1,
  0,
};

// Y displacement for each walk direction.
int dispY[] = {
  0,
  1,
  0,
  -1,
};

// Extra constant to mark child locations on the map.
const int GROUND_CHILD = 10;

// Constante sobre o tempo padrão de fuga.
const int TEMPO_FUGA = 5;

// Another extra constant to mark spaces we should not pick
// up from.
const int GROUND_TAKEN = 11;

// How big of a fort do we want to try to build.
const int FORT_SIZE = 6;

// Walk direction for each child.
int walkDir[] = {
  1,
  0,
  2,
  3
};

// Current game score, for red and blue
int score[ 2 ];

// Current snow height in each cell.
vector< vector< int > > height( SIZE, vector< int >( SIZE ) );

// Contents of each cell.
vector< vector< int > > ground( SIZE, vector< int >( SIZE ) );

// List of current information about each child.
vector< Child > cList( CCOUNT * 2 );

bool RamboChooseMove(int i)
{

  Child &child = cList[ i ];

  // If we're dazed, we have to stay idle.
  if ( child.dazed > 0 ) {
    cout << "idle" << endl;
    child.fuga = TEMPO_FUGA;
    return true;
  }

  if(child.fuga > 0)
  {
    child.fuga--;
    // Ver se o oponente pode me acertar.
    for ( int j = CCOUNT; j < CCOUNT * 2; j++ ) {
      if ( cList[ j ].x >= 0 ) {
        int dx = cList[ j ].x - child.x;
        int dy = cList[ j ].y - child.y;
        int dsq = dx * dx + dy * dy;
        // Se um oponente estiver por perto, fuja na direção oposta!
        if ( dsq < 8 * 8 ) {
          cout << "run " << (child.x - ((2*dx+1)%2)) << " "
                         << (child.y - ((2*dy+1)%2)) << endl;
          child.fugaX = (((2*dx+1)%2));
          child.fugaY = (((2*dy+1)%2));
          return true;
        }
      }  
    }
    cout << "run " << (child.x - child.fugaX) << " " 
         << (child.y - child.fugaY) << endl;
    return true; 
  }

  if(child.holding == HOLD_EMPTY)
  {
    // See if there's some snow nearby.
    int sx = -1, sy = -1;
    for ( int ox = child.x - 1; ox <= child.x + 1; ox++ )
      for ( int oy = child.y - 1; oy <= child.y + 1; oy++ ) {
        // Is there snow to pick up?  Can we pick it up?  Do we want to?
        if ( ox >= 0 && 
             oy >= 0 && 
             ( ox != child.x || oy != child.y ) &&
             ground[ ox ][ oy ] == GROUND_EMPTY &&
             height[ ox ][ oy ] > 0 ) {
          sx = ox;
          sy = oy;
        }
      }
        
    // Try to crouch down and pick up snow if there is some.
    if ( sx >= 0 ) {
      if ( child.standing ) {
        cout << "crouch" << endl;
        return true;
      }

      // Pick up snow from the space, and mark it
      // as taken so another one of our children
      // won't try to take it also.
      cout << "pickup " << sx << " " << sy << endl;
      ground[ sx ][ sy ] = GROUND_TAKEN;
      return true;

    }
  }

  // Crush one unit of powdered snow into a snowball.
  if ( child.holding == HOLD_P1 ) {
    cout << "crush" << endl;
    return true;
  }

  if ( child.holding == HOLD_S1 ) {
    // We're holding a small snowball.
    if ( !child.standing ) {
      cout << "stand" << endl;
      return true;
    }

    // See if we should throw a snowball at an opponent.
    for ( int j = CCOUNT; j < CCOUNT * 2; j++ ) {
      if ( cList[ j ].x >= 0 ) {
        int dx = cList[ j ].x - child.x;
        int dy = cList[ j ].y - child.y;
        int dsq = dx * dx + dy * dy;
        // If we see a nearby child, throw past them.
        if ( dsq < 8 * 8 ) {
          if(height[cList[j].x-(dx%2)][cList[j].y-(dy%2)] < 5){ 
              cout << "throw " << child.x + dx * 2 << " "
                   << child.y + dy * 2 << endl;
              child.holding = HOLD_EMPTY;
              child.fuga = TEMPO_FUGA;
          }
          else{
              // Desvia
              if((dx>0) && (dy>0)){
                    cout << "run " << child.x << " "
                        << (child.y - (2)) << endl;
              }
              else
              {
                    cout << "run " << (child.x - (((2*dx+1)%2))) << " "
                        << (child.y - (((2*dy+1)%2))) << endl;
                    child.fuga = TEMPO_FUGA;
              }
          }
          child.fugaX = ((2*dx+1)%2);
          child.fugaY = ((2*dy+1)%2);
          return true;
        }
      }
    }
  }

  // See where the child is moving.
  int newX = child.x + dispX[ walkDir[ i ] ];
  int newY = child.y + dispY[ walkDir[ i ] ];

  // Child may turn just because she feels like it.
  if ( rand() % 10 == 0 ) {
    walkDir[ i ] = ( walkDir[ i ] + 1 ) % 4;
    return false;
  }


  // Child must turn if she hits the edge of the map.
  if ( newX < 0 || newY < 0 || newX > (SIZE-1) || newY > (SIZE-1)){
    walkDir[ i ] = ( walkDir[ i ] + 1 ) % 4;
    return false;
  } 
  
  // Turn must turn if the walk direction is blocked.
  if ( ground[ newX ][ newY ] == GROUND_TREE ||
       ground[ newX ][ newY ] == GROUND_CHILD ||
       height[ newX ][ newY ] >= OBSTACLE_HEIGHT ) {
    walkDir[ i ] = ( walkDir[ i ] + 1 ) % 4;
    return false;
  }

  // Move on around the perimiter if we have nothing better
  // to do.
  if ( child.standing ) {
    cout << "run ";
  } else {
    cout << "crawl ";
  }
  cout << newX << " " << newY << endl;

  // Update the map, so that the next child won't try to mvoe
  // right into this one.
  ground[ child.x ][ child.y ] = GROUND_TAKEN;
  ground[ newX ][ newY ] = GROUND_CHILD;

  return true;
}

// Try to pick a next move.  Return false if the child just
// turns in place and chooses no move.
bool chooseMove( int i ) {
  // Figure out where this child is about to move.
  Child &child = cList[ i ];

  // If we're dazed, we have to stay idle.
  if ( child.dazed > 0 ) {
    cout << "idle" << endl;
    return true;
  }

  if(child.fuga > 0)
  {
    child.fuga--;
    // Ver se o oponente pode me acertar.
    for ( int j = CCOUNT; j < CCOUNT * 2; j++ ) {
      if ( cList[ j ].x >= 0 ) {
        int dx = cList[ j ].x - child.x;
        int dy = cList[ j ].y - child.y;
        int dsq = dx * dx + dy * dy;
        // Se um oponente estiver por perto, fuja na direção oposta!
        if ( dsq < 8 * 8 ) {
          cout << "run " << (child.x - ((2*dx+1)%2)) << " "
                         << (child.y - ((2*dy+1)%2)) << endl;
          child.fugaX = (((2*dx+1)%2));
          child.fugaY = (((2*dy+1)%2));
          return true;
        }
      }  
    }
    cout << "run " << (child.x - child.fugaX) << " " 
         << (child.y - child.fugaY) << endl;
    return true; 
  }

  

  // Pick up some snow if there's some nearby.
  if ( child.holding == HOLD_EMPTY ) {
    // See if there's some snow nearby.
    int sx = -1, sy = -1;
    for ( int ox = child.x - 1; ox <= child.x + 1; ox++ )
      for ( int oy = child.y - 1; oy <= child.y + 1; oy++ ) {
        // Is there snow to pick up?  Can we pick it up?  Do we want to?
        if ( ox >= 0 && ox < FORT_SIZE &&
             oy >= 0 && oy < FORT_SIZE &&
             ( ox != child.x || oy != child.y ) &&
             ground[ ox ][ oy ] == GROUND_EMPTY &&
             height[ ox ][ oy ] > 0 ) {
          sx = ox;
          sy = oy;
        }
      }
        
    // Try to crouch down and pick up snow if there is some.
    if ( sx >= 0 ) {
      if ( child.standing ) {
        cout << "crouch" << endl;
        return true;
      }

      // Pick up snow from the space, and mark it
      // as taken so another one of our children
      // won't try to take it also.
      cout << "pickup " << sx << " " << sy << endl;
      ground[ sx ][ sy ] = GROUND_TAKEN;
      return true;
    }
  }

  // Crush one unit of powdered snow into a snowball.
  if ( child.holding == HOLD_P1 ) {
    cout << "crush" << endl;
    return true;
  }

  if ( child.holding == HOLD_S1 ) {
    // We're holding a small snowball.
    if ( !child.standing ) {
      cout << "stand" << endl;
      return true;
    }

    // See if we should throw a snowball at an opponent.
    for ( int j = CCOUNT; j < CCOUNT * 2; j++ ) {
      if ( cList[ j ].x >= 0 ) {
        int dx = cList[ j ].x - child.x;
        int dy = cList[ j ].y - child.y;
        int dsq = dx * dx + dy * dy;
        // If we see a nearby child, throw past them.
        if ( dsq < 8 * 8 ) {
          cout << "throw " << child.x + dx * 2 << " "
               << child.y + dy * 2 << endl;
          return true;
        }
      }
    }

    // Is the wall to our right?
    int r = ( walkDir[ i ] + 3 ) % 4;
    int rightX = child.x + dispX[ r ];
    int rightY = child.y + dispY[ r ];

    if ( rightX >= 0 && rightY >= 0 &&
         ( rightX == FORT_SIZE || rightY == FORT_SIZE ) &&
         height[ rightX ][ rightY ] < OBSTACLE_HEIGHT ) {
      cout << "drop " << rightX << " " << rightY << endl;
      return true;
    }
  }

  // See where the child is moving.
  int newX = child.x + dispX[ walkDir[ i ] ];
  int newY = child.y + dispY[ walkDir[ i ] ];

  // Child may turn just because she feels like it.
  if ( rand() % 10 == 0 ) {
    walkDir[ i ] = ( walkDir[ i ] + 1 ) % 4;
    return false;
  }

   // Child must turn come back to the fort
  if ( child.x >= FORT_SIZE || child.y >= FORT_SIZE ) {
   if ( child.x >= FORT_SIZE)
    newX = child.x - 1;
   if ( child.y  >= FORT_SIZE )
    newY = child.y - 1;
  }
  else {
     // Child must turn if she hits the edge of the fort.
     if ( newX < 0 || newY < 0 ||
          newX >= FORT_SIZE || newY >= FORT_SIZE ) {
       walkDir[ i ] = ( walkDir[ i ] + 1 ) % 4;
       return false;
     } 
  }
  
  // Turn must turn if the walk direction is blocked.
  if ( ground[ newX ][ newY ] == GROUND_TREE ||
       ground[ newX ][ newY ] == GROUND_CHILD ||
       height[ newX ][ newY ] >= OBSTACLE_HEIGHT ) {
    walkDir[ i ] = ( walkDir[ i ] + 1 ) % 4;
    return false;
  }

  // Move on around the perimiter if we have nothing better
  // to do.
  if ( child.standing ) {
    cout << "run ";
  } else {
    cout << "crawl ";
  }
  cout << newX << " " << newY << endl;

  // Update the map, so that the next child won't try to mvoe
  // right into this one.
  ground[ child.x ][ child.y ] = GROUND_TAKEN;
  ground[ newX ][ newY ] = GROUND_CHILD;
  
  return true;
}


int main() {
  int turnNum;
  cin >> turnNum;
  while ( turnNum >= 0 ) {
    // Read the current score.
    cin >> score[ RED ] >> score[ BLUE ];
    
    // Read the state of the field.
    for ( int i = 0; i < SIZE; i++ )
      for ( int j = 0; j < SIZE; j++ ) {
        char a, b;
        cin >> a >> b;
        if ( a != '*' ) {
          height[ i ][ j ] = a - '0';
          ground[ i ][ j ] = b - 'a';
        } else {
          height[ i ][ j ] = -1;
          ground[ i ][ j ] = -1;
        }
      }

    string token;
    for ( int i = 0; i < cList.size(); i++ ) {
      cin >> token;
      if ( token == "*" ) {
        // Can't see this child
        cList[ i ].x = -1;
        cList[ i ].y = -1;
      } else {
        // Can see this child, read the description.
        cList[ i ].x = atoi( token.c_str() );
        cin >> cList[ i ].y;

        // Fill in the child's side.
        cList[ i ].color = i / CCOUNT;

        // Read the child's stanse, what he's holding and how much
        // longer he's dazed.
        char ch;
        cin >> ch;
        cList[ i ].standing = ( ch == 'S' );
        cin >> ch;
        cList[ i ].holding = ( ch - 'a' );
        cin >> cList[ i ].dazed;
      }
    }

    // Mark child locations, so we can walk around them.
    for ( int i = 0; i < cList.size(); i++ ) {
      if ( cList[ i ].x >= 0 ) {
        ground[ cList[ i ].x ][ cList[ i ].y ] = GROUND_CHILD;
      }
    }

    int i;
    // Choose an action for each child.
    for ( i = 0; i < (CCOUNT-1); i++ ) {
      // Try up to three times to get a useful move
      int tryCount = 0;

      
      while ( tryCount < 3 &&
                  !chooseMove( i ) )
            tryCount++;
      
      // If we can't think of a good move, wait and see if something happens.
      if ( tryCount >= 3 )
        cout << "idle" << endl;
    }
    
    int tryCount=0;

   if((turnNum < 100) && (score[RED] <= score[BLUE]) ){
       while(tryCount < 3 && !RamboChooseMove(i))
           tryCount++;
   }
   else {
       while(tryCount < 3 && !chooseMove(i))
           tryCount++;
   }
     
    if( tryCount >= 3)
        cout << "idle" << endl;
    
    cin >> turnNum;
  }
}
