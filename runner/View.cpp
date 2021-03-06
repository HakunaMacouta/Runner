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
#include "View.h"
#include "Model.h"

//!
//

#include <math.h>
#include <mutex>

volatile bool thread_client_r_has_to_end=true;      ///< Volatile bool that define if the thread thread_client_r has to end
std::mutex client_thread_mtx;                       ///< Client's threads mutex
volatile bool thread_host_send_has_to_end=true;     ///< Volatile bool that define if the thread thread_host_send has to end
std::mutex host_thread_mtx;                         ///< Host's threads mutex
volatile bool thread_host_re_has_to_end=true;       ///< Valitile bool that define if th thread thread_host_re has to end


#include <SFML/Graphics.hpp>
#include <sstream>
#include <iostream>
#include <fstream>
#include "client.h"

using namespace std;


//!
//! \brief Thread with Client network listening loop
//! \param h Client pointer used in the thread
//!
//! Stop itself when boolean thread_client_r_has_to_end is true.
void thread_client_r(Client* h)
{
    Clock time;
    while(!thread_client_r_has_to_end)
    {
        //Listen only every 5ms
        if(time.asMilliseconds()%5==0)
        {
            client_thread_mtx.lock();
            h->receive();
            client_thread_mtx.unlock();
        }
    }
}

//!
//! \brief Thread with Host global network sending loop
//! \param h Host pointer used in the thread
//!
//! Stop itself when boolean thread_host_send_has_to_end is true.
void thread_host_send(Host* h)
{
   Clock tmp;
   while(!thread_host_send_has_to_end)
   {
       //Send every 32ms only
       if(tmp.asMilliseconds()%32==0)
        {
           host_thread_mtx.lock();
           h->sendGlobal(h->FramePacket());
           host_thread_mtx.unlock();
       }
   }
}


//!
//! \brief Thread with Host network listening loop
//! \param h Host pointer used in the thread
//!
//! Stop itself when boolean thread_host_send_has_to_end is true.
void thread_host_re(Host *h)
{
    Clock tmp;
    while(!thread_host_send_has_to_end)
    {
        if(tmp.asMilliseconds()%10==0)
        {
            host_thread_mtx.lock();
            h->receive();
            host_thread_mtx.unlock();
        }
    }
}

const unsigned int SPEED = 2;
const unsigned int SPEED1 = 1;

//!
//! \brief Method that load all background ressources
//!
void View::loadBackgrounds() {
    if(_Background.loadFromFile(BACKGROUND_IMAGE)){
        _Background.setSmooth(true);
        GraphicElement tmp1{_Background,0,0,_w,_h};
        _BackgroundSprite = tmp1;
    }

    if (_SlidingBackground1.loadFromFile(SLIDING_BACKGROUND_IMAGE1)) {
        _SlidingBackground1.setSmooth(true);
        SlidingBackground tmp{_SlidingBackground1,_w,_h,SPEED};
        _SlidingBackgroundSprite1 = tmp;
    }

    if (_SlidingBackground2.loadFromFile(SLIDING_BACKGROUND_IMAGE2)){
        _SlidingBackground2.setSmooth(true);
        SlidingBackground tmp{_SlidingBackground2,_w,_h,SPEED1};
        _SlidingBackgroundSprite2 = tmp;
    }
}

//!
//! \brief Method that load all ressources to draw bonuses
//! \return vector<AnimatedGraphicElement> with all bonuses' AnimatedGraphicElement
//!
vector<AnimatedGraphicElement> View::loadBonuses() {
    vector<sf::IntRect> readBonus;
    readBonus.push_back(bonusRect1);
    readBonus.push_back(bonusRect2);
    readBonus.push_back(bonusRect3);
    readBonus.push_back(bonusRect4);
    readBonus.push_back(bonusRect5);
    readBonus.push_back(bonusRect6);

    if(_HealBonus.loadFromFile(HEAL_BONUS_IMAGE)) {
        AnimatedGraphicElement tmp{readBonus, _HealBonus,0,0,50,50};
        _HealBonusSprite = tmp;
    }


    if(_JumpBonus.loadFromFile(JUMP_BONUS_IMAGE)) {
        AnimatedGraphicElement tmp{readBonus, _JumpBonus, 0,0,50,50};
        _JumpBonusSprite = tmp;
    }


    if(_InvBonus.loadFromFile(INV_BONUS_IMAGE)) {
        AnimatedGraphicElement tmp{readBonus, _InvBonus, 0,0,50,50};
        _InvBonusSprite = tmp;
    }


    if(_2xBonus.loadFromFile(MULTIPLIER_BONUS_IMAGE)) {
        AnimatedGraphicElement tmp{readBonus, _2xBonus, 0,0,50,50};
        _2xBonusSprite = tmp;
    }

    if (_coin.loadFromFile(SPINNING_COIN)){
        AnimatedGraphicElement tmp{readBonus, _coin, 0,0,50,50};
        _coinSprite = tmp;
    }


    readBonus.clear();
    readBonus.push_back(sf::IntRect(0,0,15,20));
    if(_maxLife.loadFromFile(MAXLIFE_IMAGE)) {
        AnimatedGraphicElement tmp{readBonus, _maxLife,0,0,50,50};
        _maxLifeSprite = tmp;
    }
    vector<AnimatedGraphicElement> Bonuses;
    Bonuses.push_back(_maxLifeSprite);
    Bonuses.push_back(_HealBonusSprite);
    Bonuses.push_back(_JumpBonusSprite);
    Bonuses.push_back(_InvBonusSprite);
    Bonuses.push_back(_2xBonusSprite);

    return Bonuses;
}


//!
//! \brief View Constructor
//! \param w Width of the game screen
//! \param h Height of the game screen
//!
View::View(int w, int h)
    : _w(w), _h(h),
      m_state{SPLASHSCREEN},
      m_mainmenu{w,h,MAIN_MENU_ITEMS},
      m_optionmenu{w,h,OPTIONS_MENU_ITEMS},
      m_languagemenu{w,h,LANGUAGE_MENU_ITEMS},
      m_shopmenu{w,h,SHOP_MENU_ITEMS},
      m_gameovermenu{w,h,GAMEOVER_ITEMS},
      m_multiplayermenu{w,h,MULTIPLAYER_ITEMS},
      m_hostselecmenu{w,h,HOSTSELEC_ITEMS},
      m_hostlobbymenu{w,h,HOSTLOBBY_ITEMS}
{
    _window = new sf::RenderWindow(sf::VideoMode(w, h, 32), "Runner", sf::Style::Close);
    _window->setFramerateLimit(60);
    _window->setKeyRepeatEnabled(false);

    changeLanguage("en");
    loadBackgrounds();

    if (_elem.loadFromFile(ELEM_IMG)) {
        GraphicElement tmp{_elem, 0,0,50,50};
        _elemSprite = tmp;
    }
    if(_GUI.loadFromFile(GUI_IMAGE)) {
        Life tmp{_GUI,sf::Color::Green, sf::Color::Red, 100.0f};
        m_lifeUI = tmp;
    }

    m_shopmenu.setBonuses(loadBonuses());

    if(_coinStack.loadFromFile(COIN_STACK)) {
        GraphicElement tmp{_coinStack, 0,0,50,50};
        _coinStackSprite = tmp;
    }

    if(_fontDisplay.loadFromFile("../Font/givre.TTF")) {
        _coinDisplayText.setFont(_fontDisplay);
        _coinDisplayText.setCharacterSize(30);
        _coinDisplayText.setPosition(_coinStackSprite.getGlobalBounds().width + 10, 5);
        _scoreDisplayText.setFont(_fontDisplay);
        _scoreDisplayText.setCharacterSize(30);
        _scoreDisplayText.setPosition(_w/2, 5);
    }
    if(_gameOver.loadFromFile(GAME_OVER_IMAGE)) {
        GraphicElement tmp{_gameOver, _w/2, _h-500, 500,500};
        _gameOverSprite = tmp;

    }

    if (_GameMusic.openFromFile(SONG_GAME)) {
        _GameMusic.setLoop(false);
        _GameMusic.setVolume(40.);
    }
    if(_MenusMusic.openFromFile(SONG_MENUS)) {
        _MenusMusic.setLoop(false);
        _MenusMusic.setVolume(40.);
    }

    if (izi.loadFromFile(SOUND_JUMP))
        Jump.setBuffer(izi);
    Jump.setVolume(40.);

    if (carre.loadFromFile(SOUND_CARRE))
        Collision.setBuffer(carre);

    std::vector<sf::IntRect> clipRect_balle;
    clipRect_balle.push_back(poposwag_run1_rect);
    clipRect_balle.push_back(poposwag_run2_rect);
    clipRect_balle.push_back(poposwag_run3_rect);

    if (_balle.loadFromFile(BALLE_IMAGE)) {
        _balle.setSmooth(true);
        AnimatedGraphicElement tmp{clipRect_balle, _balle, 1,1,40,40};
        _balleSprite = tmp;
    }
}

//!
//! \brief View destructor
//!
View::~View(){
    if(_window!= NULL)
        delete _window;
}

//!
//! \brief Set the linked model to the view
//! \param model Model pointer to link with the view
//!
void View::setModel(Model * model){
    _model = model;
    _model->pause();
}

//!
//! \brief Method that draw things on the screen
//!
void View::draw(){
    _window->clear();

    //for every state or most
    if(m_state !=SPLASHSCREEN) {
        _BackgroundSprite.draw(_window);
        _SlidingBackgroundSprite2.draw(_window);
        _SlidingBackgroundSprite1.draw(_window);
    }
    if(m_state !=GAME) {
        _GameMusic.stop();
        if(_MenusMusic.getStatus()==sf::SoundSource::Stopped && _MenusMusic.getStatus()!=sf::SoundSource::Playing)
            _MenusMusic.play();
        else if(_MenusMusic.getStatus()!=sf::SoundSource::Playing && _MenusMusic.getStatus()!=sf::SoundSource::Playing)
            _MenusMusic.play();
    }
    else {
        _MenusMusic.stop();
        if(_GameMusic.getStatus()==sf::SoundSource::Stopped && _GameMusic.getStatus()!=sf::SoundSource::Playing)
            _GameMusic.play();
        else if(_GameMusic.getStatus()!=sf::SoundSource::Playing && _GameMusic.getStatus()!=sf::SoundSource::Playing)
            _GameMusic.play();
    }


    //What to draw in which game state
    switch(m_state) {
    case SPLASHSCREEN:
        m_splashscreen.draw(_window);
        break;
    case MAIN_MENU:
        m_mainmenu.draw(_window);
        break;
    case MULTIPLAYER:
        m_multiplayermenu.draw(_window);
        break;
    case HOST_SELEC:
        m_hostselecmenu.draw(_window);
        break;
    case GAME_CREATION:
        m_hostlobbymenu.draw(_window);
        break;
    case OPTIONS:
        m_optionmenu.draw(_window);
        break;
    case BEST_SCORES:
        break;
    case SPECTATE_SELEC:
        m_hostselecmenu.draw(_window);
        break;
    case SHOP:
        _coinStackSprite.draw(_window);
        _window->draw(_coinDisplayText);
        m_shopmenu.draw(_window);
        break;
    case LANGUAGE:
        m_languagemenu.draw(_window);
        break;
    case GAME_SPECTATOR:
        drawObstacles();
        for(auto x : _multiSprite)
            x.draw(_window);
        break;
    case GAME_MULTIPLAYER:
        drawObstacles();
        for(auto x : _multiSprite)
            x.draw(_window);
        m_lifeUI.draw(_window);
        _balleSprite.draw(_window);
        break;
    case GAME:
        drawObstacles();
        m_lifeUI.draw(_window);
        _balleSprite.draw(_window);
        _coinStackSprite.draw(_window);
        _window->draw(_coinDisplayText);
        _window->draw(_scoreDisplayText);
        break;
    case GAMEOVER:
        m_gameovermenu.draw(_window);
        _gameOverSprite.draw(_window);
        _window->draw(_scoreDisplayText);
        break;
    }
    _window->display();
}

//!
//! \brief Obstacle drawing method
//!
void View::drawObstacles() {
    for(auto x : m_elemPos)
    {
        switch (x.first) {
        case 21:
            _HealBonusSprite.setPosition(sf::Vector2f{x.second.first, x.second.second});
            _HealBonusSprite.draw(_window);
            break;
        case 22:
            _coinSprite.setPosition(sf::Vector2f{x.second.first, x.second.second});
            _coinSprite.draw(_window);
            break;
        case 23:
            _InvBonusSprite.setPosition(sf::Vector2f{x.second.first, x.second.second});
            _InvBonusSprite.draw(_window);
            break;
        case 24:
            _JumpBonusSprite.setPosition(sf::Vector2f{x.second.first, x.second.second});
            _JumpBonusSprite.draw(_window);
            break;
        case 25:
            _2xBonusSprite.setPosition(sf::Vector2f{x.second.first, x.second.second});
            _2xBonusSprite.draw(_window);
            break;
        case 11:
            _elemSprite.setPosition(sf::Vector2f{x.second.first, x.second.second});
            _elemSprite.draw(_window);
            break;
        case 12:
            _elemSprite.setPosition(sf::Vector2f{x.second.first, x.second.second});
            _elemSprite.draw(_window);
            _elemSprite.setPosition(sf::Vector2f{x.second.first, x.second.second+50});
            _elemSprite.draw(_window);
            break;
        case 13:
            _elemSprite.setPosition(sf::Vector2f{x.second.first, x.second.second});
            _elemSprite.draw(_window);
            _elemSprite.setPosition(sf::Vector2f{x.second.first, x.second.second+50});
            _elemSprite.draw(_window);
            _elemSprite.setPosition(sf::Vector2f{x.second.first+50, x.second.second});
            _elemSprite.draw(_window);
            _elemSprite.setPosition(sf::Vector2f{x.second.first+50, x.second.second+50});
            _elemSprite.draw(_window);
            break;
        }
    }
}

//!
//! \brief Method that manage players inputs
//! \return true if the programme has to end, false otherwise
//!
bool View::treatEvents(){
    bool result = false;
    bool left(false), right(false);
    _model->getCharDir(left,right);

    if(_window->isOpen()){
        result = true;
        sf::Event event;
        while (_window->pollEvent(event)) {
            switch(m_state) {
            case SPLASHSCREEN:
                if(event.type == sf::Event::KeyReleased)
                    if(event.key.code == sf::Keyboard::Return)
                        m_splashscreen.event(m_state);
                break;
            case MAIN_MENU:
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) m_mainmenu.MoveUp();
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) m_mainmenu.MoveDown();
                if(event.type == sf::Event::KeyReleased) {
                    if(event.key.code == sf::Keyboard::Return) {
                        switch(m_mainmenu.getSelectedItem()) {
                        case 0:
                            m_state = GAME;
                            m_lifeUI.setMaxLife(_model->getSavedParam()[SAVED_PARAM_ID_MAXLIFE]);
                            _model->restart();
                            break;
                        case 1:
                            m_state = MULTIPLAYER;
                            break;
                        case 2:
                            /*** NOT IMPLEMENTED ***/
                            //m_state = BEST_SCORES;
                            break;
                        case 3:
                            m_state = SHOP;
                            break;
                        case 4:
                            m_state = OPTIONS;
                            break;
                        case 5:
                            result = false;
                            _window->close();
                            break;
                        }
                    }
                }
                break;
            case MULTIPLAYER:
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) m_multiplayermenu.MoveUp();
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) m_multiplayermenu.MoveDown();
                if(event.type == sf::Event::KeyReleased) {
                    if(event.key.code == sf::Keyboard::Return) {
                        switch(m_multiplayermenu.getSelectedItem()) {
                        case 0:
                            /*** DO NOT WORK ***/
                            /*
                            thread_host_send_has_to_end=false;
                            m_host->rebind(Host::DEFAULT_PORT);
                            m_thread = std::thread(thread_host_send, m_host);
                            m_state = GAME_CREATION;
                            */
                            break;
                        case 1:
                            thread_client_r_has_to_end=false;
                            m_client->rebind(Client::DEFAULT_PORT);
                            m_thread = std::thread(thread_client_r, m_client);
                            m_state = HOST_SELEC;
                            m_hostselecmenu.setHostsList(m_client->getHosts());
                            break;
                        case 2:
                            thread_client_r_has_to_end=false;
                            m_client->rebind(Client::DEFAULT_PORT);
                            m_thread = std::thread(thread_client_r, m_client);
                            m_state = SPECTATE_SELEC;
                            m_hostselecmenu.setHostsList(m_client->getHosts());
                            break;
                        case 3:
                            m_state = MAIN_MENU;
                            break;
                        }
                    }
                }
                break;
            case HOST_SELEC:
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) m_hostselecmenu.moveUp();
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) m_hostselecmenu.moveDown();
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) m_hostselecmenu.moveLeft();
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) m_hostselecmenu.moveRight();
                if(event.type == sf::Event::KeyReleased) {
                    if(event.key.code == sf::Keyboard::Return) {
                        switch(m_hostselecmenu.getSelectedItem()) {
                        case 0:
                            /*** DO NOT WORK ***/
                            //m_client->connect(m_hostselecmenu.getSelectedHost());
                            //m_state = GAME_MULTIPLAYER;
                            break;
                        case 1:
                            m_hostselecmenu.setHostsList(m_client->getHosts());
                            break;
                        case 2:
                            thread_client_r_has_to_end=true;
                            m_thread.join();
                            m_client->stop();
                            m_state = MULTIPLAYER;
                            break;
                        }
                    }
                }
                break;
            case GAME_CREATION:
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) m_hostlobbymenu.moveUp();
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) m_hostlobbymenu.moveDown();
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) m_hostlobbymenu.moveLeft();
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) m_hostlobbymenu.moveRight();
                if(event.type == sf::Event::KeyReleased) {
                    if(event.key.code == sf::Keyboard::Return) {
                        switch(m_hostlobbymenu.getSelectedItem()) {
                        case 0:
                            m_state = GAME_MULTIPLAYER;
                            break;
                        case 1:
                            break;
                        case 2:
                            thread_host_send_has_to_end=true;
                            m_thread.join();
                            m_host->stop();
                            m_state=MULTIPLAYER;
                            break;
                        }
                    }
                }
                break;
            case SPECTATE_SELEC:
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) m_hostselecmenu.moveUp();
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) m_hostselecmenu.moveDown();
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) m_hostselecmenu.moveLeft();
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) m_hostselecmenu.moveRight();
                if(event.type == sf::Event::KeyReleased) {
                    if(event.key.code == sf::Keyboard::Return) {
                        switch(m_hostselecmenu.getSelectedItem()) {
                        case 0:
                            /*** WORK BUT NEED HOST ***/
                            //m_state = GAME_SPECTATOR;
                            break;
                        case 1:
                            m_hostselecmenu.setHostsList(m_client->getHosts());
                            break;
                        case 2:
                            thread_client_r_has_to_end=true;
                            m_thread.join();
                            m_client->stop();
                            m_state = MULTIPLAYER;
                            break;
                        }
                    }
                }
                break;
            case OPTIONS:
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) m_optionmenu.MoveUp();
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) m_optionmenu.MoveDown();
                if(event.type == sf::Event::KeyReleased) {
                    if(event.key.code == sf::Keyboard::Return) {
                        switch(m_optionmenu.getSelectedItem()) {
                        case 0:
                            m_state = LANGUAGE;
                            break;
                        case 1:
                            break;
                        case 2:
                            m_state = MAIN_MENU;
                            break;
                        }
                    }
                }
                break;
            case BEST_SCORES:
                break;
            case SHOP:
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) m_shopmenu.MoveUp();
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) m_shopmenu.MoveDown();
                if(event.type == sf::Event::KeyReleased) {
                    if(event.key.code == sf::Keyboard::Return) {
                        if(m_shopmenu.getSelectedItem()!=5) {
                            m_shopmenu.update(m_shopmenu.getSelectedItem());
                            _model->shopUpdate(m_shopmenu.getSavedParam());
                            m_lifeUI.setMaxLife((m_shopmenu.getSavedParam())[SAVED_PARAM_ID_MAXLIFE]);
                        }
                        else
                            m_state = MAIN_MENU;
                    }
                }
                break;
            case LANGUAGE:
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) m_languagemenu.MoveUp();
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) m_languagemenu.MoveDown();
                if(event.type == sf::Event::KeyReleased) {
                    if(event.key.code == sf::Keyboard::Return) {
                        switch(m_languagemenu.getSelectedItem()) {
                        case 0:
                            changeLanguage("en");
                            m_state = OPTIONS;
                            break;
                        case 1:
                            changeLanguage("fr");
                            m_state = OPTIONS;
                            break;
                        case 2:
                            changeLanguage("gr");
                            m_state = OPTIONS;
                            break;
                        case 3:
                            changeLanguage("es");
                            m_state = OPTIONS;
                            break;
                        case 4:
                            changeLanguage("po");
                            break;
                        case 5:
                            m_state = MAIN_MENU;
                            break;
                        case 6:
                            m_state = OPTIONS;
                            break;
                        }
                    }
                }
                break;
            case GAME:
                sf::Keyboard::isKeyPressed(sf::Keyboard::Left)?left=true:left=false;
                sf::Keyboard::isKeyPressed(sf::Keyboard::Right)?right=true:right=false;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                {
                    _model->jumpBall();
                    Jump.play();
                }
                break;
            case GAME_MULTIPLAYER:
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) m_client->sendAction(rvb::Action::LEFT);
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) m_client->sendAction(rvb::Action::RIGHT);
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) m_client->sendAction(rvb::Action::UP);
                if (event.type == sf::Event::KeyReleased
                        && (event.key.code==sf::Keyboard::Left || event.key.code==sf::Keyboard::Right))
                                m_client->sendAction(rvb::Action::K_RELEASED);
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
                {
                    m_client->disconnect();
                    thread_client_r_has_to_end=true;
                    m_thread.join();
                    m_client->stop();
                    m_state=MAIN_MENU;
                }
                break;
            case GAME_SPECTATOR:
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
                {
                    thread_client_r_has_to_end=true;
                    m_thread.join();
                    m_client->stop();
                    m_state=MAIN_MENU;
                }
                break;
            case GAMEOVER:
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) m_gameovermenu.MoveUp();
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) m_gameovermenu.MoveDown();
                if(event.type == sf::Event::KeyReleased) {
                    if(event.key.code == sf::Keyboard::Return) {
                        switch(m_gameovermenu.getSelectedItem()) {
                        case 0:
                            m_state = GAME;
                            _model->restart();
                            break;
                        case 1:
                            m_state = MAIN_MENU;
                            break;
                        case 2:;
                            _window->close();
                            result = true;
                            break;
                        }
                    }
                }
                break;
            }
            if ((event.type == sf::Event::Closed) ||
                    ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))) {
                _window->close();
                result = false;
            }
        }
    }
    _model->setCharDir(left,right);
    return result;
}


//!
//! \brief Method called when changing language
//! \param lang Language to which you want to change
//!
void View::changeLanguage(string lang) {
    fstream f;
    int cpt = 0;
    int cpt2 = -1;
    string test;
    f.open("../Languages/" + lang + ".ini", ios::in);
    if(f.is_open()) {
        while(!f.eof()) {
            getline(f, test);
            cpt2++;
            if(test != "___") {
                switch(cpt) {
                case 0:
                    m_mainmenu.changeString(test,cpt2);
                    break;
                case 1:
                    m_optionmenu.changeString(test,cpt2);
                    break;
                case 2:
                    m_languagemenu.changeString(test,cpt2);
                    break;
                case 3:
                    m_shopmenu.changeString(test,cpt2);
                    break;
                case 4:
                    break;
                }
            }
            else {
                cpt++;
                cpt2=-1;
            }


        }
    }
}

//!
//! \brief Method that update the view elements with model's data
//!
void View::synchronize()
{
    if(m_state == SPLASHSCREEN)
        m_splashscreen.synchronize(m_state);
    if(m_state == GAME_CREATION)
    {
        m_hostlobbymenu.setClientList(m_host->getPlayersInfo());
    }
    if(m_state == GAME){
        _model->getElemsPos(m_elemPos);

        std::pair<float,float> a = _model->getBallPosition();
        _balleSprite.setPosition(sf::Vector2f{a.first, a.second});
        m_lifeUI.synchronize(_model->getLife(),a.first, a.second);

        _SlidingBackgroundSprite1.setSpeed((float)-1*(_model->getAllSpeed()));
        _SlidingBackgroundSprite2.setSpeed((float)-1*_model->getAllSpeed()-1);

        if(_model->hasCollide())
            Collision.play();

        _model->moveBall();

        if(_model->getLife() <= 0) {
            m_state = GAMEOVER;
            _scoreDisplayText.setString("SCORE :" + std::to_string(_model->getScore()));
        }
        else
            _scoreDisplayText.setString(std::to_string(_model->getScore()));
    }
    else if(m_state == GAME_MULTIPLAYER || m_state == GAME_SPECTATOR)
    {
        float gs;
        std::pair<int,int> paire;
        std::set<std::pair<int, std::pair<int, int> > > chars;

        m_elemPos.clear();
        _multiSprite.clear();
        m_client->getFrame(gs,m_elemPos,paire,chars);
        _balleSprite.setPosition(sf::Vector2f{(float)paire.first,(float)paire.second});

        for(std::pair<int, std::pair<int, int> > x : chars)
        {
            AnimatedGraphicElement tmp{_balleSprite};
            tmp.setPosition((float)x.second.first, (float)x.second.second);
            _multiSprite.push_back(tmp);
        }

        _SlidingBackgroundSprite2.setSpeed((float)-1*(gs));
        _SlidingBackgroundSprite1.setSpeed((float)-1*(gs-1));

    }
    _coinDisplayText.setString(std::to_string(_model->getMoney()) + "g");
    m_shopmenu.setGold(_model->getMoney());
    m_shopmenu.setSavedParam(_model->getSavedParam());

}

//!
//! \brief Method that set the actual Client linked to the view
//! \param c Client pointer linked to the view
//!
void View::setClient(Client *c)
{
    m_client=c;
}
