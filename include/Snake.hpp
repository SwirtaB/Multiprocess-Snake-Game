//
// Created by swirta on 02.05.2021.
//

#ifndef SCZR_SNAKE_SNAKE_HPP
#define SCZR_SNAKE_SNAKE_HPP

#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <deque>

const int fruitRadius = 20;

enum GAME_STATE{
    PREPARING,
    PLAYING,
    LOST,
    ENDED
};

class Snake{
public:
    void clear_snake();
    void grow(cv::Point chunk);
    void move(cv::Point point);
    int length();
    void draw(cv::Mat &frame);
    bool check_snake();
    bool eat_fruit(cv::Point &fruit);
    cv::Point get_point(int i);

private:
    bool is_intersected(const cv::Point &a,const  cv::Point &b,const cv::Point &c,const cv::Point &d);
    int orientation(const cv::Point &a, const cv::Point &b, const cv::Point &c);
    std::deque<cv::Point> snakeBody;
    int maxLength;
};

class Game{
public:
    void draw(cv::Mat &frame);
    void draw_lost_message(cv::Mat &frame);
    void draw_info_message(cv::Mat &frame);
    void create_snake(cv::Point &head);
    void generate_fruit();
    void run(cv::Point &marker, cv::Mat &frame, bool *close);

    Game(cv::Point windowSize_) : gameState(PREPARING), points(0), lives(3), windowSize(windowSize_) {}
private:
    GAME_STATE gameState;
    int points, lives;
    cv::Point windowSize;
    cv::Point fruit;
    Snake snake;

};

#endif //SCZR_SNAKE_SNAKE_HPP
