//
// Created by swirta on 02.05.2021.
//

#include "../include/Snake.hpp"
#include <random>

using namespace std;
using namespace cv;

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
            cv::line(frame, *it, *(it + 1), {127, 8, 255}, 5);
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
        a = get_point(0);
        b = get_point(1);
        int snakeSize = snakeBody.size();
        for(int i = 2; i < snakeSize; ++i){
            c = get_point(i);
            d = get_point(i + 1);
            if(is_intersected(a, b, c, d))
                return true;
        }
    }
    return false;
}
bool Snake::eat_fruit(Point fruit) {
    Point a = get_point(0), b = get_point(1);
    a.x -= fruit.x;
    a.y -= fruit.y;
    b.x -= fruit.x;
    b.y -= fruit.y;
    int diffX = b.x - a.x;
    int diffY = b.y - a.y;
    int diffSquared = diffX^2 + diffY^2;
    int distance = a.x*b.y - b.x*a.y;
    return fruitRadius*fruitRadius * diffSquared > distance*distance;
}

void Game::generate_fruit() {
    random_device randomDevice;
    mt19937 randomEngine(randomDevice());
    uniform_int_distribution<> posXGen(50, sizeX);
    uniform_int_distribution<> posYGen(50, sizeY);
    fruit.x = posXGen(randomEngine);
    fruit.y = posYGen(randomEngine);
}

void Game::create_snake(Point head) {
    for(int i = 1; i <= 10; ++i)
        snake.grow(Point(head.x - 5*i, head.y));
}

void Game::draw(Mat &frame){
    snake.draw(frame);
    circle(frame, fruit, fruitRadius, Scalar(255, 0, 0), FILLED);

    putText(frame, "LIVES: " + to_string(lives), Point(50, 50), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 255, 0), 2);
    putText(frame, "SCORE: " + to_string(points), Point(sizeX - 50, sizeY - 50), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 255, 0), 2);

}