#pragma once
// DK2VR 0.2.1 - Demeo-style manipulation of the table pose (host side, pure math).
// One grip moves the table and turns it with the hand; two grips turn it around
// the midpoint of the hands and scale it with their distance. The table stays level.
#include "Math.hpp"

namespace dk2vr {
inline float wrapAngle(float a) {while(a>Pi) a-=2*Pi;while(a<-Pi) a+=2*Pi;return a;}
inline float horizontalAngle(Vec3 v) {return std::atan2(v.x,v.z);}
struct TableGrab {
    int hands=0;Pose table0{};Pose hand0{};Vec3 mid0{},vec0{};float width0=1,distance0=0;
    void release() {hands=0;}
    // Returns true when the table has just been released.
    // oneHandMoves: one grip moves the table (0.2.0). In 0.2.1 one grip pans the dungeon instead (game side).
    // twoHandScales: hand spread scales the table (0.2.0). In 0.2.1 the spread zooms the native camera instead.
    bool oneHandMoves=true,twoHandScales=true;
    // 0.3.2: one hand on a handle turns the table on itself (turntable); position unchanged.
    bool oneHandTurns=false;
    bool update(const bool grip[2],const Pose poses[2],Pose& table,float& width,float minWidth,float maxWidth) {
        int n=int(grip[0])+int(grip[1]);
        if(n==0) {bool was=hands>0;hands=0;return was;}
        if(n!=hands) {
            hands=n;table0=table;width0=width;
            if(n==1) hand0=grip[0]?poses[0]:poses[1];
            else {
                mid0=scale(add(poses[0].position,poses[1].position),.5f);
                vec0=sub(poses[1].position,poses[0].position);vec0.y=0;
                distance0=std::sqrt(vec0.x*vec0.x+vec0.z*vec0.z);
            }
        }
        if(n==1&&oneHandTurns) {
            const Pose& h=grip[0]?poses[0]:poses[1];
            Vec3 a=sub(hand0.position,table0.position),b=sub(h.position,table0.position);
            if(a.x*a.x+a.z*a.z<1e-4f||b.x*b.x+b.z*b.z<1e-4f) return false;
            Quat turn=yawQuat(wrapAngle(horizontalAngle(b)-horizontalAngle(a)));
            table.orientation=normalized(multiply(turn,table0.orientation));table.position=table0.position;
            return false;
        }
        if(n==1) {
            if(!oneHandMoves) return false;
            const Pose& h=grip[0]?poses[0]:poses[1];
            Quat turn=yawQuat(wrapAngle(yawAngle(h.orientation)-yawAngle(hand0.orientation)));
            table.orientation=normalized(multiply(turn,table0.orientation));
            table.position=add(h.position,rotate(turn,sub(table0.position,hand0.position)));
        } else if(distance0>.03f) {
            Vec3 mid=scale(add(poses[0].position,poses[1].position),.5f);
            Vec3 v=sub(poses[1].position,poses[0].position);v.y=0;
            float d=std::sqrt(v.x*v.x+v.z*v.z);
            if(d>.01f) {
                width=twoHandScales?std::clamp(width0*d/distance0,minWidth,maxWidth):width0;
                float k=width/width0;
                Quat turn=yawQuat(wrapAngle(horizontalAngle(v)-horizontalAngle(vec0)));
                table.orientation=normalized(multiply(turn,table0.orientation));
                // Only the horizontal offset follows the zoom: the table must not sink or rise.
                Vec3 offset=sub(table0.position,mid0);float height=offset.y;offset.y=0;
                offset=scale(rotate(turn,offset),k);offset.y=height+(mid.y-mid0.y);
                table.position={mid.x+offset.x,mid0.y+offset.y,mid.z+offset.z};
            }
        }
        return false;
    }
};
}
