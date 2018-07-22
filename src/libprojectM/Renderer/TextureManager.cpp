#include <algorithm>
#include <vector>

#include "projectM-opengl.h"

#include "SOIL2/SOIL2.h"

#ifdef WIN32
#include "win32-dirent.h"
#endif

#ifdef __unix__
#include <dirent.h>
#endif
#ifdef EMSCRIPTEN
#include <dirent.h>
#endif

#ifdef __APPLE__
#include <dirent.h>
#endif
#include "TextureManager.hpp"
#include "Common.hpp"
#include "IdleTextures.hpp"
#include "Texture.hpp"
#include "PerlinNoise.hpp"




TextureManager::TextureManager(const std::string _presetsURL, const int texsizeX, const int texsizeY):
    presetsURL(_presetsURL)
{
    extensions.push_back(".jpg");
    extensions.push_back(".dds");
    extensions.push_back(".png");
    extensions.push_back(".tga");
    extensions.push_back(".bmp");
    extensions.push_back(".dib");

    Preload();
    loadTextureDir();

    // Create main texture ans associated samplers
    mainTexture = new Texture(texsizeX, texsizeY, false);
    mainTexture->getSampler(GL_REPEAT, GL_LINEAR);
    mainTexture->getSampler(GL_REPEAT, GL_NEAREST);
    mainTexture->getSampler(GL_CLAMP_TO_EDGE, GL_LINEAR);
    mainTexture->getSampler(GL_CLAMP_TO_EDGE, GL_NEAREST);
    textures["main"] = mainTexture;

    PerlinNoise noise;

    GLuint noise_texture_lq_lite;
    glGenTextures(1, &noise_texture_lq_lite);
    glBindTexture(GL_TEXTURE_2D, noise_texture_lq_lite);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_LUMINANCE, GL_FLOAT, noise.noise_lq_lite);
    Texture * textureNoise_lq_lite = new Texture(noise_texture_lq_lite, GL_TEXTURE_2D, 32, 32, false);
    textureNoise_lq_lite->getSampler(GL_REPEAT, GL_LINEAR);
    textures["noise_lq_lite"] = textureNoise_lq_lite;

    GLuint noise_texture_lq;
    glGenTextures(1, &noise_texture_lq);
    glBindTexture(GL_TEXTURE_2D, noise_texture_lq);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_LUMINANCE, GL_FLOAT, noise.noise_lq);
    Texture * textureNoise_lq = new Texture(noise_texture_lq, GL_TEXTURE_2D, 256, 256, false);
    textureNoise_lq->getSampler(GL_REPEAT, GL_LINEAR);
    textures["noise_lq"] = textureNoise_lq;

    GLuint noise_texture_mq;
    glGenTextures(1, &noise_texture_mq);
    glBindTexture(GL_TEXTURE_2D, noise_texture_mq);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_LUMINANCE, GL_FLOAT, noise.noise_mq);
    Texture * textureNoise_mq = new Texture(noise_texture_mq, GL_TEXTURE_2D, 256, 256, false);
    textureNoise_mq->getSampler(GL_REPEAT, GL_LINEAR);
    textures["noise_mq"] = textureNoise_mq;

    GLuint noise_texture_hq;
    glGenTextures(1, &noise_texture_hq);
    glBindTexture(GL_TEXTURE_2D, noise_texture_hq);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_LUMINANCE, GL_FLOAT, noise.noise_hq);
    Texture * textureNoise_hq = new Texture(noise_texture_hq, GL_TEXTURE_2D, 256, 256, false);
    textureNoise_hq->getSampler(GL_REPEAT, GL_LINEAR);
    textures["noise_hq"] = textureNoise_hq;

    GLuint noise_texture_lq_vol;
    glGenTextures( 1, &noise_texture_lq_vol );
    glBindTexture( GL_TEXTURE_3D, noise_texture_lq_vol );
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, 32 ,32 ,32 ,0 ,GL_LUMINANCE ,GL_FLOAT ,noise.noise_lq_vol);
    Texture * textureNoise_lq_vol = new Texture(noise_texture_lq_vol, GL_TEXTURE_3D, 32, 32, false);
    textureNoise_lq_vol->getSampler(GL_REPEAT, GL_LINEAR);
    textures["noisevol_lq"] = textureNoise_lq_vol;

    GLuint noise_texture_hq_vol;
    glGenTextures( 1, &noise_texture_hq_vol );
    glBindTexture( GL_TEXTURE_3D, noise_texture_hq_vol );
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, 32, 32, 32, 0, GL_LUMINANCE, GL_FLOAT, noise.noise_hq_vol);
    Texture * textureNoise_hq_vol = new Texture(noise_texture_hq_vol, GL_TEXTURE_3D, 32, 32, false);
    textureNoise_hq_vol->getSampler(GL_REPEAT, GL_LINEAR);
    textures["noisevol_hq"] = textureNoise_hq_vol;
}

TextureManager::~TextureManager()
{
    Clear();
}

void TextureManager::Preload()
{
    int width, height;

    unsigned int tex = SOIL_load_OGL_texture_from_memory(
                M_data,
                M_bytes,
                SOIL_LOAD_AUTO,
                SOIL_CREATE_NEW_ID,
                SOIL_FLAG_POWER_OF_TWO
                |  SOIL_FLAG_MULTIPLY_ALPHA
                ,&width,&height);


    Texture * newTex = new Texture(tex, GL_TEXTURE_2D, width, height, true);
    newTex->getSampler(GL_CLAMP_TO_EDGE, GL_LINEAR);
    textures["M.tga"] = newTex;

    //    tex = SOIL_load_OGL_texture_from_memory(
    //                project_data,
    //                project_bytes,
    //                SOIL_LOAD_AUTO,
    //                SOIL_CREATE_NEW_ID,
    //                SOIL_FLAG_POWER_OF_TWO
    //                |  SOIL_FLAG_MULTIPLY_ALPHA
    //                ,&width,&height);

    //    newTex = new Texture(tex, GL_TEXTURE_2D, width, height, true);
    //    newTex->getSampler(GL_CLAMP_TO_EDGE, GL_LINEAR);
    //    textures["project.tga"] = newTex;

    tex = SOIL_load_OGL_texture_from_memory(
                headphones_data,
                headphones_bytes,
                SOIL_LOAD_AUTO,
                SOIL_CREATE_NEW_ID,
                SOIL_FLAG_POWER_OF_TWO
                |  SOIL_FLAG_MULTIPLY_ALPHA
                ,&width,&height);

    newTex = new Texture(tex, GL_TEXTURE_2D, width, height, true);
    newTex->getSampler(GL_CLAMP_TO_EDGE, GL_LINEAR);
    textures["headphones.tga"] = newTex;
}

void TextureManager::Clear()
{
    for(std::map<std::string, Texture*>::const_iterator iter = textures.begin(); iter != textures.end(); iter++)
        delete(iter->second);

    textures.clear();
}


TextureSamplerDesc TextureManager::getTexture(const std::string fullName, const GLenum defaultWrap, const GLenum defaultFilter)
{
    std::string name;
    GLint wrap_mode;
    GLint filter_mode;

    ExtractTextureSettings(fullName, wrap_mode, filter_mode, name);
    if (textures.find(name) == textures.end())
    {
        return {NULL, NULL};
    }

    if (fullName == name) {
        // Warp and filter mode not specified in sampler name
        // applying default
        wrap_mode = defaultWrap;
        filter_mode = defaultFilter;
    }

    Texture * texture = textures[name];
    Sampler * sampler = texture->getSampler(wrap_mode, filter_mode);

    return {texture, sampler};
}


TextureSamplerDesc TextureManager::tryLoadingTexture(const std::string name)
{
    TextureSamplerDesc texDesc;

    for (size_t x = 0; x < extensions.size(); x++)
    {
        std::string filename = name + extensions[x];
        std::string fullURL = presetsURL + PATH_SEPARATOR + filename;

        texDesc = loadTexture(name, fullURL);

        if (texDesc.first != NULL)
        {
            break;
        }
    }

    return texDesc;
}

TextureSamplerDesc TextureManager::loadTexture(const std::string name, const std::string fileName)
{
    int width, height;

    unsigned int tex = SOIL_load_OGL_texture(
                fileName.c_str(),
                SOIL_LOAD_AUTO,
                SOIL_CREATE_NEW_ID,
                SOIL_FLAG_MULTIPLY_ALPHA
                ,&width,&height);

    if (tex == 0)
    {
        return {NULL, NULL};
    }

    Texture * newTexture = new Texture(tex, GL_TEXTURE_2D, width, height, true);
    Sampler * sampler = newTexture->getSampler(GL_CLAMP_TO_EDGE, GL_LINEAR);

    textures[name] = newTexture;

    return {newTexture, sampler};
}


void TextureManager::loadTextureDir()
{
    std::string dirname = std::string(DATADIR_PATH) + "/presets";

    DIR * m_dir;

    // Allocate a new a stream given the current directory name
    if ((m_dir = opendir(dirname.c_str())) == NULL)
    {
        std::cout<<"No Textures Loaded from "<<dirname<<std::endl;
        return; // no files loaded. m_entries is empty
    }

    struct dirent * dir_entry;

    while ((dir_entry = readdir(m_dir)) != NULL)
    {
        // Convert char * to friendly string
        std::string filename(dir_entry->d_name);

        if (filename.length() > 0 && filename[0] == '.')
            continue;

        std::string lowerCaseFileName(filename);
        std::transform(lowerCaseFileName.begin(), lowerCaseFileName.end(), lowerCaseFileName.begin(), tolower);

        // Remove extension
        for (size_t x = 0; x < extensions.size(); x++)
        {
            size_t found = lowerCaseFileName.find(extensions[x]);
            if (found != std::string::npos)
            {
                std::string name = filename;
                name.replace(int(found), extensions[x].size(), "");

                // Create full path name
                std::string fullname = dirname + PATH_SEPARATOR + filename;
                loadTexture(name, fullname);

                break;
            }
        }
    }

    if (m_dir)
    {
        closedir(m_dir);
        m_dir = 0;
    }
}

std::string TextureManager::getRandomTextureName(std::string random_id)
{

    std::vector<std::string> user_texture_names;

    for(std::map<std::string, Texture*>::const_iterator iter = textures.begin(); iter != textures.end(); iter++)
    {
        if (iter->second->userTexture) {
            user_texture_names.push_back(iter->first);
        }
    }

    if (user_texture_names.size() > 0)
    {
        std::string random_name = user_texture_names[rand() % user_texture_names.size()];
        random_textures.push_back(random_id);
        textures[random_id] = textures[random_name];
        return random_name;
    }
    else return "";
}

void TextureManager::clearRandomTextures()
{
    for (std::vector<std::string>::iterator pos = random_textures.begin(); pos	!= random_textures.end(); ++pos)
    {
        textures.erase(*pos);
    }
    random_textures.clear();

}

void TextureManager::ExtractTextureSettings(const std::string qualifiedName, GLint & _wrap_mode, GLint & _filter_mode, std::string & name)
{
    std::string lowerQualifiedName(qualifiedName);
    std::transform(lowerQualifiedName.begin(), lowerQualifiedName.end(), lowerQualifiedName.begin(), tolower);

    _wrap_mode = GL_REPEAT;
    _filter_mode = GL_LINEAR;

    if (lowerQualifiedName.substr(0,3) == "fc_")
    {
        name = qualifiedName.substr(3);
        _filter_mode = GL_LINEAR;
        _wrap_mode = GL_CLAMP_TO_EDGE;
    }
    else if (lowerQualifiedName.substr(0,3) == "fw_")
    {
        name = qualifiedName.substr(3);
        _filter_mode = GL_LINEAR;
        _wrap_mode = GL_REPEAT;
    }
    else if (lowerQualifiedName.substr(0,3) == "pc_")
    {
        name = qualifiedName.substr(3);
        _filter_mode = GL_NEAREST;
        _wrap_mode = GL_CLAMP_TO_EDGE;
    }
    else if (lowerQualifiedName.substr(0,3) == "pw_")
    {
        name = qualifiedName.substr(3);
        _filter_mode = GL_NEAREST;
        _wrap_mode = GL_REPEAT;
    }
    else
    {
        name = qualifiedName;
    }
}

const Texture * TextureManager::getMainTexture() const {
    return mainTexture;
}

const std::vector<Texture*> & TextureManager::getBlurTextures() const {
    return blurTextures;
}


void TextureManager::updateMainTexture()
{
    glBindTexture(GL_TEXTURE_2D, mainTexture->texID);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, mainTexture->width, mainTexture->height);
    glBindTexture(GL_TEXTURE_2D, 0);
}
