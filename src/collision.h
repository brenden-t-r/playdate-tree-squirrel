/**
 * 2D Collision detection
 *
 * Supports box colliders, in upright orientation (no rotation)
 *
 *  Pivot
 *  TOP_LEFT | Collider is positioned at vector2 `pos` relative to top-left
 *  CENTER   | Collider is centered, with `pos` translation from center.
 */

struct vector2 {
    int x;
    int y;
};

enum PIVOT {
    TOP_LEFT = 0,
    CENTER = 1
};

struct box_collider {
    struct vector2 pos;
    int width;
    int height;
    enum PIVOT pivot;
};

// TODO
struct box_collider_adv {
    // Corners, counter-clockwise from top left.
    struct vector2 p1;
    struct vector2 p2;
    struct vector2 p3;
    struct vector2 p4;
};

int detect(
        struct vector2 p1, struct vector2 p2,
        struct box_collider c1, struct box_collider c2
);

