//
// Created pointB.y swirta on 02.05.2021.
//

#include "../include/Snake.hpp"
#include <random>

using namespace std;
using namespace cv;

void Snake::clear_snake() {
    snakeBody.clear();
}

void Snake::grow(cv::Point chunk) {
    snakeBody.push_back(chunk);
}
int Snake::length() {
    return snakeBody.size();
}
void Snake::move(Point point){
    snakeBody.push_front(point);
    snakeBody.pop_back();
}
Point Snake::get_point(int i) {
    if(i >= 0 && i < snakeBody.size())
        return snakeBody[i];
    else
        return Point(-1, -1);
}
void Snake::draw(cv::Mat &frame) {
    if (snakeBody.size() > 1)
        for (auto it = snakeBody.crbegin(); it != snakeBody.crend() - 1; ++it)
            cv::line(frame, *it, *(it + 1), {0, 200, 0}, 5);
}
int Snake::orientation(const cv::Point &a, const cv::Point &b, const cv::Point &c) {
    int value = int(((b.y - a.y) * (c.x - b.x)) - ((b.x - a.x) * (c.y - b.y)));
    return value == 0 ? 0 : value > 0 ? 1 : 2;
}
bool Snake::is_intersected(const cv::Point &a, const cv::Point &b, const cv::Point &c, const cv::Point &d) {
    int orientationArray[4] = {};
    orientationArray[0] = orientation(a, b, c);
    orientationArray[1] = orientation(a, b, d);
    orientationArray[2] = orientation(c, d, a);
    orientationArray[3] = orientation(c, d, b);
    return (orientationArray[0] != orientationArray[1]) && (orientationArray[2] != orientationArray[3]);
}
bool Snake::check_snake() {
    if(snakeBody.size() > 3){
        Point a, b, c, d;
        int snakeSize = snakeBody.size() - 1;
        a = get_point(0);
        b = get_point(1);
        for(int i = 3; i < snakeSize; ++i){
            c = get_point(i);
            d = get_point(i + 1);
            if(is_intersected(a, b, c, d))
                return true;
        }
    }
    return false;
}
bool Snake::eat_fruit(Point &fruit) {
    Point a = get_point(0);
    return std::pow(a.x - fruit.x, 2) + std::pow(a.y - fruit.y, 2) <= std::pow(fruitRadius, 2) && a.x > -1;
}

void Game::generate_fruit() {
    random_device randomDevice;
    mt19937 randomEngine(randomDevice());
    uniform_int_distribution<> posXGen(100, windowSize.x - 100);
    uniform_int_distribution<> posYGen(100, windowSize.y - 100);
    fruit.x = posXGen(randomEngine);
    fruit.y = posYGen(randomEngine);
}

void Game::create_snake(Point &head) {
    for(int i = 1; i <= 10; ++i)
        snake.grow(Point(head.x - 5*i, head.y));
}

void Game::draw(Mat &frame){
    snake.draw(frame);
    circle(frame, this->fruit, fruitRadius, Scalar(255, 0, 0), FILLED);
    putText(frame, "LIVES: " + to_string(lives), Point(50, 50), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 255, 0), 2);
    putText(frame, "SCORE: " + to_string(points), Point(windowSize.x - 200, 50), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 255, 0), 2);

}

void Game::draw_lost_message(cv::Mat &frame) {
    putText(frame, "PRZEGRALES", Point(windowSize.x/2 - 500, windowSize.y/2), FONT_HERSHEY_DUPLEX, 5, Scalar(0, 0, 255), 3);
    putText(frame, "Nacisnij ENTER by kontynuowac", Point(windowSize.x/2 - 300, windowSize.y/2 + 100), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 255, 0), 2);
}

void Game::draw_info_message(cv::Mat &frame) {
    putText(frame, "By rozpoczac od nowa kliknij R, by wyjsc kliknij Esc", Point(windowSize.x/2 - 400, windowSize.y/2), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 255, 0), 2);
}

GAME_STATE Game::get_state() {
    return gameState;
}

void Game::run(Point &marker, Mat &frame) {
    switch (gameState) {
        case PREPARING:
            create_snake(marker);
            generate_fruit();
            gameState = PLAYING;
            break;
        case PLAYING:
            snake.move(marker);
            if(snake.check_snake())
                --lives;
            if(lives == 0)
                gameState = LOST;
            if(snake.eat_fruit(fruit)){
                ++points;
                auto lastPoint = snake.get_point(snake.length() - 1);
                snake.grow(cv::Point(lastPoint.x - 2, lastPoint.y - 2));
                generate_fruit();
            }
            this->draw(frame);
            if(waitKey(1) == 27){
                gameState = ENDED;
            }
            break;
        case LOST:
            draw_lost_message(frame);
            if(waitKey(1) == 13)
                gameState = ENDED;
            break;
        case ENDED:
            lives = 3, points = 0;
            snake.clear_snake();
            draw_info_message(frame);

            int key = waitKey(1);
            if(key == 27)
                gameState = QUIT;
            else if(key == 114)
                gameState = PREPARING;
            break;
    }
}