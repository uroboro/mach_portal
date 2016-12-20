//
//  AppGroup.m
//  mach_portal
//
//  Created by uroboro on 12/19/16.
//  Copyright © 2016 uroboro. All rights reserved.
//

#import <Foundation/Foundation.h>

char * copyAppGroup() {
	NSString * path = [NSBundle.mainBundle pathForResource:@"mach_portal" ofType:@"entitlements"];
	NSDictionary * dict = [NSDictionary dictionaryWithContentsOfFile:path];
	NSString * appGroup = [dict[@"com.apple.security.application-groups"] firstObject];
	return strdup(appGroup.UTF8String);
}
