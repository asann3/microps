#include <stdio.h>
#include <unistd.h>

#include "net.h"
#include "signal.h"
#include "test.h"
#include "util.h"

static volatile sig_atomic_t terminate;

static void on_signal(int s) {
    (void)s;
    debugf("signal received, s=%d", s);
    terminate = 1;
}

struct net_device *dummy_init(void);

int main(int argc, char *argv[]) {
    struct net_device *dev;

    signal(SIGINT, on_signal);
    if (net_init() == -1) {
        errorf("net_init() failure");
        return -1;
    }
    dev = dummy_init();
    if (!dev) {
        errorf("dummy_init() failure");
        return -1;
    }
    if (net_run() == -1) {
        errorf("net_run() failure");
        return -1;
    }
    while (!terminate) {
        if (net_device_output(dev, 0x0800, test_data, sizeof(test_data), NULL) == -1) {
            errorf("net_device_output() failure");
            return -1;
        }
        sleep(1);
    }
    net_shutdown();
    return 0;
}
