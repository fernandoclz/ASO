#ifndef DAEMON_SYSTEM_V_H

#define DAEMON_SYSTEM_V_H

void close_fd();
void reset_signals();
void reset_mask();
void delete_env();
void double_fork();
int c_daemon();
void demonizar();

#endif