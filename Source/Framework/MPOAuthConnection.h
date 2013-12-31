//
//  MPOAuthConnection.h
//  MPOAuthConnection
//
//  Created by Karl Adam on 08.12.05.
//  Copyright 2008 matrixPointer. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol MPOAuthCredentialStore;
@protocol MPOAuthParameterFactory;

@class MPOAuthURLRequest;
@class MPOAuthURLResponse;
@class MPOAuthCredentialConcreteStore;

@interface MPOAuthConnection : NSURLConnection {
@private
	MPOAuthCredentialConcreteStore *__weak _credentials;
}

@property (weak, nonatomic, readonly) id <MPOAuthCredentialStore, MPOAuthParameterFactory> credentials;

+ (MPOAuthConnection *)connectionWithRequest:(MPOAuthURLRequest *)inRequest delegate:(id)inDelegate credentials:(NSObject <MPOAuthCredentialStore, MPOAuthParameterFactory> *)inCredentials;
+ (NSData *)sendSynchronousRequest:(MPOAuthURLRequest *)inRequest usingCredentials:(NSObject <MPOAuthCredentialStore, MPOAuthParameterFactory> *)inCredentials returningResponse:(MPOAuthURLResponse **)outResponse error:(NSError **)inError;
+ (void)sendAsynchronousRequest:(MPOAuthURLRequest *)inRequest usingCredentials:(NSObject <MPOAuthCredentialStore, MPOAuthParameterFactory> *)inCredentials queue:(NSOperationQueue *)queue completionHandler:(void (^)(MPOAuthURLResponse *, NSData *, NSError *))handler;
- (id)initWithRequest:(MPOAuthURLRequest *)inRequest delegate:(id)inDelegate credentials:(NSObject <MPOAuthCredentialStore, MPOAuthParameterFactory> *)inCredentials;

@end
