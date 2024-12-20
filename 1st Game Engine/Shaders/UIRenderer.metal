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
    const device GUIVertexUniforms *vUniforms [[buffer(1)]],
    const device GUITextureUniforms *tUniforms [[buffer(3)]],
    const device float *aspectRatio [[buffer(2)]],
    uint vertexID [[vertex_id]],
    uint instanceID [[instance_id]]
)
{
    // Declare commenly used variables to optimise memory access
    GUIVertex currentVertex = vertices[instanceID];
    float2 vertexUniform = vUniforms[currentVertex.uniformVertexIndex].sizes;
    GUITextureUniforms textureUniform = tUniforms[currentVertex.uniformTextureIndex];
    float aspect = (*aspectRatio);
    
    // The positions of each vertex with a clockwise square:
    //  -1,  1
    //   1,  1
    //  -1, -1
    //   1, -1
    
    // vertexID & 1      1 if the vertexID is odd, which sets x to the larger x value
    // vertexID >> 1     1 if the vertexID is more than 1, which sets the y to the larger y value
    
    // Location of the individual vertex based off of winding
    float4 pos = float4(currentVertex.position + float2((vertexID & 1) * vertexUniform.x, (vertexID >> 1) * vertexUniform.y), 1.0, 1.0);
    
    // Adjust the positions based off of aspect ratio (1.6 is the screen aspect ratio)
    pos.x *= ((aspect < 1.6f) ? 1/1.6 : (1/aspect));
    pos.y *= ((aspect > 1.6f) ? 1 : aspect/1.6);
    
    // Return the final variable with calculated positions and texture coords
    return (ColorInOut) {
        pos, // Vertex location
        textureUniform.texCoords + float2((vertexID & 1) * textureUniform.texSizes.x, (vertexID >> 1) * textureUniform.texSizes.y) // Texture coordinate location
    };
}


fragment half4 colorMapFragmentShader
(
    ColorInOut      in                 [[stage_in]],
    texture2d<half> colorMap           [[texture(0)]],
    half4           forwardOpaqueColor [[color(0), raster_order_group(0)]]
 )
{
    constexpr sampler colorSampler(mip_filter::none);

    half4 colorSample = colorMap.sample(colorSampler, in.texCoord.xy);
    colorSample.xyz = colorSample.xyz + (1.0h - colorSample.w) * forwardOpaqueColor.xyz;

    return colorSample;
}


typedef struct
{
    float4 position [[position]];
    float2 texCoord;
    int numCurves;
} InOut;


vertex InOut textVertex
(
    const device float *fontSizes [[buffer(0)]],
    const device textPerameters *params [[buffer(1)]],
    const device float *aspectRatio [[buffer(2)]],
    uint vertexID [[vertex_id]],
    uint instanceID [[instance_id]]
)
{
    float2 coords = float2(0, 0);
    
    
    // Line wrapping
    for (uint i = 0; i<instanceID; ++i){
        coords.x += params->fontSize * fontSizes[i];
        
        float temp = coords.x + fontSizes[instanceID] * params->fontSize;
        
        coords.y -= int(temp / params->lineWrappingLength) * params->fontSize * 1.2;
        coords.x -= (temp >= params->lineWrappingLength) ? coords.x : params->fontSize * -0.2;
    }
    
    // Origin
    coords += float2(params->originX, params->originY);
    
    
    
    // Aspect ratios
    float aspect = (*aspectRatio);
    
    // Location of the individual vertex based off of winding
    float4 pos = { float4(coords + float2((vertexID & 1) * fontSizes[instanceID] * params->fontSize, (vertexID >> 1) * params->fontSize), 1.0, 1.0)} ;
    
    // Adjust the positions based off of aspect ratio (1.6 is the screen aspect ratio)
    pos.x *= ((aspect < 1.6f) ? 1/1.6 : (1/aspect));
    pos.y *= ((aspect > 1.6f) ? 1 : aspect/1.6);
    
    // Return the final variable with calculated positions
    return (InOut) {
        pos, // Vertex location
        float2((vertexID & 1), (vertexID >> 1)), // Texture coordinate location
        params->numCurves
    };
}


fragment half4 colorTextFragment
(
    InOut                     in      [[stage_in]],
    const device bezierCurve *curves [[buffer(0)]]
 )
{
    
    uint8_t numCurves = 0;
    
    half4 inside = half4(0.1h, 0.9h, 0.2, 1.h);
    half4 outside = half4(0.9h, 0.1h, 0.2, 1.h);
    
    for (int i = 0; i<in.numCurves; ++i) {
        
        bezierCurve curve = curves[i];
        
        // Compute quadratic coefficients for y(t)
        float a = (curve.coords[5] - 2.0 * curve.coords[3] + curve.coords[1]) * 2.0;
        float b = -2.0 * (curve.coords[3] - curve.coords[1]);
        
        // Solve for t using the quadratic formula
        float discriminant = b * b - 2.0 * a * (curve.coords[1] - in.texCoord.y);
        
        // Ensure discriminant is non-negative (clamp to avoid NaNs)
        if (discriminant < 0.0) continue; // Invalid case (no intersection)
        discriminant = sqrt(discriminant);
        
        // Two potential solutions for t
        float t1 = (b + discriminant) / a;
        float t2 = (b - discriminant) / a;
        
        // Pre-calculate two potential 1 - t constants
        float one_minus_t1 = 1.0 - t1;
        float one_minus_t2 = 1.0 - t2;
            
        // Solve for x(t1) & add a curve if the solution is in the positive X direction
        numCurves += (one_minus_t1 * one_minus_t1 * curve.coords[0] + 2.0 * one_minus_t1 * t1 * curve.coords[2] + t1 * t1 * curve.coords[4] <= in.texCoord.x) ? 0.0 : t1 > 0.0 && t1 < 1.0 && !isnan(t1);
        
        // Solve for x(t2) & add a curve if the solution is in the positive X direction
        numCurves += (one_minus_t2 * one_minus_t2 * curve.coords[0] + 2.0 * one_minus_t2 * t2 * curve.coords[2] + t2 * t2 * curve.coords[4] <= in.texCoord.x) ? 0.0 : t2>0.0 && t2<1.0 && !isnan(t2);
        
    }

    return (numCurves % 2 == 0) ? outside : inside;
}
