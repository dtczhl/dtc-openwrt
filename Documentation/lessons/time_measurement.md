
1. `gettimeofday()` should never be used to measure time.  Use `clock_getime(CLOCK_MONITONIC, ...)` instead.

	refer to  [this](https://blog.habets.se/2010/09/gettimeofday-should-never-be-used-to-measure-time.html)