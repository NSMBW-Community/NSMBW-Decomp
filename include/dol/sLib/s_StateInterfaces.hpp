#pragma once

/// @addtogroup state
/// @{

/// @brief The interface for state IDs.
/// @details A state ID is made up of a name string and a unique number, where 0 denotes a null state.
/// Null states do not have any corresponding behaviour. They can be used, for example,
/// if a state holder needs to be initialized but the initial state ID is not known yet.
class sStateIDIf_c {
public:
    virtual ~sStateIDIf_c() {}

    virtual bool isNull() const = 0; ///< @pure Returns whether this is a null state.
    virtual bool isEqual(const sStateIDIf_c &other) const = 0; ///< @pure Returns whether both states have the same number.

    virtual bool operator==(const sStateIDIf_c &other) const = 0; ///< @pure Overloaded equality operator, using ::isEqual.
    virtual bool operator!=(const sStateIDIf_c &other) const = 0; ///< @pure Overloaded inequality operator, using ::isEqual.

    virtual bool isSameName(const char *name) const = 0; ///< @pure Returns whether this state ID is called @p name.
    virtual const char *name() const = 0; ///< @pure Returns the name of this state ID.
    virtual unsigned int number() const = 0; ///< @pure Returns the number of this state ID.
};

/// @brief The interface for state ID checkers.
/// @details [This class is not really used, but it seems to be intended for testing if a state is "normal", but not sure how this would be useful].
class sStateIDChkIf_c {
public:
    virtual ~sStateIDChkIf_c() {}
    virtual bool isNormalID(const sStateIDIf_c &id) const = 0;
};

/// @brief The interface for a state holder.
/// @details A state holder is linked to a state ID and can execute its corresponding behaviour.
class sStateIf_c {
public:
    virtual ~sStateIf_c() {}
    virtual const void initialize() = 0; ///< @pure Initializes the state.
    virtual const void execute() = 0; ///< @pure Executes the state.
    virtual const void finalize() = 0; ///< @pure Prepares the state for termination.
};

/// @brief The interface for state factories.
/// @details A state factory produces a state holder from a given state ID.
class sStateFctIf_c {
public:
    virtual ~sStateFctIf_c() {}
    virtual sStateIf_c *build(sStateIDIf_c const &id) = 0; ///< @pure Returns a new state with a given state ID.
    virtual void dispose(sStateIf_c *&id) = 0; ///< @pure Clears out the pointer to a state.
};

/// @brief The interface for state managers.
/// @details A state manager handles execution of and transitioning between state IDs.
class sStateMgrIf_c {
public:
    virtual ~sStateMgrIf_c() {}
    virtual void initializeState() = 0; ///< @pure Initializes the current state.
    virtual void executeState() = 0; ///< @pure Executes the current state.
    virtual void finalizeState() = 0; ///< @pure Prepares the current state for termination.
    virtual void changeState(const sStateIDIf_c &newStateID) = 0; ///< @pure Transitions to a new state ID.
    virtual void refreshState() = 0; ///< @pure Marks the current state to be executed again.
    virtual sStateIf_c *getState() const = 0; ///< @pure Gets the state holder.
    virtual sStateIDIf_c *getNewStateID() const = 0; ///< @pure Gets the next state ID.
    virtual sStateIDIf_c *getStateID() const = 0; ///< @pure Gets the current state ID.
    virtual sStateIDIf_c *getOldStateID() const = 0; ///< @pure Gets the previous state ID.
};

/// @}
