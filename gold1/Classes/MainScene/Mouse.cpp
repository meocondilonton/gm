//
//  Gold.cpp
//  GoldMiner
//
//  项目GitHub地址:https://github.com/ZhongTaoTian
//  项目思路和架构讲解博客:http://www.jianshu.com/users/5fe7513c7a57/latest_articles
//  微博:http://weibo.com/5622363113/fans?topnav=1&wvr=6&mod=message&need_filter=1

#include "Mouse.hpp"

// Score
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

Mouse *Mouse::create(  float scaleX, float scaleY, float rotate, bool isBuyPotion, bool isBuyDiamonds, bool isStoneBook  , MouseType type)
{
    Mouse *gold = new Mouse();
    
    if (gold && gold->init(  scaleX, scaleY, rotate, isBuyPotion, isBuyDiamonds, isStoneBook ,type)) {
        gold->autorelease();
        return gold;
    } else {
        delete gold;
        gold = nullptr;
    }
    
    return nullptr;
}

bool Mouse::init(  float scaleX, float scaleY, float rotate, bool isBuyPotion, bool isBuyDiamonds, bool isStoneBook  , MouseType type)
{
    this->type = type;
    
    
    if (isBuyPotion) power = 1.2;
    if (isBuyDiamonds) diamondsCoe = 3;
    if (isStoneBook) stoneCoe = 3;
    
    int scale = ((int)(scaleX * 100));
    this->setScale(scaleX, scaleY);
    this->setRotation(rotate);
    
    
    score = kSmallGoldScore;
    backSpeed = kSmallGoldBackSpeed * power;
    hookRote = 16;
    this->setPosition(7.52, -21.24);
    this->setAnchorPoint(Vec2(0.5, 0.5));
    
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
                //SpriteFrame::create(str,Rect(0,0,60,50)); //we assume that the sprites' dimentions are 40*40 rectangles.
                animFrames.pushBack(frame);
            }
            
            auto animation = Animation::createWithSpriteFrames(animFrames, 0.2f);
            auto animate = Animate::create(animation);
            this->runAction(RepeatForever::create(animate));
            
            auto movement = MoveTo::create(15, Vec2(1048,620));
            auto resetPosition = MoveTo::create(1, Vec2(0,620));
            auto sequence = Sequence::create(movement, resetPosition, NULL);
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
                //SpriteFrame::create(str,Rect(0,0,60,50)); //we assume that the sprites' dimentions are 40*40 rectangles.
                animFrames.pushBack(frame);
            }
            
            auto animation = Animation::createWithSpriteFrames(animFrames, 0.2f);
            auto animate = Animate::create(animation);
            this->runAction(RepeatForever::create(animate));
            
            auto movement = MoveTo::create(30, Vec2(1048,620));
            auto resetPosition = MoveTo::create(1, Vec2(0,620));
            auto sequence = Sequence::create(movement, resetPosition, NULL);
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
                //SpriteFrame::create(str,Rect(0,0,60,50)); //we assume that the sprites' dimentions are 40*40 rectangles.
                animFrames.pushBack(frame);
            }
            
            auto animation = Animation::createWithSpriteFrames(animFrames, 0.2f);
            auto animate = Animate::create(animation);
            this->runAction(RepeatForever::create(animate));
            
            auto movement = MoveTo::create(45, Vec2(1048,620));
            auto resetPosition = MoveTo::create(1, Vec2(0,620));
            auto sequence = Sequence::create(movement, resetPosition, NULL);
            this->runAction(RepeatForever::create(sequence));
        }
            break;
            
        default:
            break;
    }
    
 
    
    return true;
}

