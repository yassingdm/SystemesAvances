/* --- Generated the 23/11/2025 at 17:56 --- */
/* --- heptagon compiler, version 1.05.00 (compiled mon. sep. 15 11:46:4 CET 2025) --- */
/* --- Command line: /Users/omarvatchagaev/.opam/default/bin/heptc -target c ../scheduler_types.ept --- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "scheduler_types_types.h"

Scheduler_types__task_state Scheduler_types__task_state_of_string(char* s) {
  if ((strcmp(s, "Running")==0)) {
    return Scheduler_types__Running;
  };
  if ((strcmp(s, "Ready")==0)) {
    return Scheduler_types__Ready;
  };
  if ((strcmp(s, "Waiting")==0)) {
    return Scheduler_types__Waiting;
  };
}

char* string_of_Scheduler_types__task_state(Scheduler_types__task_state x,
                                            char* buf) {
  switch (x) {
    case Scheduler_types__Running:
      strcpy(buf, "Running");
      break;
    case Scheduler_types__Ready:
      strcpy(buf, "Ready");
      break;
    case Scheduler_types__Waiting:
      strcpy(buf, "Waiting");
      break;
    default:
      break;
  };
  return buf;
}

