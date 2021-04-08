#ifndef _SNAKE_H_
#define _SNAKE_H_

//////////////////////////////////////////////////////////////////////////////////////////////////
//                  _                                                                           //
//                 | |                                                                          //
//  ___ _ __   __ _| | _____     __                                                             //
// / __| '_ \ / _` | |/ / _ \   {OO}                                                            //
// \__ \ | | | (_| |   <  __/   \__/                                                            //
// |___/_| |_|\__,_|_|\_\___|   |^|                                                             //
//  ____________________________/ /                                                             //
// /  ___________________________/                                                              //
//  \_______ \                                                                                  //
//          \|                                                                                  //
//                                                                                              //
// FILE: snake.h                                                                                //
// AUTHOR: Ian Murfin - github.com/pixrex                                                       //
//                                                                                              //
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <array>
#include "pxr_game.h"
#include "pxr_gfx.h"
#include "pxr_vec.h"
#include "pxr_input.h"

using namespace pxr;

class Snake final : public pxr::Game
{
public:
  static constexpr const char* name {"snake"};
  static constexpr int versionMajor {0};
  static constexpr int versionMinor {1};

  ////////////////////////////////////////////////////////////////////////////////////////////////
  // CONFIGURATION
  ////////////////////////////////////////////////////////////////////////////////////////////////

  static constexpr Vector2i worldSize_rx       {200, 200};
  static constexpr Vector2i boardSize          {40, 36};
  static constexpr int      blockSize_rx       {4};

  static constexpr Vector2i boardPosition {
    (worldSize_rx._x - (boardSize._x * blockSize_rx)) / 2,
    (worldSize_rx._x - (boardSize._x * blockSize_rx)) / 2
  };

  static constexpr int      boardMarginLoX     {boardPosition._x};
  static constexpr int      boardMarginHiX     {boardPosition._x + (boardSize._x * blockSize_rx)};
  static constexpr int      boardMarginLoY     {boardPosition._y};
  static constexpr int      boardMarginHiY     {boardPosition._y + (boardSize._y * blockSize_rx)};

  static constexpr int      maxSnakeLength     {400};
  static constexpr int      babySnakeLength    {6};
  static constexpr float    stepFrequency_hz   {10.f};
  static constexpr float    stepPeriod_s       {1.f / stepFrequency_hz};

  static constexpr int      snakeHeadSpawnCol  {(boardSize._x / 2) - (babySnakeLength / 2)};
  static constexpr int      snakeHeadSpawnRow  {(boardSize._y / 2)};

  static constexpr int      maxNuggetsInWorld  {5};

  ////////////////////////////////////////////////////////////////////////////////////////////////
  // CONTROLS       
  ////////////////////////////////////////////////////////////////////////////////////////////////

  static constexpr pxr::input::KeyCode moveLeftKey  {pxr::input::KEY_LEFT };
  static constexpr pxr::input::KeyCode moveRightKey {pxr::input::KEY_RIGHT};
  static constexpr pxr::input::KeyCode moveUpKey    {pxr::input::KEY_UP   };
  static constexpr pxr::input::KeyCode moveDownKey  {pxr::input::KEY_DOWN };

  static constexpr pxr::input::KeyCode smoothToggle {pxr::input::KEY_s    };

  ////////////////////////////////////////////////////////////////////////////////////////////////
  // SNAKE BLOCKS 
  ////////////////////////////////////////////////////////////////////////////////////////////////

  //
  // Each snake block can exist in 1 of 24 possible states which depend on the arrangement of its
  // neighbouring blocks and on the direction to the head. These states are render states which
  // control how to draw the snake.
  //
  // A block can have either 1 or 2 neighbours. These neighbours can be located in 4 relative
  // positions: north, east, south, west. Each neighbouring block can be either in the direction
  // of (closer to) the head or the tail, which adds a direction property to blocks.
  //
  // There are 6 possible configurations of 2 neighbours:
  //
  // +------------------------------------------------------------------+
  // |  description              pattern      equivilent_to             |
  // +------------------------------------------------------------------+
  // |                           N                                      |
  // |  north-this-south         T            south-this-north          |
  // |                           S                                      |
  // |                                                                  |
  // |  west-this-east         W T E          east-this-west            |
  // |                                                                  |
  // |                           N                                      |
  // |  north-this-east          T E          east-this-north           |
  // |                                                                  |
  // |                           N                                      |
  // |  north-this-west        W T            west-this-north           |
  // |                                                                  |
  // |                         W T                                      |
  // |  south-this-west          S            west-this-south           |
  // |                                                                  |
  // |                                                                  |
  // |  south-this-east          T E          east-this-south           |
  // |                           S                                      |
  // +------------------------------------------------------------------+
  //
  // and 4 possible configurations of 1 neighbour:
  //
  // +------------------------------------------------------------------+
  // | description              pattern      equivilent_to              |
  // +------------------------------------------------------------------+
  // | this-west                T W          west-this                  |
  // |                                                                  |
  // |                                                                  |
  // | this-east                E T          east-this                  |
  // |                                                                  |
  // |                          T                                       |
  // | this-south               S            south-this                 |
  // |                                                                  |
  // |                          N                                       |
  // | this-north               T            north-this                 |
  // +------------------------------------------------------------------+
  //
  // Thus we have 10 possible states with 10 equivilencies where the equivilent versions of
  // each state are the same state but in the opposite direction, e.g. north-this-south could
  // mean the north neighbour is closer to the head, and south-this-north would then mean the 
  // south neighbour is closer to the head. You could also define the opposite, the choice is 
  // arbitrary.
  //
  // The 1 neighbour configurations correspond to either head or tail blocks depending on 
  // direction. So this-west could mean this is the tail and to the west is the head, and 
  // west-this could mean this is the head and west is the tail. Again you could define the
  // opposite.
  //
  // The IDs defined in this enumeration are used to index into the snake spritesheet to select
  // the sprite for a given block state. Thus the order of the sprites in the spritesheet must
  // match the order defined by the values in this enumeration.
  //
  enum SnakeBlockSpriteID
  {
    SID_NULL = -1,

    // BODY BLOCKS 

    SID_HEAD_NORTH_THIS_SOUTH_TAIL = 0,
    SID_HEAD_SOUTH_THIS_NORTH_TAIL = 1,
    SID_HEAD_WEST_THIS_EAST_TAIL   = 2,
    SID_HEAD_EAST_THIS_WEST_TAIL   = 3,
    SID_HEAD_NORTH_THIS_EAST_TAIL  = 4,
    SID_HEAD_EAST_THIS_NORTH_TAIL  = 4,
    SID_HEAD_NORTH_THIS_WEST_TAIL  = 4,
    SID_HEAD_WEST_THIS_NORTH_TAIL  = 4,
    SID_HEAD_SOUTH_THIS_WEST_TAIL  = 5,
    SID_HEAD_WEST_THIS_SOUTH_TAIL  = 5,
    SID_HEAD_SOUTH_THIS_EAST_TAIL  = 5,
    SID_HEAD_EAST_THIS_SOUTH_TAIL  = 5,

    // TAIL BLOCKS
    
    SID_HEAD_WEST_THIS  = 6,
    SID_HEAD_EAST_THIS  = 7,
    SID_HEAD_NORTH_THIS = 8,
    SID_HEAD_SOUTH_THIS = 9,

    // HEAD BLOCKS  

    SID_THIS_WEST_TAIL  = 10,
    SID_THIS_EAST_TAIL  = 11,
    SID_THIS_NORTH_TAIL = 12,
    SID_THIS_SOUTH_TAIL = 13, 

    //
    // TONGUE BLOCKS
    //
    SID_WESTWARD_TONGUE  = 14,
    SID_EASTWARD_TONGUE  = 15,
    SID_NORTHWARD_TONGUE = 16,
    SID_SOUTHWARD_TONGUE = 17,

    //
    // BLOOD BLOCK
    //
    SID_BLOOD_BLOCK = 18
  };

  //
  // The offset between the sprites for each hero snake. So for example if Montezuma's sprites
  // start at spriteid = 0, then the next snakes sprites will start at spriteid = 0 + 
  // SID_SNAKE_OFFSET.
  //
  static constexpr int SID_SNAKE_OFFSET {18};

  //
  // Total number of sprites in the snakes spritesheet.
  //
  static constexpr int SID_SNAKE_SHEET_COUNT = 18;

  enum Direction { NORTH, SOUTH, EAST, WEST, DIRECTION_COUNT };

  //
  // Defines a tree which maps all possible neighbour configurations for body blocks (blocks
  // with 2 neighbours) to snake spritesheet sprite ids.
  //
  // The tree has the structure,
  //                            
  //                          root
  //                            |
  //       +-------------+------+------+-------------+
  //       |             |             |             |
  //       N             S             E             W          [ head neighbour ]
  //       |             |             |             |
  //    +--+--+--+    +--+--+--+    +--+--+--+    +--+--+--+
  //    |  |  |  |    |  |  |  |    |  |  |  |    |  |  |  |
  //    N  S  E  W    N  S  E  W    N  S  E  W    N  S  E  W    [ tail neighbour ]
  //    :  :  :  :    :  :  :  :    :  :  :  :    :  :  :  :
  //    X  s  s  s    s  X  s  s    s  s  X  s    s  s  s  X   <-- s = placeholder for sprite id,
  //       ^                              ^                         (not the same in every node)
  //       |                              |
  //   so this path would be for,        the X's indicate invalid
  //    SID_HEAD_NORTH_THIS_SOUTH_TAIL     routes
  //
  // This tree can be indexed with the directions enum, thus to get the SID for a northward head
  // neighbour and a southward tail neighbour,
  //     sid = snakeBodyBlockTree[NORTH][SOUTH]
  //
  static constexpr std::array<std::array<SnakeBlockSpriteID, DIRECTION_COUNT>, DIRECTION_COUNT> snakeBodyBlockTree {{
    // NORTH [head]
    {
      SID_NULL, 
      SID_HEAD_NORTH_THIS_SOUTH_TAIL,
      SID_HEAD_NORTH_THIS_EAST_TAIL,
      SID_HEAD_NORTH_THIS_WEST_TAIL
    },
    // SOUTH [head]
    {
      SID_HEAD_SOUTH_THIS_NORTH_TAIL,
      SID_NULL,
      SID_HEAD_SOUTH_THIS_EAST_TAIL,
      SID_HEAD_SOUTH_THIS_WEST_TAIL
    },
    // EAST  [head]
    {
      SID_HEAD_EAST_THIS_NORTH_TAIL,
      SID_HEAD_EAST_THIS_SOUTH_TAIL,
      SID_NULL,
      SID_HEAD_EAST_THIS_WEST_TAIL
    },
    // WEST  [head]
    {
    SID_HEAD_WEST_THIS_NORTH_TAIL,
    SID_HEAD_WEST_THIS_SOUTH_TAIL,
    SID_HEAD_WEST_THIS_EAST_TAIL,
    SID_NULL
    }
  }};

  //
  // Maps the neighbour configurations for blocks with a single head neighbour but no tail
  // neighbour to snake spritesheet sprite ids. This is not really much of a tree however as
  // it only has a single layer.
  //
  static constexpr std::array<SnakeBlockSpriteID, DIRECTION_COUNT> snakeHeadBlockTree {
    SID_THIS_NORTH_TAIL,
    SID_THIS_SOUTH_TAIL,
    SID_THIS_EAST_TAIL,
    SID_THIS_WEST_TAIL
  };

  //
  // Maps the neighbour configurations for blocks with a single tail neighbour but no head
  // neighbour to snake spritesheet sprite ids. This is not really much of a tree however as
  // it only has a single layer.
  //
  static constexpr std::array<SnakeBlockSpriteID, DIRECTION_COUNT> snakeTailBlockTree {
    SID_HEAD_NORTH_THIS,
    SID_HEAD_SOUTH_THIS,
    SID_HEAD_EAST_THIS,
    SID_HEAD_WEST_THIS
  };

  //
  // Used when drawing smooth moving snakes. For smooth movement the sprite used depends only on
  // the direction of a block's movement not on the block's neighbour configuration.
  //
  static constexpr std::array<SnakeBlockSpriteID, DIRECTION_COUNT> smoothSnakeBodyBlockTree {
    SID_HEAD_NORTH_THIS_SOUTH_TAIL,
    SID_HEAD_SOUTH_THIS_NORTH_TAIL,
    SID_HEAD_EAST_THIS_WEST_TAIL,
    SID_HEAD_WEST_THIS_EAST_TAIL
  };

  //
  // Used when drawing the head for smooth moving snakes.
  //
  static constexpr std::array<SnakeBlockSpriteID, DIRECTION_COUNT> smoothSnakeHeadBlockTree {
    SID_THIS_SOUTH_TAIL,
    SID_THIS_NORTH_TAIL,
    SID_THIS_WEST_TAIL,
    SID_THIS_EAST_TAIL
  };
  
  ////////////////////////////////////////////////////////////////////////////////////////////////
  // SPRITESHEETS
  ////////////////////////////////////////////////////////////////////////////////////////////////

  //
  // Spritesheet gfx::ResourceKeys are assigned at runtime and so cannot be cleanly made compile
  // time constants. Thus these IDs must not be used raw but instead be used with a call to,
  //    getSpritesheetKey(SpritesheetID sheetID)
  //
  enum SpritesheetID
  {
    SSID_SNAKES,
    SSID_NUGGETS,
    SSID_BACKGROUND,
    SSID_FOREGROUND,
    SSID_COUNT
  };

  static constexpr std::array<gfx::ResourceName_t, SSID_COUNT> spritesheetNames {
    "snakes",
    "nuggets",
    "background",
    "foreground"
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////
  // SNAKES        
  ////////////////////////////////////////////////////////////////////////////////////////////////

  //
  // The heroes availible to play as.
  //
  enum SnakeHero
  {
    MONTEZUMA,
    ITZCOATL
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////
  // NUGGETS       
  ////////////////////////////////////////////////////////////////////////////////////////////////

  enum NuggetSpriteID
  {
    SID_NUGGET_GOLD,
    SID_NUGGET_SILVER,
    SID_NUGGET_OBSIDIAN,
    SID_NUGGET_RUBY,
    SID_NUGGET_JADE,
    SID_NUGGET_LAPIS,
    SID_NUGGET_AMETHYST
  };

  enum NuggetClassID
  {
    NUGGET_GOLD, 
    NUGGET_SILVER,
    NUGGET_OBSIDIAN,
    NUGGET_RUBY,
    NUGGET_JADE,
    NUGGET_LAPIS,
    NUGGET_AMETHYST
  };

  static constexpr int nuggetClassCount {7};

  struct NuggetClass
  {
    gfx::SpriteID_t _spriteid; 
    float _lifetime;
    int _spawnChance;
    int _score;
  };

  static constexpr std::array<NuggetClass, nuggetClassCount> nuggetClasses {{
  //----------------------------------------------------------------------------------------------
  // spriteid             lifetime   chance   score
  //----------------------------------------------------------------------------------------------
    {SID_NUGGET_GOLD    , 2.f,          10,   70 },
    {SID_NUGGET_SILVER  , 3.f,          20,   60 },
    {SID_NUGGET_OBSIDIAN, 4.f,          30,   50 },
    {SID_NUGGET_RUBY    , 5.f,          40,   40 },
    {SID_NUGGET_JADE    , 6.f,          50,   30 },
    {SID_NUGGET_LAPIS   , 7.f,          70,   20 },
    {SID_NUGGET_AMETHYST, 8.f,         100,   10 }
  }};

  ////////////////////////////////////////////////////////////////////////////////////////////////
  // GFX SCREENS       
  ////////////////////////////////////////////////////////////////////////////////////////////////

  //
  // Screens are created in the order they are defined in this enum, this means they will also
  // be drawn in this order, meaning the first defined is the bottom of the screen layers.
  //
  enum GFXScreenName
  {
    SCREEN_BACKGROUND,
    SCREEN_STAGE,
    SCREEN_FOREGROUND,
    SCREEN_COUNT
  };

public:
  Snake() = default;
  ~Snake() = default;

  bool onInit();
  void onShutdown();

  std::string getName() const {return name;}
  int getVersionMajor() const {return versionMajor;}
  int getVersionMinor() const {return versionMinor;}

  gfx::ResourceKey_t getSpritesheetKey(SpritesheetID sheetID);
  gfx::ScreenID_t getScreenID(GFXScreenName screenName);

  SnakeHero getSnakeHero() const {return _snakeHero;}

  void addScore(int score) {_score += score;}
  int getScore() const {return _score;}

private:
  void loadSpritesheets();

private:
  std::array<gfx::ResourceKey_t, SSID_COUNT> _spritesheetKeys;
  SnakeHero _snakeHero;

  int _score;
};

#endif
