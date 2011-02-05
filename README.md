# JSONKit

### A Very High Performance Objective-C JSON Library

JSONKit is licensed under the terms of the BSD License.  Copyright &copy; 2011, John Engelhart.

JavaScript Object Notation, or [JSON][], is a lightweight, text-based, serialization format for structured data that is used by many web-based services and API's.  It is defined by [RFC 4627][].

JSON provides the following primitive types:

* `null`
* Boolean `true` and `false`
* Number
* String
* Array
* Associative Arrays (Objects in [RFC 4627][] nomenclature, a.k.a. Key / Value Hash Tables, Maps, Dictionaries, etc.)

These primitive types are mapped to the following Objective-C Foundation classes:

<table>
<tr><th>JSON</th><th>Objective-C</th></tr>
<tr><td><code>null</code></td><td><a href="http://developer.apple.com/mac/library/documentation/Cocoa/Reference/Foundation/Classes/NSNull_Class/index.html"><code>NSNull</code></a></td></tr>
<tr><td><code>true</code> and <code>false</code></td><td><a href="http://developer.apple.com/mac/library/documentation/Cocoa/Reference/Foundation/Classes/NSNumber_Class/index.html"><code>NSNumber</code></a></td></tr>
<tr><td>Number</td><td><a href="http://developer.apple.com/mac/library/documentation/Cocoa/Reference/Foundation/Classes/NSNumber_Class/index.html"><code>NSNumber</code></a></td></tr>
<tr><td>String</td><td><a href="http://developer.apple.com/mac/library/documentation/Cocoa/Reference/Foundation/Classes/NSString_Class/index.html"><code>NSString</code></a></td></tr>
<tr><td>Array</td><td><a href="http://developer.apple.com/mac/library/documentation/Cocoa/Reference/Foundation/Classes/NSArray_Class/index.html"><code>NSArray</code></a></td></tr>
<tr><td>Associative Arrays</td><td><a href="http://developer.apple.com/mac/library/documentation/Cocoa/Reference/Foundation/Classes/NSDictionary_Class/index.html"><code>NSDictionary</code></a></td></tr>
</table>

JSONKit uses Core Foundation internally, and it is assumed that Core Foundation &equiv; Foundation for every equivalent base type, i.e. [`CFString`][CFString] &equiv; [`NSString`][NSString].

### JSON To Objective-C Primitive Mapping Details

*   While the [JSON specification][RFC 4627] specifies that the serialized JSON must be encoded in Unicode, it does not specify how Unicode encoding errors should be handled.  In general, JSONKit will not accept JSON that contains ill-formed Unicode.
    
    When the `JKParseOptionLooseUnicode` option is used, JSONKit follows the specifications and recommendations given in [The Unicode 5.2 standard, Chapter 3][Unicode_CH3], section 3.9 *Unicode Encoding Forms*.  As a general rule of thumb, the Unicode code point `U+FFFD` is substituted for any ill-formed Unicode encountered. JSONKit attempts to follow the recommended *Best Practice for Using U+FFFD*:  ***Replace each maximal subpart of an ill-formed subsequence by a single U+FFFD.***  Additionally, the following Unicode code points are treated as ill-formed Unicode, and if `JKParseOptionLooseUnicode` is enabled, cause `U+FFFD` to be substituted in their place:

    `U+0000`.<br>
    `U+D800` thru `U+DFFF`, inclusive.<br>
    `U+FDD0` thru `U+FDEF`, inclusive.<br>
    <code>U+<i style="padding-right:0.325ex">n</i>FFFE</code> and <code>U+<i style="padding-right:0.325ex">n</i>FFFF</code>, where *n* is from `0x0` to `0x10`

    The code points `U+FDD0` thru `U+FDEF`, <code>U+<i style="padding-right:0.325ex">n</i>FFFE</code>, and <code>U+<i style="padding-right:0.325ex">n</i>FFFF</code> (where *n* is from `0x0` to `0x10`), are defined as ***Noncharacters*** by the Unicode standard and "should never be interchanged".

    [RFC 4627][] allows for these limitations under section 4, Parsers: `An implementation may set limits on the length and character contents of strings.`

    The [`NSString`][NSString] class may place additional restrictions or otherwise transform the JSON String in such a way so that the JSON String is not bijective with the instantiated [`NSString`][NSString] object.  In other words, JSONKit can not guarantee that when you round trip a JSON String to a [`NSString`][NSString] and then back to a JSON String that the two JSON Strings will be exactly the same, even though in practice they are.  For clarity, "exactly" in this case means bit for bit identical.  JSONKit can not even guarantee that the two JSON Strings will be [Unicode equivalent][Unicode_equivalence], even though in practice they will be and would be the most likely cause for the two round tripped JSON Strings to no longer be bit for bit identical.  JSONKit does not perform any form of Unicode Normalization on the parsed strings.  While JSONKit makes an effort (subject to the reasonable caveats above) to preserve the parsed JSON string exactly, it can not guarantee that [`NSString`][NSString] will preserve the exact Unicode semantics of the original string.

*   JSONKit maps `true` and `false` to the [`CFBoolean`][CFBoolean] values [`kCFBooleanTrue`][kCFBooleanTrue] and [`kCFBooleanFalse`][kCFBooleanFalse], respectively.  Conceptually, [`CFBoolean`][CFBoolean] values can be thought of, and treated as, [`NSNumber`][NSNumber] class objects.  The benefit to using [`CFBoolean`][CFBoolean] is that `true` and `false` JSON values can be round trip deserialized and serialized without conversion or promotion to a [`NSNumber`][NSNumber] with a value of `0` or `1`.

*   The [JSON specification][RFC 4627] does not specify the details or requirements for JSON Number values, nor does it specify how errors due to conversion should be handled.  In general, JSONKit will not accept JSON that contains JSON Number values that it can not convert with out error or loss of precision.

    For non-floating-point numbers (i.e., JSON Number values that do not include a `.` or `e|E`), JSONKit uses a 64-bit C primitive type internally, regardless of whether the target architecture is 32-bit or 64-bit.  For unsigned values (i.e., those that do not begin with a `-`), this allows for values up to <code>2<span style="font-family: 'Lucida Grande', Menlo,'DejaVu Sans Mono', Monaco,'Courier New','Bitstream Vera Sans  Mono',monospace;">&#8310;&#8308;</span>-1</code> and up to <code>-2<span style="font-family: 'Lucida Grande', Menlo,'DejaVu Sans Mono', Monaco,'Courier New','Bitstream Vera Sans  Mono',monospace;">&#8310;&sup3;</span></code>   for negative values.  As a special case, the JSON Number `-0` is treated as a floating-point number since the underlying floating-point primitive type is capable of representing a negative zero, whereas the underlying twos-complement non-floating-point primitive type can not.  JSON that contains Number values that exceed these limits will fail to parse and optionally return a [`NSError`][NSError] object. The functions [`strtoll()`][strtoll] and [`strtoull()`][strtoull] are used to perform the conversions.

    The C `double` primitive type, or IEEE 754 Double 64-bit floating-point, is used to represent floating-point JSON Number values.  JSON that contains floating-point Number values that can not be represented as a `double` (i.e., due to over or underflow) will fail to parse and optionally return a [`NSError`][NSError] object.  The function [`strtod()`][strtod] is used to perform the conversion.  Note that the JSON standard does not allow for infinities or `NaN` (Not a Number).
    
*   For JSON Associative Arrays (or `object` in [RFC 4627][] nomenclature), [RFC 4627][] says `The names within an object SHOULD be unique` (note: `name` is a `key` in JSONKit nomenclature). At this time the JSONKit behavior is `undefined` for JSON that contains names within an object that are not unique.  However, JSONKit currently tries to follow a "the last key / value pair parsed is the one chosen" policy. This behavior is not finalized and should not be depended on.

### Objective-C To JSON Primitive Mapping Details

*   The [`NSDictionary`][NSDictionary] class allows for any object, which can be of any class, to be used as a `key`.  JSON, however, only permits Strings to be used as `keys`. Therefore JSONKit will fail with an error if it encounters a [`NSDictionary`][NSDictionary] that contains keys that are not [`NSString`][NSString] objects during serialization.

*   JSON does not allow for Numbers that are <code>&plusmn;Infinity</code> or <code>&plusmn;NaN</code>.  Therefore JSONKit will fail with an error if it encounters a [`NSNumber`][NSNumber] that contains such a value during serialization.

*   JSONKit will fail with an error if it encounters an object that is not a [`NSNull`][NSNull], [`NSNumber`][NSNumber], [`NSString`][NSString], [`NSArray`][NSArray], or [`NSDictionary`][NSDictionary] class object during serialization.

*   Objects created with [`[NSNumber numberWithBool:YES]`][NSNumber_numberWithBool] and [`[NSNumber numberWithBool:NO]`][NSNumber_numberWithBool] will be mapped to the JSON values of `true` and `false`, respectively.

### Reporting Bugs

Please use the [github.com JSONKit Issue Tracker](https://github.com/johnezang/JSONKit/issues) to report bugs.

The author requests that you do not file a bug report with JSONKit regarding problems reported by the `clang` static analyzer unless you first manually verify that it is an actual, bona-fide problem with JSONKit and, if appropriate, is not "legal" C code as defined by the C99 language specification.  If the `clang` static analyzer is reporting a problem with JSONKit that is not an actual, bona-fide problem and is perfectly legal code as defined by the C99 language specification, then the appropriate place to file a bug report or complaint is with the developers of the `clang` static analyzer.

### Important Details

*   JSONKit is not designed to be used with the Mac OS X Garbage Collection.  The behavior of JSONKit when compiled with `-fobj-gc` is `undefined`.  It is extremely unlikely that Mac OS X Garbage Collection will ever be supported.

*   The JSON to be parsed by JSONKit must be encoded as Unicode.  In the unlikely event you end up with JSON that is not encoded as Unicode, you must first convert the JSON to Unicode, preferably as `UTF8`.  One way to accomplish this is with the [`NSString`][NSString] methods [`-initWithBytes:length:encoding:`][NSString_initWithBytes] and [`-initWithData:encoding:`][NSString_initWithData].

*   Internally, the low level parsing engine uses `UTF8` exclusively.  The `JSONDecoder` method `-parseJSONData:` takes a [`NSData`][NSData] object as its argument and it is assumed that the raw bytes contained in the [`NSData`][NSData] is `UTF8` encoded, otherwise the behavior is `undefined`.

*   It is not safe to use the same instantiated `JSONDecoder` object from multiple threads at the same time.  If you wish to share a `JSONDecoder` between threads, you are responsible for adding mutex barriers to ensure that only one thread is decoding JSON using the shared `JSONDecoder` object at a time.

### Tips for speed

*   Enable the `NS_BLOCK_ASSERTIONS` pre-processor flag.  JSONKit makes heavy use of [`NSCParameterAssert()`][NSCParameterAssert] internally to ensure that various arguments, variables, and other state contains only legal and expected values.  If an assertion check fails it causes a run time exception that will normally cause a program to terminate.  These checks and assertions come with a price: they take time to execute and do not contribute to the work being performed.  It is perfectly safe to enable `NS_BLOCK_ASSERTIONS` as JSONKit always performs checks that are required for correct operation.  The checks performed with [`NSCParameterAssert()`][NSCParameterAssert] are completely optional and are meant to be used in "debug" builds where extra integrity checks are usually desired.  While your mileage may vary, the author has found that adding `-DNS_BLOCK_ASSERTIONS` to an `-O2` optimization setting can generally result in an approximate <span style="white-space: nowrap;">7-12%</span> increase in performance.

*   Enable the `JK_ENABLE_CF_TRANSFER_OWNERSHIP_CALLBACKS` pre-processor flag.  Please see the entry for JSONKit version 1.3 in `CHANGELOG.md` for some important caveats and potential complications that may result in the use of this option.  Enabling this option ***does not*** violate any officially documented requirements or behavior, or make use of undocumented API's.  The most important caveat is you must ensure that the collection objects (i.e., [`NSArray`][NSArray] and [`NSDictionary`][NSDictionary]) created by JSONKit are never sent a [`-mutableCopy`][-mutableCopy] message when this option is enabled.   While your mileage may vary, the author has found that adding `JK_ENABLE_CF_TRANSFER_OWNERSHIP_CALLBACKS` can generally result in an approximate <span style="white-space: nowrap;">10-26%</span> increase in performance.

*   Since the very low level parsing engine works exclusively with `UTF8` byte streams, anything that is not already encoded as `UTF8` must first be converted to `UTF8`.  While JSONKit provides additions to the [`NSString`][NSString] class which allows you to conveniently convert JSON contained in a [`NSString`][NSString], this convenience does come with a price.  JSONKit uses the [`-UTF8String`][NSString_UTF8String] method to obtain a `UTF8` encoded version of a [`NSString`][NSString], and while the details of how a strings performs that conversion are an internal implementation detail, it is likely that this conversion carries a cost both in terms of time and the memory needed to store the conversion result.  Therefore, if speed is a priority, you should avoid using the [`NSString`][NSString] convenience methods if possible.

*   If you are receiving JSON data from a web server, and you are able to determine that the raw bytes returned by the web server is JSON encoded as `UTF8`, you should use the `JSONDecoder` method `-parseUTF8String:length:` which immediately begins parsing the pointers bytes. In practice, every JSONKit method that converts JSON to an Objective-C object eventually calls this method to perform the conversion.

*   If you are using one of the various ways provided by the `NSURL` family of classes to receive JSON results from a web server, which typically return the results in the form of a [`NSData`][NSData] object, and you are able to determine that the raw bytes contained in the [`NSData`][NSData] are encoded as `UTF8`, then you should use either the `JSONDecoder` method `parseJSONData:` or the [`NSData`][NSData] method `-objectFromJSONData`.  If are going to be converting a lot of JSON, the better choice is to instantiate a `JSONDecoder` object once and use the same instantiated object to perform all your conversions.  This has two benefits:
    1. The [`NSData`][NSData] method `-objectFromJSONData` creates an autoreleased `JSONDecoder` object to perform the one time conversion.  By instantiating a `JSONDecoder` object once and using the `parseJSONData:` method repeatedly, you can avoid this overhead.
    2. The instantiated object cache from the previous JSON conversion is reused.  This can result in both better performance and a reduction in memory usage if the JSON your are converting is very similar.  A typical example might be if you are converting JSON at periodic intervals that consists of real time status updates.

### Parsing Interface

#### JSONDecoder Interface

**Note:** The bytes contained in a [`NSData`][NSData] object ***must*** be `UTF8` encoded.

**Important:** Methods will raise [`NSInvalidArgumentException`][NSInvalidArgumentException] if `parseOptionFlags` is not valid.  
**Important:** `parseUTF8String:` will raise [`NSInvalidArgumentException`][NSInvalidArgumentException] if `parseUTF8String` is `NULL`.  
**Important:** `parseJSONData:` will raise [`NSInvalidArgumentException`][NSInvalidArgumentException] if `jsonData` is `NULL`.

<pre>+ (id)decoder;
+ (id)decoderWithParseOptions:(JKParseOptionFlags)parseOptionFlags;
- (id)initWithParseOptions:(JKParseOptionFlags)parseOptionFlags;

- (void)clearCache;

- (id)parseUTF8String:(const unsigned char *)string length:(size_t)length;
- (id)parseUTF8String:(const unsigned char *)string length:(size_t)length error:(NSError **)error;

- (id)parseJSONData:(NSData *)jsonData;
- (id)parseJSONData:(NSData *)jsonData error:(NSError **)error;</pre>

#### NSString Interface

<pre>- (id)objectFromJSONString;
- (id)objectFromJSONStringWithParseOptions:(JKParseOptionFlags)parseOptionFlags;
- (id)objectFromJSONStringWithParseOptions:(JKParseOptionFlags)parseOptionFlags error:(NSError **)error;</pre>

#### NSData Interface

<pre>- (id)objectFromJSONData;
- (id)objectFromJSONDataWithParseOptions:(JKParseOptionFlags)parseOptionFlags;
- (id)objectFromJSONDataWithParseOptions:(JKParseOptionFlags)parseOptionFlags error:(NSError **)error;</pre>

#### JKParseOptionFlags

<table>
  <tr><th>Parsing Option</th><th>Description</th></tr>
  <tr><td><code>JKParseOptionNone</code></td><td>This is the default if no other other parse option flags are specified, and the option used when a convenience method does not provide an argument for explicitly specifying the parse options to use.  Synonymous with <code>JKParseOptionStrict</code>.</td></tr>
  <tr><td><code>JKParseOptionStrict</code></td><td>The JSON will be parsed in strict accordance with the <a href="http://tools.ietf.org/html/rfc4627">RFC 4627</a> specification.</td></tr>
  <tr><td><code>JKParseOptionComments</code></td><td>Allow C style <code>//</code> and <code>/* &hellip; */</code> comments in JSON.  This is a fairly common extension to JSON, but JSON that contains C style comments is not strictly conforming JSON.</td></tr>
  <tr><td><code>JKParseOptionUnicodeNewlines</code></td><td>Allow Unicode recommended <code>(?:\r\n|[\n\v\f\r\x85\p{Zl}\p{Zp}])</code> newlines in JSON.  The <a href="http://tools.ietf.org/html/rfc4627">JSON specification</a> only allows the newline characters <code>\r</code> and <code>\n</code>, but this option allows JSON that contains the <a href="http://en.wikipedia.org/wiki/Newline#Unicode">Unicode recommended newline characters</a> to be parsed.  JSON that contains these additional newline characters is not strictly conforming JSON.</td></tr>
  <tr><td><code>JKParseOptionLooseUnicode</code></td><td>Normally the decoder will stop with an error at any malformed Unicode. This option allows JSON with malformed Unicode to be parsed without reporting an error. Any malformed Unicode is replaced with <code>\uFFFD</code>, or <code>REPLACEMENT CHARACTER</code>, as specified in <a href="http://www.unicode.org/versions/Unicode5.2.0/ch03.pdf">The Unicode 5.2 standard, Chapter 3</a>, section 3.9 <em>Unicode Encoding Forms</em>.</td></tr>
  <tr><td><code>JKParseOptionPermitTextAfterValidJSON</code></td><td>Normally, <code>white-space</code> that follows the JSON is interpreted as a parsing failure. This option allows for any trailing <code>white-space</code> to be ignored and not cause a parsing error.</td></tr>
</table>

### Serializing Interface

**Note:** The bytes contained in the returned [`NSData`][NSData] object is `UTF8` encoded.

#### NSArray and NSDictionary Interface

<pre>- (NSData *)JSONData;
- (NSData *)JSONDataWithOptions:(JKSerializeOptionFlags)serializeOptions error:(NSError **)error;
- (NSString *)JSONString;
- (NSString *)JSONStringWithOptions:(JKSerializeOptionFlags)serializeOptions error:(NSError **)error;</pre>

#### JKSerializeOptionFlags

<table>
  <tr><th>Serializing Option</th><th>Description</th></tr>
  <tr><td><code>JKSerializeOptionNone</code></td><td>This is the default if no other other serialize option flags are specified, and the option used when a convenience method does not provide an argument for explicitly specifying the serialize options to use.</td></tr>
  <tr><td><code>JKSerializeOptionEscapeUnicode</code></td><td>When JSONKit encounters Unicode characters in <a href="http://developer.apple.com/mac/library/documentation/Cocoa/Reference/Foundation/Classes/NSString_Class/index.html"><code>NSString</code></a> objects, the default behavior is to encode those Unicode characters as <code>UTF8</code>.  This option causes JSONKit to encode those characters as <code>\u<i>XXXX</i></code>.  For example,<br/><code>["w&isin;L&#10234;y(&#8739;y&#8739;&le;&#8739;w&#8739;)"]</code><br/>becomes:<br/><code>["w\u2208L\u27fa\u2203y(\u2223y\u2223\u2264\u2223w\u2223)"]</code></td></tr>
</table>

[JSON]: http://www.json.org/
[RFC 4627]: http://tools.ietf.org/html/rfc4627
[CFBoolean]: http://developer.apple.com/mac/library/documentation/CoreFoundation/Reference/CFBooleanRef/index.html
[kCFBooleanTrue]: http://developer.apple.com/mac/library/documentation/CoreFoundation/Reference/CFBooleanRef/Reference/reference.html#//apple_ref/doc/c_ref/kCFBooleanTrue
[kCFBooleanFalse]: http://developer.apple.com/mac/library/documentation/CoreFoundation/Reference/CFBooleanRef/Reference/reference.html#//apple_ref/doc/c_ref/kCFBooleanFalse
[Unicode_CH3]: http://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
[Unicode_equivalence]: http://en.wikipedia.org/wiki/Unicode_equivalence
[NSNull]: http://developer.apple.com/mac/library/documentation/Cocoa/Reference/Foundation/Classes/NSNull_Class/index.html
[NSNumber]: http://developer.apple.com/mac/library/documentation/Cocoa/Reference/Foundation/Classes/NSNumber_Class/index.html
[NSNumber_numberWithBool]: http://developer.apple.com/library/mac/documentation/Cocoa/Reference/Foundation/Classes/NSNumber_Class/Reference/Reference.html#//apple_ref/occ/clm/NSNumber/numberWithBool:
[NSString]: http://developer.apple.com/mac/library/documentation/Cocoa/Reference/Foundation/Classes/NSString_Class/index.html
[NSString_initWithBytes]: http://developer.apple.com/library/mac/documentation/Cocoa/Reference/Foundation/Classes/NSString_Class/Reference/NSString.html#//apple_ref/occ/instm/NSString/initWithBytes:length:encoding:
[NSString_initWithData]: http://developer.apple.com/library/mac/documentation/Cocoa/Reference/Foundation/Classes/NSString_Class/Reference/NSString.html#//apple_ref/occ/instm/NSString/initWithData:encoding:
[NSString_UTF8String]: http://developer.apple.com/library/mac/documentation/Cocoa/Reference/Foundation/Classes/NSString_Class/Reference/NSString.html#//apple_ref/occ/instm/NSString/UTF8String
[NSArray]: http://developer.apple.com/mac/library/documentation/Cocoa/Reference/Foundation/Classes/NSArray_Class/index.html
[NSDictionary]: http://developer.apple.com/mac/library/documentation/Cocoa/Reference/Foundation/Classes/NSDictionary_Class/index.html
[NSError]: http://developer.apple.com/mac/library/documentation/Cocoa/Reference/Foundation/Classes/NSError_Class/index.html
[NSData]: http://developer.apple.com/mac/library/documentation/Cocoa/Reference/Foundation/Classes/NSData_Class/index.html
[NSInvalidArgumentException]: http://developer.apple.com/mac/library/documentation/Cocoa/Reference/Foundation/Miscellaneous/Foundation_Constants/Reference/reference.html#//apple_ref/doc/c_ref/NSInvalidArgumentException
[CFString]: http://developer.apple.com/library/mac/#documentation/CoreFoundation/Reference/CFStringRef/Reference/reference.html
[strtoll]: http://developer.apple.com/library/mac/#documentation/Darwin/Reference/ManPages/man3/strtoll.3.html
[strtod]: http://developer.apple.com/library/mac/#documentation/Darwin/Reference/ManPages/man3/strtod.3.html
[strtoull]: http://developer.apple.com/library/mac/#documentation/Darwin/Reference/ManPages/man3/strtoull.3.html
[NSCParameterAssert]: http://developer.apple.com/library/mac/documentation/Cocoa/Reference/Foundation/Miscellaneous/Foundation_Functions/Reference/reference.html#//apple_ref/c/macro/NSCParameterAssert
[UnicodeNewline]: http://en.wikipedia.org/wiki/Newline#Unicode
[-mutableCopy]: http://developer.apple.com/library/mac/#documentation/Cocoa/Reference/Foundation/Classes/NSObject_Class/Reference/Reference.html%23//apple_ref/occ/instm/NSObject/mutableCopy
