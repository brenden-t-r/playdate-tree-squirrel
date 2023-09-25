#include <assert.h>
#include <stdio.h>

#include "collision.h"

struct box_collider SQUIRREL_L2 = {{65, 43},40, 40, TOP_LEFT};
struct box_collider SQUIRREL_L1 = {{84,23}, 40, 40, TOP_LEFT};
struct box_collider SQUIRREL_C = {{106, 20}, 40, 40, TOP_LEFT};
struct box_collider SQUIRREL_R1 = {{130,23}, 40, 40, TOP_LEFT};
struct box_collider SQUIRREL_R2 = {{151, 43}, 40, 40, TOP_LEFT};
struct box_collider ACORN = {{0, 0, }, 40, 40, TOP_LEFT};
struct box_collider SPIDER = {{0, 0}, 30, 10, CENTER};

int detect(
        struct vector2 p1, struct vector2 p2,
        struct box_collider c1, struct box_collider c2
) {
    int cd1_x = p1.x + c1.pos.x;
    int cd1_y = p1.y + c1.pos.y;
    int cd2_x = p2.x + c2.pos.x;
    int cd2_y = p2.y + c2.pos.y;

    if (c1.pivot == CENTER) {
        cd1_x -= c1.width / 2;
        cd1_y += c1.height / 2;
    }

    if ((cd1_x >= cd2_x && cd1_x <= cd2_x + c2.width
         && cd1_y >= cd2_y && cd1_y <= cd2_y + c2.height)
        || (cd1_x + c1.width >= cd2_x && cd1_x + c1.width <= cd2_x + c2.width
         && cd1_y + c1.height >= cd2_y && cd1_y + c1.height <= cd2_y + c2.height)
        ) {
        // We have a collision, folks.
        return 1;
    }
    return 0;
}

// Tests
int main() {
    assert(SQUIRREL_L2.pos.y == 43);
    printf("PASSED");
    return 0;
}