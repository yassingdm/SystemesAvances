/* --- Generated the 19/11/2025 at 19:0 --- */
/* --- heptagon compiler, version 1.05.00 (compiled tue. sep. 23 21:17:51 CET 2025) --- */
/* --- Command line: /home/yassi/.opam/4.14.1/bin/heptc -target c scheduler_data.ept externc.epi scheduler.ept --- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "scheduler.h"

void Scheduler__complete_step(Scheduler_data__task_status tsi, int te,
                              int tid, Scheduler__complete_out* _out) {
  
  Scheduler_data__task_status v;
  v.status = Scheduler_data__Waiting;
  v.current_proc = Scheduler_data__nproc;
  v.current_deadline = tsi.current_deadline;
  v.left = 0;
  if (te) {
    _out->o = v;
  } else {
    _out->o = tsi;
  };;
}

void Scheduler__check_deadline_step(int current_date,
                                    Scheduler_data__task_status tsi, int tid,
                                    Scheduler__check_deadline_out* _out) {
  Externc__deadline_miss_log_out Externc__deadline_miss_log_out_st;
  
  Scheduler_data__task_status v_2;
  int v_1;
  int v;
  int c;
  v_1 = (tsi.current_deadline==current_date);
  v = (tsi.status==Scheduler_data__Ready);
  c = (v&&v_1);
  if (c) {
    Externc__deadline_miss_log_step(current_date, tid,
                                    &Externc__deadline_miss_log_out_st);
  };
  v_2 = tsi;
  v_2.status = Scheduler_data__Waiting;
  if (c) {
    _out->tso = v_2;
  } else {
    _out->tso = tsi;
  };;
}

void Scheduler__start_inst_step(int current_date,
                                Scheduler_data__task_status tsi,
                                Scheduler_data__task_attributes ta,
                                Scheduler__start_inst_out* _out) {
  
  Scheduler_data__task_status v_5;
  int v_4;
  int v_3;
  int v;
  int c;
  v = (current_date-ta.first_start);
  v_3 = (v%ta.period);
  c = (v_3==0);
  if (c) {
    v_4 = (current_date+ta.deadline);
    v_5.status = Scheduler_data__Ready;
    v_5.current_proc = Scheduler_data__nproc;
    v_5.current_deadline = v_4;
    v_5.left = ta.capacity;
    _out->tso = v_5;
  } else {
    _out->tso = tsi;
  };;
}

void Scheduler__update_aux_step(int sel, int tid, int proc_id,
                                int proc_acc_in,
                                Scheduler__update_aux_out* _out) {
  
  int v_7;
  int v_6;
  int v;
  v_6 = (proc_acc_in==Scheduler_data__nproc);
  v = (sel==tid);
  v_7 = (v&&v_6);
  if (v_7) {
    _out->proc_acc_out = proc_id;
  } else {
    _out->proc_acc_out = proc_acc_in;
  };;
}

void Scheduler__update_selected_step(Scheduler_data__task_status ts,
                                     int selected[2], int tid,
                                     Scheduler__update_selected_out* _out) {
  Scheduler__update_aux_out Scheduler__update_aux_out_st;
  
  Scheduler_data__task_status v_10;
  Scheduler_data__task_status v_9;
  int v_8;
  int v[2];
  int proc_id;
  {
    int i_7;
    for (i_7 = 0; i_7 < Scheduler_data__nproc; ++i_7) {
      v[i_7] = tid;
    }
  };
  proc_id = 2;
  {
    int i;
    for (i = 0; i < Scheduler_data__nproc; ++i) {
      Scheduler__update_aux_step(selected[i], v[i], i, proc_id,
                                 &Scheduler__update_aux_out_st);
      proc_id = Scheduler__update_aux_out_st.proc_acc_out;
    }
  };
  v_8 = (proc_id==Scheduler_data__nproc);
  v_9 = ts;
  v_9.status = Scheduler_data__Running;
  v_10 = v_9;
  v_10.current_proc = proc_id;
  if (v_8) {
    _out->tso = ts;
  } else {
    _out->tso = v_10;
  };;
}

void Scheduler__slp_aux_step(Scheduler__select_acc sa, int proc,
                             Scheduler__aux_type acc,
                             Scheduler__slp_aux_out* _out) {
  
  Scheduler__aux_type v_11;
  int v;
  v = (acc.sel_period<sa.speriod);
  v_11.sel_proc = proc;
  v_11.sel_period = sa.speriod;
  if (v) {
    _out->o = v_11;
  } else {
    _out->o = acc;
  };;
}

void Scheduler__select_largest_period_step(Scheduler__select_acc acc[2],
                                           Scheduler__select_largest_period_out* _out) {
  Scheduler__slp_aux_out Scheduler__slp_aux_out_st;
  
  Scheduler__aux_type v_12;
  Scheduler__select_acc v;
  Scheduler__aux_type search_result;
  v = acc[0];
  v_12.sel_proc = 0;
  v_12.sel_period = v.speriod;
  search_result = v_12;
  {
    int i;
    for (i = 0; i < Scheduler_data__nproc; ++i) {
      Scheduler__slp_aux_step(acc[i], i, search_result,
                              &Scheduler__slp_aux_out_st);
      search_result = Scheduler__slp_aux_out_st.o;
    }
  };
  _out->lp = search_result.sel_proc;;
}

void Scheduler__select_aux_step(Scheduler_data__task_status ts,
                                Scheduler_data__task_attributes ta, int tid,
                                Scheduler__select_acc acc[2],
                                Scheduler__select_aux_out* _out) {
  Scheduler__select_largest_period_out Scheduler__select_largest_period_out_st;
  
  Scheduler__select_acc v_17[2];
  Scheduler__select_acc v_16[2];
  Scheduler__select_acc v_15;
  int v_14;
  Scheduler__select_acc v_13;
  int v;
  int lp;
  v = (ts.status==Scheduler_data__Ready);
  Scheduler__select_largest_period_step(acc,
                                        &Scheduler__select_largest_period_out_st);
  lp = Scheduler__select_largest_period_out_st.lp;
  v_15.tid = tid;
  v_15.speriod = ta.period;
  {
    int _1;
    for (_1 = 0; _1 < 2; ++_1) {
      v_16[_1] = acc[_1];
    }
  };
  if (((lp<Scheduler_data__nproc)&&(0<=lp))) {
    v_16[lp] = v_15;
  };
  v_13 = acc[between(lp, Scheduler_data__nproc)];
  v_14 = (ta.period<v_13.speriod);
  if (v_14) {
    {
      int _2;
      for (_2 = 0; _2 < 2; ++_2) {
        v_17[_2] = v_16[_2];
      }
    };
  } else {
    {
      int _3;
      for (_3 = 0; _3 < 2; ++_3) {
        v_17[_3] = acc[_3];
      }
    };
  };
  if (v) {
    {
      int _4;
      for (_4 = 0; _4 < 2; ++_4) {
        _out->acc_o[_4] = v_17[_4];
      }
    };
  } else {
    {
      int _5;
      for (_5 = 0; _5 < 2; ++_5) {
        _out->acc_o[_5] = acc[_5];
      }
    };
  };;
}

void Scheduler__select_tid_step(Scheduler__select_acc s,
                                Scheduler__select_tid_out* _out) {
  _out->o = s.tid;
}

void Scheduler__select_tasks_step(Scheduler_data__task_status ts[2],
                                  Scheduler__select_tasks_out* _out) {
  Scheduler__select_tid_out Scheduler__select_tid_out_st;
  Scheduler__select_aux_out Scheduler__select_aux_out_st;
  
  Scheduler__select_acc v[2];
  Scheduler__select_acc tmp[2];
  {
    int _6;
    for (_6 = 0; _6 < Scheduler_data__nproc; ++_6) {
      v[_6].speriod = 9999;
      v[_6].tid = 2;
    }
  };
  {
    int _7;
    for (_7 = 0; _7 < 2; ++_7) {
      tmp[_7] = v[_7];
    }
  };
  {
    int i_6;
    for (i_6 = 0; i_6 < Scheduler_data__ntasks; ++i_6) {
      Scheduler__select_aux_step(ts[i_6], Scheduler_data__tasks[i_6], i_6,
                                 tmp, &Scheduler__select_aux_out_st);
      {
        int _8;
        for (_8 = 0; _8 < 2; ++_8) {
          tmp[_8] = Scheduler__select_aux_out_st.acc_o[_8];
        }
      };
    }
  };
  {
    int i;
    for (i = 0; i < Scheduler_data__nproc; ++i) {
      Scheduler__select_tid_step(tmp[i], &Scheduler__select_tid_out_st);
      _out->selected[i] = Scheduler__select_tid_out_st.o;
    }
  };;
}

void Scheduler__rate_monotonic_mc_step(Scheduler_data__task_status ts[2],
                                       Scheduler__rate_monotonic_mc_out* _out) {
  Scheduler__select_tasks_out Scheduler__select_tasks_out_st;
  Scheduler__update_selected_out Scheduler__update_selected_out_st;
  
  int v[2][2];
  int selected[2];
  Scheduler__select_tasks_step(ts, &Scheduler__select_tasks_out_st);
  {
    int _9;
    for (_9 = 0; _9 < 2; ++_9) {
      selected[_9] = Scheduler__select_tasks_out_st.selected[_9];
    }
  };
  {
    int i_5;
    for (i_5 = 0; i_5 < Scheduler_data__ntasks; ++i_5) {
      {
        int _10;
        for (_10 = 0; _10 < 2; ++_10) {
          v[i_5][_10] = selected[_10];
        }
      };
    }
  };
  {
    int i;
    for (i = 0; i < Scheduler_data__ntasks; ++i) {
      Scheduler__update_selected_step(ts[i], v[i], i,
                                      &Scheduler__update_selected_out_st);
      _out->tso[i] = Scheduler__update_selected_out_st.tso;
    }
  };;
}

void Scheduler__extract_aux_step(Scheduler_data__task_status s, int tid,
                                 int acc[2], Scheduler__extract_aux_out* _out) {
  
  int v_18[2];
  int v;
  v = (s.status==Scheduler_data__Running);
  {
    int _11;
    for (_11 = 0; _11 < 2; ++_11) {
      v_18[_11] = acc[_11];
    }
  };
  if (((s.current_proc<Scheduler_data__nproc)&&(0<=s.current_proc))) {
    v_18[s.current_proc] = tid;
  };
  if (v) {
    {
      int _12;
      for (_12 = 0; _12 < 2; ++_12) {
        _out->acc_o[_12] = v_18[_12];
      }
    };
  } else {
    {
      int _13;
      for (_13 = 0; _13 < 2; ++_13) {
        _out->acc_o[_13] = acc[_13];
      }
    };
  };;
}

void Scheduler__extract_proc_step(Scheduler_data__task_status ts[2],
                                  Scheduler__extract_proc_out* _out) {
  Scheduler__extract_aux_out Scheduler__extract_aux_out_st;
  
  int base[2];
  {
    int _14;
    for (_14 = 0; _14 < Scheduler_data__nproc; ++_14) {
      base[_14] = 2;
    }
  };
  {
    int _15;
    for (_15 = 0; _15 < 2; ++_15) {
      _out->run[_15] = base[_15];
    }
  };
  {
    int i;
    for (i = 0; i < Scheduler_data__ntasks; ++i) {
      Scheduler__extract_aux_step(ts[i], i, _out->run,
                                  &Scheduler__extract_aux_out_st);
      {
        int _16;
        for (_16 = 0; _16 < 2; ++_16) {
          _out->run[_16] = Scheduler__extract_aux_out_st.acc_o[_16];
        }
      };
    }
  };;
}

void Scheduler__scheduler_reset(Scheduler__scheduler_mem* self) {
  {
    int i_4;
    for (i_4 = 0; i_4 < Scheduler_data__ntasks; ++i_4) {
    }
  };
  {
    int i_1;
    for (i_1 = 0; i_1 < Scheduler_data__ntasks; ++i_1) {
    }
  };
  {
    int i;
    for (i = 0; i < Scheduler_data__ntasks; ++i) {
    }
  };
  {
    int _20;
    for (_20 = 0; _20 < 2; ++_20) {
      self->ss.tasks[_20].left = 0;
      self->ss.tasks[_20].current_deadline = 0;
      self->ss.tasks[_20].current_proc = 2;
      self->ss.tasks[_20].status = Scheduler_data__Waiting;
    }
  };
  self->ss.current_date = -1;
}

void Scheduler__scheduler_step(int task_end[2],
                               Scheduler__scheduler_out* _out,
                               Scheduler__scheduler_mem* self) {
  Scheduler__start_inst_out Scheduler__start_inst_out_st;
  Scheduler__complete_out Scheduler__complete_out_st;
  Scheduler__check_deadline_out Scheduler__check_deadline_out_st;
  Scheduler__extract_proc_out Scheduler__extract_proc_out_st;
  Scheduler__rate_monotonic_mc_out Scheduler__rate_monotonic_mc_out_st;
  
  int v_20[2];
  int v_19[2];
  Scheduler_data__scheduler_state v;
  int new_date;
  Scheduler_data__task_status tmp1[2];
  Scheduler_data__task_status tmp2[2];
  Scheduler_data__task_status tmp3[2];
  Scheduler_data__task_status tmp4[2];
  {
    int i_4;
    for (i_4 = 0; i_4 < Scheduler_data__ntasks; ++i_4) {
      Scheduler__complete_step(self->ss.tasks[i_4], task_end[i_4], i_4,
                               &Scheduler__complete_out_st);
      tmp1[i_4] = Scheduler__complete_out_st.o;
    }
  };
  new_date = (self->ss.current_date+1);
  {
    int i_3;
    for (i_3 = 0; i_3 < Scheduler_data__ntasks; ++i_3) {
      v_20[i_3] = new_date;
    }
  };
  {
    int i_2;
    for (i_2 = 0; i_2 < Scheduler_data__ntasks; ++i_2) {
      v_19[i_2] = new_date;
    }
  };
  {
    int i_1;
    for (i_1 = 0; i_1 < Scheduler_data__ntasks; ++i_1) {
      Scheduler__check_deadline_step(v_19[i_1], tmp1[i_1], i_1,
                                     &Scheduler__check_deadline_out_st);
      tmp2[i_1] = Scheduler__check_deadline_out_st.tso;
    }
  };
  {
    int i;
    for (i = 0; i < Scheduler_data__ntasks; ++i) {
      Scheduler__start_inst_step(v_20[i], tmp2[i], Scheduler_data__tasks[i],
                                 &Scheduler__start_inst_out_st);
      tmp3[i] = Scheduler__start_inst_out_st.tso;
    }
  };
  Scheduler__rate_monotonic_mc_step(tmp3,
                                    &Scheduler__rate_monotonic_mc_out_st);
  {
    int _17;
    for (_17 = 0; _17 < 2; ++_17) {
      tmp4[_17] = Scheduler__rate_monotonic_mc_out_st.tso[_17];
    }
  };
  Scheduler__extract_proc_step(tmp4, &Scheduler__extract_proc_out_st);
  {
    int _18;
    for (_18 = 0; _18 < 2; ++_18) {
      _out->task_run[_18] = Scheduler__extract_proc_out_st.run[_18];
    }
  };
  v.current_date = new_date;
  {
    int _19;
    for (_19 = 0; _19 < 2; ++_19) {
      v.tasks[_19] = tmp4[_19];
    }
  };
  self->ss = v;;
}

