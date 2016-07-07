/**
 * @file osal_core.h
 * @brief OS Abstraction Layer
 * @author Florin Iucha <florin@signbit.net>
 * @copyright Apache License, Version 2.0
 */

/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file is part of serial_base library
 */

#ifndef __OSAL_CORE_H__
#define __OSAL_CORE_H__

#include <stdbool.h>
#include <stdint.h>

/** @addtogroup OSAL OS Abstraction layer
 *
 * @{
 *
 * @defgroup OSAL_Core Core
 *
 * @{
 */

/** Initialize the OS abstraction library
 */
int os_initialize(void);

/** Clean-up allocated resources; reverse initialize
 */
void os_cleanup(void);

/** Event invoked by the runtime when the user interrupts the application
 * (using Ctrl-C for instance)
 *
 */
void os_on_shutdownRequested(void);

/** Returns true if the user requested application termination
 */
bool os_interrupted(void);

/** Suspend current thread
 *
 * @param duration_ms is the time to sleep
 */
void os_sleep_ms(uint32_t duration_ms);

/** Suspends thread until the user interrupts the process
 *
 */
void os_waitForKeyboardInterrupt(void);

//void os_executeLater(uint32_t interval_ms, void (* func)(void* arg), void* arg);

/** @}
 *
 * @defgroup OSAL_Locks Locks
 *
 * @{
 */

/** Opaque lock object
 */
struct os_lock;

/** Creates and initializes a lock object
 *
 * @return the lock object
 */
struct os_lock* os_createLock(void);

/** Destroys a lock object
 *
 * @param lock is the lock object
 */
void os_destroyLock(struct os_lock* lock);

/** Waits until the lock object is unlocked, and locks it
 *
 * @param lock is the lock object
 */
void os_lock(struct os_lock* lock);

/** Unlocks a locked lock object
 *
 * @param lock is the lock object
 */
void os_unlock(struct os_lock* lock);

/** @}
 *
 * @defgroup OSAL_Condition Conditions
 *
 * @{
 */

/** Opaque condition object
 */
struct os_condition;

/** Creates and initializes a condition object
 *
 * @return the condition object
 */
struct os_condition* os_createCondition(void);

/** Destroys the condition object
 *
 * @param cond is the condition object
 */
void os_destroyCondition(struct os_condition* cond);

/** Resets a condition object to be unsignalled
 *
 * @param cond is the condition object
 */
void os_resetCondition(struct os_condition* cond);

/** Signals a condition object
 *
 * @param cond is the condition object
 * @param status is a user pointer, which will be passed to the waiting thread
 */
void os_signalCondition(struct os_condition* cond, void* status);

/** Blocks the current thread until the condition is signaled
 *
 * @param cond is the condition object
 * @param timeout_ms is the maximum amount of time to wait for the signal
 * @param status is a user pointer sent from the signaling thread
 * @return true if the condition was signaled, false if timeout occurred
 */
bool os_waitForCondition(struct os_condition* cond, uint32_t timeout_ms, void** status);

/** @}
 *
 * @}
 */

#endif // __OSAL_CORE_H__

