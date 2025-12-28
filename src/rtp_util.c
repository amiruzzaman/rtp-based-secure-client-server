#include "rtp_util.h"
#include <assert.h>

struct rtp_str_view rtp_str_view_create(size_t len, char view[len]) {
    return (struct rtp_str_view){.len = len, .view = view};
}

struct rtp_str_view rtp_str_view_substr_pre(const struct rtp_str_view* view,
        size_t prefix_cutoff) {
    assert(view != nullptr);
    if(view->len <= prefix_cutoff) {
        return (struct rtp_str_view){.len = 0, .view = nullptr};
    }
    return (struct rtp_str_view){.len = view->len - prefix_cutoff,
        .view = view->view + prefix_cutoff};
}

struct rtp_str_view rtp_str_view_substr_post(const struct rtp_str_view* view,
        size_t postfix_cutoff) {
    assert(view != nullptr);
    if(view->len <= postfix_cutoff) {
        return (struct rtp_str_view){.len = 0, .view = nullptr};
    }
    return (struct rtp_str_view){.len = view->len - postfix_cutoff,
        .view = view->view};

}

size_t rtp_str_find(const struct rtp_str_view* target, struct rtp_str_view pat) {
    assert(target != nullptr);
    if(pat.len > target->len) {
        return RTP_STR_NPOS;
    }

    for(size_t idx = 0; idx < target->len - pat.len; ++idx) {
        for(size_t off = 0; off < pat.len; ++off) {
            if(target->view[idx + off] != pat.view[off]) {
                goto next_idx_loop;
            }
        }
        return idx;
next_idx_loop:
    }
    return RTP_STR_NPOS;
}
