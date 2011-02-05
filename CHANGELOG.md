# JSONKit Changelog

## Version 1.3 2011/05/04

### New Features

*   Added the `JK_ENABLE_CF_TRANSFER_OWNERSHIP_CALLBACKS` pre-processor define flag.
    
    This is typically enabled by adding <span style="white-space: nowrap;">`-DJK_ENABLE_CF_TRANSFER_OWNERSHIP_CALLBACKS`</span> to the compilers command line arguments or in `Xcode.app` by adding `JK_ENABLE_CF_TRANSFER_OWNERSHIP_CALLBACKS` to a projects / targets `Pre-Processor Macros` settings.
    
    The `JK_ENABLE_CF_TRANSFER_OWNERSHIP_CALLBACKS` option enables the use of custom Core Foundation collection call backs which omit the [`CFRetain`][CFRetain] calls.  This results in saving several [`CFRetain`][CFRetain] and [`CFRelease`][CFRelease] calls typically needed for every single object from the parsed JSON.  While the author has used this technique for years without any issues, an unexpected interaction with the Foundation [`-mutableCopy`][-mutableCopy] method and Core Foundation Toll-Free Bridging resulting in a condition in which the objects contained in the collection to be over released.  This problem does not occur with the use of [`-copy`][-copy] due to the fact that the objects created by JSONKit are immutable, and therefore [`-copy`][-copy] does not require creating a completely new object and copying the contents, instead [`-copy`][-copy] simply returns a [`-retain`][-retain]'d version of the immutable object which is significantly faster along with the obvious reduction in memory usage.
    
    Prior to version 1.3, JSONKit always used custom "Transfer of Ownership Collection Callbacks", and thus `JK_ENABLE_CF_TRANSFER_OWNERSHIP_CALLBACKS` was effectively implicitly defined.
    
    Beginning with version 1.3, the default behavior of JSONKit is to use the standard Core Foundation collection callbacks ([`kCFTypeArrayCallBacks`][kCFTypeArrayCallBacks], [`kCFTypeDictionaryKeyCallBacks`][kCFTypeDictionaryKeyCallBacks], and [`kCFTypeDictionaryValueCallBacks`][kCFTypeDictionaryValueCallBacks]).  The intention is to follow "the principle of least surprise", and the author believes the use of the standard Core Foundation collection callbacks as the default behavior for JSONKit results in the least surprise.
    
    **NOTE**: `JK_ENABLE_CF_TRANSFER_OWNERSHIP_CALLBACKS` is only applicable to `(CF|NS)` `Dictionary` and `Array` class objects.
    
    For the vast majority of users, the author believes JSONKits custom "Transfer of Ownership Collection Callbacks" will not cause any problems.  As previously stated, the author has used this technique in performance critical code for years and has never had a problem.  Until a user reported a problem with [`-mutableCopy`][-mutableCopy], the author was unaware that the use of the custom callbacks could even cause a problem.  This is probably due to the fact that the vast majority of time the typical usage pattern tends to be "iterate the contents of the collection" and very rarely mutate the returned collection directly (although this last part is likely to vary significantly from programmer to programmer).  The author tends to avoid the use of [`-mutableCopy`][-mutableCopy] as it results in a significant performance and memory consumption penalty.  The reason for this is in "typical" Cocoa coding patterns, using [`-mutableCopy`][-mutableCopy] will instantiate an identical, albeit mutable, version of the original object.  This requires both memory for the new object and time to iterate the contents of the original object and add them to the new object.  Furthermore, under "typical" Cocoa coding patterns, the original collection object continues to consume memory until the autorelease pool is released.  However, clearly there are cases where the use of [`-mutableCopy`][-mutableCopy] makes sense or may be used by an external library which is out of your direct control.
    
    The use of the standard Core Foundation collection callbacks results in a 9% to 23% reduction in parsing performance, with an "eye-balled average" of around 13% according to some benchmarking done by the author using Real World&trade; JSON (i.e., actual JSON from various web services, such as Twitter, etc) using `gcc-4.2 -arch x86_64 -O3 -DNS_BLOCK_ASSERTIONS` with the only change being the addition of <span style="white-space: nowrap;">`-DJK_ENABLE_CF_TRANSFER_OWNERSHIP_CALLBACKS`</span>.
    
    `JK_ENABLE_CF_TRANSFER_OWNERSHIP_CALLBACKS` is only applicable to parsing / deserializing (i.e. converting from) of JSON.  Serializing (i.e., converting to JSON) is completely unaffected by this change.

### Bug Fixes

*   Fixed a [bug report regarding `-mutableCopy`](https://github.com/johnezang/JSONKit/issues#issue/3).  This is related to the addition of the pre-processor define flag `JK_ENABLE_CF_TRANSFER_OWNERSHIP_CALLBACKS`.

### Other Changes

*   Added `JK_EXPECTED` optimization hints around several conditionals.
*   When serializing objects, JSONKit first starts with a small, on stack buffer.  If the encoded JSON exceeds the size of the stack buffer, JSONKit switches to a heap allocated buffer.  If JSONKit switched to a heap allocated buffer, [`CFDataCreateWithBytesNoCopy`][CFDataCreateWithBytesNoCopy] is used to create the [`NSData`][NSData] object, which in most cases causes the heap allocated buffer to "transfer" to the [`NSData`][NSData] object which is substantially faster than allocating a new buffer and copying the bytes.
*   Added a pre-processor check in `JSONKit.m` to see if Objective-C Garbage Collection is enabled and issue a `#error` notice that JSONKit does not support Objective-C Garbage Collection.
*   Various other minor or trivial modifications, such as updating `README.md`.

### Other Issues

*   When using the `clang` static analyzer (the version used at the time of this writing was `Apple clang version 1.5 (tags/Apple/clang-60)`), the static analyzer reports a number of problems with `JSONKit.m`.
    
    The author has investigated these issues and determined that the problems reported by the current version of the static analyzer are "false positives".  Not only that, the reported problems are not only "false positives", they are very clearly and obviously wrong.  Therefore, the author has made the decision that no action will be taken on these non-problems, which includes not modifying the code for the sole purpose of silencing the static analyzer.  The justification for this is "the dog wags the tail, not the other way around."

## Version 1.2 2011/01/08

### Bug Fixes

*   When JSONKit attempted to parse and decode JSON that contained `{"key": value}` dictionaries that contained the same key more than once would likely result in a crash.  This was a serious bug.
*   Under some conditions, JSONKit could potentially leak memory.
*   There was an off by one error in the code that checked whether or not the parser was at the end of the `UTF8` buffer.  This could result in JSONKit reading one by past the buffer bounds in some cases.

### Other Changes

*   Some of the methods were missing `NULL` pointer checks for some of their arguments.  This was fixed.  In generally, when JSONKit encounters invalid arguments, it throws a `NSInvalidArgumentException` exception.
*   Various other minor changes such as tightening up numeric literals with `UL` or `L` qualification, assertion check tweaks and additions, etc.
*   The README.md file was updated with additional information.

### Version 1.1

No change log information was kept for versions prior to 1.2.

[kCFTypeArrayCallBacks]: http://developer.apple.com/library/mac/documentation/CoreFoundation/Reference/CFArrayRef/Reference/reference.html#//apple_ref/c/data/kCFTypeArrayCallBacks
[kCFTypeDictionaryKeyCallBacks]: http://developer.apple.com/library/mac/documentation/CoreFoundation/Reference/CFDictionaryRef/Reference/reference.html#//apple_ref/c/data/kCFTypeDictionaryKeyCallBacks
[kCFTypeDictionaryValueCallBacks]: http://developer.apple.com/library/mac/documentation/CoreFoundation/Reference/CFDictionaryRef/Reference/reference.html#//apple_ref/c/data/kCFTypeDictionaryValueCallBacks
[-mutableCopy]: http://developer.apple.com/library/mac/#documentation/Cocoa/Reference/Foundation/Classes/NSObject_Class/Reference/Reference.html%23//apple_ref/occ/instm/NSObject/mutableCopy
[-copy]: http://developer.apple.com/library/mac/#documentation/Cocoa/Reference/Foundation/Classes/NSObject_Class/Reference/Reference.html%23//apple_ref/occ/instm/NSObject/copy
[-retain]: http://developer.apple.com/library/mac/documentation/Cocoa/Reference/Foundation/Protocols/NSObject_Protocol/Reference/NSObject.html#//apple_ref/occ/intfm/NSObject/retain
[CFRetain]: http://developer.apple.com/library/mac/documentation/CoreFoundation/Reference/CFTypeRef/Reference/reference.html#//apple_ref/c/func/CFRetain
[CFRelease]: http://developer.apple.com/library/mac/documentation/CoreFoundation/Reference/CFTypeRef/Reference/reference.html#//apple_ref/c/func/CFRelease
[CFDataCreateWithBytesNoCopy]: http://developer.apple.com/library/mac/documentation/CoreFoundation/Reference/CFDataRef/Reference/reference.html#//apple_ref/c/func/CFDataCreateWithBytesNoCopy
[NSData]: http://developer.apple.com/mac/library/documentation/Cocoa/Reference/Foundation/Classes/NSData_Class/index.html
