#ifndef NW4R_EF_DRAW_STRIPE_STRATEGY_H
#define NW4R_EF_DRAW_STRIPE_STRATEGY_H
#include <nw4r/types_nw4r.h>
#include <nw4r/ef/drawstrategy/ef_drawstrategyimpl.h>

namespace nw4r
{
    namespace ef
    {
        class DrawStripeStrategy : public DrawStrategyImpl
        {
        public:
            DrawStripeStrategy();
            virtual UNKTYPE GetCalcAheadFunc(ParticleManager *); // at 0x18

            virtual UNKTYPE Draw(const DrawInfo &, ParticleManager *);
        };
    }
}

#endif
