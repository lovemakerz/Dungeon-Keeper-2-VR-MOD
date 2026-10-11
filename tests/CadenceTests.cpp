#include "../src/common/Cadence.hpp"
#include <cassert>
#include <cstdio>
using namespace dk2vr;
int main(){Cadence c;c.sample(100);c.sample(110);c.sample(120);c.sample(170);c.sample(270);
assert(c.seen==4&&c.over22==2&&c.over50==1);assert(c.percentile(.5f)==10&&c.worst==100);
c.clearWindow();c.sample(280);assert(c.seen==1&&c.percentile(.95f)==10);
c.pause();c.sample(1000);assert(c.seen==1);for(int i=0;i<800;++i)c.sample(1010+i*10);
assert(c.stored==512&&c.percentile(.95f)==10);puts("PASS: source cadence, percentiles, bounded storage and focus pauses.");}
