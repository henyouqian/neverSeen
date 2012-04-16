#include "stdafx.h"
#include "adView.h"
#import "lwAppDelegate.h"
#import "store.h"
#import "GADBannerView.h"

namespace {
    GADBannerView *bannerView_ = nil;
    bool _admobShow = true;
}

void createAdmob(){
    return;
    if ( isProVersion() ){
        return;
    }
    if ( bannerView_ == nil ){
        bannerView_ = [[GADBannerView alloc]
                       initWithFrame:CGRectMake(0.0,
                                                0.0,
                                                GAD_SIZE_320x50.width,
                                                GAD_SIZE_320x50.height)];
        
        // Specify the ad's "unit identifier." This is your AdMob Publisher ID.
        bannerView_.adUnitID = @"a14f59073060a5a";
        
        // Let the runtime know which UIViewController to restore after taking
        // the user wherever the ad goes and add it to the view hierarchy.
        bannerView_.rootViewController = getViewController();
        [getViewController().view addSubview:bannerView_];
        
        // Initiate a generic request to load it with an ad.
        [bannerView_ loadRequest:[GADRequest request]];
    }
}

void deleteAdmob(){
    return;
    if ( bannerView_ ){
        [bannerView_ removeFromSuperview];
        [bannerView_ release];
        bannerView_ = nil;
    }
}

void showAdmob(bool show){
    if ( !bannerView_ ){
        return;
    }
    if ( _admobShow == show ){
        return;
    }
    if ( show ){
        [getViewController().view addSubview:bannerView_];
        [bannerView_ release];
    }else{
        [bannerView_ retain];
        [bannerView_ removeFromSuperview];
    }
    _admobShow = show;
}
