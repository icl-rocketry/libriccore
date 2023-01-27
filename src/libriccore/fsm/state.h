#pragma once
/**
 * @file state.h
 * @author Kiran de Silva
 * @brief Base class for a state. Discrete initialzie and exit functions as order of exit and entry of states
 * needs to be carefully controleld to ensure we can guarantee only one state is present in the state machine
 * at a given time.
 * @version 0.1
 * @date 2023-01-27
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <cstdint>
#include <memory>

#include "systemstatus.h"
#include "systemflags_tweak.h"

#include "util/millis_stub.h"

class State
{

public:
  /**
   * @brief Constructor - requires system status object
   *
   * @param ID
   * @param systemstatus
   */
  State(SYSTEM_FLAGS ID, SystemStatus systemstatus) : stateID(ID),
                                                      _systemstatus(systemstatus){};

  virtual ~State(){};

  /**
   * @brief Initialize the state, records entry time
   *
   */
  virtual void initialize()
  {
    time_entered_state = millis();
    _systemstatus.newFlag(stateID, "state entered");
  };

  /**
   * @brief Update hook for state. If we want to loop the state, we return a nullptr, otherwise
   * return a new uniqueptr<state> for the desired new state
   *
   * @return std::unique_ptr<State>
   */
  virtual std::unique_ptr<State> update()
  {
    return std::unique_ptr<State>(nullptr);
  };

  /**
   * @brief Exit state transition, records exit and duration time
   *
   */
  virtual void exit()
  {
    time_exited_state = millis();
    time_duration_state = time_exited_state - time_entered_state;
    _systemstatus.deleteFlag(stateID, "state exited | state duration: " + std::to_string(time_duration_state));
  };

  /**
   * @brief Returns state id which is a system flag
   *
   * @return SYSTEM_FLAGS
   */
  SYSTEM_FLAGS getID()
  {
    return stateID;
  };

protected:
  const SYSTEM_FLAGS stateID;

  uint64_t time_entered_state;
  uint64_t time_exited_state;
  uint64_t time_duration_state;

  SystemStatus &_systemstatus;
};
