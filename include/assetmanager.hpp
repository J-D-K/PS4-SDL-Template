#pragma once
#include "font.hpp"
#include "texture.hpp"
#include <memory>
#include <string>
#include <unordered_map>

template <typename Type>
class assetmanager
{
    public:
        assetmanager(const assetmanager &) = delete;
        assetmanager(assetmanager &&) = delete;
        assetmanager &operator=(const assetmanager &) = delete;
        assetmanager &operator=(assetmanager &&) = delete;

        // This template function relies on using overloaded constructors.
        template <typename... Arguments>
        static std::shared_ptr<Type> createAndLoadAsset(const std::string &assetName, Arguments &&...arguments)
        {
            // Pointer to asset to return.
            std::shared_ptr<Type> assetPointer;
            // Get instance
            assetmanager &manager = getInstance();
            // Search by path/asset name.
            auto assetSearch = manager.m_AssetMap.find(assetName);
            if (assetSearch != manager.m_AssetMap.end())
            {
                // If it's found, try to lock it so it doesn't get freed.
                assetPointer = manager.m_AssetMap.at(assetName).lock();
            }
            else
            {
                // Load it using constructor.
                assetPointer = std::make_shared<Type>(std::forward<Arguments>(arguments)...);
                manager.m_AssetMap[assetName] = assetPointer;
            }
            return assetPointer;
        }

    private:
        // Non-constructable... is that even a word?
        assetmanager(void) = default;
        // Returns instance.
        static assetmanager &getInstance(void)
        {
            static assetmanager instance;
            return instance;
        }
        // Map of assets.
        std::unordered_map<std::string, std::weak_ptr<Type>> m_AssetMap;
};

using fontmanager = assetmanager<font::ttf>;
using texturemanager = assetmanager<SDL::texture>;
