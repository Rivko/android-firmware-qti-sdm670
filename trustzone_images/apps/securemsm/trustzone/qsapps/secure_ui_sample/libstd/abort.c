
extern void qsee_err_fatal(void);

void abort(void)
{
  qsee_err_fatal();
  while(1) {}
}
