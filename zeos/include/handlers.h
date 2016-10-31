void keyboard_handler();
void system_call_handler();
void clock_handler();

void task_switch(union task_union*t);
int ret_from_fork();
