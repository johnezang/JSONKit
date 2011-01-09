# JSONKit Changelog

## Version 1.2 2011/01/08

### Bug Fixes

*   When JSONKit attempted to parse and decode JSON that contained `{"key": value}` dictionaries that contained the same key more than once would likely result in a crash.  This was a serious bug.
*   Under some conditions, JSONKit could potentially leak memory.
*   There was an off by one error in the code that checked whether or not the parser was at the end of the UTF8 buffer.  This could result in JSONKit reading one by past the buffer bounds in some cases.

### Other Changes

*   Some of the methods were missing `NULL` pointer checks for some of their arguments.  This was fixed.  In generally, when JSONKit encounters invalid arguments, it throws a `NSInvalidArgumentException` exception.
*   Various other minor changes such as tightening up numeric literals with `UL` or `L` qualification, assertion check tweaks and additions, etc.
*   The README.md file was updated with additional information.

### Version 1.1

No change log information was kept for versions prior to 1.2.

