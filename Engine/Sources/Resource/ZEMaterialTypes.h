#pragma once

#include <vector>

namespace ZE
{

enum class EStencilOperation
{
    Keep,
    Zero,
    Replace,
    IncreaseSaturate,
    DecreaseSaturate,
    Invert,
    IncreaseWrap,
    DecreaseWrap,
};

enum class ECompareOperation : uint8_t
{
    Never,
    Less,
    Equal,
    LEqual,
    Greater,
    NotEqual,
    GEqual,
    Always
};

struct StencilOperationState
{
    StencilOperationState() :
        passOperation(EStencilOperation::Keep), failOperation(EStencilOperation::Keep), depthFailOperation(EStencilOperation::Keep), compareFunction(ECompareOperation::Never), readMask(0), writeMask(0), ref(0)
    {
    }

    EStencilOperation passOperation, failOperation, depthFailOperation;
    ECompareOperation compareFunction;
    uint32_t readMask, writeMask, ref;
};

enum class EZWriteType : uint8_t
{
    Disable,
    Enable,
};

struct DepthStencilState
{
    DepthStencilState() :
        zTestType(ECompareOperation::Never), zWriteType(EZWriteType::Disable)
    {
    }

    StencilOperationState front, back;
    EZWriteType zWriteType;
    ECompareOperation zTestType;
};

enum class EBlendOperation : uint8_t
{
    Add,
    Sub,
};

enum class EBlendFactor : uint8_t
{
    Zero,
    One,
    SrcColor,
    SrcAlpha,
    DstColor,
    DstAlpha,
    OneMinusSrcColor,
    OneMinusSrcAlpha,
    OneMinusDstColor,
    OneMinusDstAlpha,
};

struct BlendState
{
    BlendState() : 
        srcFactor(EBlendFactor::One), dstFactor(EBlendFactor::One), srcAlphaFactor(EBlendFactor::One), dstAlphaFactor(EBlendFactor::Zero), operation(EBlendOperation::Add)
    {
    }

    EBlendFactor srcFactor, dstFactor, srcAlphaFactor, dstAlphaFactor;
    EBlendOperation operation;
};

enum class ECullingType : uint8_t
{
    Font,
    Back,
    None,
};

struct RenderState
{
    RenderState() :
        cullingType(ECullingType::Back)
    {
    }

    ECullingType cullingType;

    std::vector<BlendState> blendState;
    DepthStencilState depthStencilState;
};

}