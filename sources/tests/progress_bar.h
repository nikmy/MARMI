#pragma once

#include <string>
#include <iostream>

class ProgressBar
{
 public:
    explicit ProgressBar(int max_len);
    void make_progress();
    void update() const;
    void assign(int max_len);

 private:
    int progress_;
    int max_progress_;
};

ProgressBar::ProgressBar(int max_len)
{
    progress_ = 0;
    max_progress_ = max_len;
}

void ProgressBar::make_progress()
{
    progress_ = std::min(progress_ + 1, max_progress_);
}

void ProgressBar::update() const
{
    std::cout << std::string("\r[") + std::string(progress_, '#')
              << std::string(max_progress_ - progress_, '.')
              << std::string("] ")
              << std::to_string(progress_ * 100 / max_progress_)
              << std::string(" %");
    std::flush(std::cout);
}

void ProgressBar::assign(int max_len)
{
    progress_ = 0;
    max_progress_ = max_len;
}
