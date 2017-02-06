//
//  Gold.hpp
//  GoldMiner
//
//  项目GitHub地址:https://github.com/ZhongTaoTian
//  项目思路和架构讲解博客:http://www.jianshu.com/users/5fe7513c7a57/latest_articles
//  微博:http://weibo.com/5622363113/fans?topnav=1&wvr=6&mod=message&need_filter=1

#ifndef Mouse_hpp
#define Mouse_hpp

#include <stdio.h>
#include "Const.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>

class Mouse : public Sprite{
    
public:
    enum class MouseType
    {
        EMPTYMOUSE = 1,
        MOUSEWITHDIAMOND,
        MOUSECATCH,
       
    };
    
    enum class DirectionType
    {
        LEFT = 1,
        RIGHT,
    };
    
    DirectionType directionType;
    MouseType type;
    float hookRote;
    int score;
    int backSpeed;
    int stoneCoe = 1;
    int diamondsCoe = 1;
    int power = 1;
    
    static Mouse *create(  float scaleX, float scaleY , bool isBuyPotion, bool isBuyDiamonds, bool isStoneBook , MouseType type ,  DirectionType direction);
    virtual bool init(  float scaleX, float scaleY,   bool isBuyPotion, bool isBuyDiamonds, bool isStoneBook  , MouseType type , DirectionType direction);
    void goBack();
    void getDiamond();
    void createAnimation(MouseType type );
    void randomPositionY( );
    
private:
    
   
    int random_Width;
    int random_Height ;
    int start_x;
    int end_x;
    
};

#endif /* Gold_hpp */
