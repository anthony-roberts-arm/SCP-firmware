/*
 * Arm SCP/MCP Software
 * Copyright (c) 2017-2018, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Description:
 *     Timer HAL
 */

#ifndef MOD_TIMER_H
#define MOD_TIMER_H

#include <stdbool.h>
#include <stdint.h>
#include <fwk_id.h>
#include <fwk_module_idx.h>

/*!
 * \addtogroup GroupModules Modules
 * @{
 */

/*!
 * \defgroup GroupModuleTimer Timer HAL
 *
 * \brief Hardware Abstraction Layer for Timers.
 *
 * \details Provides functionality for setting timer events, tracking elapsed
 *      time, and synchronously delaying execution.
 *
 * @{
 */

/*!
 * \brief Timer module API indicies
 */
enum mod_timer_api_idx {
    /*! Timer API index */
    MOD_TIMER_API_IDX_TIMER,

    /*! Alarm API index */
    MOD_TIMER_API_IDX_ALARM,

    /*! Number of APIs */
    MOD_TIMER_API_COUNT,
};

/*!
 * \brief Timer API ID
 */
#define MOD_TIMER_API_ID_TIMER FWK_ID_API(FWK_MODULE_IDX_TIMER, \
                                          MOD_TIMER_API_IDX_TIMER)

/*!
 * \brief Alarm API ID
 */
#define MOD_TIMER_API_ID_ALARM FWK_ID_API(FWK_MODULE_IDX_TIMER, \
                                          MOD_TIMER_API_IDX_ALARM)

/*!
 * \brief Alarm type.
 */
enum mod_timer_alarm_type {
    /*! Alarm that will trigger once */
    MOD_TIMER_ALARM_TYPE_ONCE,

    /*! Alarm that will trigger at regular intervals */
    MOD_TIMER_ALARM_TYPE_PERIODIC,

    /*! Number of alarm types */
    MOD_TIMER_ALARM_TYPE_COUNT,
};

/*!
 * \brief Timer device descriptor
 */
struct mod_timer_dev_config {
    /*! Element identifier for the device's associated driver */
    fwk_id_t id;

    /*! Timer device IRQ number */
    unsigned int timer_irq;
};

/*!
 * \brief Timer driver interface.
 */
struct mod_timer_driver_api {
    /*! Name of the driver. */
    const char *name;

    /*! Enable timer events */
    int (*enable)(fwk_id_t dev_id);

    /*! Disable timer events */
    int (*disable)(fwk_id_t dev_id);

    /*! Set timer event for a specified timestamp */
    int (*set_timer)(fwk_id_t dev_id, uint64_t timestamp);

    /*! Get remaining time until the next pending timer event is due to fire */
    int (*get_timer)(fwk_id_t dev_id, uint64_t *timestamp);

    /*! Get current counter value */
    int (*get_counter)(fwk_id_t dev_id, uint64_t *value);

    /*! Get counter frequency */
    int (*get_frequency)(fwk_id_t dev_id, uint32_t *value);
};

/*!
 * \brief Timer HAL interface
 */
struct mod_timer_api {
    /*!
     * \brief Get the frequency of a given timer.
     *
     * \details Get the frequency in Hertz (Hz) that a timer is running at.
     *
     * \param dev_id Element identifier that identifies the timer device.
     * \param frequency Pointer to storage for the timer frequency.
     *
     * \retval FWK_SUCCESS Operation succeeded.
     * \retval FWK_E_PARAM The frequency pointer was invalid.
     * \retval One of the other specific error codes described by the framework.
     */
    int (*get_frequency)(fwk_id_t dev_id, uint32_t *frequency);

    /*!
     * \brief Get a counter timestamp that represents a given time period in
     *      microseconds (µS).
     *
     * \note The value of the resulting timestamp is only valid for the given
     *      device, since other timer devices may operate at different rates.
     *
     * \param dev_id Element identifier that identifies the timer device.
     * \param microseconds Period, in microseconds.
     * \param timestamp Pointer to storage for the resulting counter timestamp.
     *
     * \retval FWK_SUCCESS Operation succeeded.
     * \retval FWK_E_PARAM The timestamp pointer was invalid.
     * \retval One of the other specific error codes described by the framework.
     */
    int (*time_to_timestamp)(fwk_id_t dev_id,
                             uint32_t microseconds,
                             uint64_t *timestamp);

    /*!
     * \brief Get the current counter value of a given timer.
     *
     * \details Directly returns the counter value of the timer at the present
     *      moment.
     *
     * \param dev_id Element identifier that identifies the timer device.
     * \param counter Pointer to storage for the counter value.
     *
     * \retval FWK_SUCCESS Operation succeeded.
     * \retval FWK_E_PARAM The counter pointer was invalid.
     * \retval One of the other specific error codes described by the framework.
     */
    int (*get_counter)(fwk_id_t dev_id, uint64_t *counter);

    /*!
     * \brief Delay execution by synchronously waiting for a specified amount
     *      of time.
     *
     * \details Blocks the calling thread for the specified amount of time.
     *
     * \param dev_id Element identifier that identifies the timer device.
     * \param microseconds The amount of time, given in microseconds, to delay.
     *
     * \retval FWK_SUCCESS Operation succeeded.
     * \retval One of the other specific error codes described by the module.
     */
    int (*delay)(fwk_id_t dev_id, uint32_t microseconds);

    /*!
     * \brief Delay execution, waiting until a given condition is true or until
     *      a given timeout period has been exceeded, whichever occurs first.
     *
     * \note The calling thread is blocked until either condition has been met.
     *
     * \param dev_id Element identifier that identifies the timer device.
     * \param microseconds Maximum amount of time, in microseconds, to wait for
     *      the given condition to be met.
     * \param cond Pointer to the function that evaluates the condition and
     *      which returns a boolean value indicating if it has been met or not.
     *      The condition function is called repeatedly until it returns true,
     *      or until the timeout period has elapsed.
     * \param data Pointer passed to the condition function when it is called.
     *
     * \retval FWK_SUCCESS The condition was met before the timeout period
     *      elapsed.
     * \retval FWK_E_TIMEOUT The timeout period elapsed before the condition was
     *      met.
     * \retval One of the other specific error codes described by the module.
     */
    int (*wait)(fwk_id_t dev_id,
                uint32_t microseconds,
                bool (*cond)(void*),
                void *data);

    /*!
     * \brief Get the time difference, expressed in timer ticks, between the
     *      current timer counter value and the given timestamp. This represents
     *      the remaining number of ticks until the given timestamp is reached.
     *
     * \note If the given timestamp is in the past then the remaining_ticks is
     *      set to zero.
     *
     * \param dev_id Element identifier that identifies the timer device.
     * \param timestamp Timestamp to compare to the current timer value.
     * \param remaining_ticks Pointer to storage for the remaining number of
     *      ticks before the timer value reaches the given timestamp.
     *
     * \retval FWK_SUCCESS Operation succeeded.
     * \retval FWK_E_PARAM The remaining_ticks pointer was invalid.
     * \retval One of the other specific error codes described by the module.
     *
     * \note remaining_ticks is also a timestamp.
     */
    int (*remaining)(fwk_id_t dev_id,
                     uint64_t timestamp,
                     uint64_t *remaining_ticks);
};

/*!
 * \brief Alarm interface
 */
struct mod_timer_alarm_api {
    /*!
     * \brief Start an alarm so it will trigger after a specified time.
     *
     * \details When an alarm is triggered, an event with identifier
     *      \p event_id will be sent to the entity that is bound to the alarm.
     *      The first word of the event's parameter will be set to \p param.
     *
     *     If the alarm is periodic, it will automatically be started again
     *     with the same time delay after it triggers.
     *
     *     An alarm can be started multiple times without being stopped. In this
     *     case, internally, the alarm will be stopped then started again with
     *     the new configuration.
     *
     * \param alarm_id Sub-element identifier of the alarm.
     * \param event_id Identifier of the event the caller is expecting.
     * \param milliseconds The time delay, given in milliseconds, until the
     *     alarm should trigger.
     * \param type \ref MOD_TIMER_ALARM_TYPE_ONCE or
     *     \ref MOD_TIMER_ALARM_TYPE_PERIODIC.
     * \param param Word-size parameter for the event.
     *
     * \pre \p alarm_id must be a valid sub-element alarm identifier that has
     *     previously been bound to.
     *
     * \retval FWK_SUCCESS The alarm was started.
     */
    int (*start)(fwk_id_t alarm_id,
                 unsigned int milliseconds,
                 enum mod_timer_alarm_type type,
                 fwk_id_t event_id,
                 uintptr_t param);

    /*!
     * \brief Stop a previously started alarm.
     *
     * \details Stop an alarm that was previously started. This will prevent the
     *     alarm from triggering. This does not undo the binding of the alarm
     *     and it can be started again afterwards.
     *
     *     Any pending alarm events associated with the alarm are not cancelled
     *      or removed when the alarm is stopped.
     *
     * \param alarm_id Sub-element identifier of the alarm item.
     *
     * \pre \p alarm_id must be a valid sub-element alarm identifier that has
     *     previously been bound to.
     *
     * \retval FWK_SUCCESS The alarm was stopped.
     * \retval FWK_E_STATE The alarm was already stopped.
     */
    int (*stop)(fwk_id_t alarm_id);
};

/*!
 * @}
 */

/*!
 * @}
 */

#endif /* MOD_TIMER_H */
