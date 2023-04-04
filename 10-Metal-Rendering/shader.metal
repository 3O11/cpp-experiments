#include <metal_stdlib>
using namespace metal;

struct Vertex
{
    float3 position [[addribute(0)]];
    float3 normal [[addribute(1)]];
    float2 texCoord [[addribute(2)]];
};

struct Instance
{
    float4x4 transform;
};

struct Camera
{
    float4x4 view;
    float4x4 projection;
};

struct StageTransfer
{
    float4 position [[position]];
    float4 normal;
    float2 texCoord;
};

StageTransfer vertex vertexMain(
    device const Vertex *vertices [[buffer(0)]],
    device const Instance *instances [[buffer(1)]],
    device const Camera& camera [[buffer(2)]],
    uint vertexId [[vertex_id]],
    uint instanceId [[instance_id]])
{
    StageTransfer result;
    result.position = camera.projection * camera.view * instances[instanceId].transform * float4(vertices[vertexId].position, 1.0f);
    result.normal = float4(vertices[vertexId].normal, 0.0f);
    result.texCoord = vertices[vertexId].texCoord;
    return result;
}

half4 fragment fragmentMain(StageTransfer in [[stage_in]])
{
    float3 colour = abs(in.normal.xyz);
    return half4(half3(colour.rgb), 1.0f);
    //return half4(1.0f, 0.0f, 0.0f, 1.0f);
}
