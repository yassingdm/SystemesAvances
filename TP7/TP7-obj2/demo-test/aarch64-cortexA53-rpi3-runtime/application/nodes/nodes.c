#include <librpi3/stdio.h>
#include <librpi3/svc.h>
#include <nodes.h>

void f_step(int*x) {
  static int s = 123 ; 
  char buf[128] ;
  uint32_t cpuid = get_cpuid() ;
  s = s + 50 ;
  *x = s ;
  snprintf(buf,127,"Core%d: f()->(x=%d)\n",cpuid,*x) ;
  console_puts(get_cpuid(),buf) ;
}
void g_step(int z_0, int*y) {
  char buf[128] ;
  uint32_t cpuid = get_cpuid() ;
  *y = z_0 + 20 ;
  snprintf(buf,127,"Core%d: f(z_0=%d)->(y=%d)\n",cpuid,z_0,*y) ;
  console_puts(get_cpuid(),buf) ;
}
void h_step(int x, int y, int* z_1) {
  char buf[128] ;
  uint32_t cpuid = get_cpuid() ;
  *z_1 = x + y ;
  snprintf(buf,127,"Core%d: h(x=%d,y=%d)->(z_1=%d)\n",cpuid,x,y,*z_1) ;
  console_puts(get_cpuid(),buf) ;
}
void m_step(int x, int* u) {
    char buf[128];
    uint32_t cpuid = get_cpuid();
    *u = x - 17;
    snprintf(buf, 127, "Core%d: m(x=%d)->(u=%d)\n", cpuid, x, *u);
    console_puts(get_cpuid(), buf);
}

void n_step(int u, int z) {
    char buf[128];
    uint32_t cpuid = get_cpuid();
    snprintf(buf, 127, "Core%d: n(u=%d, z=%d)\n", cpuid, u, z);
    console_puts(get_cpuid(), buf);
}
