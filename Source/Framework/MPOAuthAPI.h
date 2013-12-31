//
//  MPOAuthAPI.h
//  MPOAuthConnection
//
//  Created by Karl Adam on 08.12.05.
//  Copyright 2008 matrixPointer. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MPOAuthCredentialStore.h"
#import "MPOAuthParameterFactory.h"
#import "MPOAuthURLResponse.h"

extern NSString * const MPOAuthNotificationAccessTokenReceived;
extern NSString * const MPOAuthNotificationAccessTokenRejected;
extern NSString * const MPOAuthNotificationAccessTokenRefreshed;
extern NSString * const MPOAuthNotificationOAuthCredentialsReady;
extern NSString * const MPOAuthNotificationErrorHasOccurred;

extern NSString * const MPOAuthCredentialRequestTokenKey;
extern NSString * const MPOAuthCredentialRequestTokenSecretKey;
extern NSString * const MPOAuthCredentialAccessTokenKey;
extern NSString * const MPOAuthCredentialAccessTokenSecretKey;
extern NSString * const MPOAuthCredentialSessionHandleKey;

extern NSString * const MPOAuthTokenRefreshDateDefaultsKey;

extern NSString * const MPOAuthBaseURLKey;
extern NSString * const MPOAuthAuthenticationURLKey;

typedef enum {
	MPOAuthSignatureSchemePlainText,
	MPOAuthSignatureSchemeHMACSHA1,
	MPOAuthSignatureSchemeRSASHA1
} MPOAuthSignatureScheme;

typedef enum {
	MPOAuthAuthenticationStateUnauthenticated		= 0,
	MPOAuthAuthenticationStateAuthenticating		= 1,
	MPOAuthAuthenticationStateAuthenticated			= 2
} MPOAuthAuthenticationState;

//typedef void (^)(MPOAuthURLResponse *, NSData *, NSError *) performBlock;
typedef void(^performBlockType)(MPOAuthURLResponse *, NSData *, NSError *);

@protocol MPOAuthAPIInternalClient
@end

@class MPOAuthAuthenticationMethod;

@interface MPOAuthAPI : NSObject <MPOAuthAPIInternalClient> {
@private
	id <MPOAuthCredentialStore, MPOAuthParameterFactory>		credentials_;
	NSURL														*baseURL_;
	NSURL														*authenticationURL_;
	MPOAuthAuthenticationMethod									*authenticationMethod_;
	MPOAuthSignatureScheme										signatureScheme_;
	NSMutableArray												*activeLoaders_;
	MPOAuthAuthenticationState									oauthAuthenticationState_;
    NSOperationQueue                                            *asyncQueue;
}

@property (nonatomic, readonly, strong) id <MPOAuthCredentialStore, MPOAuthParameterFactory> credentials;
@property (nonatomic, readonly, strong) NSURL *baseURL;
@property (nonatomic, readonly, strong) NSURL *authenticationURL;
@property (nonatomic, readwrite, strong) MPOAuthAuthenticationMethod *authenticationMethod;
@property (nonatomic, readwrite, assign) MPOAuthSignatureScheme signatureScheme;

@property (nonatomic, readonly, assign) MPOAuthAuthenticationState authenticationState;


- (id)initWithCredentials:(NSDictionary *)inCredentials andBaseURL:(NSURL *)inURL;
- (id)initWithCredentials:(NSDictionary *)inCredentials authenticationURL:(NSURL *)inAuthURL andBaseURL:(NSURL *)inBaseURL;
- (id)initWithCredentials:(NSDictionary *)inCredentials authenticationURL:(NSURL *)inAuthURL andBaseURL:(NSURL *)inBaseURL autoStart:(BOOL)aFlag;
- (id)initWithCredentials:(NSDictionary *)inCredentials withConfiguration:(NSDictionary *)inConfiguration autoStart:(BOOL)aFlag;

- (void)authenticate;
- (BOOL)isAuthenticated;

- (void)performMethod:(NSString *)inMethod then:(performBlockType)handler;
- (void)performMethod:(NSString *)inMethod withParameters:(NSArray *)inParameters then:(performBlockType)handler;
- (void)performMethod:(NSString *)inMethod atURL:(NSURL *)inURL withParameters:(NSArray *)inParameters then:(performBlockType)handler;
- (void)performPOSTMethod:(NSString *)inMethod withParameters:(NSArray *)inParameters then:(performBlockType)handler;
- (void)performPOSTMethod:(NSString *)inMethod atURL:(NSURL *)inURL withParameters:(NSArray *)inParameters then:(performBlockType)handler;
- (void)performURLRequest:(NSURLRequest *)inRequest then:(performBlockType)handler;

- (void)performMethod:(NSString *)inMethod withTarget:(id)inTarget andAction:(SEL)inAction;
- (void)performMethod:(NSString *)inMethod withParameters:(NSArray *)inParameters withTarget:(id)inTarget andAction:(SEL)inAction;
- (void)performMethod:(NSString *)inMethod atURL:(NSURL *)inURL withParameters:(NSArray *)inParameters withTarget:(id)inTarget andAction:(SEL)inAction;
- (void)performPOSTMethod:(NSString *)inMethod withParameters:(NSArray *)inParameters withTarget:(id)inTarget andAction:(SEL)inAction;
- (void)performPOSTMethod:(NSString *)inMethod atURL:(NSURL *)inURL withParameters:(NSArray *)inParameters withTarget:(id)inTarget andAction:(SEL)inAction;
- (void)performURLRequest:(NSURLRequest *)inRequest withTarget:(id)inTarget andAction:(SEL)inAction;

- (NSData *)dataForMethod:(NSString *)inMethod;
- (NSData *)dataForMethod:(NSString *)inMethod withParameters:(NSArray *)inParameters;
- (NSData *)dataForURL:(NSURL *)inURL andMethod:(NSString *)inMethod withParameters:(NSArray *)inParameters;

- (id)credentialNamed:(NSString *)inCredentialName;
- (void)setCredential:(id)inCredential withName:(NSString *)inName;
- (void)removeCredentialNamed:(NSString *)inName;

- (void)discardCredentials;

@end
