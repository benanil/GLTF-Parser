# GLTF-Parser
Fast Gltf scene parser. one Cpp file only, mostly C code but some C++ 11 and C++ 14 features used.<br> <br>
No external dependency! C runtime is also haven't used in this project except stdio.h for file interactions <br>
Matrix conversion code may require ARM neon or SSE4.1, but in case these instruction sets are not exist there is scalar versions as well.<br><br>
Custom string and integer allocators have been used for performance, unlike other json parsers parser doesn't store strings or hash strings, <br>
compares the values and stores the required values immediately that's why this is faster than other gltf parsers. <br><br>
haven't tested mac and ios platform but Android, Windows and gcc, clang msvc compilers works fine.<br><br>
no .glb support yet. only .gltf + .bin + image files
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
    FreeGLTFBuffers(&sponzaScene);
    FreeGltf(&sponzaScene);
}
```
for custom allocators define this macros before including GLTFParser.h 
```c
#ifndef AX_MALLOC
    #define AX_MALLOC(size) (malloc(size))
    #define AX_CALLOC(size) (calloc(size, 1))
    #define AX_FREE(ptr)    (free(ptr))
#endif
```
![animation and scene imported using this lib](https://github.com/benanil/GLTF-Parser/assets/48527900/a23d504e-4d97-44e5-bd69-687765b8138d)<br>
Library has been used by my engine:
[Template Sane Program](https://github.com/benanil/TemplateSaneProgram)

# Contributing

feel free to contribute

#Contact
If you have any questions, feedback, or suggestions, feel free to reach out:<br>

Email: anilcangulkaya7@gmail.com<br>
Twitter: @anilcanglk12<br>
GitHub: @benanil<br>
<br>
Feel free to reach out regarding any inquiries related to the project.<br>
