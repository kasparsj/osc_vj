#include "Layer.h"
#include "Shader.h"
#include "Video.h"
#include "Sketch.h"
#include "Image.h"

Gen* Layer::factory(string type, string path) {
    Gen *gen = NULL;
    auto it = SourceMap.find(type);
    if (it != SourceMap.end()) {
        switch (it->second) {
            case S_VIDEO:
                gen = new Video(path);
                break;
            case S_SHADER:
                gen = new Shader(path);
                break;
            case S_SKETCH:
                gen = new Sketch(path);
                break;
            case S_IMAGE:
                gen = new Image(path);
                break;
        }
    }
    return gen;
}

Gen* Layer::factory(string source) {
    string type = source;
    string path = "";
    bool explicitType = source.find(":") != string::npos;
    if (explicitType) {
        type = source.substr(0, source.find(":"));
        path = source.substr(source.find(":") + 1);
    }
    if (path == "") {
        auto it = SourceMap.find(type);
        if (it != SourceMap.end()) {
            switch (it->second) {
                case S_VIDEO:
                    path = Video::random();
                    break;
                case S_SHADER:
                    path = Shader::random();
                    break;
                case S_SKETCH:
                    path = Sketch::random();
                    break;
                case S_IMAGE:
                    path = Image::random();
                    break;
            }
        }
    }
    return factory(type, path);
}

void Layer::setup(int index, int numVisuals, string dataSource)
{
    this->index = index;
    this->total = numVisuals;
    dataSources.clear();
    if (dataSource != "") {
        dataSources.push_back(dataSource);
    }
}

void Layer::layout(Layout layout)
{
    switch (layout) {
        case L_COLUMN:
            pos = glm::vec3(0, ofGetHeight() / total * index, 0);
            size = glm::vec3(ofGetWidth(), ofGetHeight() / total, 0);
            break;
        case L_ROW:
            pos = glm::vec3(ofGetWidth() / total * index, 0, 0);
            size = glm::vec3(ofGetWidth() / total, ofGetHeight(), 0);
            break;
        case L_GRID: {
            int half = (int) ceil(total / 2.f);
            pos = glm::vec3(ofGetWidth() / half * (index % half), ofGetHeight() / 2 * floor(index / half), 0);
            size = glm::vec3(ofGetWidth() / half, ofGetHeight() / 2, 0);
            break;
        }
        case L_STACK:
            pos = glm::vec3(0, 0, 0);
            size = glm::vec3(ofGetWidth(), ofGetHeight(), 0);
            break;
    }
}

void Layer::update(const vector<Sound> &sounds, const vector<TidalNote> &notes) {
    if (data != NULL) {
        data->update(sounds, notes);
    }
    if (gen != NULL) {
        gen->update(this);
    }
}

void Layer::draw(const glm::vec3 &pos, const glm::vec3 &size) {
    if (gen != NULL) {
        ofPushStyle();
        ofSetColor(255 * bri, alpha * 255);
        ofPushMatrix();
        ofTranslate(ofGetWidth()/2.f, ofGetHeight()/2.f);
        ofRotateDeg(rotAngle, rotAxis.x, rotAxis.y, rotAxis.z);
        gen->draw(pos, size);
        ofPopMatrix();
        ofPopStyle();
    }
}

void Layer::draw() {
    if (data != NULL) {
        if (data->visible) {
            draw(pos, size);
        }
        data->afterDraw();
    }
}

void Layer::setDataSources(vector<string> ds) {
    dataSources.clear();
    addDataSources(ds);
}

void Layer::addDataSources(vector<string> ds) {
    for (int i=0; i<ds.size(); i++) {
        string dsName = ds[i];
        string dsMax = "";
        if (dsName.find(":") != string::npos) {
            dsMax = dsName.substr(dsName.find(":") + 1);
            dsName = dsName.substr(0, dsName.find(":"));
        }
        auto it = DataSourceMap.find(dsName);
        if (it != DataSourceMap.end()) {
            // todo: should hold struct instead of string
            dataSources.push_back(it->first + dsMax);
        }
        else {
            ofLog() << "invalid data source " << ds[i];
        }
    }
}

void Layer::load(string source) {
    unload();
    bool explicitType = source.find(":") != string::npos;
    if (explicitType) {
        gen = factory(source);
    }
    else {
        string extension = ofFile(source).getExtension();
        for (int i=0; i<extension.size(); i++) {
            extension[i] = tolower(extension[i]);
        }
        if (extension == "frag") {
            gen = factory("shader", source);
        }
        else if (extension == "jpg" || extension == "jpeg" || extension == "png") {
            gen = factory("image", source);
        }
        else if (extension == "mov") {
            gen = factory("video", source);
        }
        else {
            if (Sketch::exists(source)) {
                gen = factory("sketch", source);
            }
        }
    }
    if (gen != NULL) {
        data = new LayerData(this);
    }
    else {
        ofLog() << "invalid source " << source;
    }
}

void Layer::choose(string type) {
    if (type == "") {
        auto it = SourceMap.begin();
        advance(it, int(ofRandom(SourceMap.size())));
        type = it->first;
    }
    gen = factory(type);
    if (gen != NULL) {
        data = new LayerData(this);
    }
    else {
        ofLog() << "invalid source type " << type;
    }
}

void Layer::unload() {
    if (gen != NULL) {
        delete gen;
        gen = NULL;
    }
    if (data != NULL) {
        delete data;
        data = NULL;
    }
}

void Layer::reload() {
    if (gen != NULL) {
        gen->reload();
    }
    else {
        ofLog() << "cannot reload layer " << index;
    }
}

void Layer::clear() {
    if (gen != NULL) {
        gen->clear();
    }
    else {
        ofLog() << "cannot clear layer " << index;
    }
}

void Layer::reset() {
    if (gen != NULL) {
        gen->reset();
    }
    else {
        ofLog() << "cannot reset layer " << index;
    }
}
