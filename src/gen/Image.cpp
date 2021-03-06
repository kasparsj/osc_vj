#include "Image.h"
#include "Layer.h"

vector<string> loadLocalImages()
{
    ofDirectory dir("images");
    vector<string> images;
    for (int i = 0; i < dir.getFiles().size(); i++){
        ofFile file = dir.getFile(i);
        images.push_back(file.getFileName());
    }
    return images;
}

vector<string> Image::cache = loadLocalImages();

string Image::random() {
    return cache[int(ofRandom(cache.size()))];
}

void Image::update(Layer *layer) {
    if (!image.isAllocated()) {
        string absPath = path;
        if (absPath.substr(0, 7) != "http://" && absPath.substr(0, 8) != "https://" && !ofFilePath::isAbsolute(absPath)) {
            absPath = ofToDataPath("images/" + path);
            if (!ofFile(absPath).exists()) {
                absPath = ofToDataPath(absPath);
            }
        }
        if (image.load(absPath)) {
            prevPath = path;
            layer->randomSeed = ofRandom(1000);
            if (layer->color == ofFloatColor(0, 0)) {
                layer->color = ofFloatColor(1);
            }
        }
        else {
            ofLog() << "could not load image: " << path;
            path = prevPath;
            return;
        }
    }
    aspectRatio = layer->aspectRatio;
}

void Image::draw(const glm::vec3 &pos, const glm::vec3 &size) {
    if (aspectRatio) {
        if (image.getWidth() > image.getHeight()) {
            image.draw(pos - size/2.f, size.x, size.x/image.getWidth() * image.getHeight());
        }
        else {
            image.draw(pos - size/2.f, size.y/image.getHeight() * image.getWidth(), size.y);
        }
    }
    else {
        image.draw(pos - size/2.f, size.x, size.y);
    }
}

void Image::choose() {
    path = random();
}

ofFloatColor Image::getTint(Layer *layer) {
    return layer->getColor();
}
