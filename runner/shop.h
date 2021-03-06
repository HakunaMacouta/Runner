/*
Copyright (c) 2016 Florent VAIN, Thomas BLANC

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:


The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.


THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#ifndef SHOP_H
#define SHOP_H

#include "menu.h"
#include "animatedgraphicelement.h"
#include <array>

const std::vector<std::string> bonusString = {"Max Life","Heal","Jump","Invincibility","Multiplier"};

class Shop : public Menu
{
private:
    int m_w,m_h, m_gold;
    std::array<int,10> m_parametres;
    std::vector<AnimatedGraphicElement> m_bonuses;
    std::array<int,5>m_prix;
    std::vector<std::string>m_string;
    std::vector<sf::Text> m_text;
    GraphicElement m_plus;

public:
    Shop(int w, int h, std::vector<std::string> MenuItems);
    void setBonuses(std::vector<AnimatedGraphicElement> bonuses);
    void draw(sf::RenderWindow *window);
    void setGold(int gold);
    std::array<int,10> getSavedParam();
    void setSavedParam(std::array<int,10> param);
    void update(int i);
};

#endif // SHOP_H
