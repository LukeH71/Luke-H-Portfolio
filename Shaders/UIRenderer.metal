//
//  UIRenderer.metal
//  1st game
//
//  Created by Luke Hickey on 9/14/24.
//

#include <metal_stdlib>
using namespace metal;

#import "MainShaderTypes.h"

typedef struct
{
    float4 position   [[position]];
    float2 texCoord;
} ColorInOut;

typedef struct
{
    float2 position;
    float2 texCoord;
} Vertex2d;


vertex ColorInOut flatVertexShader
(
    const device GUIVertex *vertices [[buffer(0)]],
    const device GUIVertexUniforms *uniforms [[buffer(1)]],
    uint vertexID [[vertex_id]],
    uint instanceID [[instance_id]]
)
{
    // Declare commenly used variables to optimise memory access
    GUIVertex currentVertex = vertices[instanceID];
    GUIVertexUniforms uniform = uniforms[currentVertex.uniformIndex];
    
    // The positions of each vertex with a clockwise square:
    //  -1,  1
    //   1,  1
    //  -1, -1
    //   1, -1
    
    // vertexID & 1      1 if the vertexID is odd, which sets x to the larger x value
    // vertexID >> 1     1 if the vertexID is more than 1, which sets the y to the larger y value
    
    // Return the final variable with calculated positions and texture coords
    return (ColorInOut) {
        float4(currentVertex.position + float2((vertexID & 1) * uniform.sizes.x, (vertexID >> 1) * uniform.sizes.y), 1.0, 1.0), // Vertex location
        uniform.texCoords + float2((vertexID & 1) * uniform.texSizes.x, (vertexID >> 1) * uniform.texSizes.y) // Texture coordinate location
    };
}


fragment half4 colorMapFragmentShader
(
    ColorInOut      in                 [[stage_in]],
    texture2d<half> colorMap           [[texture(0)]],
    half4           forwardOpaqueColor [[color(0), raster_order_group(0)]]
 )
{
    constexpr sampler colorSampler(mip_filter::none,
                                   mag_filter::nearest,
                                   min_filter::nearest);

    half4 colorSample = colorMap.sample(colorSampler, in.texCoord.xy);
    colorSample.xyz = colorSample.xyz + (1.0h - colorSample.w) * forwardOpaqueColor.xyz;

    return colorSample;
}
