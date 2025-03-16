#include <qassert.h>
#pragma hdrstop
#include "TetrisEngine.h"

#ifdef ASSERT
#define TETRIS_ASSERT ASSERT
#elif defined Q_ASSERT
#define TETRIS_ASSERT Q_ASSERT
#else
#define TETRIS_ASSERT assert
#endif

using namespace tetris;

//all shapes matrices
using shape_layouts_t = shape_matrix_t[SHAPE_LAYOUT_COUNT];
constexpr shape_layouts_t TETRIS_SHAPES[SHAPE_TYPE_COUNT]  =
{
    //SHAPE_TYPE_LINE
    {
        {
            {0,0,0,0},
            {1,1,1,1},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,0,0},
            {0,1,0,0},
            {0,1,0,0}
        },
        {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}
    },

    //SHAPE_TYPE_T
    {
        {
            {0,0,0,0},
            {1,1,1,0},
            {0,1,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {1,1,0,0},
            {0,1,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {1,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,1,0},
            {0,1,0,0},
            {0,0,0,0}
        }
    },

    //SHAPE_TYPE_LZ
    {
        {
            {0,1,0,0},
            {0,1,1,0},
            {0,0,1,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {0,1,1,0},
            {1,1,0,0},
            {0,0,0,0}
        },
        {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}
    },

    //SHAPE_TYPE_RZ
    {
        {
            {0,0,1,0},
            {0,1,1,0},
            {0,1,0,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {1,1,0,0},
            {0,1,1,0},
            {0,0,0,0}
        },
        {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}
    },

    //SHAPE_TYPE_SQUARE
    {
        {
            {1,1,0,0},
            {1,1,0,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}
    },

    //SHAPE_TYPE_LL
    {
        {
            {0,0,1,0},
            {1,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,0,0},
            {0,1,1,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {1,1,1,0},
            {1,0,0,0},
            {0,0,0,0}
        },
        {
            {1,1,0,0},
            {0,1,0,0},
            {0,1,0,0},
            {0,0,0,0}
        },
    },

    //SHAPE_TYPE_RL
    {
        {
            {0,0,0,0},
            {1,1,1,0},
            {0,0,1,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,0,0},
            {1,1,0,0},
            {0,0,0,0}
        },
        {
            {1,0,0,0},
            {1,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,1,0},
            {0,1,0,0},
            {0,1,0,0},
            {0,0,0,0}
        },
    },
};

static_assert(SHAPE_TYPE_COUNT == sizeof(TETRIS_SHAPES) / sizeof(shape_layouts_t));

constexpr size_t get_layout_count(int shape_type)
{
    switch(shape_type)
    {
    case SHAPE_TYPE_LINE: return 2;
    case SHAPE_TYPE_T: return 4;
    case SHAPE_TYPE_LZ: return 2;
    case SHAPE_TYPE_RZ: return 2;
    case SHAPE_TYPE_SQUARE: return 1;
    case SHAPE_TYPE_LL: return 4;
    case SHAPE_TYPE_RL: return 4;
    }

    TETRIS_ASSERT(0);
    return 0;
}

shape_t::shape_t() : Matrix{TETRIS_SHAPES[0]}
{
}

shape_t::shape_t(const shape_t &rop) :
    Matrix{rop.Matrix},
    BlockType{rop.BlockType}
{
}
shape_t::shape_t(int shape_type) :
    Matrix{&TETRIS_SHAPES[shape_type][0]}
{
    TETRIS_ASSERT(0 <= shape_type && shape_type < SHAPE_TYPE_COUNT);
}

shape_t &shape_t::operator=(const shape_t &rop)
{
    Matrix = rop.Matrix;
    BlockType = rop.BlockType;
    return *this;
}

void shape_t::reset(int shape_type, block_t block_type)
{
    Matrix = &TETRIS_SHAPES[shape_type][0];
    BlockType = block_type;
}
int shape_t::get_type() const
{
    const shape_matrix_t* pos = Matrix;
    return (pos - TETRIS_SHAPES[0]) / SHAPE_LAYOUT_COUNT;
}

int shape_t::get_layout() const
{
    const int type = get_type();
    const shape_matrix_t* begin = TETRIS_SHAPES[type];
    const shape_matrix_t* pos = Matrix;
    return pos - begin;
}

void shape_t::rotate_left()
{
    const int type = get_type();
    const size_t layout_count = get_layout_count(type);
    int layout = get_layout();
    ++layout;
    if(layout >= layout_count)
        layout = 0;
    Matrix = &TETRIS_SHAPES[type][layout];
}

void shape_t::rotate_right()
{
    const int type = get_type();
    const int layout_count = get_layout_count(type);
    int layout = get_layout();
    --layout;
    if(layout < 0)
        layout = layout_count - 1;
    Matrix = &TETRIS_SHAPES[type][layout];
}

class default_shape_generator : public shape_generator_t
{
public:
    void generate(shape_t& shape)
    {
        shape.reset(SHAPE_TYPE_SQUARE, static_cast<block_t>(0x808080));
    }
};
static default_shape_generator default_generator;

engine::engine() : ShapeGenerator{&default_generator}
{
    clear();
}
void engine::clear()
{
    Score = 0;
	for(size_t x = 0; x < GAME_FIELD_WIDTH; ++x)
		for(size_t y = 0; y < GAME_FIELD_HEIGHT; ++y)
			Blocks[x][y] = BLOCK_NONE;
}

void engine::set_generator(shape_generator_t *generator /*= nullptr*/)
{
    ShapeGenerator = generator ? generator : &default_generator;
}
block_t engine::get_block(int x, int y) const
{
    if(Blocks[x][y] != BLOCK_NONE)
        return Blocks[x][y];
    return get_shape_block(x, y, Shape, ShapePos);
}
block_t engine::get_shape_block(int x, int y, const shape_t& shape, const point_t& pos) const
{
    const short x_pos = x - pos.X;
    if(x_pos < 0 || SHAPE_MATRIX_SIZE <= x_pos)
        return BLOCK_NONE;
    const short y_pos = y - pos.Y;
    if(y_pos < 0 || SHAPE_MATRIX_SIZE <= y_pos)
        return BLOCK_NONE;

    const shape_matrix_t& matrix = *shape.GetMatrix();
    const block_t matrix_block = matrix[x_pos][y_pos];

    return (BLOCK_NONE == matrix_block) ? BLOCK_NONE : shape.get_block_type();
}
int engine::new_game()
{
    clear();
    ShapeGenerator->generate(NextShape);
    return new_shape();
}
int engine::new_shape()
{
    //TODO:
    //new shape pos
    ShapePos.X = GAME_FIELD_WIDTH / 2 - 1;
    ShapePos.Y = 0;

    Shape = NextShape;
    if(test_shape(Shape, ShapePos))
        return RESULT_GAME_OVER;

    ShapeGenerator->generate(NextShape);
    return RESULT_SHAPE;
}
bool engine::test_shape(const shape_t& shape, const point_t& pos)
{
    const shape_matrix_t& matrix = *shape.GetMatrix();

    for(int x = 0; x < SHAPE_MATRIX_SIZE; ++x)
    {
        for(int y = 0; y < SHAPE_MATRIX_SIZE; ++y)
        {
            const block_t shape_block = matrix[x][y];
            if(BLOCK_NONE == shape_block)
                continue;

            //game field coords
            const short x_pos = pos.X + x;
            const short y_pos = pos.Y + y;

            //test walls
            if(x_pos < 0 || GAME_FIELD_WIDTH <= x_pos)
                return true;

            //test floor
            if(y_pos >= GAME_FIELD_HEIGHT)
                return true;

            //test game field blocks
            const block_t field_block = Blocks[x_pos][y_pos];
            if(field_block != BLOCK_NONE)
                return true;
        }
    }

    return false;
}
int engine::move_left()
{
    const point_t test_pos(ShapePos.X - 1, ShapePos.Y);
    if(test_shape(Shape, test_pos))
        return RESULT_NONE;
    ShapePos = test_pos;
    return RESULT_CHANGED;
}
int engine::move_right()
{
    const point_t test_pos(ShapePos.X + 1, ShapePos.Y);
    if(test_shape(Shape, test_pos))
        return RESULT_NONE;
    ShapePos = test_pos;
    return RESULT_CHANGED;
}

int engine::move_down()
{
    const point_t test_pos(ShapePos.X, ShapePos.Y + 1);
    if(test_shape(Shape, test_pos))
    {
        insert_shape();
        return new_shape();
    }
    ShapePos = test_pos;
    return RESULT_CHANGED;
}
int engine::drop()
{
    int result{RESULT_NONE};
    for(;;)
    {
        result = move_down();
        if(RESULT_CHANGED != result)
            return result;
    }
    return result;
}
int engine::rotate_left()
{
    shape_t shape(Shape);
    shape.rotate_left();
    if(test_shape(shape, ShapePos))
        return RESULT_NONE;
    Shape = shape;
    return RESULT_CHANGED;
}
int engine::rotate_right()
{
    shape_t shape(Shape);
    shape.rotate_right();
    if(test_shape(shape, ShapePos))
        return RESULT_NONE;
    Shape = shape;
    return RESULT_CHANGED;
}
bool engine::is_solid_line(int y) const
{
    TETRIS_ASSERT(0 <= y && y <= GAME_FIELD_HEIGHT);
	if (y < 0 || GAME_FIELD_HEIGHT <= y)
		return false;

	for (coord_t x = 0; x < GAME_FIELD_WIDTH; ++x)
		if (BLOCK_NONE == Blocks[x][y])
			return false;
	return true;
}
void engine::insert_shape()
{
    //add shape blocks to game field
    const shape_matrix_t& matrix = *Shape.GetMatrix();
    for(int x = 0; x < SHAPE_MATRIX_SIZE; ++x)
    {
        for(int y = 0; y < SHAPE_MATRIX_SIZE; ++y)
        {
            const block_t shape_block = matrix[x][y];
            if(BLOCK_NONE == shape_block)
                continue;

            //game field coords
            const short x_pos = ShapePos.X + x;
            const short y_pos = ShapePos.Y + y;
            TETRIS_ASSERT(x_pos < GAME_FIELD_WIDTH && y_pos < GAME_FIELD_HEIGHT);

            Blocks[x_pos][y_pos] = Shape.get_block_type();
        }
    }

    //check for solid lines
    for(int y = 0; y < GAME_FIELD_HEIGHT; ++y)
    {
        if (false == is_solid_line(y))
            continue;

        remove_line(y);
        ++Score;
    }
}
void engine::remove_line(int line_y)
{
    TETRIS_ASSERT(line_y >= 0 && line_y < GAME_FIELD_HEIGHT);

	//remove line
	for(int x = 0; x < GAME_FIELD_WIDTH; ++x)
        Blocks[x][line_y] = BLOCK_NONE;

	//move down upper blocks
    for(int y = line_y - 1; y >= 0; --y)
	{
		for(int x = 0; x < GAME_FIELD_WIDTH; ++x)
		{
            Blocks[x][y + 1] = Blocks[x][y];
            Blocks[x][y] = BLOCK_NONE;
        }
	}
}
