
#ifndef __WF_TIMER_H__
#define __WF_TIMER_H__

/* include */

/* macro */

/* type */
typedef struct
{
    wf_u32 start;
    wf_u32 expire;
    wf_u32 interval;
} wf_timer_t;

/* function declaration */

/**
 * Set a timer.
 *
 * This function is used to set a timer for a time sometime in the
 * future. The function wf_timer_expired() will evaluate to true after
 * the timer has expired.
 *
 * \param t A pointer to the timer
 * \param interval The interval before the timer expires.
 *
 */
wf_inline static void wf_timer_set (wf_timer_t *ptimer, wf_u32 intv_ms)
{
    ptimer->start = wf_os_api_timestamp();
    ptimer->interval = wf_os_api_msecs_to_timestamp(intv_ms);
    ptimer->expire = ptimer->start + ptimer->interval;
}

/**
 * Reset the timer with the same interval.
 *
 * This function resets the timer with the same interval that was
 * given to the wf_timer_set() function. The start point of the interval
 * is the exact time that the timer last expired. Therefore, this
 * function will cause the timer to be stable over time, unlike the
 * wf_timer_restart() function.
 *
 * \note Must not be executed before timer expired
 *
 * \param t A pointer to the timer.
 * \sa wf_timer_restart()
 */
wf_inline static void wf_timer_reset (wf_timer_t *ptimer)
{
    ptimer->start = ptimer->expire;
    ptimer->expire = ptimer->start + ptimer->interval;
}

/**
 * Restart the timer from the current point in time
 *
 * This function restarts a timer with the same interval that was
 * given to the wf_timer_set() function. The timer will start at the
 * current time.
 *
 * \note A periodic timer will drift if this function is used to reset
 * it. For preioric timers, use the wf_timer_reset() function instead.
 *
 * \param t A pointer to the timer.
 *
 * \sa wf_timer_reset()
 */
wf_inline static void wf_timer_restart (wf_timer_t *ptimer)
{
    ptimer->start = wf_os_api_timestamp();
    ptimer->expire = ptimer->start + ptimer->interval;
}

/**
 * modefiy the timer interval
 *
 * This function modefiy the timer interval with start point the same as before
 * timestamp.
 *
 * \param t A pointer to the timer.
 * \param interval The interval before the timer expires.
 *
 * \sa wf_timer_reset()
 */
wf_inline static void wf_timer_mod (wf_timer_t *ptimer, wf_u32 intv_ms)
{
    ptimer->interval = wf_os_api_msecs_to_timestamp(intv_ms);
    ptimer->expire = ptimer->start + ptimer->interval;
}

/**
 * Check if a timer has expired.
 *
 * This function tests if a timer has expired and returns true or
 * false depending on its status.
 *
 * \param t A pointer to the timer
 *
 * \return Non-zero if the timer has expired, zero otherwise.
 *
 */
wf_inline static wf_bool wf_timer_expired (wf_timer_t *ptimer)
{
    return (wf_s32)(ptimer->expire - wf_os_api_timestamp()) < 0;
}

/**
 * The time until the timer expires
 *
 * This function returns the time until the timer expires.
 *
 * \param t A pointer to the timer
 *
 * \return The time until the timer expires
 *
 */
wf_inline static wf_s32 wf_timer_remaining (wf_timer_t *ptimer)
{
    return (wf_s32)(ptimer->expire - wf_os_api_timestamp());
}

/**
 * The time that has elapsed since starting
 *
 * This function returns the time has elapsed since starting.
 *
 * \param t A pointer to the timer
 *
 * \return The time until the timer expires
 *
 */
wf_inline static wf_u32 wf_timer_elapsed (wf_timer_t *ptimer)
{
    return wf_os_api_timestamp_to_msecs(wf_os_api_timestamp() - ptimer->start);
}

int wf_timer_init (void);
int wf_timer_term (void);

#endif

