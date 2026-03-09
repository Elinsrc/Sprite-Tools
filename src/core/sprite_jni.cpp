#include <jni.h>
#include "sprite_core.h"
#include <cstring>
#include <android/log.h>
#include <vector>

#include "sprite_converter_capi.h"
#include "stb_image_write.h"

#define TAG "SpriteToolsJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

extern "C" {

JNIEXPORT jlong JNICALL
Java_com_spritetools_core_SpriteNative_nativeLoadMemory(JNIEnv* env, jclass, jbyteArray data, jstring name)
{
    if (!data || !name) 
    {
        LOGE("nativeLoadMemory: null arguments");
        return 0;
    }

    jsize len = env->GetArrayLength(data);
    if (len <= 0) 
    {
        LOGE("nativeLoadMemory: empty data");
        return 0;
    }

    jbyte* bytes = env->GetByteArrayElements(data, nullptr);
    if (!bytes) 
    {
        LOGE("nativeLoadMemory: failed to get byte array");
        return 0;
    }

    const char* cname = env->GetStringUTFChars(name, nullptr);
    if (!cname) 
    {
        env->ReleaseByteArrayElements(data, bytes, JNI_ABORT);
        LOGE("nativeLoadMemory: failed to get name string");
        return 0;
    }

    LOGI("Loading: %s (%d bytes)", cname, len);

    SpriteHandle h = sprite_load_memory(reinterpret_cast<const uint8_t*>(bytes), (size_t)len, cname);

    env->ReleaseStringUTFChars(name, cname);
    env->ReleaseByteArrayElements(data, bytes, JNI_ABORT);

    if (!h) 
    {
        LOGE("sprite_load_memory returned null");
        return 0;
    }

    LOGI("Loaded successfully, handle=%p", h);
    return reinterpret_cast<jlong>(h);
}

JNIEXPORT void JNICALL
Java_com_spritetools_core_SpriteNative_nativeFree(JNIEnv*, jclass, jlong handle)
{
    if (handle != 0) 
    {

        LOGI("Freeing handle %p", reinterpret_cast<void*>(handle));
        sprite_free(reinterpret_cast<SpriteHandle>(handle));
    }
}

JNIEXPORT jintArray JNICALL
Java_com_spritetools_core_SpriteNative_nativeGetInfo(JNIEnv* env, jclass, jlong handle)
{
    if (handle == 0)
     {
        LOGE("nativeGetInfo: null handle");
        return nullptr;
    }

    SpriteInfo info;
    if (sprite_get_info(reinterpret_cast<SpriteHandle>(handle), &info) != 0) 
    {
        LOGE("sprite_get_info failed");
        return nullptr;
    }

    jintArray arr = env->NewIntArray(9);
    if (!arr)
        return nullptr;

    jint vals[9] = 
    {
        info.version, 
        (jint)info.type, 
        (jint)info.texFormat, 
        (jint)info.facetype,
        info.bounds[0], 
        info.bounds[1],
        info.numframes, 
        info.num_groups, 
        info.palette_colors
    };
    env->SetIntArrayRegion(arr, 0, 9, vals);

    LOGI("Info: v%d, %d frames, %d groups", info.version, info.numframes, info.num_groups);
    return arr;
}

JNIEXPORT jfloatArray JNICALL
Java_com_spritetools_core_SpriteNative_nativeGetFrameInfo(JNIEnv* env, jclass, jlong handle, jint frameIndex)
{
    if (handle == 0) 
        return nullptr;

    FrameInfo info;
    if (sprite_get_frame_info(reinterpret_cast<SpriteHandle>(handle), frameIndex, &info) != 0)
        return nullptr;

    jfloatArray arr = env->NewFloatArray(7);
    if (!arr) 
        return nullptr;

    jfloat vals[7] = {
        (jfloat)info.width, 
        (jfloat)info.height,
        (jfloat)info.origin[0], 
        (jfloat)info.origin[1],
        info.interval,
        (jfloat)info.group_index, 
        (jfloat)info.frame_in_group
    };
    env->SetFloatArrayRegion(arr, 0, 7, vals);
    return arr;
}

JNIEXPORT jintArray JNICALL
Java_com_spritetools_core_SpriteNative_nativeGetFrameARGB(JNIEnv* env, jclass, jlong handle, jint frameIndex)
{
    if (handle == 0) 
        return nullptr;

    FrameInfo info;
    if (sprite_get_frame_info(reinterpret_cast<SpriteHandle>(handle), frameIndex, &info) != 0) 
    {
        LOGE("nativeGetFrameARGB: frame info failed for index %d", frameIndex);
        return nullptr;
    }

    size_t count = (size_t)info.width * info.height;
    if (count == 0) return nullptr;

    std::vector<uint8_t> rgba(count * 4);
    if (sprite_get_frame_rgba(reinterpret_cast<SpriteHandle>(handle), frameIndex, rgba.data()) != 0)
    {
        LOGE("nativeGetFrameARGB: get rgba failed for index %d", frameIndex);
        return nullptr;
    }

    jintArray arr = env->NewIntArray(count);
    if (!arr) 
        return nullptr;

    std::vector<jint> argb(count);
    for (size_t i = 0; i < count; i++) 
    {
        int r = rgba[i * 4 + 0];
        int g = rgba[i * 4 + 1];
        int b = rgba[i * 4 + 2];
        int a = rgba[i * 4 + 3];
        argb[i] = (a << 24) | (r << 16) | (g << 8) | b;
    }

    env->SetIntArrayRegion(arr, 0, count, argb.data());
    return arr;
}

JNIEXPORT jbyteArray JNICALL
Java_com_spritetools_core_SpriteNative_nativeGetPalette(JNIEnv* env, jclass, jlong handle)
{
    if (handle == 0) 
        return nullptr;

    uint8_t palette[768];
    int count = sprite_get_palette(reinterpret_cast<SpriteHandle>(handle), palette);
    if (count <= 0) 
        return nullptr;

    jbyteArray arr = env->NewByteArray(count * 3);
    if (!arr) 
        return nullptr;

    env->SetByteArrayRegion(arr, 0, count * 3, reinterpret_cast<jbyte*>(palette));
    return arr;
}

JNIEXPORT jintArray JNICALL
Java_com_spritetools_core_SpriteNative_nativeGetGroupInfo(JNIEnv* env, jclass, jlong handle, jint groupIndex)
{
    if (handle == 0) 
        return nullptr;

    GroupInfo info;
    if (sprite_get_group_info(reinterpret_cast<SpriteHandle>(handle), groupIndex, &info) != 0)
        return nullptr;

    jintArray arr = env->NewIntArray(2);
    if (!arr) 
        return nullptr;

    jint vals[2] = { info.type, info.num_frames };
    env->SetIntArrayRegion(arr, 0, 2, vals);
    return arr;
}

static void jni_stbi_write_cb(void* ctx, void* data, int size)
{
    auto* v = static_cast<std::vector<uint8_t>*>(ctx);
    auto* p = static_cast<const uint8_t*>(data);
    v->insert(v->end(), p, p + size);
}

JNIEXPORT jbyteArray JNICALL
Java_com_spritetools_core_SpriteNative_nativeExportFrameToImage(JNIEnv* env, jclass, jlong handle, jint frameIndex, jint format)
{
    if (handle == 0) 
        return nullptr;

    FrameInfo info;
    if (sprite_get_frame_info(reinterpret_cast<SpriteHandle>(handle), frameIndex, &info) != 0)
        return nullptr;

    size_t pc = (size_t)info.width * info.height;
    std::vector<uint8_t> rgba(pc * 4);

    if (sprite_get_frame_rgba(reinterpret_cast<SpriteHandle>(handle), frameIndex, rgba.data()) != 0)
        return nullptr;

    std::vector<uint8_t> img_data;
    bool ok = false;

    switch (format)
    {
        case 0:
            ok = stbi_write_png_to_func(jni_stbi_write_cb, &img_data, info.width, info.height, 4, rgba.data(), info.width * 4) != 0;
            break;
        case 1:
        {
            std::vector<uint8_t> rgb(pc * 3);
            for (size_t i = 0; i < pc; i++)
            {
                rgb[i * 3 + 0] = rgba[i * 4 + 0];
                rgb[i * 3 + 1] = rgba[i * 4 + 1];
                rgb[i * 3 + 2] = rgba[i * 4 + 2];
            }
            ok = stbi_write_bmp_to_func(jni_stbi_write_cb, &img_data, info.width, info.height, 3, rgb.data()) != 0;
            break;
        }
        default:
            return nullptr;
    }

    if (!ok || img_data.empty()) 
        return nullptr;

    jbyteArray arr = env->NewByteArray((jsize)img_data.size());
    if (!arr) 
        return nullptr;

    env->SetByteArrayRegion(arr, 0, (jsize)img_data.size(), reinterpret_cast<const jbyte*>(img_data.data()));
    return arr;
}

JNIEXPORT jbyteArray JNICALL
Java_com_spritetools_core_SpriteNative_nativeCreateSprFromImages(JNIEnv* env, jclass, jobjectArray imageDataArray, jint version, jint type, jint texFormat, jfloat interval)
{
    if (!imageDataArray) 
        return nullptr;

    int count = env->GetArrayLength(imageDataArray);
    if (count <= 0) 
        return nullptr;

    struct E { jbyteArray arr; jbyte* bytes; jsize len; };
    std::vector<E> entries(count);
    std::vector<const uint8_t*> ptrs;
    std::vector<size_t> sizes;

    for (int i = 0; i < count; i++)
    {
        auto a = (jbyteArray)env->GetObjectArrayElement(imageDataArray, i);
        if (!a)
        {
            for (int j = 0; j < i; j++)
                env->ReleaseByteArrayElements(entries[j].arr, entries[j].bytes, JNI_ABORT);
            return nullptr;
        }

        entries[i].arr = a;
        entries[i].len = env->GetArrayLength(a);
        entries[i].bytes = env->GetByteArrayElements(a, nullptr);

        ptrs.push_back(reinterpret_cast<const uint8_t*>(entries[i].bytes));
        sizes.push_back((size_t)entries[i].len);
    }

    uint8_t* out_data = nullptr;
    size_t out_size = 0;

    int result = sprite_create_from_buffers( ptrs.data(), sizes.data(), count, version, type, texFormat, interval, &out_data, &out_size);

    for (int i = 0; i < count; i++)
        env->ReleaseByteArrayElements(entries[i].arr, entries[i].bytes, JNI_ABORT);

    if (result != 0 || !out_data)
    {
        if (out_data) sprite_free_buffer(out_data);
        return nullptr;
    }

    jbyteArray out_arr = env->NewByteArray((jsize)out_size);
    if (out_arr)
        env->SetByteArrayRegion(out_arr, 0, (jsize)out_size, reinterpret_cast<const jbyte*>(out_data));

    sprite_free_buffer(out_data);
    return out_arr;
}

JNIEXPORT jstring JNICALL
Java_com_spritetools_core_SpriteNative_nativeGetLastError(JNIEnv* env, jclass)
{
    return env->NewStringUTF(sprite_converter_last_error());
}
}