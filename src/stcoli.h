#ifndef _SRC_STCOLI_H_
#define _SRC_STCOLI_H_

enum
{
    COLI_FLAG_OCCURRED = 1 << 0, // If at least one ball collision occurred on the current frame
};

struct ColiPlane
{
    Point3d point; // A point on the plane
    Vec normal;    // Normal of plane
};

struct PhysicsBall
{
    /*0x00*/ u32 flags;

    // Current center position in animGroupId's local space
    /*0x04*/ Point3d pos;

    // Center position at end of previous frame in animGroupId's previous frame local space
    /*0x10*/ Point3d prevPos;

    // Current velocity in animGroupId's local space
    /*0x1C*/ Vec vel;

    /*0x28*/ float radius;
    /*0x2C*/ float gravityAccel;
    /*0x30*/ float restitution;

    // The ball may collide with more than one surface during a frame. The "hardest" collision is
    // recorded, which is used to draw visual collision effects for example.

    // Largest (in magnitude) animGroup-relative ball velocity along the collision normal. It's
    // always negative because when a collision occurs, the ball's animGroup-relative velocity is
    // pointing away from the normal.
    /*0x34*/ float hardestColiSpeed;

    // Collision plane of the hardest collision, in hardestColiAnimGroupId's local space
    /*0x38*/ struct ColiPlane hardestColiPlane;

    // animGroup ID of the hardest collision
    /*0x50*/ s32 hardestColiAnimGroupId;

    // Friction applied to the ball's velocity on each contact with a surface.
    //
    // Specifically, it is the fraction of the ball's velocity parallel to the contact surface which
    // is thrown away upon contact. The ball's velocity in this context is relative to the contact
    // surface's velocity. For example, the relative velocity of a motionless ball on a platform
    // with velocity (1, 0, 0) would be (-1, 0, 0).
    /*0x54*/ float friction;

    // animGroup whose local space we are in.
    // As a reminder, ID 0 is world space.
    /*0x58*/ s32 animGroupId;
};

struct ColiEdge
{
    // Winds counterclockwise around tri up normal
    Point2d start;
    Point2d end;

    // Coplanar with triangle, points inside triangle
    Vec2d normal;
};

struct RaycastHit
{
    u32 flags;
    Point3d pos;
    Vec normal;
};

struct ColiCircle;
struct ColiRect;
struct DynamicStagePart;
struct Stage;
struct StageColiCone;
struct StageColiCylinder;
struct StageColiSphere;
struct StageColiTri;
struct StageGoal;

void collide_ball_with_stage(struct PhysicsBall *b, struct Stage *);
s16 *coligrid_lookup(struct StageAnimGroup *coliHeader, f32 x, f32 z);
void collide_ball_with_tri_face(struct PhysicsBall *physBall, struct StageColiTri *tri);
void collide_ball_with_tri_edges(struct PhysicsBall *physBall, struct StageColiTri *tri);
void collide_ball_with_tri_edge(struct PhysicsBall *physBall, Vec *localBallPos,
                                Vec *localBallPrevPos, struct ColiEdge *edge);
void collide_ball_with_tri_verts(struct PhysicsBall *physBall, struct StageColiTri *tri);
void collide_ball_with_tri_vert(struct PhysicsBall *, Point3d *, Point2d *);
void collide_ball_with_rect(struct PhysicsBall *physBall, struct ColiRect *rect);
void collide_ball_with_cylinder(struct PhysicsBall *, struct StageColiCylinder *cylinder);
void collide_ball_with_circle(struct PhysicsBall *ball, struct ColiCircle *circle);
void collide_ball_with_sphere(struct PhysicsBall *, struct StageColiSphere *sphere);
void collide_ball_with_cone(struct PhysicsBall *, struct StageColiCone *cone);
void collide_ball_with_plane(struct PhysicsBall *physBall, struct ColiPlane *coliHit);
u32 test_line_intersects_rect(Point3d *lineStart, Point3d *lineEnd, struct ColiRect *rect);
void collide_ball_with_jamabar(struct PhysicsBall *ball, struct Stobj *stobj);
int raycast_stage_down(Point3d *rayOrigin, struct RaycastHit *outHit, Vec *outVelAtPoint);
u32 raycast_tri(Point3d *rayOrigin, Vec *rayDir, struct StageColiTri *tri);
u32 raycast_cone(Point3d *rayOrigin, Point3d *rayDir, struct StageColiCone *cone,
                 Point3d *outHitPos, Vec *outHitNormal);
u32 raycast_sphere(Point3d *rayOrigin, Point3d *rayDir, struct StageColiSphere *sphere,
                   Point3d *outHitPos, Vec *outHitNormal);
u32 raycast_cylinder(Point3d *rayOrigin, Point3d *rayDir, struct StageColiCylinder *cylinder,
                     Point3d *outHitPos, Vec *outHitNormal);
void collide_ball_with_goal(struct PhysicsBall *ball, struct StageGoal *goal);
// ? stcoli_sub22();
// ? stcoli_sub23();
// ? stcoli_sub24();
void u_collide_ball_with_dynstageparts(struct PhysicsBall *,
                                       struct DynamicStagePart *dynStageParts);
void u_draw_stage_collision(void);
// ? draw_dynamic_stage_collision();
// ? draw_collision_triangle();
// ? stcoli_sub29();
void stcoli_sub30(struct PhysicsBall *src, struct PhysicsBall *dest);
void stcoli_sub31(struct PhysicsBall *src, struct PhysicsBall *dest);
void tf_physball_to_anim_group_space(struct PhysicsBall *physBall, int animGroupId);
// ? stcoli_sub33();
int stcoli_sub34(struct PhysicsBall *ball, int arg1);

#endif
