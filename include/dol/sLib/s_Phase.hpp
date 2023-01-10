#pragma once

/// @brief A phase is a list of methods to be called in order.
class sPhase_c {
public:
    /// @brief Return value of a phase method and ::callMethod.
    /// @note Unofficial name.
    enum CHAIN_RESULT_e {
        WAIT, ///< Do not proceed to the next method in the phase.
        OK, ///< Proceed to the next method in the phase.
        DONE ///< The phase is done.
    };
    typedef CHAIN_RESULT_e (phaseMethod)(void *);

    /**
     * @brief Constructs a new phase with a given method list.
     * 
     * @param methodList The list of methods in the phase.
     * @param count The length of the method list.
     */
    sPhase_c(phaseMethod **methodList, int count);
    /**
     * @brief Executes the phase until the end is reached or a method returns CHAIN_RESULT_e::WAIT.
     * 
     * @param thisPtr A pointer to the owner.
     * @return CHAIN_RESULT_e::WAIT if the phase is not done yet, and CHAIN_RESULT_e::DONE if the phase is finished.
     */
    CHAIN_RESULT_e callMethod(void *thisPtr);

    phaseMethod **mpMethodList; ///< The method list.
    unsigned short mPhaseLength; ///< The length of the method list.
    unsigned short mCurrMethod; ///< The index of the method to execute.
};
