#include "stdafx.h"
#include "weibo.h"
#import "WBEngine.h"
#import "WBSendView.h"
#import <Foundation/Foundation.h>

#define weiboAppKey @"3199008918"
#define weiboAppSecret @"2bafc0ae54aac2d7fbb2fbf3978988da"

@interface Weibo : NSObject <WBEngineDelegate, WBSendViewDelegate>
{
    WBEngine *engine;
    WBSendView *sendView;
}

- (WBEngine*) getEngine;
- (id)init;
- (void)showSendView;

@end

Weibo* _weibo = nil;

@implementation Weibo
- (id)init
{
    if (self = [super init])
    {
        engine = [[WBEngine alloc] initWithAppKey:weiboAppKey appSecret:weiboAppSecret];
        [engine setDelegate:self];
        sendView = nil;
    }
    return self;
}

- (WBEngine*)getEngine
{
    return engine;
}

- (void)showSendView
{
    if ( sendView ){
        [sendView release];
    }
    sendView = [[WBSendView alloc] initWithAppKey:weiboAppKey appSecret:weiboAppSecret text:@"神作!!!《逼逼逼逼逼》 by @adultsOnly @刀豆的豆 http://itunes.apple.com/us/app/bbbbb/id509209212?ls=1&mt=8" image:[UIImage imageNamed:@"weibo.jpg"]];
    [sendView setDelegate:self];
    [sendView show:YES];
}

- (void)dealloc
{
    [engine setDelegate:nil];
    [engine release], engine = nil;
    if ( sendView ){
        [sendView setDelegate:nil];
        [sendView release], sendView = nil;
    }
    [super dealloc];
}

- (void)engineDidLogIn:(WBEngine *)engine
{
    weiboShowSendView();
}

- (void)engine:(WBEngine *)engine didFailToLogInWithError:(NSError *)error
{
    lwerror("xxxxxxx");
}

- (void)engine:(WBEngine *)engine requestDidSucceedWithResult:(id)result
{
    NSLog(@"requestDidSucceedWithResult: %@", result);
}

- (void)engine:(WBEngine *)engine requestDidFailWithError:(NSError *)error
{
    NSLog(@"requestDidFailWithError: %@", error);
}

- (void)sendViewLogout:(WBSendView *)view
{
    weiboLogout();
}

@end

void weiboInit(){
    _weibo = [[Weibo alloc] init];
}

void weiboQuit(){
    [_weibo release];
}

void weiboLogin(){
    [[_weibo getEngine] logIn];
}

void weiboLogout(){
    [[_weibo getEngine] logOut];
}


bool weiboIsLoggedIn(){
    WBEngine* engin = [_weibo getEngine];
    if ([engin isLoggedIn] && ![engin isAuthorizeExpired]){
        return true;
    }
    return false;
}

void weiboShowSendView(){
    [_weibo showSendView];
}

@interface ShareWebView : UIView <UIWebViewDelegate> 
{
    UIWebView *webView;
    UIButton  *closeButton;
    
}
- (void)home;
@end

@implementation ShareWebView



- (id)init
{
    if (self = [super initWithFrame:CGRectMake(0, 0, 320, 480)])
    {
        
        
        [self setBackgroundColor:[UIColor clearColor]];
        [self setAutoresizingMask:UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight];
        
        webView = [[UIWebView alloc] initWithFrame:CGRectMake(0, 0, 320, 480)];
		[webView setDelegate:self];
		[self addSubview:webView];
        [self home];
        
        closeButton = [UIButton buttonWithType:UIButtonTypeCustom];
		[closeButton setShowsTouchWhenHighlighted:YES];
		[closeButton setFrame:CGRectMake(20, 435, 60, 40)];
		[closeButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
		[closeButton setBackgroundImage:[UIImage imageNamed:@"btn.png"] forState:UIControlStateNormal];
        [closeButton setTitle: NSLocalizedString(@"退出", nil) forState:UIControlStateNormal];
		[closeButton.titleLabel setFont:[UIFont boldSystemFontOfSize:13.0f]];
		[closeButton addTarget:self action:@selector(onCloseButtonTouched:) forControlEvents:UIControlEventTouchUpInside];
		[self addSubview:closeButton];
        
        UIButton *backButton = [UIButton buttonWithType:UIButtonTypeCustom];
        [backButton setShowsTouchWhenHighlighted:YES];
		[backButton setFrame:CGRectMake(240, 435, 60, 40)];
		[backButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
		[backButton setBackgroundImage:[UIImage imageNamed:@"btn.png"] forState:UIControlStateNormal];
		[backButton setTitle: NSLocalizedString(@"返回", nil) forState:UIControlStateNormal];
		[backButton.titleLabel setFont:[UIFont boldSystemFontOfSize:13.0f]];
		[backButton addTarget:self action:@selector(onBackButtonTouched:) forControlEvents:UIControlEventTouchUpInside];
		[self addSubview:backButton];
    }
    return self;
}

- (void)home
{
    NSString *strHtml = @"<body><br/><br/><br/><br/>若登录后没有出现分享界面，请按右下返回键返回本页面后重新进入。<br/><br/>\
    <!-- JiaThis Button BEGIN -->\
    <div id='jiathis_style_32x32'>\
    <a class='jiathis_button_qzone'></a>\
    <a class='jiathis_button_tqq'></a>\
    <a class='jiathis_button_renren'></a>\
    <a class='jiathis_button_douban'></a>\
    <a class='jiathis_button_kaixin001'></a>\
    <a class='jiathis_button_t163'></a>\
    <a class='jiathis_button_tsohu'></a>\
    <a class='jiathis_button_xiaoyou'></a>\
    <a class='jiathis_button_fb'></a>\
    <a class='jiathis_button_twitter'></a>\
    <a href='http://www.jiathis.com/share' class='jiathis jiathis_txt jiathis_separator jtico jtico_jiathis' target='_blank'></a>\
    </div>\
    <script type='text/javascript' >\
    var jiathis_config={\
    url:'http://itunes.apple.com/us/app/bbbbb/id509209212?ls=1&mt=8',\
    summary:'',\
    title:'神作!!!《逼逼逼逼逼》 by @adultsOnly @刀豆的豆 http://itunes.apple.com/us/app/bbbbb/id509209212?ls=1&mt=8',\
    hideMore:true,\
    pic:'http://ww3.sinaimg.cn/large/989412b3tw1drq8nrbiaij.jpg',\
    }\
    </script>\
    <script type='text/javascript' src='http://v2.jiathis.com/code/jia.js' charset='utf-8'></script>\
    <!-- JiaThis Button END -->\
    </body>";
    [webView loadHTMLString:strHtml baseURL:[NSURL fileURLWithPath:[[NSBundle mainBundle] resourcePath]]];
}

- (void)dealloc
{
    [webView release], webView = nil;
    [super dealloc];
}

- (void)onCloseButtonTouched:(id)sender
{
    [self removeFromSuperview];
    [self release];
}

- (void)onBackButtonTouched:(id)sender
{
    [self home];
}

- (void)webViewDidFinishLoad:(UIWebView *)wbView
{
    NSString* script = @"var elem = document.getElementsByClassName('jiathis')[0];elem.parentNode.removeChild(elem);";
    
    [wbView stringByEvaluatingJavaScriptFromString:script];
}

@end

void shareShowView(){
    ShareWebView* shareView = [[ShareWebView alloc] init];
    UIWindow *window = [UIApplication sharedApplication].keyWindow;
    if (!window)
    {
        window = [[UIApplication sharedApplication].windows objectAtIndex:0];
    }
    [window addSubview:shareView];
}


