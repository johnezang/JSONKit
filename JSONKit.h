//
//  JSONKit.h
//  http://github.com/johnezang/JSONKit
//  Licensed under the terms of the BSD License, as specified below.
//

/*
 Copyright (c) 2011, John Engelhart
 
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 
 * Neither the name of the Zang Industries nor the names of its
 contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <TargetConditionals.h>
#include <AvailabilityMacros.h>

#ifdef    __OBJC__
#import <Foundation/NSArray.h>
#import <Foundation/NSData.h>
#import <Foundation/NSDictionary.h>
#import <Foundation/NSError.h>
#import <Foundation/NSObjCRuntime.h>
#import <Foundation/NSString.h>
#endif // __OBJC__
 
#ifdef __cplusplus
extern "C" {
#endif
  

// For Mac OS X < 10.5.
#ifndef   NSINTEGER_DEFINED
#define   NSINTEGER_DEFINED
#if       defined(__LP64__) || defined(NS_BUILD_32_LIKE_64)
typedef long           NSInteger;
typedef unsigned long  NSUInteger;
#define NSIntegerMin   LONG_MIN
#define NSIntegerMax   LONG_MAX
#define NSUIntegerMax  ULONG_MAX
#else  // defined(__LP64__) || defined(NS_BUILD_32_LIKE_64)
typedef int            NSInteger;
typedef unsigned int   NSUInteger;
#define NSIntegerMin   INT_MIN
#define NSIntegerMax   INT_MAX
#define NSUIntegerMax  UINT_MAX
#endif // defined(__LP64__) || defined(NS_BUILD_32_LIKE_64)
#endif // NSINTEGER_DEFINED


#ifndef _JSONKIT_H_
#define _JSONKIT_H_

#define JSONKIT_VERSION_MAJOR 1
#define JSONKIT_VERSION_MINOR 2

typedef NSUInteger JKHash;
typedef NSUInteger JKFlags;
typedef NSUInteger JKTokenType;
typedef JKFlags    JKManagedBufferFlags;
typedef JKFlags    JKObjectStackFlags;

typedef struct {
  unsigned char *ptr;
  size_t         length;
} JKPtrRange;

typedef struct {
  const unsigned char *ptr;
  size_t               length;
} JKConstPtrRange;

typedef struct {
  size_t location, length;
} JKRange;

typedef struct {
  JKPtrRange           bytes;
  JKManagedBufferFlags flags;
  size_t               roundSizeUpToMultipleOf;
} JKManagedBuffer;

typedef struct {
  void               **objects, **keys;
  JKHash              *hashes;
  size_t              *sizes;
  size_t               count, index, roundSizeUpToMultipleOf;
  JKObjectStackFlags   flags;
} JKObjectStack;

typedef struct {
  JKPtrRange bytes;
} JKBuffer;

typedef struct {
  JKConstPtrRange bytes;
} JKConstBuffer;

typedef NSUInteger JKValueType;

typedef struct {
  JKConstPtrRange ptrRange;
  JKHash          hash;
  JKValueType     type;
  union {
    long long          longLongValue;
    unsigned long long unsignedLongLongValue;
    double             doubleValue;
  } number;
} JKTokenValue;

typedef struct {
  JKConstPtrRange tokenPtrRange;
  JKTokenType     type;
  JKTokenValue    value;
  JKManagedBuffer tokenBuffer;
} JKParseToken;


typedef struct {
  void          *object;
  JKHash         hash;
  size_t         size;
  unsigned char *bytes;
  JKValueType    type;
  unsigned char  age;
} JKTokenCacheItem;

typedef struct {
  JKTokenCacheItem *items;
  size_t            count, clockIdx;
} JKTokenCache;

/*
  JKParseOptionComments        : Allow C style // and /_* ... *_/ (without a _, obviously) comments in JSON.
  JKParseOptionUnicodeNewlines : Allow Unicode recommended (?:\r\n|[\n\v\f\r\x85\p{Zl}\p{Zp}]) newlines.
  JKParseOptionLooseUnicode    : Normally the decoder will stop with an error at any malformed Unicode.
                                 This option allows JSON with malformed Unicode to be parsed without reporting an error.
                                 Any malformed Unicode is replaced with \uFFFD, or "REPLACEMENT CHARACTER".
 */

enum {
  JKParseOptionNone                     = 0,
  JKParseOptionStrict                   = 0,
  JKParseOptionComments                 = (1 << 0),
  JKParseOptionUnicodeNewlines          = (1 << 1),
  JKParseOptionLooseUnicode             = (1 << 2),
  JKParseOptionPermitTextAfterValidJSON = (1 << 3),
  JKParseOptionValidFlags               = (JKParseOptionComments | JKParseOptionUnicodeNewlines | JKParseOptionLooseUnicode | JKParseOptionPermitTextAfterValidJSON),
};
typedef JKFlags JKParseOptionFlags;

typedef id (*NSNumberAllocImp)(id object, SEL selector);
typedef id (*NSNumberInitWithUnsignedLongLongImp)(id object, SEL selector, unsigned long long value);

typedef struct {
  Class NSNumberClass;
  NSNumberAllocImp NSNumberAlloc;
  NSNumberInitWithUnsignedLongLongImp NSNumberInitWithUnsignedLongLong;
} JKObjCImpCache;

typedef struct {
  JKParseOptionFlags  parseOptionFlags;
  JKConstBuffer       stringBuffer;
  size_t              atIndex, lineNumber, lineStartIndex;
  size_t              prev_atIndex, prev_lineNumber, prev_lineStartIndex;
  int                 errorIsPrev;
  JKParseToken        token;
  JKObjectStack       objectStack;
  JKTokenCache        cache;
  JKObjCImpCache      objCImpCache;
  NSError            *error;
} JKParseState;


enum {
  JKSerializeOptionNone           = 0,
  JKSerializeOptionPretty         = (1 << 0), // Not implemented yet...
  JKSerializeOptionEscapeUnicode  = (1 << 1),
  JKSerializeOptionValidFlags     = (JKSerializeOptionPretty | JKSerializeOptionEscapeUnicode),
};
typedef JKFlags JKSerializeOptionFlags;


#ifdef    __OBJC__

// As a general rule of thumb, if you use a method that doesn't accept a JKParseOptionFlags argument, it defaults to JKParseOptionStrict

@interface JSONDecoder : NSObject {
  JKParseState parseState;
}
+ (id)decoder;
+ (id)decoderWithParseOptions:(JKParseOptionFlags)parseOptionFlags;
- (id)initWithParseOptions:(JKParseOptionFlags)parseOptionFlags;
- (void)clearCache;
- (id)parseUTF8String:(const unsigned char *)string length:(size_t)length;
- (id)parseUTF8String:(const unsigned char *)string length:(size_t)length error:(NSError **)error;
// The NSData needs to be UTF8 encoded JSON.
- (id)parseJSONData:(NSData *)jsonData;
- (id)parseJSONData:(NSData *)jsonData error:(NSError **)error;
@end

@interface NSString (JSONKit)
- (id)objectFromJSONString;
- (id)objectFromJSONStringWithParseOptions:(JKParseOptionFlags)parseOptionFlags;
- (id)objectFromJSONStringWithParseOptions:(JKParseOptionFlags)parseOptionFlags error:(NSError **)error;
@end

@interface NSData (JSONKit)
// The NSData needs to be UTF8 encoded JSON.
- (id)objectFromJSONData;
- (id)objectFromJSONDataWithParseOptions:(JKParseOptionFlags)parseOptionFlags;
- (id)objectFromJSONDataWithParseOptions:(JKParseOptionFlags)parseOptionFlags error:(NSError **)error;
@end

@interface NSArray (JSONKit)
- (NSData *)JSONData;
- (NSData *)JSONDataWithOptions:(JKSerializeOptionFlags)serializeOptions error:(NSError **)error;
- (NSString *)JSONString;
- (NSString *)JSONStringWithOptions:(JKSerializeOptionFlags)serializeOptions error:(NSError **)error;
@end

@interface NSDictionary (JSONKit)
- (NSData *)JSONData;
- (NSData *)JSONDataWithOptions:(JKSerializeOptionFlags)serializeOptions error:(NSError **)error;
- (NSString *)JSONString;
- (NSString *)JSONStringWithOptions:(JKSerializeOptionFlags)serializeOptions error:(NSError **)error;
@end

#endif // __OBJC__

#endif // _JSONKIT_H_

#ifdef __cplusplus
}  // extern "C"
#endif
