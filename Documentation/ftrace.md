### ftrace

#### command line

`/sys/kernel/debug/tracing`

* available tracers

  ```
  cat available_tracers
  ```

* select tracer, e.g.,

  ```
  echo -n function > current_tracer
  ```

* show trace

  ```
  cat trace
  ```

* enable/disable tracing

  ```
  echo -n 1(0) > tracing_on
  ```

* trace marker to sync kernel and application

  ```
  echo -n hello world > trace_marker
  ```

* set buffer size

  ```
  echo -n 50 > buffer_size_kb
  ```



#### function

* `trace_printk`: similar to `printk`, but much faster

* `tracing_on`: enable tracing

* `tracing_off`: disable tracing

Application Layer Sample code
```
int trace_fd = -1;
int marker_fd = -1;

int main(int argc, char **argv)
{
  char *debugfs;
  char path[256];
  [...]

  debugfs = find_debugfs();
  if (debugfs) {
    strcpy(path, debugfs);  /* BEWARE buffer overflow */
    strcat(path,"/tracing/tracing_on");
    trace_fd = open(path, O_WRONLY);
    if (trace_fd >= 0)
      write(trace_fd, "1", 1);

    strcpy(path, debugfs);
    strcat(path,"/tracing/trace_marker");
    marker_fd = open(path, O_WRONLY);


    if (marker_fd >= 0)
      write(marker_fd, "In critical area\n", 17);

    if (critical_function() < 0) {
      /* we failed! */
      if (trace_fd >= 0)
        write(trace_fd, "0", 1);

    req.tv_sec = 0;
    req.tv_nsec = 1000;
    write(marker_fd, "before nano\n", 12);
    nanosleep(&req, NULL);
    write(marker_fd, "after nano\n", 11);
    write(trace_fd, "0", 1);

```
`find_debugfs()` defined [here](https://lwn.net/Articles/366800/)


#### Tutorials
* [Debugging the kernel using Ftrace - part 1](https://lwn.net/Articles/365835/)
* [Debugging the kernel using Ftrace - part 2](https://lwn.net/Articles/366796/)
