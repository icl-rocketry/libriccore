
#include <libriccore/riccorelogging.h>


int main() {
    RicCoreLogging::log<RicCoreLoggingConfig::LOGGERS::SYS>("test log");
}