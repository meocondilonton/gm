//
//  Gold.cpp
//  GoldMiner
//
//  项目GitHub地址:https://github.com/ZhongTaoTian
//  项目思路和架构讲解博客:http://www.jianshu.com/users/5fe7513c7a57/latest_articles
//  微博:http://weibo.com/5622363113/fans?topnav=1&wvr=6&mod=message&need_filter=1

#include "Mouse.hpp"

// Score
#define kMouseScore 100
#define kSmallGoldScore 100
#define kMiddleGoldScore 200
#define kBigGoldScore 400
#define kSmallStoneScore 25
#define kMiddleStoneScore 50
#define kBigStoneScore 75
#define kDiamondScore 500
#define kBagScore (arc4random_uniform(200) + 50)

// Speed
#define kSmallGoldBackSpeed 3
#define kMiddleGolBackSpeed 2
#define kBigGoldBackSpeed 1.5
#define kSmallStoneBackSpeed 3
#define kMiddleStoneBackSpeed 2
#define kBigStoneBackSpeed 1.5
#define kBagBackSpeed 3
#define kDiamondSpeed 3

#define kTimeGo 90

Mouse *Mouse::create(  float scaleX, float scaleY, bool isBuyPotion, bool isBuyDiamonds, bool isStoneBook  , MouseType type , DirectionType direction)
{
    Mouse *mouse = new Mouse();
    
    if (mouse && mouse->init(  scaleX, scaleY, isBuyPotion, isBuyDiamonds, isStoneBook ,type ,direction)) {
        mouse->autorelease();
        return mouse;
    } else {
        delete mouse;
        mouse = nullptr;
    }
    
    return nullptr;
}

void Mouse::getDiamond(){
    this->type =  MouseType::MOUSEWITHDIAMOND;
    
    this->stopAllActions();
    this->createAnimation(this->type);
}

void Mouse::goBack(){
    if(this->directionType == DirectionType::LEFT){
        this->directionType = DirectionType::RIGHT;
        this->setRotationSkewY( 180);
    }else{
         this->directionType = DirectionType::LEFT;
        this->setRotationSkewY( 0);
    }
   
    
    auto screenSize = Director::getInstance()->getWinSize();
    this->start_x =   this->getPosition().x;
    this->end_x =   this->directionType ==  DirectionType::LEFT ?   (screenSize.width + 20) * 2 : -20 -  (screenSize.width + 20) ;
    this->stopAllActions();
    this->createAnimation(this->type);
}


void Mouse::randomPositionY(){
    auto screenSize = Director::getInstance()->getWinSize();
    this->random_Height =  std::rand() % static_cast<int>(screenSize.height - 100) + 50 ;
}

void Mouse::createAnimation( MouseType type ){
    switch (type) {
        case MouseType::EMPTYMOUSE:
        {
            //create  animation mouse
            Sprite::initWithSpriteFrameName("mouse-0.png");
            Vector<SpriteFrame*> animFrames(8);
            char str[100] = {0};
            for(int i = 0; i < 8; i++)
            {
                sprintf(str, "mouse-%d.png",i);
                auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
                animFrames.pushBack(frame);
            }
            
            auto animation = Animation::createWithSpriteFrames(animFrames, 0.2f);
            auto animate = Animate::create(animation);
            this->runAction(RepeatForever::create(animate));
            
            auto callbackChangePosition =  CallFunc::create([&, this](){
                log("Rotated!");
                auto screenSize = Director::getInstance()->getWinSize();
                this->random_Height =  std::rand() % static_cast<int>(screenSize.height - 100) + 50 ;
            });
            auto movement = MoveTo::create(kTimeGo, Vec2( this->end_x,random_Height));
            auto resetPosition = MoveTo::create(0, Vec2(this->start_x,random_Height));
            auto sequence = Sequence::create(movement, callbackChangePosition, resetPosition, NULL);
            this->runAction(RepeatForever::create(sequence));
        }
            
            break;
        case MouseType::MOUSEWITHDIAMOND:
        {
            //create  animation mouse
            Sprite::initWithSpriteFrameName("gem-mouse-0.png");
            Vector<SpriteFrame*> animFrames(8);
            char str[100] = {0};
            for(int i = 0; i < 8; i++)
            {
                sprintf(str, "gem-mouse-%d.png",i);
                auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
                animFrames.pushBack(frame);
            }
            
            auto animation = Animation::createWithSpriteFrames(animFrames, 0.2f);
            auto animate = Animate::create(animation);
            this->runAction(RepeatForever::create(animate));
            
            auto callbackChangePosition =  CallFunc::create([&, this](){
                log("Rotated!");
                auto screenSize = Director::getInstance()->getWinSize();
                this->random_Height =  std::rand() % static_cast<int>(screenSize.height - 100) + 50 ;
            });
            auto movement = MoveTo::create(kTimeGo*0.75, Vec2(this->end_x,random_Height));
            auto resetPosition = MoveTo::create(0, Vec2(this->start_x,random_Height));
            auto sequence = Sequence::create(movement, callbackChangePosition ,resetPosition, NULL);
            this->runAction(RepeatForever::create(sequence));
        }
            break;
        case MouseType::MOUSECATCH:
        {
            //create  animation mouse
            Sprite::initWithSpriteFrameName("pulled-gem-mouse-0.png");
            Vector<SpriteFrame*> animFrames(8);
            char str[100] = {0};
            for(int i = 0; i < 8; i++)
            {
                sprintf(str, "pulled-gem-mouse-%d.png",i);
                auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
                animFrames.pushBack(frame);
            }
            
            auto animation = Animation::createWithSpriteFrames(animFrames, 0.2f);
            auto animate = Animate::create(animation);
            this->runAction(RepeatForever::create(animate));
            
            
        }
            break;
            
        default:
            break;
            
            
    }
}

bool Mouse::init(  float scaleX, float scaleY , bool isBuyPotion, bool isBuyDiamonds, bool isStoneBook  , MouseType type , DirectionType direction)
{
    this->type = type;
    this->directionType = direction;
    
    
    if (isBuyPotion) power = 1.2;
    if (isBuyDiamonds) diamondsCoe = 3;
    if (isStoneBook) stoneCoe = 3;
    
    int scale = ((int)(scaleX * 100));
    this->setScale(scaleX, scaleY);
  
    
    if( direction ==  DirectionType::RIGHT ){
        this->setRotationSkewY(180);
    }
    
    score = kMouseScore;
    backSpeed = kSmallGoldBackSpeed * power;
    hookRote = 16;
//    this->setPosition(7.52, -21.24);
    this->setAnchorPoint(Vec2(0.5, 0.5));
     auto screenSize = Director::getInstance()->getWinSize();
   
    this->random_Width = direction == DirectionType::LEFT ?  -20 : screenSize.width + 20;
    this->random_Height = std::rand() % static_cast<int>(screenSize.height - 100) + 50 ;
    this->start_x =  direction ==  DirectionType::LEFT ?  -20 : screenSize.width + 20;
    this->end_x =  direction ==  DirectionType::LEFT ?   (screenSize.width + 20) * 2 : -20 -  (screenSize.width + 20) ;
    
    this->setPosition(random_Width, random_Height);

//    CCLOG("random_number: %d", random_Width  );
    this->createAnimation(type);
    
 
    
    return true;
}

