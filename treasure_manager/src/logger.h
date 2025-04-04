#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <time.h>

void log_event(const char *event);
void initialize_logger(const char *log_file);
void close_logger();
void log_action(const char *log_file, const char *action);

#endif // LOGGER_H