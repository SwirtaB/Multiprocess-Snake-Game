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

void Snake::grow(const Point &chunk) {
    snakeBody.push_back(chunk);
}

int Snake::length() {
    return snakeBody.size();
}

void Snake::move(const Point &point){
    snakeBody.push_front(point);
    snakeBody.pop_back();
}

std::optional<Point> Snake::get_point(unsigned long i) {
    if(i < snakeBody.size())
        return snakeBody[i];
    else
        return std::nullopt;
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
        unsigned long snakeSize = snakeBody.size() - 1;
        a = get_point(0).value();
        b = get_point(1).value();
        for(unsigned long i = 3; i < snakeSize; ++i){
            c = get_point(i).value();
            d = get_point(i + 1).value();
            if(is_intersected(a, b, c, d))
                return true;
        }
    }
    return false;
}

bool Snake::eat_fruit(Point &fruit) {
    Point pointA = get_point(0).value(), pointB = get_point(1).value();
    if((pointA.x - fruit.x)*(pointA.x - fruit.x) + (pointA.y - fruit.y)*(pointA.y - fruit.y) <= fruitRadius*fruitRadius)
        return true;
    pointA.x -= fruit.x;
    pointA.y -= fruit.y;
    pointB.x -= fruit.x;
    pointB.y -= fruit.y;
    int a = (pointB.x - pointA.x)*(pointB.x - pointA.x) + (pointB.y - pointA.y)*(pointB.y - pointA.y);
    int b = 2*(pointA.x*(pointB.x - pointA.x) + pointA.y*(pointB.y - pointA.y));
    int c = pointA.x*pointA.x + pointB.x*pointB.x - fruitRadius*fruitRadius;
    int disc = b*b - 4*a*c;
    if(disc <= 0)
        return false;
    auto sqrtdisc = sqrt(disc);
    auto t1 = (-b + sqrtdisc)/(2*a);
    auto t2 = (-b - sqrtdisc)/(2*a);
    return (0 < t1 && t1 < 1) && (0 < t2 && t2 < 1);
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
    circle(frame, this->fruit, fruitRadius, Scalar( 0, 0, 255), FILLED);
    putText(frame, "LIVES: " + to_string(lives), Point(50, 50), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 255, 0), 2);
    putText(frame, "SCORE: " + to_string(points), Point(windowSize.x - 200, 50), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 255, 0), 2);

}

void Game::draw_lost_message(cv::Mat &frame) const {
    putText(frame, "PRZEGRALES", Point(windowSize.x/2 - 500, windowSize.y/2), FONT_HERSHEY_DUPLEX, 5, Scalar(0, 0, 255), 3);
    putText(frame, "Nacisnij ENTER by kontynuowac", Point(windowSize.x/2 - 300, windowSize.y/2 + 100), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 255, 0), 2);
}

void Game::draw_info_message(cv::Mat &frame) const {
    putText(frame, "By rozpoczac od nowa kliknij R, by wyjsc kliknij Esc", Point(windowSize.x/2 - 400, windowSize.y/2), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 255, 0), 2);
}

GAME_STATE Game::get_state() {
    return gameState;
}

void Game::run(Point &marker, Mat &frame) {
    switch (gameState) {
        case QUIT:
            break;
        case PREPARING:
            //Pomija "nieustalone" wartości markera -> brak artefaktów tuż po uruchomieniu
            if(marker.x == 0 && marker.y == 0)
                break;

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
                auto lastPoint = snake.get_point(snake.length() - 1).value();
                snake.grow(cv::Point(lastPoint.x - 2, lastPoint.y - 2));
                generate_fruit();
            }

            draw(frame);

            if(waitKey(1) == ESC_BUTTON)
                gameState = ENDED;
            break;
        case LOST:
            draw_lost_message(frame);
            if(waitKey(1) == ENTER_BUTTON)
                gameState = ENDED;
            break;
        case ENDED:
            lives = 3, points = 0;
            snake.clear_snake();
            draw_info_message(frame);

            int key = waitKey(1);
            if(key == ESC_BUTTON)
                gameState = QUIT;
            else if(key == R_BUTTON)
                gameState = PREPARING;
            break;
    }
}