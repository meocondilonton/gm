//
//  Game.hpp
//  GoldMiner
//
//  项目GitHub地址:https://github.com/ZhongTaoTian
//  项目思路和架构讲解博客:http://www.jianshu.com/users/5fe7513c7a57/latest_articles
//  微博:http://weibo.com/5622363113/fans?topnav=1&wvr=6&mod=message&need_filter=1

#ifndef Game_hpp
#define Game_hpp

#include <stdio.h>
#include "Const.hpp"
#include "Gold.hpp"
#include "Mouse.hpp"

class Game : public Layer {
    
public:
    static Scene *createScene(bool isBuyBomb, bool isBuyPotion, bool isBuyDiamonds, bool isStoneBook, int payMoney);
    virtual bool init(bool isBuyBomb, bool isBuyPotion, bool isBuyDiamonds, bool isStoneBook, int payMoney);
    static Game *create(bool isBuyBomb, bool isBuyPotion, bool isBuyDiamonds, bool isStoneBook, int payMoney);

private:
    Text *allMoney;
    Text *targetMoney;
    Text *stageNum;
    Text *time;
    long passScroe;
    bool showStageTips;
    ImageView *rope;
    Sprite *middleCircle;
    Sprite *leftHook;
    Sprite *rightHook;
    
    cocostudio::timeline::ActionTimeline *minerTimeLine;
    
    int timeCount;
    int backSpeed = 10;
    int curStageScore;
    bool ropeChangeing;
    int ropeHeight = 20;
    int curPayMoney;
    
    Point circlePosition;
    bool isOpenHook;
    
    Gold *goldSprite;
    Mouse *mouseSprite;
    
    bool isBuyPotion;
    bool isBuyDiamonds;
    bool isBuyStoneBook;
    Button *bompButton;
    
private:
    void addButtonAction(Node *csbNode);
    void onEnter();
    void setUpText(Widget *csb);
    
    void startGame();
    void stopGame();
    
    void updateTime(float dt);
    
    void startShakeHookAnimation();
    void stopShakeHookAnimation();
    
    void addRopeHeight(float dt);
    void subRopeHeight(float dt);
    
    void loadStageInfo();

    bool touchCallBack(Touch *touch, Event *event);
    bool physicsBegin(PhysicsContact &contact);
    void pullGold(PhysicsContact &contact);
    void pullMouse(PhysicsContact &contact);
    void pullTnt(PhysicsContact &contact); 
    void mouseColision(PhysicsContact &contact);
    Vec2 repositionMouse( Mouse::DirectionType direction);
    void  showTntElip(Vec2 pos );
    Vector<Node *> getArrObjectNearPos( Vec2 pos);
    void removeArrObjectNearPos( Vec2 pos);
    
    Widget*  getname(Widget* root);
    
    Vector<Sprite *> arrMouse ;
    Vector<Node *> arrGold ;
    Vector<Sprite *> arrTnt ;
    
};

#endif /* Game_hpp */
