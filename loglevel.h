/* loglevel.h - routines to modify kernel console loglevel
 *
 * Released under GPL v2.
 * (c) 2007 Tim Dijkstra
 */

inline void open_printk(void);
inline int get_kernel_console_loglevel(void);
inline void set_kernel_console_loglevel(int level);
inline void close_printk(void);
