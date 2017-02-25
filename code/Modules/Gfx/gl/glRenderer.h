#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::glRenderer
    @brief OpenGL wrapper and state cache
*/
#include "Core/Types.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/BlendState.h"
#include "Gfx/Core/DepthStencilState.h"
#include "Gfx/Core/RasterizerState.h"
#include "Gfx/Core/PrimitiveGroup.h"
#include "Gfx/Core/PassState.h"
#include "Gfx/Core/gfxPointers.h"
#include "Gfx/Attrs/DisplayAttrs.h"
#include "Gfx/Attrs/ImageDataAttrs.h"
#include "Gfx/Setup/GfxSetup.h"
#include "Gfx/gl/gl_decl.h"
#include "Gfx/gl/glVertexAttr.h"
#include "glm/vec4.hpp"

namespace Oryol {
namespace _priv {

class texture;
class pipeline;
class mesh;
class shader;
class renderPass;

class glRenderer {
public:
    /// constructor
    glRenderer();
    /// destructor
    ~glRenderer();
    
    /// setup the renderer
    void setup(const GfxSetup& setup, const gfxPointers& ptrs);
    /// discard the renderer
    void discard();
    /// return true if renderer has been setup
    bool isValid() const;
    
    /// test if a feature is supported
    bool queryFeature(GfxFeature::Code feat) const;
    /// commit current frame
    void commitFrame();
    /// get the current render pass attributes
    const DisplayAttrs& renderPassAttrs() const;

    /// begin rendering pass (both ptrs can be nullptr)
    void beginPass(renderPass* pass, const PassState* passState);
    /// end current rendering pass
    void endPass();

    /// apply viewport
    void applyViewPort(int x, int y, int width, int height, bool originTopLeft);
    /// apply scissor rect
    void applyScissorRect(int x, int y, int width, int height, bool originTopLeft);
    /// apply draw state
    void applyDrawState(pipeline* pip, mesh** meshes, int numMeshes);
    /// apply a shader uniform block (called after applyDrawState)
    void applyUniformBlock(ShaderStage::Code bindStage, int bindSlot, uint32_t layoutHash, const uint8_t* ptr, int byteSize);
    /// apply a group of textures
    void applyTextures(ShaderStage::Code bindStage, texture** textures, int numTextures);

    /// submit a draw call with primitive group index in current mesh
    void draw(int primGroupIndex, int numInstances);
    /// submit a draw call with direct primitive group
    void draw(int baseElementIndex, int numElements, int numInstances);

    /// update vertex data
    void updateVertices(mesh* msh, const void* data, int numBytes);
    /// update index data
    void updateIndices(mesh* msh, const void* data, int numBytes);
    /// update texture pixel data
    void updateTexture(texture* tex, const void* data, const ImageDataAttrs& offsetsAndSizes);
    
    /// invalidate bound mesh state
    void invalidateMeshState();
    /// bind vertex buffer with state caching
    void bindVertexBuffer(GLuint vb);
    /// bind index buffer with state caching
    void bindIndexBuffer(GLuint ib);

    /// invalidate shader state
    void invalidateShaderState();
    /// invoke glUseProgram (if changed)
    void useProgram(GLuint prog);

    /// invalidate texture state
    void invalidateTextureState();
    /// bind a texture to a sampler index
    void bindTexture(int samplerIndex, GLenum target, GLuint tex);

    /// generate mipmaps for texture
    void generateMipmaps(texture* tex);
    
private:
    /// setup the initial depth-stencil-state
    void setupDepthStencilState();
    /// setup the initial blend-state
    void setupBlendState();
    /// setup rasterizer state
    void setupRasterizerState();
    /// apply front/back side stencil state
    void applyStencilState(const DepthStencilState& state, const DepthStencilState& curState, GLenum glFace);

    bool valid;
    gfxPointers pointers;
    #if !ORYOL_OPENGLES2
    GLuint globalVAO;
    #endif
    uint64_t frameIndex;

    static GLenum mapCompareFunc[CompareFunc::NumCompareFuncs];
    static GLenum mapStencilOp[StencilOp::NumStencilOperations];
    static GLenum mapBlendFactor[BlendFactor::NumBlendFactors];
    static GLenum mapBlendOp[BlendOperation::NumBlendOperations];
    static GLenum mapCullFace[Face::NumFaceCodes];

    bool rpValid;
    DisplayAttrs rpAttrs;
    GfxSetup gfxSetup;
    
    // high-level state cache
    renderPass* curRenderPass;
    pipeline* curPipeline;
    mesh* curPrimaryMesh;

    // GL state cache
    BlendState blendState;
    DepthStencilState depthStencilState;
    RasterizerState rasterizerState;

    GLint scissorX;
    GLint scissorY;
    GLsizei scissorWidth;
    GLsizei scissorHeight;
    
    glm::vec4 blendColor;
    
    GLint viewPortX;
    GLint viewPortY;
    GLsizei viewPortWidth;
    GLsizei viewPortHeight;
    
    GLuint vertexBuffer;
    GLuint indexBuffer;
    GLuint program;
    
    static const int MaxTextureSamplers = 16;
    StaticArray<GLuint, MaxTextureSamplers> samplers2D;
    StaticArray<GLuint, MaxTextureSamplers> samplersCube;
    StaticArray<glVertexAttr, VertexAttr::NumVertexAttrs> glAttrs;
    StaticArray<GLuint, VertexAttr::NumVertexAttrs> glAttrVBs;
};

//------------------------------------------------------------------------------
inline const DisplayAttrs&
glRenderer::renderPassAttrs() const {
    return this->rpAttrs;
}
    
} // namespace _priv
} // namespace Oryol
