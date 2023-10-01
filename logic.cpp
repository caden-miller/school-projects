#include <iostream>
#include <fstream>
#include <string>
#include "logic.h"

using std::cout, std::endl, std::ifstream, std::string;

/**
 * TODO: Student implement this function
 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  pointer to 2D dynamic array representation of dungeon map with player's location., or nullptr if loading fails for any reason
 * @updates  maxRow, maxCol, player
 */
char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) {
    ifstream inFile;
    inFile.open(fileName);
    if (!inFile) {
        return nullptr;
    }
    int row;
    int col;
    inFile >> row;
    inFile >> col;
    if (row < 1 || col < 1) {
        inFile.close();
        return nullptr;
    } 
    maxRow = row;
    maxCol = col;
    char** map = new char*[maxRow];
    for(int i = 0; i < maxRow; ++i)
    {
        map[i]  = new char[maxCol];
    }
    inFile >> row;
    inFile >> col;
    player.row = row;
    player.col = col;
    char mapItem;
    for (int numRow = 0; numRow < maxRow; ++numRow) {
      for (int numCol = 0; numCol < maxCol; ++numCol) {
        
        inFile >> mapItem;
        if (mapItem == TILE_AMULET || mapItem == TILE_DOOR || mapItem == TILE_EXIT || mapItem == TILE_MONSTER || mapItem == TILE_OPEN || mapItem == TILE_PILLAR || mapItem == TILE_TREASURE) {
          map[numRow][numCol] = mapItem;
          }
        else {
          inFile.close();
          return nullptr;
        }
      }
    }
    map[player.row][player.col] = TILE_PLAYER;
    inFile.close();
    return map;
}

/**
 * TODO: Student implement this function
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @updates  nextRow, nextCol
 */
void getDirection(char input, int& nextRow, int& nextCol) {
    switch(tolower(input)) {
        case MOVE_UP:
            --nextRow;
            break;
        case MOVE_LEFT:
            --nextCol;
            break;
        case MOVE_DOWN:
            ++nextRow;
            break;
        case MOVE_RIGHT:
            ++nextCol;
            break;
        default:
            break;
    }
}

/**
 * TODO: [suggested] Student implement this function
 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */
char** createMap(int maxRow, int maxCol) {
    if (maxRow < 1 || maxCol < 1) {
        return nullptr;
    }
    char** map = new char*[maxRow];
    for(int i = 0; i < maxRow; ++i)
    {
        map[i]  = new char[maxCol];
    }
    for(int i = 0; i < maxRow; ++i )
    {
        for(int j = 0; j < maxCol; ++j)
        {
            map[i][j] = TILE_OPEN;
        }
    }
    return map;
}

/**
 * TODO: Student implement this function
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @return None
 * @update map, maxRow
 */
void deleteMap(char**& map, int& maxRow) {
    if (map) {
        for (int row = 0; row < maxRow; ++row) {
            delete[] map[row];
        }
        delete[] map;
    }
    map = nullptr;
    maxRow = 0;
}

/**
 * TODO: Student implement this function
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  pointer to a dynamically-allocated 2D array (map) that has twice as many columns and rows in size.
 * @update maxRow, maxCol
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) {
    int initMaxRow = maxRow;
    int initMaxCol = maxCol;
    maxRow *= 2;
    maxCol *= 2;
    if (maxRow < 1 || maxCol < 1) {
      return nullptr;
    }
    char** initMap = new char*[initMaxRow];
    for(int i = 0; i < initMaxRow; ++i)
    {
        initMap[i]  = new char[initMaxCol];
    }
  int playerRow;
  int playerCol;
    for (int i = 0; i < initMaxRow; ++i) {
      for (int x = 0; x < initMaxCol; ++x) {
        if (map[i][x] != TILE_PLAYER) {
          initMap[i][x] = map[i][x];
        }
        else {
          playerRow = i;
          playerCol = x;
          initMap[i][x] = TILE_OPEN;
        }
      }
    }
    char** newMap = new char*[maxRow];
    for(int i = 0; i < maxRow; ++i)
    {
        newMap[i]  = new char[maxCol];
    }
    for (int i = 0; i < initMaxRow; ++i) {
      for (int x = 0; x < initMaxCol; ++x) {
        newMap[i][x] = initMap[i][x];
        newMap[i + initMaxRow][x] = initMap[i][x];
        newMap[i][x + initMaxCol] = initMap[i][x];
        newMap[i + initMaxRow][x + initMaxCol ] = initMap[i][x];
      }
    }
    newMap[playerRow][playerCol] = TILE_PLAYER;
    int initMaxRow2 = initMaxRow;
    deleteMap(initMap, initMaxRow);
    deleteMap(map, initMaxRow2);
    return newMap;
}

/**
 * TODO: Student implement this function
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 * @update map contents, player
 */
int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) {
    if (nextRow >= maxRow || nextCol >= maxCol || nextRow < 0 || nextCol < 0) {return 0;}
    char nextSpot = map[nextRow][nextCol];
    if (nextSpot == TILE_PILLAR || nextSpot == TILE_MONSTER) {return 0;}
    else if (nextSpot == TILE_TREASURE) {
        map[player.row][player.col] = TILE_OPEN;
        player.row = nextRow;
        player.col = nextCol;
        ++player.treasure;
        map[nextRow][nextCol] = TILE_PLAYER; 
        return 2;
    }
    else if (nextSpot == TILE_AMULET) {
        map[player.row][player.col] = TILE_OPEN;
        player.row = nextRow;
        player.col = nextCol;
        map[nextRow][nextCol] = TILE_PLAYER; 
        return 3;
    }
    else if (nextSpot == TILE_DOOR) {
        map[player.row][player.col] = TILE_OPEN;
        player.row = nextRow;
        player.col = nextCol;
        map[nextRow][nextCol] = TILE_PLAYER;
        return 4;
    }
    else if (nextSpot == TILE_EXIT) {
        if (player.treasure > 0) {
            map[player.row][player.col] = TILE_OPEN;
            player.row = nextRow;
            player.col = nextCol;
            map[nextRow][nextCol] = TILE_PLAYER;
            return 5;
        }
        else {
            return 0;
        }
    
    }
    else {
      map[player.row][player.col] = TILE_OPEN;
      player.row = nextRow;
      player.col = nextCol;
      map[nextRow][nextCol] = TILE_PLAYER;
      return 1;
    }
}

/**
 * TODO: Student implement this function
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 * @update map contents
 */
bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) {
    for (int row = player.row; row < maxRow; ++row) {
      if (map[row][player.col] == TILE_PILLAR) {
        break;
      }
      else if (map[row][player.col] == TILE_MONSTER) {
        map[row][player.col] = TILE_OPEN;
        map[row - 1][player.col] = TILE_MONSTER;
        break;
      }
    }
    for (int row = player.row; row >= 0; --row) {
      if (map[row][player.col] == TILE_PILLAR) {
        break;
      }
      else if (map[row][player.col] == TILE_MONSTER) {
        map[row][player.col] = TILE_OPEN;
        map[row + 1][player.col] = TILE_MONSTER;
        break;
      }
    }
    for (int col = player.col; col >= 0; --col) {
      if (map[player.row][col] == TILE_PILLAR) {
        break;
      }
      else if (map[player.row][col] == TILE_MONSTER) {
        map[player.row][col] = TILE_OPEN;
        map[player.row][col + 1] = TILE_MONSTER;
        break;
      }
    }
    for (int col = player.col; col < maxCol; ++col) {
      if (map[player.row][col] == TILE_PILLAR) {
        break;
      }
      else if (map[player.row][col] == TILE_MONSTER) {
        map[player.row][col] = TILE_OPEN;
        map[player.row][col - 1] = TILE_MONSTER;
        break;
      }
    }
    if (map[player.row][player.col] == TILE_MONSTER) {
      return true;
    }
    return false;
}
