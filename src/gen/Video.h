#pragma once

#include "Gen.h"

class Video : public Gen {
public:
    static vector<string> cache;
    static string random();
    
    Video(string name) : Gen(name) {}
    ~Video() {}
    void update(Layer *layer) override;
    void seek(float pct);
    void draw(const glm::vec3 &pos, const glm::vec3 &size) override;
    void choose() override;
    ofFloatColor getTint(Layer *layer) override;
    
private:
    ofVideoPlayer videoPlayer;
};
