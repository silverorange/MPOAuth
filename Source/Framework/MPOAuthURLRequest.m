//
//  MPOAuthURLRequest.m
//  MPOAuthConnection
//
//  Created by Karl Adam on 08.12.05.
//  Copyright 2008 matrixPointer. All rights reserved.
//

#import "MPOAuthURLRequest.h"
#import "MPURLRequestParameter.h"
#import "MPOAuthSignatureParameter.h"

#import "NSURL+MPURLParameterAdditions.h"
#import "NSString+URLEscapingAdditions.h"

@interface MPOAuthURLRequest ()
@property (nonatomic, readwrite, strong) NSURLRequest *urlRequest;
@end

@implementation MPOAuthURLRequest

- (id)initWithURL:(NSURL *)inURL andParameters:(NSArray *)inParameters {
	if ((self = [super init])) {
		self.url = inURL;
		_parameters = inParameters ? [inParameters mutableCopy] : [[NSMutableArray alloc] initWithCapacity:10];
		self.HTTPMethod = @"GET";
	}
	return self;
}

- (id)initWithURLRequest:(NSURLRequest *)inRequest {
	if ((self = [super init])) {
		self.url = [[inRequest URL] urlByRemovingQuery];
		self.parameters = [[MPURLRequestParameter parametersFromString:[[inRequest URL] query]] mutableCopy];
		self.HTTPMethod = [inRequest HTTPMethod];
		self.urlRequest = [inRequest mutableCopy];
	}
	return self;
}


@synthesize url = _url;
@synthesize HTTPMethod = _httpMethod;
@synthesize urlRequest = _urlRequest;
@synthesize parameters = _parameters;

#pragma mark -

- (NSArray *)nonOAuthParameters {
	NSArray *oauthParameters = [NSArray arrayWithObjects:@"oauth_signature", @"oauth_nonce", @"oauth_token", @"oauth_consumer_key", @"oauth_timestamp", @"oauth_version", @"oauth_signature_method", @"realm", nil];
	NSPredicate *filterPredicate = [NSPredicate predicateWithFormat:@"!(name IN %@)", oauthParameters];
	return [self.parameters filteredArrayUsingPredicate:filterPredicate];
}

- (NSArray *)signatureParameters {
    NSArray *nonSignatureParameters = [NSArray arrayWithObjects:@"realm", nil];
	NSPredicate *filterPredicate = [NSPredicate predicateWithFormat:@"!(name IN %@)", nonSignatureParameters];
	return [self.parameters filteredArrayUsingPredicate:filterPredicate];
}

- (NSString *)authorizationHeaderValueFromParameterString:(NSString *)parameterString {
	NSDictionary *paramsDict = [MPURLRequestParameter parameterDictionaryFromString:parameterString]; // TODO: this might break if multiple params with same name
    NSString *realm = [[paramsDict objectForKey:@"realm"] stringByAddingURIPercentEscapesUsingEncoding:NSUTF8StringEncoding];
	NSString *signature = [[paramsDict objectForKey:@"oauth_signature"] stringByAddingURIPercentEscapesUsingEncoding:NSUTF8StringEncoding];
	NSString *nonce = [[paramsDict objectForKey:@"oauth_nonce"] stringByAddingURIPercentEscapesUsingEncoding:NSUTF8StringEncoding];
	NSString *token = [[paramsDict objectForKey:@"oauth_token"] stringByAddingURIPercentEscapesUsingEncoding:NSUTF8StringEncoding];
	NSString *consumerKey = [[paramsDict objectForKey:@"oauth_consumer_key"] stringByAddingURIPercentEscapesUsingEncoding:NSUTF8StringEncoding];
	NSString *timeStamp = [[paramsDict objectForKey:@"oauth_timestamp"] stringByAddingURIPercentEscapesUsingEncoding:NSUTF8StringEncoding];
	NSString *version = [[paramsDict objectForKey:@"oauth_version"] stringByAddingURIPercentEscapesUsingEncoding:NSUTF8StringEncoding];
	NSString *method = [[paramsDict objectForKey:@"oauth_signature_method"] stringByAddingURIPercentEscapesUsingEncoding:NSUTF8StringEncoding];
	NSString *fullAuthString = nil;
	
	if (token) {
        if (realm) {
            fullAuthString = [NSString stringWithFormat:@"OAuth realm=\"%@\",oauth_token=\"%@\",oauth_consumer_key=\"%@\",oauth_version=\"%@\",oauth_signature_method=\"%@\", oauth_timestamp=\"%@\",oauth_nonce=\"%@\",oauth_signature=\"%@\"", realm, token, consumerKey, version, method, timeStamp, nonce, signature];
        } else {
            fullAuthString = [NSString stringWithFormat:@"OAuth oauth_token=\"%@\",oauth_consumer_key=\"%@\",oauth_version=\"%@\",oauth_signature_method=\"%@\", oauth_timestamp=\"%@\",oauth_nonce=\"%@\",oauth_signature=\"%@\"", token, consumerKey, version, method, timeStamp, nonce, signature];
        }
	} else {
        if (realm) {
            fullAuthString = [NSString stringWithFormat:@"OAuth realm=\"%@\",oauth_consumer_key=\"%@\",oauth_version=\"%@\",oauth_signature_method=\"%@\", oauth_timestamp=\"%@\",oauth_nonce=\"%@\",oauth_signature=\"%@\"", realm, consumerKey, version, method, timeStamp, nonce, signature];
        } else {
            fullAuthString = [NSString stringWithFormat:@"OAuth oauth_consumer_key=\"%@\",oauth_version=\"%@\",oauth_signature_method=\"%@\", oauth_timestamp=\"%@\",oauth_nonce=\"%@\",oauth_signature=\"%@\"", consumerKey, version, method, timeStamp, nonce, signature];
        }
	}

	return fullAuthString;
}

- (NSURLRequest  *)urlRequestSignedWithSecret:(NSString *)inSecret usingMethod:(NSString *)inScheme {
	NSMutableURLRequest *aRequest = [self.urlRequest mutableCopy];
	[self.parameters sortUsingSelector:@selector(compare:)];

	if (!aRequest ) {
		aRequest = [[NSMutableURLRequest alloc] init];
	}
	
	NSString *urlString = nil;
    NSMutableString *parameterString = [[NSMutableString alloc] initWithString:[MPURLRequestParameter parameterStringForParameters:self.parameters]];
    NSString *signatureParameterString = [MPURLRequestParameter parameterStringForParameters:[self signatureParameters]];
	MPOAuthSignatureParameter *signatureParameter = [[MPOAuthSignatureParameter alloc] initWithText:signatureParameterString andSecret:inSecret forRequest:self usingMethod:inScheme];

	[parameterString appendFormat:@"&%@", [signatureParameter URLEncodedParameterString]];
	[aRequest setHTTPMethod:self.HTTPMethod];
	
	if ([[self HTTPMethod] isEqualToString:@"GET"] && [self.parameters count]) {
		urlString = [NSString stringWithFormat:@"%@?%@", [self.url absoluteString], parameterString];
	} else if  ([[self HTTPMethod] isEqualToString:@"POST"]) {
		NSArray *nonOauthParameters = [self nonOAuthParameters];
		urlString = [self.url absoluteString];
		[aRequest setValue: [self authorizationHeaderValueFromParameterString:parameterString] forHTTPHeaderField:@"Authorization"];

		if ([nonOauthParameters count] && [aRequest HTTPBody]) {
			[NSException raise:@"MalformedHTTPPOSTMethodException" format:@"The request has both an HTTP Body and additional parameters. This is not supported."];
		} else if ([nonOauthParameters count]) {
			NSString *postDataString = [MPURLRequestParameter parameterStringForParameters:nonOauthParameters];
			NSData *postData = [postDataString dataUsingEncoding:NSUTF8StringEncoding];
			MPLog(@"postDataString - %@", postDataString);
			
			[aRequest setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-Type"];
			[aRequest setValue:[NSString stringWithFormat:@"%lu", (unsigned long)[postData length]] forHTTPHeaderField:@"Content-Length"];
			[aRequest setHTTPBody:postData];
		}
	} else {
		[NSException raise:@"UnhandledHTTPMethodException" format:@"The requested HTTP method, %@, is not supported", self.HTTPMethod];
	}

	MPLog( @"urlString - %@", urlString);
	[aRequest setURL:[NSURL URLWithString:urlString]];
	self.urlRequest = aRequest;
	
	
	return aRequest;
}

#pragma mark -

- (void)addParameters:(NSArray *)inParameters {
	[self.parameters addObjectsFromArray:inParameters];
}

@end
