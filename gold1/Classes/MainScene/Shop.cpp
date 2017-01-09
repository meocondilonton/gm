//
//  Shop.cpp
//  GoldMiner
//
//  项目GitHub地址:https://github.com/ZhongTaoTian
//  项目思路和架构讲解博客:http://www.jianshu.com/users/5fe7513c7a57/latest_articles
//  微博:http://weibo.com/5622363113/fans?topnav=1&wvr=6&mod=message&need_filter=1

#include "Shop.hpp"
#include "UserDataManager.hpp"
#include "Game.hpp"

#define kBombPrice 200
#define kPotionPrice 300
#define kDiamondsPrice 200
#define kStoneBookPrice 100

Scene *Shop::createScene()
{
    Scene *scene = Scene::create();
    
    auto shopLayer = Shop::create();
    scene->addChild(shopLayer);
 
    return scene;
}

bool Shop::init()
{
    if (! Layer::init()) {
        return false;
    }
    
    auto csb = CSLoader::createNode("ShopScene.csb");
    this->addChild(csb);
    this->remakeContentSence(static_cast<Widget *>(csb));
    goodsDesVec.push_back(Value("Explosives.When pulling the unwanted things, touch at the explosives to blow up them"));
    goodsDesVec.push_back(Value(" Tonic . Miners will pull heavy  20% faster .Just effect in next level"));
    goodsDesVec.push_back(Value("Polished diamonds. In next level,diamond will increase by 300%  value.Just effect in next level"));
    goodsDesVec.push_back(Value("Stones Collection, Stones will increase 300% in the next level.Just effect in next level"));
 

    goodsDesText = static_cast<Text *>(Helper::seekWidgetByName(static_cast<Widget *>(csb), "shopDetail"));
 
    Text *text0 = static_cast<Text *>(Helper::seekWidgetByName(static_cast<Widget *>(csb), "Text_2"));
    text0->setString("Gold");
    
    Text *userMoney = static_cast<Text *>(Helper::seekWidgetByName(static_cast<Widget *>(csb), "userMoney"));
    userMoney->setString("$" + to_string(UserDataManager::getInstance()->getAllMoney()));
 
    
    Button *buyButton = static_cast<Button *>(Helper::seekWidgetByName(static_cast<Widget *>(csb), "buyButton"));
    buyButton->setTitleText("Buy");
    buyButton->addTouchEventListener([=](Ref *sender, Widget::TouchEventType type){
        if (type == Widget::TouchEventType::ENDED) {
            int index = lastSelected->getTag() - 1;
            
            auto oneIV = buyOnes.at(index);
            if (oneIV->isVisible()) {
                return;
            }
            // 获取商品价格
            int price = 0;
            switch (index) {
                case 0:
                    price = kBombPrice;
                    break;
                case 1:
                    price = kPotionPrice;
                    break;
                case 2:
                    price = kDiamondsPrice;
                    break;
                case 3:
                    price = kStoneBookPrice;
                    break;
            }
            
            if (UserDataManager::getInstance()->getAllMoney() - payMoneyCount - price >= 0) {
                kPlayClickSound
                payMoneyCount += price;
                userMoney->setString("$" + to_string(UserDataManager::getInstance()->getAllMoney() - payMoneyCount));
                oneIV->setVisible(true);
            }
        }
    });
    
    
  
    // addAction
    Button *nextButton = static_cast<Button *>(Helper::seekWidgetByName(static_cast<Widget *>(csb), "nextButton"));
      nextButton->setTitleText("Next");
    nextButton->addTouchEventListener([=](Ref *sender, Widget::TouchEventType type){
        if (type == Widget::TouchEventType::ENDED) {
            // next Stage
            ((Button *)sender)->setEnabled(false);
            kPlayClickSound
            Director::getInstance()->replaceScene(Game::createScene(buyOnes.at(0)->isVisible(), buyOnes.at(1)->isVisible(), buyOnes.at(2)->isVisible(), buyOnes.at(3)->isVisible(), payMoneyCount));
        }
    });

    for (int i = 1; i < 5; i++) {
        Button *goodsButton = static_cast<Button *>(Helper::seekWidgetByName(static_cast<Widget *>(csb), "goodsButton" + to_string(i)));
        goodsButton->setTag(i);
        goodsButton->addTouchEventListener(CC_CALLBACK_2(Shop::selectedGoodsButton, this));
        
        if (i == 1) {
            selectedGoodsButton(goodsButton, Widget::TouchEventType::ENDED);
        }
    }

    for (int i = 0; i < 4; i++) {
        ImageView *oneIV = static_cast<ImageView *>(Helper::seekWidgetByName(static_cast<Widget *>(csb), "buyOne" + to_string(i)));
        oneIV->setTag(i);
        buyOnes.pushBack(oneIV);
    }
    
    return true;
}

Widget* Shop::remakeContentSence(Widget* root){
    if (!root)
    {
        return nullptr;
    }
    const std::string str =  root->getName();
    
    
    const auto& arrayRootChildren = root->getChildren();
    for (auto& subWidget : arrayRootChildren)
    {
        const std::string str =  subWidget->getName();
       
        if (subWidget->getName() == "tip_5") {
            for(auto& aGrandChildNode : subWidget->getChildren())
            {
                const std::string str =  aGrandChildNode->getName();
                if (str == "Text_1") {
                    static_cast<Text *>(aGrandChildNode)->setString("Click on the items you want to buy.Click on Next when you are ready to go on.");
 
                }
                return nullptr;
            }
            
        }
        
        Widget* child = dynamic_cast<Widget*>(subWidget);
        if (child)
        {
            const std::string str =  child->getName();
             remakeContentSence(child);
  
        }
    }
    return nullptr;
}

void Shop::selectedGoodsButton(cocos2d::Ref *sender, Widget::TouchEventType type)
{
    if (lastSelected == ((Button *)sender)) {
        return;
    }
    kPlayClickSound
    if (type == Widget::TouchEventType::ENDED) {
        lastSelected = ((Button *)sender);
        goodsDesText->setString(goodsDesVec.at(((Button *)sender)->getTag() - 1).asString());
    }
}
