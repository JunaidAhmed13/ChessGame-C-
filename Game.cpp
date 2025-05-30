#include "Game.h"
#include <SFML/Audio.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <iostream>
#include "Queen.h"
#include "Pawn.h"
#include "Rook.h"
#include "Bishop.h"
#include "Knight.h"

const int TILE_SIZE = 75;

bool Game::checkUser(const std::string& username, const std::string& password) {
    std::ifstream file("users.txt");
    std::string line, u, p;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        iss >> u >> p;
        if (u == username && p == password)
            return true;
    }
    return false;
}
bool Game::userExists(const std::string& username) {
    std::ifstream file("users.txt");
    std::string line, u, p;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        iss >> u >> p;
        if (u == username)
            return true;
    }
    return false;
}
void Game::addUser(const std::string& username, const std::string& password) {
    std::ofstream file("users.txt", std::ios::app);
    file << username << " " << password << "\n";
}

Game::Game() : currentTurn(WHITE), window(sf::VideoMode(600, 600), "Chess Game", sf::Style::Default | sf::Style::Resize) {}

void Game::run() {
    sf::Font font;
    font.loadFromFile("DejaVuSans.ttf");

    sf::Music music;
    if (music.openFromFile("background.ogg")) {
        music.setLoop(true);
        music.play();
    }

    // Load background image
    sf::Texture bgTexture;
    if (!bgTexture.loadFromFile("images/bg/bg.png")) {
        std::cerr << "Failed to load background image\n";
    }
    sf::Sprite bgSprite(bgTexture);

    // Modern button colors
    sf::Color colorNormal(52, 152, 219);   // Soft blue
    sf::Color colorHover(41, 128, 185);    // Darker blue
    sf::Color colorClick(174, 214, 241);   // Light blue
    sf::Color colorBorder(23, 32, 42);     // Dark border

    // Auth buttons
    sf::RectangleShape btnLogin(sf::Vector2f(120, 50));
    btnLogin.setPosition(150, 270);
    btnLogin.setFillColor(colorNormal);
    btnLogin.setOutlineThickness(2);
    btnLogin.setOutlineColor(colorBorder);

    sf::RectangleShape btnToSignup(sf::Vector2f(120, 50));
    btnToSignup.setPosition(330, 270);
    btnToSignup.setFillColor(colorNormal);
    btnToSignup.setOutlineThickness(2);
    btnToSignup.setOutlineColor(colorBorder);

    sf::Text txtLogin("Login", font, 24);
    txtLogin.setFillColor(sf::Color::White);
    txtLogin.setPosition(175, 280);

    sf::Text txtToSignup("Sign Up", font, 24);
    txtToSignup.setFillColor(sf::Color::White);
    txtToSignup.setPosition(345, 280);

    sf::RectangleShape btnSignup(sf::Vector2f(120, 50));
    btnSignup.setPosition(150, 270);
    btnSignup.setFillColor(colorNormal);
    btnSignup.setOutlineThickness(2);
    btnSignup.setOutlineColor(colorBorder);

    sf::RectangleShape btnToLogin(sf::Vector2f(120, 50));
    btnToLogin.setPosition(330, 270);
    btnToLogin.setFillColor(colorNormal);
    btnToLogin.setOutlineThickness(2);
    btnToLogin.setOutlineColor(colorBorder);

    sf::Text txtSignup("Sign Up", font, 24);
    txtSignup.setFillColor(sf::Color::White);
    txtSignup.setPosition(175, 280);

    sf::Text txtToLogin("Login", font, 24);
    txtToLogin.setFillColor(sf::Color::White);
    txtToLogin.setPosition(360, 280);

    // Menu buttons
    sf::RectangleShape btnCpu(sf::Vector2f(300, 60));
    btnCpu.setPosition(150, 150);
    btnCpu.setFillColor(colorNormal);
    btnCpu.setOutlineThickness(2);
    btnCpu.setOutlineColor(colorBorder);

    sf::RectangleShape btnPlayer(sf::Vector2f(300, 60));
    btnPlayer.setPosition(150, 250);
    btnPlayer.setFillColor(colorNormal);
    btnPlayer.setOutlineThickness(2);
    btnPlayer.setOutlineColor(colorBorder);

    sf::RectangleShape btnQuit(sf::Vector2f(300, 60));
    btnQuit.setPosition(150, 350);
    btnQuit.setFillColor(colorNormal);
    btnQuit.setOutlineThickness(2);
    btnQuit.setOutlineColor(colorBorder);

    sf::Text txtCpu("Vs CPU", font, 32);
    txtCpu.setFillColor(sf::Color::White);
    txtCpu.setPosition(260, 160);

    sf::Text txtPlayer("Vs Player", font, 32);
    txtPlayer.setFillColor(sf::Color::White);
    txtPlayer.setPosition(230, 260);

    sf::Text txtQuit("Quit", font, 32);
    txtQuit.setFillColor(sf::Color::White);
    txtQuit.setPosition(280, 360);

    // Top game buttons
    sf::RectangleShape btnBack(sf::Vector2f(80, 35));
    btnBack.setPosition(10, 10);
    btnBack.setFillColor(colorNormal);
    btnBack.setOutlineThickness(2);
    btnBack.setOutlineColor(colorBorder);

    sf::Text txtBack("Back", font, 18);
    txtBack.setFillColor(sf::Color::White);
    txtBack.setPosition(25, 15);

    sf::RectangleShape btnMute(sf::Vector2f(80, 35));
    btnMute.setPosition(100, 10);
    btnMute.setFillColor(colorNormal);
    btnMute.setOutlineThickness(2);
    btnMute.setOutlineColor(colorBorder);

    sf::Text txtMute("Mute", font, 18);
    txtMute.setFillColor(sf::Color::White);
    txtMute.setPosition(120, 15);

    sf::RectangleShape btnPause(sf::Vector2f(80, 35));
    btnPause.setPosition(190, 10);
    btnPause.setFillColor(colorNormal);
    btnPause.setOutlineThickness(2);
    btnPause.setOutlineColor(colorBorder);

    sf::Text txtPause("Pause", font, 18);
    txtPause.setFillColor(sf::Color::White);
    txtPause.setPosition(205, 15);

    int clickedBtn = -1;
    int clickedAuthBtn = -1;

    while (window.isOpen()) {
        sf::Event event;
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // --- AUTH HANDLING ---
            if (state == AUTH) {
                btnLogin.setFillColor(btnLogin.getGlobalBounds().contains(mousePos.x, mousePos.y) ? colorHover : colorNormal);
                btnToSignup.setFillColor(btnToSignup.getGlobalBounds().contains(mousePos.x, mousePos.y) ? colorHover : colorNormal);

                if (event.type == sf::Event::TextEntered) {
                    char c = static_cast<char>(event.text.unicode);
                    if (c == 8) { // Backspace
                        if (loginActive && !loginUser.empty()) loginUser.pop_back();
                        if (!loginActive && !loginPass.empty()) loginPass.pop_back();
                    }
                    else if (c == 9) { // Tab
                        loginActive = !loginActive;
                    }
                    else if (std::isprint(c)) {
                        if (loginActive) loginUser += c;
                        else loginPass += c;
                    }
                }
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    if (btnLogin.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        btnLogin.setFillColor(colorClick);
                        clickedAuthBtn = 0;
                    }
                    if (btnToSignup.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        btnToSignup.setFillColor(colorClick);
                        clickedAuthBtn = 1;
                    }
                }
                if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                    if (clickedAuthBtn == 0 && btnLogin.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        if (checkUser(loginUser, loginPass)) {
                            state = MENU;
                            authMsg = "";
                            loginUser.clear(); loginPass.clear();
                        }
                        else if (userExists(loginUser)) {
                            authMsg = "Wrong password!";
                        }
                        else {
                            authMsg = "User not found!";
                        }
                    }
                    if (clickedAuthBtn == 1 && btnToSignup.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        state = SIGNUP;
                        signupUser.clear(); signupPass.clear(); signupMsg.clear();
                    }
                    clickedAuthBtn = -1;
                }
            }

            // --- SIGNUP HANDLING ---
            if (state == SIGNUP) {
                btnSignup.setFillColor(btnSignup.getGlobalBounds().contains(mousePos.x, mousePos.y) ? colorHover : colorNormal);
                btnToLogin.setFillColor(btnToLogin.getGlobalBounds().contains(mousePos.x, mousePos.y) ? colorHover : colorNormal);

                if (event.type == sf::Event::TextEntered) {
                    char c = static_cast<char>(event.text.unicode);
                    if (c == 8) { // Backspace
                        if (signupActive && !signupUser.empty()) signupUser.pop_back();
                        if (!signupActive && !signupPass.empty()) signupPass.pop_back();
                    }
                    else if (c == 9) { // Tab
                        signupActive = !signupActive;
                    }
                    else if (std::isprint(c)) {
                        if (signupActive) signupUser += c;
                        else signupPass += c;
                    }
                }
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    if (btnSignup.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        btnSignup.setFillColor(colorClick);
                        clickedAuthBtn = 0;
                    }
                    if (btnToLogin.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        btnToLogin.setFillColor(colorClick);
                        clickedAuthBtn = 1;
                    }
                }
                if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                    if (clickedAuthBtn == 0 && btnSignup.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        if (signupUser.empty() || signupPass.empty()) {
                            signupMsg = "Fields cannot be empty!";
                        }
                        else if (userExists(signupUser)) {
                            signupMsg = "User already exists!";
                        }
                        else {
                            addUser(signupUser, signupPass);
                            signupMsg = "Sign up successful! Go to Login.";
                        }
                    }
                    if (clickedAuthBtn == 1 && btnToLogin.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        state = AUTH;
                        loginUser.clear(); loginPass.clear(); authMsg.clear();
                    }
                    clickedAuthBtn = -1;
                }
            }

            // --- MENU HANDLING ---
            if (state == MENU) {
                btnCpu.setFillColor(btnCpu.getGlobalBounds().contains(mousePos.x, mousePos.y) ? colorHover : colorNormal);
                btnPlayer.setFillColor(btnPlayer.getGlobalBounds().contains(mousePos.x, mousePos.y) ? colorHover : colorNormal);
                btnQuit.setFillColor(btnQuit.getGlobalBounds().contains(mousePos.x, mousePos.y) ? colorHover : colorNormal);

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    if (btnCpu.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        btnCpu.setFillColor(colorClick);
                        clickedBtn = 0;
                    }
                    if (btnPlayer.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        btnPlayer.setFillColor(colorClick);
                        clickedBtn = 1;
                    }
                    if (btnQuit.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        btnQuit.setFillColor(colorClick);
                        clickedBtn = 2;
                    }
                }
                if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                    if (clickedBtn == 0 && btnCpu.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        vsAI = true;
                        state = GAME;
                        board.setup();
                        currentTurn = WHITE;
                        gameOver = false;
                    }
                    if (clickedBtn == 1 && btnPlayer.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        vsAI = false;
                        state = GAME;
                        board.setup();
                        currentTurn = WHITE;
                        gameOver = false;
                    }
                    if (clickedBtn == 2 && btnQuit.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        window.close();
                    }
                    clickedBtn = -1;
                }
            }

            // --- GAME HANDLING ---
            if (state == GAME && !gameOver) {
                btnBack.setFillColor(btnBack.getGlobalBounds().contains(mousePos.x, mousePos.y) ? colorHover : colorNormal);
                btnMute.setFillColor(btnMute.getGlobalBounds().contains(mousePos.x, mousePos.y) ? colorHover : colorNormal);
                btnPause.setFillColor(btnPause.getGlobalBounds().contains(mousePos.x, mousePos.y) ? colorHover : colorNormal);

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    if (btnBack.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        btnBack.setFillColor(colorClick);
                        clickedBtn = 10;
                    }
                    if (btnMute.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        btnMute.setFillColor(colorClick);
                        clickedBtn = 11;
                    }
                    if (btnPause.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        btnPause.setFillColor(colorClick);
                        clickedBtn = 12;
                    }
                }
                if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                    if (clickedBtn == 10 && btnBack.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        state = MENU;
                        board.setup();
                        currentTurn = WHITE;
                        gameOver = false;
                    }
                    if (clickedBtn == 11 && btnMute.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        isMuted = !isMuted;
                        if (isMuted) music.pause();
                        else music.play();
                    }
                    if (clickedBtn == 12 && btnPause.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        isPaused = !isPaused;
                    }
                    clickedBtn = -1;
                }

                // Only allow moves if not paused and not in promotion
                if (!isPaused && !board.promotionPending) {
                    if (!vsAI || currentTurn == WHITE) {
                        if (event.type == sf::Event::MouseButtonPressed) {
                            int mx = sf::Mouse::getPosition(window).x / TILE_SIZE;
                            int my = sf::Mouse::getPosition(window).y / TILE_SIZE;
                            if (selectedX == -1) {
                                Piece* piece = board.getPiece(mx, my);
                                if (piece && piece->getColor() == currentTurn) {
                                    selectedX = mx;
                                    selectedY = my;
                                }
                            }
                            else {
                                Board::MoveResult moveResult = board.movePiece(selectedX, selectedY, mx, my);
                                if (moveResult == Board::MOVE_WHITE_KING_CAPTURED) {
                                    winner = "Black won!";
                                    gameOver = true;
                                    gameOverClock.restart();
                                }
                                else if (moveResult == Board::MOVE_BLACK_KING_CAPTURED) {
                                    winner = "White won!";
                                    gameOver = true;
                                    gameOverClock.restart();
                                }
                                else if (moveResult == Board::MOVE_OK) {
                                    if (!board.promotionPending)
                                        currentTurn = (currentTurn == WHITE) ? BLACK : WHITE;
                                }
                                selectedX = -1;
                                selectedY = -1;
                            }
                        }
                    }
                }

                // --- PAWN PROMOTION HANDLING ---
                if (board.promotionPending && event.type == sf::Event::KeyPressed) {
                    int promoX = -1, promoY = -1;
                    for (int x = 0; x < 8; ++x) {
                        for (int y = 0; y < 8; ++y) {
                            Piece* p = board.getPiece(x, y);
                            if (p && dynamic_cast<Pawn*>(p) && p->getColor() == board.promotionColor) {
                                if ((p->getColor() == WHITE && y == 7) || (p->getColor() == BLACK && y == 0)) {
                                    promoX = x;
                                    promoY = y;
                                    break;
                                }
                            }
                        }
                    }
                    if (promoX != -1 && promoY != -1) {
                        Piece* newPiece = nullptr;
                        switch (event.key.code) {
                        case sf::Keyboard::Q:
                            newPiece = new Queen(board.promotionColor); break;
                        case sf::Keyboard::R:
                            newPiece = new Rook(board.promotionColor); break;
                        case sf::Keyboard::B:
                            newPiece = new Bishop(board.promotionColor); break;
                        case sf::Keyboard::N:
                            newPiece = new Knight(board.promotionColor); break;
                        default: break;
                        }
                        if (newPiece) {
                            board.promotePawn(promoX, promoY, newPiece);
                            currentTurn = (currentTurn == WHITE) ? BLACK : WHITE;
                        }
                    }
                }
            }
        }

        // --- AI MOVE (only if not paused) ---
        if (state == GAME && vsAI && currentTurn == BLACK && !board.promotionPending && !isPaused && !gameOver) {
            Board::MoveResult moveResult = ai.makeMove(board, BLACK);
            if (moveResult == Board::MOVE_WHITE_KING_CAPTURED) {
                winner = "Black won!";
                gameOver = true;
                gameOverClock.restart();
            }
            else if (moveResult == Board::MOVE_BLACK_KING_CAPTURED) {
                winner = "White won!";
                gameOver = true;
                gameOverClock.restart();
            }
            else if (moveResult == Board::MOVE_OK) {
                currentTurn = WHITE;
            }
        }

        window.clear(sf::Color(220, 220, 220));

        // Draw chessboard and pieces
        if (state == GAME) {
            board.draw(window);
        }

        // --- DRAWING UI ---
        if (state == AUTH) {
            // Draw background
            sf::Vector2u winSize = window.getSize();
            sf::Vector2u texSize = bgTexture.getSize();
            float scaleX = float(winSize.x) / texSize.x;
            float scaleY = float(winSize.y) / texSize.y;
            bgSprite.setScale(scaleX, scaleY);
            window.draw(bgSprite);

            // Heading
            sf::Text title("Login", font, 36);
            title.setFillColor(sf::Color::White);
            title.setPosition(250, 60);
            window.draw(title);

            // Input boxes
            sf::RectangleShape userBox(sf::Vector2f(300, 40));
            userBox.setFillColor(sf::Color::White);
            userBox.setPosition(140, 150);
            window.draw(userBox);

            sf::RectangleShape passBox(sf::Vector2f(300, 40));
            passBox.setFillColor(sf::Color::White);
            passBox.setPosition(140, 200);
            window.draw(passBox);

            // Input text
            sf::Text user("Username: " + loginUser + (loginActive ? "|" : ""), font, 28);
            user.setFillColor(sf::Color::Black);
            user.setPosition(150, 150);
            window.draw(user);

            sf::Text pass("Password: " + std::string(loginPass.length(), '*') + (!loginActive ? "|" : ""), font, 28);
            pass.setFillColor(sf::Color::Black);
            pass.setPosition(150, 200);
            window.draw(pass);

            sf::Text msg(authMsg, font, 22);
            msg.setFillColor(sf::Color::Red);
            msg.setPosition(150, 230);
            window.draw(msg);

            window.draw(btnLogin);
            window.draw(txtLogin);
            window.draw(btnToSignup);
            window.draw(txtToSignup);
        }
        else if (state == SIGNUP) {
            // Draw background
            sf::Vector2u winSize = window.getSize();
            sf::Vector2u texSize = bgTexture.getSize();
            float scaleX = float(winSize.x) / texSize.x;
            float scaleY = float(winSize.y) / texSize.y;
            bgSprite.setScale(scaleX, scaleY);
            window.draw(bgSprite);

            // Heading
            sf::Text title("Sign Up", font, 36);
            title.setFillColor(sf::Color::White);
            title.setPosition(250, 60);
            window.draw(title);

            // Input boxes
            sf::RectangleShape userBox(sf::Vector2f(300, 40));
            userBox.setFillColor(sf::Color::White);
            userBox.setPosition(140, 150);
            window.draw(userBox);

            sf::RectangleShape passBox(sf::Vector2f(300, 40));
            passBox.setFillColor(sf::Color::White);
            passBox.setPosition(140, 200);
            window.draw(passBox);

            // Input text
            sf::Text user("Username: " + signupUser + (signupActive ? "|" : ""), font, 28);
            user.setFillColor(sf::Color::Black);
            user.setPosition(150, 150);
            window.draw(user);

            sf::Text pass("Password: " + std::string(signupPass.length(), '*') + (!signupActive ? "|" : ""), font, 28);
            pass.setFillColor(sf::Color::Black);
            pass.setPosition(150, 200);
            window.draw(pass);

            sf::Text msg(signupMsg, font, 22);
            msg.setFillColor(sf::Color::Red);
            msg.setPosition(150, 230);
            window.draw(msg);

            window.draw(btnSignup);
            window.draw(txtSignup);
            window.draw(btnToLogin);
            window.draw(txtToLogin);
        }
        else if (state == MENU) {
            // Draw background
            sf::Vector2u winSize = window.getSize();
            sf::Vector2u texSize = bgTexture.getSize();
            float scaleX = float(winSize.x) / texSize.x;
            float scaleY = float(winSize.y) / texSize.y;
            bgSprite.setScale(scaleX, scaleY);
            window.draw(bgSprite);

            window.draw(btnCpu);
            window.draw(btnPlayer);
            window.draw(btnQuit);
            window.draw(txtCpu);
            window.draw(txtPlayer);
            window.draw(txtQuit);
        }
        else if (state == GAME) {
            // Draw top buttons
            window.draw(btnBack);
            window.draw(txtBack);
            window.draw(btnMute);
            window.draw(txtMute);
            window.draw(btnPause);
            window.draw(txtPause);

            if (isPaused) {
                sf::Text pausedText("Game Paused", font, 36);
                pausedText.setFillColor(sf::Color::Blue);
                pausedText.setPosition(180, 270);
                window.draw(pausedText);
            }

            if (board.promotionPending && font.getInfo().family != "") {
                sf::Text text("Promote pawn: Q=Queen, R=Rook, B=Bishop, N=Knight", font, 24);
                text.setFillColor(sf::Color::Red);
                text.setPosition(10, 60);
                window.draw(text);
            }
        }

        // --- WINNER MESSAGE AND DELAY ---
        if (gameOver) {
            sf::Text winText(winner, font, 48);
            winText.setFillColor(sf::Color::Red);
            winText.setPosition(120, 250);
            window.draw(winText);

            if (gameOverClock.getElapsedTime().asSeconds() > 20.f) {
                window.close();
            }
        }

        window.display();
    }
}
