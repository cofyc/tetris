#include "ct_game.h"
#include "ct_block.h"
#include "ct_display.h"
#include "ct_debug.h"

int
ct_game_init()
{
    ct_display_block_new();
    return 0;
}

static int
ct_game_block_down()
{
    int status = ct_display_block_check(cur_b, cur_y + 1, cur_x);
    if (!status) {
        cur_y++;
        ct_display_block_move(cur_y, cur_x, cur_b);
    }
    return status;
}

static int
ct_game_block_left()
{
    int status = ct_display_block_check(cur_b, cur_y, cur_x - 1);
    if (!status) {
        cur_x--;
        ct_display_block_move(cur_y, cur_x, cur_b);
    }
    return status;
}

static int
ct_game_block_right()
{
    int status = ct_display_block_check(cur_b, cur_y, cur_x + 1);
    if (!status) {
        cur_x++;
        ct_display_block_move(cur_y, cur_x, cur_b);
    }
    return status;
}

static int
ct_game_block_change()
{
    int status =
        ct_display_block_check(ct_block_get(cur_type, (cur_shape + 1) % 4),
                               cur_y, cur_x);
    if (!status) {
        cur_shape++;
        cur_shape = cur_shape % 4;
        cur_b     = ct_block_get(cur_type, cur_shape);
        ct_display_block_move(cur_y, cur_x, cur_b);
    }
    return status;
}

void
ct_game_daemon()
{
    int status = ct_game_block_down();
    if (status) {
        if (!ct_display_block_set(cur_y, cur_x, cur_b)) {
            ct_display_block_new();
        } else {
            printf("You lose.\n");
            ct_display_end();
            exit(0);
        }
    }
}

/**
 * gettime returns the current monotonic time in microseconds.
 */
static uint64_t
gettime()
{
    uint64_t t;
#if !defined(__APPLE__) || defined(AVAILABLE_MAC_OS_X_VERSION_10_12_AND_LATER)
    struct timespec ti;
    clock_gettime(CLOCK_MONOTONIC, &ti);
    t  = (uint64_t)ti.tv_sec * 1000;
    t += ti.tv_nsec / 1000 / 1000;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    t  = (uint64_t)tv.tv_sec * 1000;
    t += tv.tv_usec / 1000;
#endif
    return t;
}

int
ct_game_run()
{
#ifdef __linux__
#include <sys/epoll.h>
    int efd = epoll_create(1);
    if (efd == -1) {
        fprintf(stderr, "error: %s\n", strerror(errno));
        exit(1);
    }

    // stdin
    struct epoll_event ev;
    ev.data.fd = 0;
    ev.events  = EPOLLIN;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, 0, &ev) != 0) {
        close(efd);
        fprintf(stderr, "error: %s\n", strerror(errno));
        exit(1);
    }

    #define INTERVAL_MS 400

    struct epoll_event events[1] = {0};
    int start                    = gettime();
    int next                     = start + INTERVAL_MS;
    while (true) {
        int now = gettime();
        if (now >= next) {
            ct_game_daemon();
            next = now + INTERVAL_MS;
        }
        int timeout = next - now;
        int n       = epoll_wait(efd, events,
                                 sizeof(events) / sizeof(struct epoll_event),
                                 timeout);
        if (n < 0) {
            close(efd);
            fprintf(stderr, "error: %s\n", strerror(errno));
            exit(1);
        }
        if (n == 0) {
            continue;
        }
        // process keyboard events
        assert(n == 1);
        struct epoll_event *e = &events[0];
        assert(e->data.fd == 0);
        int c = getch();
        switch (c) {
        case 'h':
            ct_game_block_left();
            break;
        case 'j':
            ct_game_block_down();
            ct_game_block_down();
            break;
        case 'k':
            ct_game_block_change();
            break;
        case 'l':
            ct_game_block_right();
            break;
        case 'q':
            ct_display_end();
            exit(0);
            break;
        case '\x20':
            while (!ct_game_block_down());
            break;
        default:
            ct_debug_log("keystroke: %d", c);
            break;
        }
    }

#else

    struct itimerval tout_val, ovalue;
    tout_val.it_interval.tv_usec = 400 * 1000;
    tout_val.it_interval.tv_sec  = 0;
    tout_val.it_value.tv_usec    = 400 * 1000;
    tout_val.it_value.tv_sec     = 0;
    if (setitimer(ITIMER_REAL, &tout_val, &ovalue) < 0)
        error("setitimer error");

    if (signal(SIGALRM, ct_game_daemon) == SIG_ERR)
        error("signal error");

    for (;;) {
        int c = getch();
        switch (c) {
        case 'h':
            ct_game_block_left();
            break;
        case 'j':
            ct_game_block_down();
            ct_game_block_down();
            break;
        case 'k':
            ct_game_block_change();
            break;
        case 'l':
            ct_game_block_right();
            break;
        case 'q':
            ct_display_end();
            exit(0);
            break;
        case '\x20':
            while (!ct_game_block_down());
            break;
        default:
            ct_debug_log("keystroke: %d", c);
            break;
        }
    }
#endif

    return 0;
}
