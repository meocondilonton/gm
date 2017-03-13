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

#define kTimeGo 0.5

Mouse *Mouse::create(  float scaleX, float scaleY, bool isBuyPotion, bool isBuyDiamonds, bool isStoneBook  , MouseType type , DirectionType direction , Vec2 postition)
{
    Mouse *mouse = new Mouse();
    
    if (mouse && mouse->init(  scaleX, scaleY, isBuyPotion, isBuyDiamonds, isStoneBook ,type ,direction , postition)) {
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
    this->createAnimation(this->type, this->directionType);
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
    this->stopAllActions();
    this->createAnimation(this->type, this->directionType);
}

void Mouse::goToLeft(){
    
    if(this->directionType == DirectionType::LEFT){
    }else{
        this->directionType = DirectionType::LEFT;
        this->setRotationSkewY( 0);
        
    }
    auto screenSize = Director::getInstance()->getWinSize();
    this->stopAllActions();
    this->createAnimation(this->type, this->directionType);
}

void Mouse::goToRight(){
    
    if(this->directionType == DirectionType::LEFT){
        this->directionType = DirectionType::RIGHT;
        this->setRotationSkewY( 180);
        
    }else{
        
    }
    auto screenSize = Director::getInstance()->getWinSize();
    this->stopAllActions();
    this->createAnimation(this->type, this->directionType);
}


void Mouse::checkPositionAndGoBack(){
     auto screenSize = Director::getInstance()->getWinSize();
    if(this->getPosition().x < -50 ){
        this->goToRight();
    }else if(this->getPosition().x > screenSize.width + 50){
        this->goToLeft();
    }
}

void Mouse::randomPositionY(){
    auto screenSize = Director::getInstance()->getWinSize();
    int ran = std::rand() % static_cast<int>(7);
    this->random_Height =   ((screenSize.height - 260 )/7) * ran + 26   ;
}

void Mouse::createAnimation( MouseType type ,DirectionType direction){
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
                this->checkPositionAndGoBack();
            });
              auto denta_x = direction == DirectionType::LEFT ? 10 : -10;
              auto movement = MoveBy::create(kTimeGo*0.75,  Vec2( denta_x,0));
            auto sequence = Sequence::create(movement, callbackChangePosition, NULL);
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
 
                this->checkPositionAndGoBack();
            });
 
            auto denta_x = direction == DirectionType::LEFT ? 10 : -10;
            auto movement = MoveBy::create(kTimeGo,  Vec2( denta_x,0));
            
            auto sequence = Sequence::create(movement, callbackChangePosition , NULL);
            this->runAction(RepeatForever::create(sequence));
        }
            break;
        case MouseType::MOUSECATCH:
        {
            //create  animation mouse
            this->setPosition(7.52, -21.24);
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

bool Mouse::init(  float scaleX, float scaleY , bool isBuyPotion, bool isBuyDiamonds, bool isStoneBook  , MouseType type , DirectionType direction , Vec2 postition)
{
    this->type = type;
    this->directionType = direction;
    
    
    if (isBuyPotion) power = 1.2;
    if (isBuyDiamonds) diamondsCoe = 3;
    if (isStoneBook) stoneCoe = 3;
 
    this->setScale(scaleX, scaleY);
  
    
    if( direction ==  DirectionType::RIGHT ){
        this->setRotationSkewY(180);
    }
    
    score = kMouseScore;
    backSpeed = kSmallGoldBackSpeed * power;
    hookRote = 16;
    this->setAnchorPoint(Vec2(0.5, 0.5));
     auto screenSize = Director::getInstance()->getWinSize();
   
//    auto start_x = direction == DirectionType::LEFT ?  -20 : screenSize.width + 20;
//    this->randomPositionY();
    this->setPosition(postition.x, postition.y);
 
    this->createAnimation(type , this->directionType);
    
 
    
    return true;
}

