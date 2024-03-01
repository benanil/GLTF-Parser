struct ASkinedVertex
{
    Vector3f position;
    uint     normal;
    uint     tangent;
    half2    texCoord;
    uint     joints;  // rgb8u
    uint     weights; // rgb8u
};

void CreateVerticesIndicesSkined(SceneBundle* gltf)
{
    AMesh* meshes = gltf->meshes;
    
    // pre allocate all vertices and indices 
    gltf->allVertices = AllocAligned(sizeof(ASkinedVertex) * gltf->totalVertices, alignof(ASkinedVertex));
    gltf->allIndices  = AllocAligned(gltf->totalIndices * sizeof(uint32_t) + 16, alignof(uint32)); // 16->give little bit of space for memcpy
    
    ASkinedVertex* currVertex = (ASkinedVertex*)gltf->allVertices;
    uint32_t* currIndices = (uint32_t*)gltf->allIndices;
    
    int vertexCursor = 0;
    int indexCursor = 0;
    
    for (int m = 0; m < gltf->numMeshes; ++m)
    {
        // get number of vertex, getting first attribute count because all of the others are same
        AMesh mesh = meshes[m];
        for (uint64_t p = 0; p < mesh.numPrimitives; p++)
        {
            APrimitive& primitive = mesh.primitives[p];
            char* beforeCopy = (char*)primitive.indices;
            primitive.indices = currIndices;
            int indexSize = GraphicsTypeToSize(primitive.indexType);

            for (int i = 0; i < primitive.numIndices; i++)
            {
                uint32_t index = 0;
                SmallMemCpy(&index, beforeCopy, indexSize);
                // we are combining all vertices and indices into one buffer, that's why we have to add vertex cursor
                currIndices[i] = index + vertexCursor; 
                beforeCopy += indexSize;
            }
            
            // https://www.yosoygames.com.ar/wp/2018/03/vertex-formats-part-1-compression/
            primitive.vertices = currVertex;
            Vector3f* positions = (Vector3f*)primitive.vertexAttribs[0];
            Vector2f* texCoords = (Vector2f*)primitive.vertexAttribs[1];
            Vector3f* normals   = (Vector3f*)primitive.vertexAttribs[2];
            vec_t*    tangents  = (vec_t*)primitive.vertexAttribs[3];

            for (int v = 0; v < primitive.numVertices; v++)
            {
                vec_t tangent     = tangents  ? tangents[v]  : VecZero();
                Vector2f texCoord = texCoords ? texCoords[v] : Vector2f{0.0f, 0.0f};
                Vector3f normal   = normals   ? normals[v]   : Vector3f{0.5f, 0.5f, 0.0};

                currVertex[v].position  = positions[v];
                currVertex[v].texCoord  = ConvertToHalf2(&texCoord.x);
                currVertex[v].normal    = Pack_INT_2_10_10_10_REV(normal);
                currVertex[v].tangent   = Pack_INT_2_10_10_10_REV(tangent);
            }

            // convert whatever joint format to rgb8u
            char* joints  = (char*)primitive.vertexAttribs[5];
            char* weights = (char*)primitive.vertexAttribs[6];

            // size and offset in bytes
            int jointSize = GraphicsTypeToSize(primitive.jointType);
            int jointOffset = MAX((int)(primitive.jointStride - (jointSize * primitive.jointCount)), 0); // stride - sizeof(rgbau16)
            // size and offset in bytes
            int weightSize   = GraphicsTypeToSize(primitive.weightType);
            int weightOffset = MAX((int)(primitive.weightStride - (weightSize * primitive.jointCount)), 0);
            
            for (int j = 0; j < primitive.numVertices; j++)
            {
                // Combine 4 indices into one integer to save space
                uint32_t packedJoints = 0u;
                // iterate over joint indices, most of the time 4 indices
                for (int k = 0, shift = 0; k < primitive.jointCount; k++) 
                {
                    uint32_t jointIndex = 0;
                    SmallMemCpy(&jointIndex, joints, jointSize); 
                    ASSERT(jointIndex < 255u && "index has to be smaller than 255");
                    packedJoints |= jointIndex << shift;
                    shift += 8;
                    joints += jointSize;
                }

                uint32_t packedWeights;
                if (weightSize == 4) // if float, pack it directly
                {
                    packedWeights = PackColorRGBAU32((float*)weights);
                    weights += weightSize * 4;
                }
                else
                {
                    for (int k = 0, shift = 0; k < primitive.jointCount && k < 4; k++, shift += 8)
                    {
                        uint32 jointWeight = 0u;
                        SmallMemCpy(&jointWeight, weights, weightSize); 
                        float weightMax = (float)((1u << (weightSize * 8)) - 1);
                        float norm = (float)jointWeight / weightMax; // divide by 255 or 65535
                        packedWeights |= uint32_t(norm * 255.0f) << shift;
                        weights += weightSize;
                    }
                }
                currVertex[j].joints  = packedJoints;
                currVertex[j].weights = packedWeights;
                joints  += jointOffset; // stride offset at the end of the struct
                weights += weightOffset;
            }

            currVertex += primitive.numVertices;
            primitive.indexOffset = indexCursor;
            indexCursor += primitive.numIndices;
            
            vertexCursor += primitive.numVertices;
            currIndices  += primitive.numIndices;
        }
    }
    
    for (int s = 0; s < gltf->numSkins; s++)
    {
        ASkin& skin = gltf->skins[s];
        Matrix4* inverseBindMatrices = new Matrix4[skin.numJoints];
        SmallMemCpy(inverseBindMatrices, skin.inverseBindMatrices, sizeof(Matrix4) * skin.numJoints);
        for (int i = 0; i < skin.numJoints; i++)
            ;//inverseBindMatrices[i] = Matrix4::Transpose(inverseBindMatrices[i]);

        skin.inverseBindMatrices = (float*)inverseBindMatrices;
    }

    if (gltf->numAnimations)
    {
        int totalSamplerInput = 0;
        for (int a = 0; a < gltf->numAnimations; a++)
            for (int s = 0; s < gltf->animations[a].numSamplers; s++)
                totalSamplerInput += gltf->animations[a].samplers[s].count;
        
        float* currSampler = new float[totalSamplerInput]{};
        vec_t* currOutput  = new vec_t[totalSamplerInput]{};

        for (int a = 0; a < gltf->numAnimations; a++)
        {
            for (int s = 0; s < gltf->animations[a].numSamplers; s++)
            {
                AAnimSampler& sampler = gltf->animations[a].samplers[s];
                SmallMemCpy(currSampler, sampler.input, sampler.count * sizeof(float));
                sampler.input = currSampler;
                currSampler += sampler.count;

                for (int i = 0; i < sampler.count; i++)
                {
                    SmallMemCpy(currOutput + i, sampler.output + (i * sampler.numComponent), sizeof(float) * sampler.numComponent);
                    // currOutput[i] = VecLoad(sampler.output + (i * sampler.numComponent));
                    // if (sampler.numComponent == 3) currOutput[i] = VecSetW(currOutput[i], 0.0f);
                }
                sampler.output = (float*)currOutput;
                currOutput += sampler.count;
            }
        }
    }

    FreeSceneBundleBuffers(gltf);
}
