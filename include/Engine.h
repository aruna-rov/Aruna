#ifndef Engine_H
#define Engine_H

typedef enum
{
    LEFT,
    RIGHT
} Direction;

typedef struct
{
    float speed;
    float position;
    int ioPort;
    Direction direction;
} Engine;

void move_engine(Engine engine);

#endif //Engine_H