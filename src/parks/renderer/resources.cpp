//
// Created by brett on 6/12/23.
//
#include <parks/renderer/resources.h>
#include <stb/stb_image.h>
#include <thread>
#include <mutex>
#include <queue>
#include <parks/config.h>

namespace parks {
    
    struct LoadableTexture {
        std::string path, name;
    };
    
    struct LoadedTexture {
        void* data = nullptr;
        int width = 0, height = 0, channels = 0;
        std::string textureName;
    };
    
    struct {
        int numThreads = 8;
        volatile int finishedThreads = 0;
        std::queue<LoadableTexture> texturesToLoad;
        std::queue<LoadedTexture> loadedTextures;
        hashmap<std::string, GLTexture2D*> gl2DTextures;
        std::mutex textureQueueMutex;
        std::mutex glLoadQueueMutex;
        std::mutex threadSyncMutex;
        std::thread** threads = nullptr;
    } TextureLoader;
    
    void resources::loadTexture(const std::string& texture_path, const std::string& texture_name) {
        auto t = LoadableTexture{texture_path,
                  texture_name == _defaults_::ASSUME_TEXTURE_NAME_ID ? texture_path : texture_name};
        TextureLoader.texturesToLoad.push(t);
        BLT_TRACE("Registered texture %s @ %s", t.name.c_str(), t.path.c_str());
    }
    
    GLTexture2D* resources::getTexture(const std::string& texture_name) {
        return TextureLoader.gl2DTextures[texture_name];
    }
    
    void resources::beginLoading() {
        BLT_DEBUG("Beginning loading of resources");
        stbi_set_flip_vertically_on_load(true);
        for (int i = 0; i < TextureLoader.numThreads; i++) {
            TextureLoader.threads[i] = new std::thread(
                    []() -> void {
                        stbi_set_flip_vertically_on_load_thread(true);
                        while (!TextureLoader.texturesToLoad.empty()) {
                            LoadableTexture texture;
                            {
                                std::scoped_lock<std::mutex> textureQueueLock{
                                        TextureLoader.textureQueueMutex};
                                texture = TextureLoader.texturesToLoad.front();
                                TextureLoader.texturesToLoad.pop();
                            }
                            LoadedTexture loadedTexture;
                            loadedTexture.data = stbi_load(texture.path.c_str(), &loadedTexture.width, &loadedTexture.height, &loadedTexture.channels, 4);
                            loadedTexture.channels = 4;
                            loadedTexture.textureName = texture.name;
                            {
                                std::scoped_lock<std::mutex> loadQueueLock{TextureLoader.glLoadQueueMutex};
                                TextureLoader.loadedTextures.push(loadedTexture);
                            }
                        }
                        {
                            std::scoped_lock<std::mutex> threadSyncLock(
                                    TextureLoader.threadSyncMutex
                            );
                            TextureLoader.finishedThreads++;
                        }
                    }
            );
        }
        
        while (TextureLoader.finishedThreads < TextureLoader.numThreads || !TextureLoader.loadedTextures.empty()) {
            LoadedTexture texture;
            {
                if (TextureLoader.loadedTextures.empty())
                    continue;
                std::scoped_lock<std::mutex> loadQueueLock{TextureLoader.glLoadQueueMutex};
                texture = TextureLoader.loadedTextures.front();
                TextureLoader.loadedTextures.pop();
            }
            auto* texture2D = new GLTexture2D;
            texture2D->upload(texture.data, GL_UNSIGNED_BYTE, texture.width, texture.height, texture.channels);
            glGenerateMipmap(GL_TEXTURE_2D);
            TextureLoader.gl2DTextures[texture.textureName] = texture2D;
            stbi_image_free(texture.data);
        }
        
        for (int i = 0; i < TextureLoader.numThreads; i++) {
            if (TextureLoader.threads[i]->joinable())
                TextureLoader.threads[i]->join();
            delete TextureLoader.threads[i];
        }
        delete[] TextureLoader.threads;
        BLT_INFO("All resources have been loaded. A total of %d textures!", TextureLoader.gl2DTextures.size());
    }
    
    void resources::init(int threads) {
        BLT_INFO("Using %d thread%s to load resources!", threads, threads > 1 ? "s" : "");
        TextureLoader.numThreads = threads;
        TextureLoader.finishedThreads = 0;
        TextureLoader.threads = new std::thread* [threads];
    }
    
    void resources::cleanup() {
        BLT_DEBUG("Beginning texture cleanup");
        for (auto& t : TextureLoader.gl2DTextures)
            delete t.second;
        BLT_INFO("Finished cleaning up loose resources!");
    }
}