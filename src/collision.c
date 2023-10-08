#include "collision.h"

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
