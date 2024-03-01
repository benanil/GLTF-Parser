# GLTF-Parser
Fast Gltf scene parser. one Cpp file only, mostly C code but some C++ 11 and C++ 14 features used.
No external dependency! C runtime is also haven't used in this project except stdio.h for file interactions <br>
Matrix conversion code may require ARM neon or SSE4.1, but in case these instruction sets are not exist there is scalar versions as well.<br>
Custom string and integer allocators have been used for performance, unlike other json parsers parser doesn't store or hash strings, compares the values and stores the required values immediately that's why this is faster than other gltf parsers.

```c
int main()
{
    SceneBundle sponzaScene;
    float scale = 1.2f;
    int parsed = ParseGLTF("SponzaGLTF/scene.gltf", &sponzaScene, scale);
    if (parsed == 0)
    {
        printf("gltf scene parsing error!: %s", ParsedSceneGetError(sponzaScene.error));
    }
    // use scene....
}
```
