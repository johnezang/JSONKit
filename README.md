# JSONKit

### A Very High Performance Objective-C JSON Library

JSONKit is licensed under the terms of the BSD License.

JavaScript Object Notation, or [JSON][JSON], is a lightweight,
text-based, serialization format for structured data that is used by many
web-based services and API's.  It is defined by [RFC 4627][RFC 4627]

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
<tr><td>Associative Arrays</td><td><a href="http://developer.apple.com/mac/library/documentation/Cocoa/Reference/Foundation/Classes/NSDictionary_Class/index.html"<code>NSDictionary</code></a></td></tr>
</table>

JSONKit uses Core Foundation internally, and it is assumed that Core
Foundation &equiv; Foundation for every equivalent base type, i.e. `CFString`
&equiv; `NSString`.

### JSON To Objective-C Primitive Mapping Details

*   While the [JSON specification][RFC 4627] specifies that the serialized
    JSON must be encoded in Unicode, it does not specify how Unicode encoding
    errors should be handled.  In general, JSONKit will not accept JSON that
    contains ill-formed Unicode.
    
    When the `JKParseOptionLooseUnicode` option is used, JSONKit follows the
    specifications and recommendations given in
    [The Unicode 5.2 standard, Chapter 3][Unicode_CH3], section 3.9 *Unicode
    Encoding Forms*.  As a general rule of thumb, the Unicode code point
    `U+FFFD` is substituted for any ill-formed Unicode encountered.
    JSONKit attempts to follow the recommended *Best Practice for Using
    U+FFFD*:  ***Replace each maximal subpart of an ill-formed subsequence by
    a single U+FFFD.***  Additionally, the following Unicode code points are
    treated as ill-formed Unicode, and if `JKParseOptionLooseUnicode` is
    enabled, cause `U+FFFD` to be substituted in their place:
    
    `U+0000`.<br>
    `U+D800` thru `U+DFFF`, inclusive.<br>
    `U+FDD0` thru `U+FDEF`, inclusive.<br>
    `U+`*n*`FFFE` and `U+`*n*`FFFF`, where *n* is from `0x0` to `0x10`
    
    The code points `U+FDD0` thru `U+FDEF`, `U+`*n*`FFFE`, and `U+`*n*`FFFF`
    (where *n* is from `0x0` to `0x10`), are defined as
    ***Noncharacters*** by the Unicode standard and "should never be
    interchanged".

    [RFC 4627][] allows for these limitations under section 4, Parsers:
    `An implementation may set limits on the length and character contents
    of strings.`

    The `NSString` class may place additional restrictions or otherwise
    transform the JSON String in such a way so that the JSON String is not
    bijective with the instantiated `NSString` object.  In other words, JSONKit
    can not guarantee that when you round trip a JSON String to a `NSString`
    and then back to a JSON String that the two JSON Strings will be exactly
    the same, even though in practice they are.  For clarity, "exactly" in
    this case means bit for bit identical.  JSONKit can not even guarantee
    that the two JSON Strings will be
    [Unicode equivalent][Unicode_equivalence], even though in practice they
    will be and would be the most likely cause for the two round tripped JSON
    Strings to no longer be bit for bit identical.

*   JSONKit maps `true` and `false` to the [`CFBoolean`][CFBoolean] values
    [`kCFBooleanTrue`][kCFBooleanTrue] and
    [`kCFBooleanFalse`][kCFBooleanFalse], respectively.  Conceptually,
    `CFBoolean` values can be thought of, and treated as, `NSNumber` class
    objects.  The benefit to using `CFBoolean` is that `true` and `false`
    JSON values can be round trip deserialized and serialized without
    conversion or promotion to a `NSNumber` with a value of `0` or `1`.

*   The [JSON specification][RFC 4627] does not specify the details or
    requirements for JSON Number values, nor does it specify how errors
    due to conversion should be handled.  In general, JSONKit will not accept
    JSON that contains JSON Number values that it can not convert with out
    error or loss of precision.

    For non-floating-point numbers (i.e., JSON Number values that do not
    include a `.` or `e|E`), JSONKit uses a 64-bit C primitive type
    internally, regardless of whether the target architecture is 32-bit or
    64-bit.  For unsigned values (i.e., those that do not begin with a
    `-`), this allows for values up to <code>2<sup>64</sup>-1</code>, and up
    to <code>-2<sup>63</sup></code> for negative values.  As a special case,
    the JSON Number `-0` is treated as a floating-point number since the
    underlying floating-point primitive type is capable of representing a
    negative zero, whereas the underlying twos-complement non-floating-point
    primitive type can not.  JSON that contains Number values that exceed
    these limits will fail to parse and optionally return a `NSError` object.
    The functions `strtoll()` and `strtoull()` are used to perform the
    conversions.

    The C `double` primitive type, or IEEE 754 Double 64-bit floating-point,
    is used to represent floating-point JSON Number values.  JSON that
    contains floating-point Number values that can not be represented as a
    `double` (i.e., due to over or underflow) will fail to parse and
    optionally return a `NSError` object.  The function `strtod()` is used to
    perform the conversion.  Note that the JSON standard does not allow for
    infinities or `NaN` (Not a Number).
    
*   For JSON Associative Arrays (or `object` in [RFC 4627][] nomenclature),
    [RFC 4627][] says `The names within an object SHOULD be unique`
    (note: `name` is a `key` in JSONKit nomenclature). At this time the
    JSONKit behavior is `undefined` for JSON that contains names within an
    object that are not unique.  However, JSONKit currently tries to
    follow a "the last key / value pair parsed is the one chosen" policy.
    This behavior is not finalized and should not be depended on.

### Objective-C To JSON Primitive Mapping Details

*   The `NSDictionary` class allows for any object, which can be of any class,
    to be used as a `key`.  JSON, however, only permits Strings to be used as
    `keys`. Therefore JSONKit will fail with an error if it encounters a
    `NSDictionary` that contains keys that are not `NSString` objects during
    serialization.

*   JSON does not allow for Numbers that are <code>&plusmn;Infinity</code> or
    <code>&plusmn;NaN</code>.  Therefore JSONKit will fail with an error if
    it encounters a `NSNumber` that contains such a value during serialization.

*   JSONKit will fail with an error if it encounters an object that is not a
    `NSNull`, `NSNumber`, `NSString`, `NSArray`, or `NSDictionary` class
    object during serialization.

*   Objects created with `[NSNumber numberWithBool:YES]` and
    `[NSNumber numberWithBool:NO]` will be mapped to the JSON values of
    `true` and `false`, respectively.

[JSON]: http://www.json.org/
[RFC 4627]: http://tools.ietf.org/html/rfc4627
[CFBoolean]: http://developer.apple.com/mac/library/documentation/CoreFoundation/Reference/CFBooleanRef/index.html
[kCFBooleanTrue]: http://developer.apple.com/mac/library/documentation/CoreFoundation/Reference/CFBooleanRef/Reference/reference.html#//apple_ref/doc/c_ref/kCFBooleanTrue
[kCFBooleanFalse]: http://developer.apple.com/mac/library/documentation/CoreFoundation/Reference/CFBooleanRef/Reference/reference.html#//apple_ref/doc/c_ref/kCFBooleanFalse
[Unicode_CH3]: http://www.unicode.org/versions/Unicode5.2.0/ch03.pdf
[Unicode_equivalence]: http://en.wikipedia.org/wiki/Unicode_equivalence
