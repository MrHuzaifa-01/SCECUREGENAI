/*
 * Copyright (c) 2009 Mark Heily <mark@heily.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <assert.h>
#include <stdio.h>

#include "private.h"

extern const struct filter evfilt_read;
extern const struct filter evfilt_write;
extern const struct filter evfilt_signal;
extern const struct filter evfilt_vnode;
extern const struct filter evfilt_proc;
extern const struct filter evfilt_timer;
extern const struct filter evfilt_user;
extern const struct filter evfilt_libkqueue;

void
filter_init_all(void)
{
#define FILTER_INIT(filt) do { \
    if (filt.libkqueue_init) \
        filt.libkqueue_init(); \
} while (0)

    FILTER_INIT(evfilt_read);
    FILTER_INIT(evfilt_write);
    FILTER_INIT(evfilt_signal);
    FILTER_INIT(evfilt_vnode);
    FILTER_INIT(evfilt_proc);
    FILTER_INIT(evfilt_timer);
    FILTER_INIT(evfilt_user);
    FILTER_INIT(evfilt_libkqueue);
}

#ifndef _WIN32
void
filter_fork_all(void)
{
#define FILTER_FORK(filt) do { \
    if (filt.libkqueue_fork) \
        filt.libkqueue_fork(); \
} while (0)

    FILTER_FORK(evfilt_read);
    FILTER_FORK(evfilt_write);
    FILTER_FORK(evfilt_signal);
    FILTER_FORK(evfilt_vnode);
    FILTER_FORK(evfilt_proc);
    FILTER_FORK(evfilt_timer);
    FILTER_FORK(evfilt_user);
    FILTER_FORK(evfilt_libkqueue);
}
#endif

void
filter_free_all(void)
{
#define FILTER_FREE(filt) do { \
    if (filt.libkqueue_free) \
        filt.libkqueue_free(); \
} while (0)

    FILTER_FREE(evfilt_read);
    FILTER_FREE(evfilt_write);
    FILTER_FREE(evfilt_signal);
    FILTER_FREE(evfilt_vnode);
    FILTER_FREE(evfilt_proc);
    FILTER_FREE(evfilt_timer);
    FILTER_FREE(evfilt_user);
    FILTER_FREE(evfilt_libkqueue);
}

static int
filter_register(struct kqueue *kq, const struct filter *src)
{
    struct filter *dst;
    unsigned int filt;
    int rv = 0;

    /*
     * This filter is not implemented, see EVFILT_NOTIMPL.
     */
    if (src->kf_id == 0) return (0);

    filt = (-1 * src->kf_id) - 1; /* flip sign, and convert to array offset */
    if (filt >= EVFILT_SYSCOUNT)
        return (-1);

    dst = &kq->kq_filt[filt];
    memcpy(dst, src, sizeof(*src));
    dst->kf_kqueue = kq;
    RB_INIT(&dst->kf_index);

    if (src->kf_id == 0) {
        dbg_puts("filter is not implemented");
        return (0);
    }

    assert(src->kf_copyout);
    assert(src->kn_create);
    assert(src->kn_modify);
    assert(src->kn_delete);
    assert(src->kn_enable);
    assert(src->kn_disable);

    /* Perform (optional) per-filter initialization */
    if (src->kf_init != NULL) {
        rv = src->kf_init(dst);
        if (rv < 0) {
            dbg_puts("filter failed to initialize");
            dst->kf_id = 0;
            return (-1);
        }
    }

    /* FIXME: should totally remove const from src */
    if ((kqops.filter_init != NULL) && (kqops.filter_init(kq, dst) < 0))
        return (-1);

    return (0);
}

int
filter_register_all(struct kqueue *kq)
{
    int rv;

    rv = 0;
    rv += filter_register(kq, &evfilt_read);
    rv += filter_register(kq, &evfilt_write);
    rv += filter_register(kq, &evfilt_signal);
    rv += filter_register(kq, &evfilt_vnode);
//    rv += filter_register(kq, &evfilt_proc);
    rv += filter_register(kq, &evfilt_timer);
    rv += filter_register(kq, &evfilt_user);
    rv += filter_register(kq, &evfilt_libkqueue);
    if (rv != 0) {
        filter_unregister_all(kq);
        return (-1);
    } else {
        dbg_puts("complete");
        return (0);
    }
}

void
filter_unregister_all(struct kqueue *kq)
{
    int i;

    for (i = 0; i < NUM_ELEMENTS(kq->kq_filt); i++) {
        struct filter *filt = &kq->kq_filt[i];

        if (filt->kf_id == 0)
            continue;

        if (filt->kf_destroy != NULL)
            filt->kf_destroy(&kq->kq_filt[i]);

        knote_delete_all(filt);

        if (kqops.filter_free != NULL)
            kqops.filter_free(kq, filt);

    }
    memset(&kq->kq_filt[0], 0, sizeof(kq->kq_filt));
}

/** Lookup filters in the array of filters registered for kq
 *
 * @param[out] filt    the specified ID resolves to.
 * @param[in] kq       to lookup the filter in.
 * @param[in] id       of the filter to lookup.
 * @return
 *    - 0 on success.
 *    - -1 on failure (filter not implemented).
 */
int
filter_lookup(struct filter **filt, struct kqueue *kq, short id)
{
    if (~id < 0 || ~id >= EVFILT_SYSCOUNT) {
        dbg_printf("filt=%d inv_filt=%d - invalid id", id, (~id));
        errno = EINVAL;
        *filt = NULL;
        return (-1);
    }
    *filt = &kq->kq_filt[~id];
    if ((*filt)->kf_copyout == NULL) {
        dbg_printf("filt=%d - filt_name=%s not implemented", id, filter_name(id));
        errno = ENOSYS;
        *filt = NULL;
        return (-1);
    }

    return (0);
}

const char *
filter_name(short filt)
{
    int id;
    const char *fname[EVFILT_SYSCOUNT] = {
        [~EVFILT_READ] = "EVFILT_READ",
        [~EVFILT_WRITE] = "EVFILT_WRITE",
        [~EVFILT_AIO] = "EVFILT_AIO",
        [~EVFILT_VNODE] = "EVFILT_VNODE",
        [~EVFILT_PROC] = "EVFILT_PROC",
        [~EVFILT_SIGNAL] = "EVFILT_SIGNAL",
        [~EVFILT_TIMER] = "EVFILT_TIMER",
#ifdef EVFILT_NETDEV
        [~EVFILT_NETDEV] = "EVFILT_NETDEV",
#endif
        [~EVFILT_FS] = "EVFILT_FS",
#ifdef EVFILT_LIO
        [~EVFILT_LIO] = "EVFILT_LIO",
#endif
        [~EVFILT_USER] = "EVFILT_USER"
    };

    id = ~filt;
    if (id < 0 || id >= NUM_ELEMENTS(fname))
        return "EVFILT_INVALID";
    else
        return fname[id];
}
