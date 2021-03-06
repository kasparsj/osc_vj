#include "Video.h"
#include "Layer.h"

vector<string> loadLocalVideos()
{
    ofDirectory dir("videos");
    vector<string> videos;
    for (int i = 0; i < dir.getFiles().size(); i++){
        ofFile file = dir.getFile(i);
        if (file.getExtension() == "mov") {
            videos.push_back(file.getFileName());
        }
    }
    return videos;
}

vector<string> Video::cache = loadLocalVideos();

string Video::random() {
    return cache[int(ofRandom(cache.size()))];
}

void Video::update(Layer *layer) {
    if (!videoPlayer.isLoaded()) {
        videoPlayer.close();
        string absPath = path;
        if (!ofFilePath::isAbsolute(absPath)) {
            absPath = ofToDataPath("videos/" + path);
            if (!ofFile(absPath).exists()) {
                absPath = ofToDataPath(absPath);
            }
        }
        if (videoPlayer.load(absPath)) {
            videoPlayer.setVolume(0);
            videoPlayer.setLoopState(OF_LOOP_NORMAL);
            seek(layer->timePct);
            videoPlayer.play();
            prevPath = path;
            layer->randomSeed = ofRandom(1000);
            if (layer->color == ofFloatColor(0, 0)) {
                layer->color = ofFloatColor(1);
            }
        }
        else {
            ofLog() << "could not load video: " << path;
            path = prevPath;
            return;
        }
    }
    else if (layer->data->onset) {
        seek(layer->timePct);
    }
    aspectRatio = layer->aspectRatio;
    videoPlayer.update();
}

void Video::seek(float pct) {
    videoPlayer.setPosition(pct);
}

void Video::draw(const glm::vec3 &pos, const glm::vec3 &size) {
    if (aspectRatio) {
        if (videoPlayer.getWidth() > videoPlayer.getHeight()) {
            videoPlayer.draw(pos - size/2.f, size.x, size.x/videoPlayer.getWidth() * videoPlayer.getHeight());
        }
        else {
            videoPlayer.draw(pos - size/2.f, size.y/videoPlayer.getHeight() * videoPlayer.getWidth(), size.y);
        }
    }
    else {
        videoPlayer.draw(pos - size/2.f, size.x, size.y);
    }
}

void Video::choose() {
    path = random();
}

ofFloatColor Video::getTint(Layer *layer) {
    return layer->getColor();
}
