#ifndef NW4R_EF_DRAW_DIRECTIONAL_STRATEGY_H
#define NW4R_EF_DRAW_DIRECTIONAL_STRATEGY_H
#include <nw4r/types_nw4r.h>
#include <nw4r/ef/drawstrategy/ef_drawstrategyimpl.h>

namespace nw4r
{
    namespace ef
    {
        class DrawDirectionalStrategy : public DrawStrategyImpl
        {
        public:
            DrawDirectionalStrategy();
            virtual UNKTYPE GetCalcAheadFunc(ParticleManager *); // at 0x18

            virtual void Draw(const DrawInfo &, ParticleManager *);
        };
    }
}

#endif
