//
//  Game.cpp
//  GoldMiner
//
//  Created by 维尼的小熊 on 16/11/27.
//  项目GitHub地址:https://github.com/ZhongTaoTian
//  项目思路和架构讲解博客:http://www.jianshu.com/users/5fe7513c7a57/latest_articles
//  微博:http://weibo.com/5622363113/fans?topnav=1&wvr=6&mod=message&need_filter=1
//

#include "Game.hpp"
#include "Pause.hpp"
#include "UserDataManager.hpp"
#include "StageTipsLayer.hpp"
#include "Shop.hpp"

#define kWorldTag 1000
#define tagGold 100
#define tagMouse 2000
#define tagHook 59
#define tagMiddleCircle 500
#define tagTnt 1500

// distance tnt
#define kdistanceTnt 200

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


Scene *Game::createScene(bool isBuyBomb, bool isBuyPotion, bool isBuyDiamonds, bool isStoneBook, int payMoney)
{
    Scene *scene = Scene::createWithPhysics();
    
    auto world = scene->getPhysicsWorld();
//    world->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    world->setGravity(Vec2::ZERO);
    
    auto gameLayer = Game::create(isBuyBomb, isBuyPotion, isBuyDiamonds, isStoneBook, payMoney);
    scene->addChild(gameLayer);
    
    PhysicsBody *body = PhysicsBody::createEdgeBox(Size(kWinSizeWidth, kWinSizeHeight));
    body->setCategoryBitmask(10);
    body->setCollisionBitmask(10);
    body->setContactTestBitmask(10);
    
    Node *node = Node::create();
    node->setPosition(kWinSize * 0.5);
    node->addComponent(body);
    node->setColor(Color3B::RED);
    node->setTag(kWorldTag);
    scene->addChild(node);
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/Resources/level-sheet.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/Resources/explosion-sheet.plist");
    
    return scene;
}

Game *Game::create(bool isBuyBomb, bool isBuyPotion, bool isBuyDiamonds, bool isStoneBook, int payMoney)
{
    Game *pRet = new Game();
    if (pRet && pRet->init(isBuyBomb, isBuyPotion, isBuyDiamonds, isStoneBook, payMoney))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

bool Game::init(bool isBuyBomb, bool isBuyPotion, bool isBuyDiamonds, bool isStoneBook, int payMoney)
{
    if (!Layer::init()) return false;
    
    auto csb = CSLoader::createNode("GameLayer.csb");
    this->addChild(csb, 10);
    
    this->isBuyPotion = isBuyPotion;
    this->isBuyDiamonds = isBuyDiamonds;
    this->isBuyStoneBook = isStoneBook;
    
   
    
    bompButton = static_cast<Button *>(Helper::seekWidgetByName(static_cast<Widget *>(csb), "BompButton"));
    bompButton->setVisible(isBuyBomb);
    bompButton->addTouchEventListener([=](Ref *ref, Widget::TouchEventType type){
        if (type == Widget::TouchEventType::ENDED) {
            // click bomp
            if (isOpenHook) {
                bompButton->setVisible(false);
                // 炸毁物品
                backSpeed = 10;
                
                isOpenHook = false;
                leftHook->setRotation(0);
                rightHook->setRotation(0);
                
                //爆炸效果
                auto postion = rope->convertToWorldSpace(middleCircle->getPosition());
                ParticleSystemQuad *bompEm = ParticleSystemQuad::create("res/Boom.plist");
                bompEm->setPosition(postion);
                this->addChild(bompEm);
                
                SoundTool::getInstance()->playEffect("res/music/bomb.mp3");
                
                goldSprite->removeFromParent();
                
            }
        }
    });
    
    // 获取序列帧动画
    minerTimeLine = CSLoader::createTimeline("GameLayer.csb");
    this->runAction(minerTimeLine);
    
    auto hookCsb = CSLoader::createNode("Hook.csb");
    hookCsb->setPosition(kWinSizeWidth * 0.48, kWinSizeHeight * 0.856);
    this->addChild(hookCsb, 11);
    
    rope = static_cast<ImageView *>(Helper::seekWidgetByName(static_cast<Widget *>(hookCsb), "rope"));
    middleCircle = static_cast<Sprite *>(rope->getChildByTag(59));
    leftHook = static_cast<Sprite *>(middleCircle->getChildByTag(60));
    rightHook = static_cast<Sprite *>(middleCircle->getChildByTag(61));
    curPayMoney = payMoney;
    
    // 添加钩子刚体
    PhysicsBody *hookBody = PhysicsBody::createCircle(20);
    hookBody->setContactTestBitmask(10);
    hookBody->setCollisionBitmask(10);
    hookBody->setCategoryBitmask(10);
    middleCircle->addComponent(hookBody);
    circlePosition = middleCircle->getPosition();
    this->addButtonAction(csb);
    
    setUpText(static_cast<Widget *>(csb));
    
    timeCount = 60;
    
    // 添加碰撞事件
    auto physicsListener = EventListenerPhysicsContact::create();
    physicsListener->onContactBegin = CC_CALLBACK_1(Game::physicsBegin, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(physicsListener, this);
    _eventDispatcher->removeCustomEventListeners("nextStage");
    _eventDispatcher->addCustomEventListener("nextStage", [=](EventCustom *cus){
        // 过关成功,保存数据
        UserDataManager *user = UserDataManager::getInstance();
        user->setAllMoney(UserDataManager::getInstance()->getAllMoney() - curPayMoney + curStageScore);
        user->setStageNum(user->getStageNum()+1);
        user->saveUserData();
        
        // 进入商店
        Scene *shopScene = Shop::createScene();
        Director::getInstance()->replaceScene(shopScene);
    });
    
    loadStageInfo();

    return true;
}

Widget* Game::getname(Widget* root){
    if (!root)
    {
        return nullptr;
    }
    const std::string str =  root->getName();
    CCLOG("%s", "getname");
    CCLOG("%s", str.c_str());
    CCLOG("%s",  typeid(*root).name());
    
    const auto& arrayRootChildren = root->getChildren();
    for (auto& subWidget : arrayRootChildren)
    {
        const std::string str =  subWidget->getName();
        CCLOG("%s", "getname");
        CCLOG("%s", str.c_str());
        CCLOG("%s",  typeid(*subWidget).name());
 
        
        Widget* child = dynamic_cast<Widget*>(subWidget);
        if (child)
        {
            const std::string str =  child->getName();
            CCLOG("%s", "getname");
            CCLOG("%s", str.c_str());
            Widget* res = getname(child);
            
            //            if (res != nullptr)
            //            {
            //                return res;
            //            }
        }
    }
    return nullptr;
}

bool Game::physicsBegin(cocos2d::PhysicsContact &contact)
{
//    CCLOG("physicsBegin Btag: %d",contact.getShapeB()->getBody()->getNode()->getTag());
//     CCLOG("physicsBegin Atag: %d",contact.getShapeA()->getBody()->getNode()->getTag());

    if(contact.getShapeB()->getBody()->getNode()->getTag() == tagHook ||
       contact.getShapeA()->getBody()->getNode()->getTag() == tagHook  ){
        if (contact.getShapeB()->getBody()->getNode()->getTag() != kWorldTag) {
            // 碰到金块, 打开钩子
            if (!isOpenHook) {
                this->pullGold(contact);
                 this->pullTnt(contact);
                this->pullMouse(contact);
            }
        } else {
            this->backSpeed = 10;
        }
        
        this->unschedule(CC_SCHEDULE_SELECTOR(Game::addRopeHeight));
        this->schedule(CC_SCHEDULE_SELECTOR(Game::subRopeHeight), 0.025);
    }else{
          if (contact.getShapeB()->getBody()->getNode()->getTag() != kWorldTag
              && contact.getShapeA()->getBody()->getNode()->getTag() != kWorldTag) {
              this->mouseColision(contact);
          }
    }
    
    
    return true;
}

void Game::loadStageInfo()
{
    // 加载关卡信息
    int stageNum = UserDataManager::getInstance()->getStageNum();
    int levelIndex = stageNum % 5;
    if (levelIndex == 0) {
        levelIndex = 5;
    }
    
    string levelCsbName = "level" + to_string(levelIndex) + ".csb";
    auto goldCsb = CSLoader::createNode(levelCsbName);
    // 所有金块
    this->addChild(goldCsb);

    auto goldsLayout = Helper::seekWidgetByName(static_cast<Widget *>(goldCsb), "goldPanel");
    this->arrGold = Vector<Node *>();
    Vector<Node *> golds = goldsLayout->getChildren();
    
    for (Node *subNode : golds) {
        arrGold.pushBack(subNode);
        Size bodySize = Size(subNode->getContentSize().width * subNode->getScaleX(), subNode->getContentSize().height * subNode->getScaleY());
        PhysicsBody *goldBody = PhysicsBody::createEdgeBox(bodySize);
 
        goldBody->setCategoryBitmask(10);
        goldBody->setCollisionBitmask(10);
        goldBody->setContactTestBitmask(10);
        subNode->addComponent(goldBody);
        subNode->setTag(tagGold);
        
    }
    
     //-------Gold---------
    
//    this->arrGold = Vector<Sprite *>();
//    //create  animation mouse
//    auto gold = Gold::create("pulled-gold-0-0.png", 1, 1, 0, true, true, true);
//    gold->setPosition(Vec2( 300, 250));
//    this->addChild(gold);
//    arrGold.pushBack(gold);
//    for (Sprite *subNode : arrGold) {
//        Size bodySize = Size(subNode->getContentSize().width * subNode->getScaleX()/2, subNode->getContentSize().height * subNode->getScaleY()/2);
//      
//        PhysicsBody *mouseBody = PhysicsBody::createEdgeBox(bodySize);
//        
//        mouseBody->setCategoryBitmask(10);
//        mouseBody->setCollisionBitmask(10);
//        mouseBody->setContactTestBitmask(10);
//        subNode->addComponent(mouseBody);
//        subNode->setTag(tagGold);
//    }
    
    //-------Tnt---------
    this->arrTnt = Vector<Sprite *>();
    //create  animation mouse
    auto tnt = Gold::create("TNT.png", 1, 1, 0, true, true, true);
    tnt->setPosition(Vec2( 300, 250));
    this->addChild(tnt);
    arrTnt.pushBack(tnt);
    for (Sprite *subNode : arrTnt) {
        Size bodySize = Size(subNode->getContentSize().width * subNode->getScaleX()/2, subNode->getContentSize().height * subNode->getScaleY()/2);
        
        PhysicsBody *mouseBody = PhysicsBody::createEdgeBox(bodySize);
        
        mouseBody->setCategoryBitmask(10);
        mouseBody->setCollisionBitmask(10);
        mouseBody->setContactTestBitmask(10);
        subNode->addComponent(mouseBody);
        subNode->setTag(tagTnt);
    }
    
    //-------Mouse---------
    
   this->arrMouse = Vector<Sprite *>();
    //create  animation mouse
    auto mouse = Mouse::create(1, 1, true, true, true, Mouse::MouseType::EMPTYMOUSE , Mouse::DirectionType::LEFT ,this->repositionMouse( Mouse::DirectionType::LEFT));
    this->addChild(mouse);
    arrMouse.pushBack(mouse);
   
    
    auto mousegame = Mouse::create(1, 1,  true, true, true, Mouse::MouseType::EMPTYMOUSE , Mouse::DirectionType::RIGHT ,this->repositionMouse( Mouse::DirectionType::RIGHT));
  
    this->addChild(mousegame);
    arrMouse.pushBack(mousegame);
   
    
    auto mouse2 = Mouse::create(1, 1, true, true, true, Mouse::MouseType::EMPTYMOUSE , Mouse::DirectionType::LEFT ,this->repositionMouse( Mouse::DirectionType::LEFT));
    this->addChild(mouse2);
    arrMouse.pushBack(mouse2);
    
    auto mousegame2 = Mouse::create(1, 1,  true, true, true, Mouse::MouseType::EMPTYMOUSE , Mouse::DirectionType::RIGHT, this->repositionMouse( Mouse::DirectionType::RIGHT));
    this->addChild(mousegame2);
    arrMouse.pushBack(mousegame2);
   
    
    auto mouse3 = Mouse::create(1, 1, true, true, true, Mouse::MouseType::EMPTYMOUSE , Mouse::DirectionType::RIGHT,this->repositionMouse( Mouse::DirectionType::RIGHT));
    
    this->addChild(mouse3);
    arrMouse.pushBack(mouse3);
    
    auto mouse4 = Mouse::create(1, 1, true, true, true, Mouse::MouseType::EMPTYMOUSE , Mouse::DirectionType::RIGHT ,this->repositionMouse( Mouse::DirectionType::RIGHT));
    
    this->addChild(mouse4);
    arrMouse.pushBack(mouse4);
   
    
    for (Node *subNode : arrMouse) {
        Size bodySize = Size(subNode->getContentSize().width * subNode->getScaleX(), subNode->getContentSize().height * subNode->getScaleY());
 
        PhysicsBody *mouseBody = PhysicsBody::createEdgeBox(bodySize);
        mouseBody->setCategoryBitmask(10);
        mouseBody->setCollisionBitmask(10);
        mouseBody->setContactTestBitmask(10);
        subNode->addComponent(mouseBody);
        subNode->setTag(tagMouse);
    }
    
}


Vec2 Game::repositionMouse(Mouse::DirectionType direction){
     auto screenSize = Director::getInstance()->getWinSize();
   
    auto start_x = direction == Mouse::DirectionType::LEFT ?  -20 : screenSize.width - 220;
    int ran = std::rand() % static_cast<int>(7);
     auto start_y =   ((screenSize.height - 260 )/7) * ran + 26   ;
    
    Vec2 mouse = Vec2(start_x,start_y);
    for(Node *subNode : arrMouse){
        if(subNode->getPosition().x  == mouse.x && subNode->getPosition().y  == mouse.y){
            Vec2 newPos = Vec2(mouse.x + 50 , mouse.y);
            return newPos;
        }
    }
    return mouse;
}

void Game::mouseColision(cocos2d::PhysicsContact &contact)
{
//    CCLOG("physicsBegin Btag: %d",contact.getShapeB()->getBody()->getNode()->getTag());
//    CCLOG("physicsBegin Atag: %d",contact.getShapeA()->getBody()->getNode()->getTag());

    if(contact.getShapeB()->getBody()->getNode()->getTag() == tagMouse && contact.getShapeA()->getBody()->getNode()->getTag() == tagGold )
        {
             auto mouse = contact.getShapeB()->getBody()->getNode();
             auto gold = contact.getShapeA()->getBody()->getNode();
//             CCLOG("1 gold->getName(): %s", gold->getName().c_str());
            
                    if(  gold->getName().compare("diamond.png") == 0){
                        dynamic_cast<Mouse*>(mouse)->getDiamond();
                        gold->getPhysicsBody()->setEnabled(false);
                        gold->setVisible(false);
                    }  else  {
                         CCLOG("1 mouse goback");
//                        dynamic_cast<Mouse*>(mouse)->goBack();
                     }
        }
    else if(contact.getShapeB()->getBody()->getNode()->getTag() == tagGold && contact.getShapeA()->getBody()->getNode()->getTag() == tagMouse){
            auto mouse = contact.getShapeA()->getBody()->getNode();
            auto gold = contact.getShapeB()->getBody()->getNode();
//           CCLOG("2 gold->getName(): %s", gold->getName().c_str());
        
                        if( gold->getName().compare("diamond.png") == 0){
                            dynamic_cast<Mouse*>(mouse)->getDiamond();
                            gold->getPhysicsBody()->setEnabled(false);
                            gold->setVisible(false);
                        } else  {
//                             dynamic_cast<Mouse*>(mouse)->goBack();
                        }
            }
    else if(contact.getShapeB()->getBody()->getNode()->getTag() == tagMouse && contact.getShapeA()->getBody()->getNode()->getTag() == tagMouse){
             auto mouse = contact.getShapeA()->getBody()->getNode();
//             dynamic_cast<Mouse*>(mouse)->goBack();
        
            auto mouseB = contact.getShapeB()->getBody()->getNode();
//            dynamic_cast<Mouse*>(mouseB)->goBack();
        }
    
}

void Game::pullMouse(cocos2d::PhysicsContact &contact)
{
    
    if(contact.getShapeB()->getBody()->getNode()->getTag() == tagMouse && (contact.getShapeA()->getBody()->getNode()->getTag() == tagHook   ))  {
        isOpenHook = true;
        auto mouse = contact.getShapeB()->getBody()->getNode();
        
        mouseSprite =  Mouse::create(1, 1, true, true, true, Mouse::MouseType::MOUSECATCH , Mouse::DirectionType::LEFT, Vec2());
        middleCircle->addChild(mouseSprite);
        contact.getShapeB()->getBody()->removeFromWorld();
        this->backSpeed = mouseSprite->backSpeed;
        if( dynamic_cast<Mouse*>(mouse)->type == Mouse::MouseType::EMPTYMOUSE){
            mouseSprite->score  = kMouseScore;
        }else if(dynamic_cast<Mouse*>(mouse)->type == Mouse::MouseType::MOUSEWITHDIAMOND){
             mouseSprite->score  = (kMouseScore + kDiamondScore) *  mouseSprite->diamondsCoe;
        }
        leftHook->runAction(RotateTo::create(0.05, -mouseSprite->hookRote));
        rightHook->runAction(RotateTo::create(0.05, mouseSprite->hookRote));
        
        mouse->getPhysicsBody()->setEnabled(false);
        mouse->setVisible(false);
        
    }else if((contact.getShapeA()->getBody()->getNode()->getTag() == tagMouse && (contact.getShapeB()->getBody()->getNode()->getTag() == tagHook ))){
        isOpenHook = true;
        auto mouse = contact.getShapeA()->getBody()->getNode();
        
        mouseSprite =  Mouse::create(1, 1, true, true, true, Mouse::MouseType::MOUSECATCH , Mouse::DirectionType::LEFT,Vec2(0,0));
        middleCircle->addChild(mouseSprite);
        contact.getShapeA()->getBody()->removeFromWorld();
        this->backSpeed = mouseSprite->backSpeed;
        if( dynamic_cast<Mouse*>(mouse)->type == Mouse::MouseType::EMPTYMOUSE){
            mouseSprite->score  = kMouseScore;
        }else if(dynamic_cast<Mouse*>(mouse)->type == Mouse::MouseType::MOUSEWITHDIAMOND){
            mouseSprite->score  = (kMouseScore + kDiamondScore) *  mouseSprite->diamondsCoe;
        }
        leftHook->runAction(RotateTo::create(0.05, -mouseSprite->hookRote));
        rightHook->runAction(RotateTo::create(0.05, mouseSprite->hookRote));
        
        mouse->getPhysicsBody()->setEnabled(false);
        mouse->setVisible(false);
    }
    
}


void Game::pullGold(cocos2d::PhysicsContact &contact)
{
    // 钓到了东西
//    CCLOG("tag: %d",contact.getShapeB()->getBody()->getNode()->getTag());
    if((contact.getShapeB()->getBody()->getNode()->getTag() == tagGold && (contact.getShapeA()->getBody()->getNode()->getTag() == tagHook   )) ||
       (contact.getShapeA()->getBody()->getNode()->getTag() == tagGold && (contact.getShapeB()->getBody()->getNode()->getTag() == tagHook   ))
       ){
         isOpenHook = true;
        auto gold = contact.getShapeB()->getBody()->getNode();
        
        goldSprite = Gold::create(gold->getName(), gold->getScaleX(), gold->getScaleY(), gold->getRotation(), isBuyPotion, isBuyDiamonds, isBuyStoneBook);
        middleCircle->addChild(goldSprite);
        contact.getShapeB()->getBody()->removeFromWorld();
        this->backSpeed = goldSprite->backSpeed;
        leftHook->runAction(RotateTo::create(0.05, -goldSprite->hookRote));
        rightHook->runAction(RotateTo::create(0.05, goldSprite->hookRote));
        
        gold->getPhysicsBody()->setEnabled(false);
        gold->setVisible(false);
        
//          CCLOG("intag: %d",contact.getShapeB()->getBody()->getNode()->getTag());
    }
    
}

void Game::pullTnt(cocos2d::PhysicsContact &contact)
    {
        // 钓到了东西
        //    CCLOG("tag: %d",contact.getShapeB()->getBody()->getNode()->getTag());
        if((contact.getShapeB()->getBody()->getNode()->getTag() == tagTnt && (contact.getShapeA()->getBody()->getNode()->getTag() == tagHook   )) ||
           (contact.getShapeA()->getBody()->getNode()->getTag() == tagTnt && (contact.getShapeB()->getBody()->getNode()->getTag() == tagHook   ))
           ){
            isOpenHook = true;
            auto gold = contact.getShapeB()->getBody()->getNode();
            this->showTntElip(gold->getPosition());
            goldSprite = Gold::create("TNT-fragment", gold->getScaleX(), gold->getScaleY(), gold->getRotation(), isBuyPotion, isBuyDiamonds, isBuyStoneBook);
            middleCircle->addChild(goldSprite);
            contact.getShapeB()->getBody()->removeFromWorld();
            this->backSpeed = goldSprite->backSpeed;
            leftHook->runAction(RotateTo::create(0.05, -goldSprite->hookRote));
            rightHook->runAction(RotateTo::create(0.05, goldSprite->hookRote));
            
            gold->getPhysicsBody()->setEnabled(false);
            gold->setVisible(false);
            
        }
   
}

void Game::showTntElip( Vec2 pos){
   
    Sprite *elip =  Sprite::createWithSpriteFrameName("tnt-explosion-0.png");
    elip->setPosition(pos);
    this->removeArrObjectNearPos(pos);
    Vector<SpriteFrame*> animFrames(9);
    char str[100] = {0};
    for(int i = 0; i < 9; i++)
    {
        sprintf(str, "tnt-explosion-%d.png",i);
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
        animFrames.pushBack(frame);
    }
    
    auto animation = Animation::createWithSpriteFrames(animFrames, 0.2f);
    auto animate = Animate::create(animation);
    elip->runAction(Sequence::create(animate , RemoveSelf::create(), nullptr) );
    this->addChild(elip);
    
    SoundTool::getInstance()->playEffect("res/music/bomb.mp3");
 
}

void Game::removeArrObjectNearPos( Vec2 pos){
    Vector<Node *> arrObject = this->getArrObjectNearPos(pos);
    for (Node *subNode : arrObject) {
        subNode->removeFromParent();
    }
}

Vector<Node *> Game::getArrObjectNearPos( Vec2 pos){
    Vector<Node *> arrObject = Vector<Node *>();
     for (Node *subNode : arrGold) {
         float width =  subNode->getPosition().x - pos.x;
         float height  = subNode->getPosition().y - pos.y;
         if(width*width + height*height < kdistanceTnt*kdistanceTnt){
             arrGold.eraseObject(subNode);
             arrObject.pushBack(subNode);
         }
     }
    return arrObject;
}

void Game::subRopeHeight(float dt)
{
    middleCircle->setPosition(circlePosition);
    
    ropeHeight -= backSpeed;
    if (ropeHeight <= 20) {
        ropeHeight = 20;
        minerTimeLine->pause();
        // 恢复原样, 继续摇摆
        this->startShakeHookAnimation();
        this->unschedule(CC_SCHEDULE_SELECTOR(Game::subRopeHeight));
        ropeChangeing = false;
        
        if (isOpenHook) {
            isOpenHook = false;
            leftHook->setRotation(0);
            rightHook->setRotation(0);
            
            if (goldSprite != nullptr) {
                // 加分动画
                Label *scoreLabel = Label::create();
                scoreLabel->setColor(Color3B(50, 200, 0));
                scoreLabel->setSystemFontSize(25);
                scoreLabel->setString(to_string(goldSprite->score));
                scoreLabel->setPosition(rope->convertToWorldSpace(middleCircle->getPosition()));
                this->addChild(scoreLabel, 1000);
                
                SoundTool::getInstance()->playEffect("res/music/laend.mp3");
                
                curStageScore += goldSprite->score;
                auto spawn = Spawn::create(MoveTo::create(0.5, Vec2(allMoney->getPosition().x + 10, allMoney->getPosition().y)), Sequence::create(ScaleTo::create(0.25, 3), ScaleTo::create(0.25, 0.1), NULL), NULL);
                auto seque = Sequence::create(spawn, CallFuncN::create([=](Node *node){
                    
                    scoreLabel->removeFromParent();
                    allMoney->setString(to_string(curStageScore + UserDataManager::getInstance()->getAllMoney() - curPayMoney));
                    
                }),NULL);
                scoreLabel->runAction(seque);
                
                // 加分
                goldSprite->removeFromParent();
                goldSprite = nullptr;
            }
            
            //pull mouse
            if (mouseSprite != nullptr) {
                // 加分动画
                Label *scoreLabel = Label::create();
                scoreLabel->setColor(Color3B(50, 200, 0));
                scoreLabel->setSystemFontSize(25);
                scoreLabel->setString(to_string(mouseSprite->score));
                scoreLabel->setPosition(rope->convertToWorldSpace(middleCircle->getPosition()));
                this->addChild(scoreLabel, 1000);
                
                SoundTool::getInstance()->playEffect("res/music/laend.mp3");
                
                curStageScore += mouseSprite->score;
                auto spawn = Spawn::create(MoveTo::create(0.5, Vec2(allMoney->getPosition().x + 10, allMoney->getPosition().y)), Sequence::create(ScaleTo::create(0.25, 3), ScaleTo::create(0.25, 0.1), NULL), NULL);
                auto seque = Sequence::create(spawn, CallFuncN::create([=](Node *node){
                    
                    scoreLabel->removeFromParent();
                    allMoney->setString(to_string(curStageScore + UserDataManager::getInstance()->getAllMoney() - curPayMoney));
                    
                }),NULL);
                scoreLabel->runAction(seque);
                
                // 加分
                mouseSprite->removeFromParent();
                mouseSprite = nullptr;
            }
        }
    }
    
    rope->setSize(Size(3, ropeHeight));
}

void Game::setUpText(Widget *csb)
{
    auto userManager = UserDataManager::getInstance();
    
    allMoney = static_cast<Text *>(Helper::seekWidgetByName(csb, "allMoney"));
    targetMoney = static_cast<Text *>(Helper::seekWidgetByName(csb, "passScore"));
    stageNum = static_cast<Text *>(Helper::seekWidgetByName(csb, "stage"));
    time = static_cast<Text *>(Helper::seekWidgetByName(csb, "time"));
    
    int stageIndex = userManager->getStageNum();
    allMoney->setString(to_string(userManager->getAllMoney() - curPayMoney));
    stageNum->setString(to_string(stageIndex));
    stageNum->setAnchorPoint(Vec2(0.25, 0.5));
    
    time->setString("60");
    passScroe = 650 + 275 * (stageIndex - 1) + 410 * (stageIndex - 1);
    targetMoney->setString(to_string(passScroe));
    targetMoney->setPosition(Vec2(targetMoney->getPositionX() - 60 ,targetMoney->getPositionY()));
    
    Text *Text_1 = static_cast<Text *>(Helper::seekWidgetByName(static_cast<Widget *>(csb), "Text_1"));
    Text_1->setString("Gold:\n");
    
    Text *Text_1_0 = static_cast<Text *>(Helper::seekWidgetByName(static_cast<Widget *>(csb), "Text_1_0"));
    Text_1_0->setString("Goal:\n");
    
    Text *Text_1_1 = static_cast<Text *>(Helper::seekWidgetByName(static_cast<Widget *>(csb), "Text_1_1"));
    Text_1_1->setString("Time:\n");
    
    Text *Text_1_1_0 = static_cast<Text *>(Helper::seekWidgetByName(static_cast<Widget *>(csb), "Text_1_1_0"));
    Text_1_1_0->setString("Level:\n");
    
    Text *Text_8_0 = static_cast<Text *>(Helper::seekWidgetByName(static_cast<Widget *>(csb), "Text_8_0"));
   
    Text_8_0->setAnchorPoint(Vec2(0.5, 0.5));
    Text_8_0->setString("Pause\n");
    
//     this->getname(csb);
}

void Game::onEnter()
{
    Layer::onEnter();
    
    if (!showStageTips) {
        showStageTips = true;
        
        SoundTool::getInstance()->playEffect("res/music/level.mp3");
        
        StageTipsLayer::showStageTipsLayer(this, UserDataManager::getInstance()->getStageNum(), [=](){
            this->startGame();
        });
    }
}

void Game::updateTime(float dt)
{
    timeCount--;
    time->setString(to_string(timeCount));
    
    if (timeCount == 0) {
        this->stopGame();
    }
}

void Game::startGame()
{
    // 添加点击事件
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(Game::touchCallBack, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    this->startShakeHookAnimation();
    schedule(CC_SCHEDULE_SELECTOR(Game::updateTime), 1, 59, 0);
}

void Game::startShakeHookAnimation()
{
    rope->setRotation(0);
    
    float duration = 1;
    float angle = 65;
    rope->runAction(RepeatForever::create(Sequence::create(RotateTo::create(duration, angle), RotateTo::create(duration, 0), RotateTo::create(duration, -angle), RotateTo::create(duration, 0), NULL)));
}

void Game::stopGame()
{
    // 停止一切时间
    rope->stopAllActions();
    this->stopAllActions();
    this->unscheduleAllSelectors();
    
    SoundTool::getInstance()->playEffect("res/music/finish.mp3");
    
    // 判断获取的分数是否能过关
    if (passScroe > (UserDataManager::getInstance()->getAllMoney() - curPayMoney + curStageScore)) {
        // 分数不够
        StageFailOrSucessLayer::showTips(this, StageFailOrSucessLayer::TipsType::FAIL, curStageScore, 0);
    } else {
        // Pass
        StageFailOrSucessLayer::showTips(this, StageFailOrSucessLayer::TipsType::SUCESS, curStageScore, curPayMoney);
    }
}

bool Game::touchCallBack(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (!ropeChangeing) {
        
        SoundTool::getInstance()->playEffect("res/music/lastart.mp3");
        
        rope->stopAllActions();
        ropeChangeing = true;
        minerTimeLine->gotoFrameAndPlay(0, 105, true);
        schedule(CC_SCHEDULE_SELECTOR(Game::addRopeHeight), 0.025);
    }
    
    return false;
}

void Game::addRopeHeight(float dt)
{
    middleCircle->setPosition(circlePosition);
    ropeHeight += 10;
    rope->setSize(Size(3, ropeHeight));
}

void Game::addButtonAction(Node *csbNode)
{
    Button *settingBtn = static_cast<Button *>(Helper::seekWidgetByName(static_cast<Widget *>(csbNode), "settingButton"));
    settingBtn->addTouchEventListener([=](Ref *sender, Widget::TouchEventType type){
        // 设置窗口
        if (type == Widget::TouchEventType::ENDED) {
            Pause::showPause(Director::getInstance()->getRunningScene(), passScroe <= (UserDataManager::getInstance()->getAllMoney() - curPayMoney + curStageScore));
            this->onExit();
        }
    });
}
