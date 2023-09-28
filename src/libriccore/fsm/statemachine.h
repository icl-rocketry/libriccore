/**
 * @file statemachine.h
 * @author Kiran de Silva
 * @brief Statemachine object implementing state transitions
 * @version 0.1
 * @date 2023-01-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <memory>
#include <cstdint>

#include "state.h"

template <typename SYSTEM_FLAG_T>
class StateMachine
{

public:
  /**
   * @brief Construct the state machine with the intial state object
   *
   * @param initialState
   */
  StateMachine() : currState(nullptr){};

  /**
   * @brief Initalize state machine with inital state pointer
   *
   * @param initialState
   */
  void initalize(std::unique_ptr<State<SYSTEM_FLAG_T>> initialState)
  {
    changeState(std::move(initialState));
  }

  /**
   * @brief Update hook for the statemachine. Calls update on the individual states. If the state
   * returns nullptr or std::unique_ptr<State>(nullptr), the statemachine will keep looping the state. If the state
   * returns a non null std::unique_ptr, the statemachine will call change state to change the current state.
   *
   */
  void update()
  {
    std::unique_ptr<State<SYSTEM_FLAG_T>> returnedState = currState->update();

    if (returnedState)
    {
      changeState(std::move(returnedState));
    }
  };

  /**
   * @brief Method forces statemachine to change state. Destructor is called on currState before being
   * replaced by the new state. Remeber to use std::make_unique when calling this
   *
   * @param newStatePtr
   */

  void changeState(std::unique_ptr<State<SYSTEM_FLAG_T>> newState)
  {
    if (currState)
    { // call exit only if currState is not null
      currState->exit();
    }
    currState = std::move(newState);
    currState->initialize();
  };

  SYSTEM_FLAG_T getCurrentStateID()
  {
    return currState->getID();
  }

private:
  std::unique_ptr<State<SYSTEM_FLAG_T>> currState;
};
