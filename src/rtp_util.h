#ifndef RTP_MOD_RTP_UTIL_H
#define RTP_MOD_RTP_UTIL_H

#include <stddef.h>
#include <stdint.h>

// sometimes i do miss C++'s constexpr
#define RTP_UTIL_BITS_TO_BYTES(bits) \
    (((bits) + 7) / 8)

#define RTP_UTIL_BYTES_TO_BITS(bytes) \
    ((bytes) * 8)

/**
 * @warn string not necessarily null-terminated.
 */
struct rtp_str_view {
    size_t len; 
    char *view;
};

constexpr size_t RTP_STR_NPOS = SIZE_MAX;

/**
 * @brief Creates a string view.
 * @warn Cut off the null-terminator if passing a string literal as the view:
 * @code{.c}
 * auto str = rtp_str_view_create(sizeof("Hello") - 1, "Hello");
 * @endcode
 */
struct rtp_str_view rtp_str_view_create(size_t len, char view[len]);

/**
 * @brief Cut off the prefix by `prefix_cutoff`.
 *
 * If `prefix_cutoff` is longer than `view->len`, returns a view whose `len`
 * is 0. Where `view` points to is unspecified (currently `nullptr`).
 */
struct rtp_str_view rtp_str_view_substr_pre(const struct rtp_str_view* view,
        size_t prefix_cutoff);

/**
 * @brief Cut off the suffix by `prefix_cutoff`.
 *
 * If `postfix_cutoff` is longer than `view->len`, returns a view whose `len`
 * is 0. Where `view` points to is unspecified (currently `nullptr`).
 */
struct rtp_str_view rtp_str_view_substr_post(const struct rtp_str_view* view,
        size_t postfix_cutoff);

/**
 * @brief Dumb O(mn) search. Find if `target` contains `pat`.
 * @warn No regex.
 * @return The index of the first character in the part of `target` that
 * matches `pat`. If none is found, return `RTP_STR_NPOS`.
 */
size_t rtp_str_find(const struct rtp_str_view* target, struct rtp_str_view pat);

#endif
