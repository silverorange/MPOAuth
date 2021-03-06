//
//  MPOAuthAuthenticationMethodOAuth.h
//  MPOAuthConnection
//
//  Created by Karl Adam on 09.12.19.
//  Copyright 2009 matrixPointer. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MPOAuthAuthenticationMethod.h"
#import "MPOAuthAPI.h"
#import "MPOAuthAPIRequestLoader.h"

extern NSString * const MPOAuthRequestTokenURLKey;
extern NSString * const MPOAuthUserAuthorizationURLKey;
extern NSString * const MPOAuthUserAuthorizationMobileURLKey;

extern NSString * const MPOAuthNotificationRequestTokenReceived;
extern NSString * const MPOAuthNotificationRequestTokenRejected;

@protocol MPOAuthAuthenticationMethodOAuthDelegate;

@interface MPOAuthAuthenticationMethodOAuth : MPOAuthAuthenticationMethod <MPOAuthAPIInternalClient> {
	NSURL									*oauthRequestTokenURL_;
	NSURL									*oauthAuthorizeTokenURL_;
	BOOL									oauth10aModeActive_;
	
	id <MPOAuthAuthenticationMethodOAuthDelegate> __weak delegate_;
}

@property (nonatomic, readwrite, weak) id <MPOAuthAuthenticationMethodOAuthDelegate> delegate;

@property (nonatomic, readwrite, strong) NSURL *oauthRequestTokenURL;
@property (nonatomic, readwrite, strong) NSURL *oauthAuthorizeTokenURL;

- (void)authenticate;

@end

@protocol MPOAuthAuthenticationMethodOAuthDelegate <NSObject>
- (NSURL *)callbackURLForCompletedUserAuthorization;
- (BOOL)automaticallyRequestAuthenticationFromURL:(NSURL *)inAuthURL withCallbackURL:(NSURL *)inCallbackURL;

@optional
- (NSString *)oauthVerifierForCompletedUserAuthorization;
- (void)authenticationDidFailWithError:(NSError *)error;
@end

