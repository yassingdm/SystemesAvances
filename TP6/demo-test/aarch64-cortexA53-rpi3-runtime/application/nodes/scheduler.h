/* --- Generated the 19/11/2025 at 18:43 --- */
/* --- heptagon compiler, version 1.05.00 (compiled tue. sep. 23 21:17:51 CET 2025) --- */
/* --- Command line: /home/yassi/.opam/4.14.1/bin/heptc -target c scheduler_data.ept externc.epi scheduler.ept --- */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "scheduler_types.h"
#include "externc.h"
#include "scheduler_data.h"
typedef struct Scheduler__complete_out {
  Scheduler_data__task_status o;
} Scheduler__complete_out;

void Scheduler__complete_step(Scheduler_data__task_status tsi, int te,
                              int tid, Scheduler__complete_out* _out);

typedef struct Scheduler__check_deadline_out {
  Scheduler_data__task_status tso;
} Scheduler__check_deadline_out;

void Scheduler__check_deadline_step(int current_date,
                                    Scheduler_data__task_status tsi, int tid,
                                    Scheduler__check_deadline_out* _out);

typedef struct Scheduler__start_inst_out {
  Scheduler_data__task_status tso;
} Scheduler__start_inst_out;

void Scheduler__start_inst_step(int current_date,
                                Scheduler_data__task_status tsi,
                                Scheduler_data__task_attributes ta,
                                Scheduler__start_inst_out* _out);

typedef struct Scheduler__update_aux_out {
  int proc_acc_out;
} Scheduler__update_aux_out;

void Scheduler__update_aux_step(int sel, int tid, int proc_id,
                                int proc_acc_in,
                                Scheduler__update_aux_out* _out);

typedef struct Scheduler__update_selected_out {
  Scheduler_data__task_status tso;
} Scheduler__update_selected_out;

void Scheduler__update_selected_step(Scheduler_data__task_status ts,
                                     int selected[2], int tid,
                                     Scheduler__update_selected_out* _out);

typedef struct Scheduler__slp_aux_out {
  Scheduler__aux_type o;
} Scheduler__slp_aux_out;

void Scheduler__slp_aux_step(Scheduler__select_acc sa, int proc,
                             Scheduler__aux_type acc,
                             Scheduler__slp_aux_out* _out);

typedef struct Scheduler__select_largest_period_out {
  int lp;
} Scheduler__select_largest_period_out;

void Scheduler__select_largest_period_step(Scheduler__select_acc acc[2],
                                           Scheduler__select_largest_period_out* _out);

typedef struct Scheduler__select_aux_out {
  Scheduler__select_acc acc_o[2];
} Scheduler__select_aux_out;

void Scheduler__select_aux_step(Scheduler_data__task_status ts,
                                Scheduler_data__task_attributes ta, int tid,
                                Scheduler__select_acc acc[2],
                                Scheduler__select_aux_out* _out);

typedef struct Scheduler__select_tid_out {
  int o;
} Scheduler__select_tid_out;

void Scheduler__select_tid_step(Scheduler__select_acc s,
                                Scheduler__select_tid_out* _out);

typedef struct Scheduler__select_tasks_out {
  int selected[2];
} Scheduler__select_tasks_out;

void Scheduler__select_tasks_step(Scheduler_data__task_status ts[2],
                                  Scheduler__select_tasks_out* _out);

typedef struct Scheduler__rate_monotonic_mc_out {
  Scheduler_data__task_status tso[2];
} Scheduler__rate_monotonic_mc_out;

void Scheduler__rate_monotonic_mc_step(Scheduler_data__task_status ts[2],
                                       Scheduler__rate_monotonic_mc_out* _out);

typedef struct Scheduler__extract_aux_out {
  int acc_o[2];
} Scheduler__extract_aux_out;

void Scheduler__extract_aux_step(Scheduler_data__task_status s, int tid,
                                 int acc[2], Scheduler__extract_aux_out* _out);

typedef struct Scheduler__extract_proc_out {
  int run[2];
} Scheduler__extract_proc_out;

void Scheduler__extract_proc_step(Scheduler_data__task_status ts[2],
                                  Scheduler__extract_proc_out* _out);

typedef struct Scheduler__scheduler_mem {
  Scheduler_data__scheduler_state ss;
} Scheduler__scheduler_mem;

typedef struct Scheduler__scheduler_out {
  int task_run[2];
} Scheduler__scheduler_out;

void Scheduler__scheduler_reset(Scheduler__scheduler_mem* self);

void Scheduler__scheduler_step(int task_end[2],
                               Scheduler__scheduler_out* _out,
                               Scheduler__scheduler_mem* self);

#endif // SCHEDULER_H
