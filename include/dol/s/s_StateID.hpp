#pragma once

#include <dol/s/s_StateInterfaces.hpp>

/// @brief A generic implementation of a state ID.
/// @details It simply contains a name string and a unique number.
class sStateID_c : public sStateIDIf_c {
public:
    sStateID_c(const char *name);
    ~sStateID_c();

    virtual bool isNull() const;
    virtual bool isEqual(const sStateIDIf_c &other) const;

    virtual bool operator==(const sStateIDIf_c &other) const;
    virtual bool operator!=(const sStateIDIf_c &other) const;

    virtual bool isSameName(const char *name) const;
    virtual const char *name() const;
    virtual unsigned int number() const;

protected:
    const char *mpName; ///< The name of this state ID.
    unsigned int mNumber; ///< The number of this state ID.

    class NumberMemo_c {
    public:
        NumberMemo_c() : curr(0) {}
        unsigned int get() {
            curr++;
            return curr;
        }
        unsigned int curr;
    };
    static NumberMemo_c sm_numberMemo; ///< Used to give each state a unique number.
};

namespace sStateID {
    extern sStateID_c null; ///< A null state instance. @todo What is the purpose of null states?
}
