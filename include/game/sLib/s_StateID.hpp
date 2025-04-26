#pragma once
#include <game/sLib/s_StateInterfaces.hpp>

/// @brief A generic implementation of a state ID.
/// @details It simply contains a name string and a unique number.
/// @ingroup state
class sStateID_c : public sStateIDIf_c {
public:
    class NumberMemo_c {
    public:
        NumberMemo_c() : curr(0) {}
        unsigned int get() {
            curr++;
            return curr;
        }
        unsigned int curr;
    };

    sStateID_c(const char *name);
    virtual ~sStateID_c();

    virtual bool isNull() const;
    virtual bool isEqual(const sStateIDIf_c &other) const;

    virtual int operator==(const sStateIDIf_c &other) const;
    virtual int operator!=(const sStateIDIf_c &other) const;

    virtual bool isSameName(const char *name) const;
    virtual const char *name() const;
    virtual unsigned int number() const;

protected:
    const char *mpName; ///< The name of this state ID.
    unsigned int mNumber; ///< The number of this state ID.

    static NumberMemo_c sm_numberMemo; ///< Used to give each state a unique number.
};

namespace sStateID {

    /// @ingroup state
    extern sStateID_c null; ///< A null state instance.
}
