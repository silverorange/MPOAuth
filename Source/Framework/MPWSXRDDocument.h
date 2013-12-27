//
//  MPWSXRDDocument.h
//  MPWebServices
//
//  Created by Karl Adam on 09.03.31.
//  Copyright 2009 matrixPointer. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface MPWSXRDDocument : NSObject {
	NSString		*_subject;
	NSDate			*_expirationDate;
	NSDictionary	*_urlRelationships;
}

@property (nonatomic, readonly, strong) NSString *subject;
@property (nonatomic, readonly, strong) NSDate *expirationDate;
@property (nonatomic, readonly, strong) NSDictionary *urlRelationships;

- (id)initFromURL:(NSURL *)inURL;
- (id)iniWithString:(NSString *)inString;
- (NSURL *)urlForRelationship:(NSString *)inRelationshipType;

@end
