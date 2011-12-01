#import "JSONKit.h"

@interface SanityTest : SenTestCase
@end

@implementation SanityTest

- (void) testBasicJSONParsing
{
    id result = [@"{\"foo\": \"bar\"}" objectFromJSONString];
    STAssertEqualObjects(result,
        [NSDictionary dictionaryWithObject:@"bar" forKey:@"foo"],
        @"Basic JSON parsing works.");
}

@end
