#include <iostream>

// FILE STILL BEING WORKED ON

// Aligned data type
static const size_t kAlignedVertexSize = (sizeof(Vertex2de));

// General class for handling UI
class UIController {
public:
    
    //
    UIController(id<MTLDevice> device) : _device(device), _state(0) {
        
        static const GUIVertex UIMesh[] = { { { -1, 0 }, 0 } };
        static const GUIVertexUniforms UIUniforms[] = { { {0.5, 1}, {0, 0}, {1, 1} } };
        static const uint16_t indices[] = { 0, 1, 2, 1, 3, 2 };
        
        id<MTLBuffer> defaultUIBuffer = [_device newBufferWithBytes:UIMesh
                                                             length:sizeof(UIMesh)
                                                            options:MTLResourceStorageModeShared];
        defaultUIBuffer.label = @"UI Mesh";
        
        id<MTLBuffer> uniformBuffer = [_device newBufferWithBytes:UIUniforms
                                                           length:sizeof(UIUniforms)
                                                          options:MTLResourceStorageModeShared];
        uniformBuffer.label = @"UI Uniforms";
        
        
        for (uint8_t i = 0; i < MaxBuffersInFlight; ++i) {
            _defaultUIBuffers[i] = defaultUIBuffer;
            _uniformBuffers[i] = uniformBuffer;
        }

        _squareIndexBuffer = [_device newBufferWithBytes:indices
                                                  length:sizeof(indices)
                                                 options:MTLResourceStorageModeShared];
        
        MTKTextureLoader *textureLoader = [[MTKTextureLoader alloc] initWithDevice:_device];
        NSDictionary *textureLoaderOptions = @{
            MTKTextureLoaderOptionTextureUsage       : @(MTLTextureUsageShaderRead),
            MTKTextureLoaderOptionTextureStorageMode : @(MTLStorageModePrivate)
        };
        
        _defaultUITextureMap = [textureLoader newTextureWithName:@"DebugPanel"
                                                     scaleFactor:1.0
                                                          bundle:nil
                                                         options:textureLoaderOptions
                                                           error:nil];
    }
        
    
    void loadFontDesignerUI (int numDots) {
        
//        static const GUIVertex UIMesh[] = {
//            { { -1, -1 }, {31/32.f, 29/31.f} }, // Square
//            { { 1,  1 }, {1, 29/31.f}},
//            { {  -1,  -1 }, {31/32.f, 28/31.f}},
//            { {  1,  -1 }, {1, 28/31.f}}
//        };
        
        static const std::vector<GUIVertex> UIMesh = {
            { { -1, -1 }, 0 }
        };
        
        static const GUIVertexUniforms UIUniforms[] = {
            { {2, 2}, {31/32.f, 27/31.f}, {1/32.f, 1/32.f}},
            { {0.05, 0.05}, {0, 0}, {31/32.f, 1} }
        };
        /*
         typedef struct
         {
             vector_float2 position; // -x, -y
             uint8_t uniformIndex;
         } GUIVertex; // 10

         typedef struct
         {
             vector_float2 sizes;
             vector_float2 texCoords;
             vector_float2 texSizes;
         } GUIVertexUniforms; // 24
         */
        
        
        
        for(uint8_t j = 0; j<MaxBuffersInFlight; ++j){
            
            _uniformDetailedBuffers[j] = [_device newBufferWithBytes:UIUniforms
                                                                 length:sizeof(UIUniforms)
                                                                options:MTLResourceStorageModeShared];
            
            _detailedBuffers[j] = [_device newBufferWithLength:(numDots+UIMesh.size())*sizeof(GUIVertex)
                                                       options:MTLResourceStorageModeShared];
            
            GUIVertex *bufferAddress = ((GUIVertex*)_detailedBuffers[j].contents);
            
            for(uint8_t i = 0; i<UIMesh.size(); ++i){
                (bufferAddress+i)->position = UIMesh[i].position;
                (bufferAddress+i)->uniformIndex = UIMesh[i].uniformIndex;
            }
            
            const float scalar = 0.1;
            for(int i = static_cast<int>(UIMesh.size()); i<(numDots+UIMesh.size()); ++i){
                (bufferAddress+(i))->position = {i*scalar, 0};
                (bufferAddress+(i))->uniformIndex = 1;
            }
        }
        
        //NSError *error;
        
        MTKTextureLoader* textureLoader = [[MTKTextureLoader alloc] initWithDevice:_device];

        NSDictionary *textureLoaderOptions =
        @{
          MTKTextureLoaderOptionTextureUsage       : @(MTLTextureUsageShaderRead),
          MTKTextureLoaderOptionTextureStorageMode : @(MTLStorageModePrivate)
          };

        _screenTextureMap = [textureLoader newTextureWithName:@"DebugRenderer"
                                          scaleFactor:1.0
                                               bundle:nil
                                              options:textureLoaderOptions
                                                error:nullptr];
        
        
        
    }
    
    bool moveDot(GUIVertex* tri, CGPoint mousePos){
        
        vector_float2 pos = tri->position;
        vector_float2 size = (((GUIVertexUniforms*)_uniformDetailedBuffers[tri->uniformIndex].contents)+tri->uniformIndex)->sizes;
        
        if (mousePos.x>pos.x&&mousePos.x<pos.x+size.x){
            if (mousePos.y>pos.y&&mousePos.y<pos.y+size.y){
                
                tri->position = {static_cast<float>(mousePos.x)-size.x/2, static_cast<float>(mousePos.y)-size.y/2};
                
                return true;
            }
        }
        
        return false;
    }
    
    
    
    void forceMoveDot(GUIVertex* tri, CGPoint mousePos){

        vector_float2 size = (((GUIVertexUniforms*)_uniformDetailedBuffers[tri->uniformIndex].contents)+tri->uniformIndex)->sizes;
        tri->position = {static_cast<float>(mousePos.x)-size.x/2, static_cast<float>(mousePos.y)-size.y/2};

    }
    
    void recalculatePositions (float aspect, uint8_t currentBufferIndex) {
        
        float plpl = 1/_aspect;
        
        Vertex2de *bufferAddress = ((Vertex2de*)_detailedBuffers[currentBufferIndex].contents);
        for (int i = 0; i < static_cast<int>(_detailedBuffers[0].length) / kAlignedVertexSize; ++i) {
            bufferAddress[i].position.y *= plpl;
            bufferAddress[i].position.y *= aspect;
        }
        
        _aspect = aspect;
    }
    
    void movingDotLogic(KeyboardMTKView *mtkView, uint8_t currentBufferIndex)
    {

        
        
        
        if(_state == 1){

            if([mtkView getKey:6]){
                GUIVertex* presentDot = (GUIVertex*)_detailedBuffers[currentBufferIndex].contents;
                
                if (otherUIInt == 0) {
                    // Iterate through buffer only if no dot is selected
                    for (int i = 1; i < _detailedBuffers[0].length / kAlignedVertexSize; ++i) {
                        if (moveDot(presentDot + i, [mtkView getMousePos])) {
                            otherUIInt = i;
                            break;
                        }
                    }
                } else {
                    // Directly move the selected dot if it's already selected
                    forceMoveDot(presentDot + otherUIInt, [mtkView getMousePos]);
                }
                
            } else {
                // Perform update only if a dot was previously selected
                GUIVertex* pastDot = (GUIVertex*)_detailedBuffers[(currentBufferIndex + 2) % 3].contents;
                
                for (int k = 0; k < 3; ++k) {
                    GUIVertex* presentDot = (GUIVertex*)_detailedBuffers[k].contents;
                    (presentDot+otherUIInt)->position = (pastDot+otherUIInt)->position; // Copy 4 vertices
                }
                
                
                otherUIInt = 0;
            }
        }
    }
    
    // A general function that can be called to handle all GUI related logic
    void GUILogic(KeyboardMTKView *mtkView, uint8_t currentBufferIndex){
        
        // Handle switching between states
        if([mtkView getLastKey] == 9) {
            if(_state == 1){
                _state = 0;
            } else {
                _state = 1;
                loadFontDesignerUI(4);
            }
            [mtkView resetLastKey];
        }
        
        if (_state == 1) movingDotLogic(mtkView, currentBufferIndex);
    }
    
    
    
    // Pass in aspect:
    // Pass x,y per
    // Pass in texture coords per
    // Pass in thicknesses per
    
    // Drawing:
    
    // Draws all the transparent meshes from the transparent actors array.
    void drawDefaultUI (id<MTLRenderCommandEncoder> renderEncoder, uint8_t currentBufferIndex)
    {
        
        [renderEncoder pushDebugGroup:@"UI Rendering"];
        
        [renderEncoder setVertexBuffer:_defaultUIBuffers[currentBufferIndex]
                                offset:0
                               atIndex:0];
        
        [renderEncoder setVertexBuffer:_uniformBuffers[currentBufferIndex]
                                offset:0
                               atIndex:1];
        
        [renderEncoder setFragmentTexture:_defaultUITextureMap atIndex:0];
        
        [renderEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                  indexCount:6
                                   indexType:MTLIndexTypeUInt16
                                 indexBuffer:_squareIndexBuffer
                           indexBufferOffset:0];
        
        [renderEncoder popDebugGroup];
    }

    // Draws all GUI related
    void drawFontRenderer (id<MTLRenderCommandEncoder> renderEncoder, uint8_t currentBufferIndex)
    {
        [renderEncoder pushDebugGroup:@"Font Designer UI"];
        
        [renderEncoder setVertexBuffer:_detailedBuffers[currentBufferIndex]
                                offset:0
                               atIndex:0];
        
        [renderEncoder setVertexBuffer:_uniformDetailedBuffers[currentBufferIndex]
                                offset:0
                               atIndex:1];
        
        [renderEncoder setFragmentTexture:_screenTextureMap atIndex:0];
        
        // Draw multiple instances of the same square
        [renderEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                  indexCount:6
                                   indexType:MTLIndexTypeUInt16
                                 indexBuffer:_squareIndexBuffer
                           indexBufferOffset:0
                               instanceCount:5];
        
        [renderEncoder popDebugGroup];
    }
    
    // A function used to draw the UI from one call
    void drawUI (id<MTLRenderCommandEncoder> renderEncoder, uint8_t currentBufferIndex)
    {
        // Draw the UI that is always present (like health bar)
        drawDefaultUI(renderEncoder, currentBufferIndex);
        
        // Check for exclusive GUI states (like inventory)
        if (_state == 1) // DEBUG - font designer
            drawFontRenderer(renderEncoder, currentBufferIndex);
    }
    
    
    
private:
    
    // Variables for all UI states
    uint8_t _state;
    id<MTLDevice> _device;
    float _aspect;
    
    // The UI that is constantly on the screen (i.e. MC hotbar)
    id <MTLBuffer> _defaultUIBuffers[MaxBuffersInFlight];
    id <MTLBuffer> _uniformBuffers[MaxBuffersInFlight];
    id <MTLTexture> _defaultUITextureMap;
    
    // Re-initiate this buffer whenever it is needed to be larger
    // Otherwise, keep it the same and reuse it for each different
    // UI states
    NSUInteger currentBufferSize;
    id <MTLBuffer> _detailedBuffers[MaxBuffersInFlight];
    id <MTLBuffer> _uniformDetailedBuffers[MaxBuffersInFlight];
    id <MTLTexture> _screenTextureMap;
    int otherUIInt; // A random int value that can be used for misc purposes in the UI
    
    id <MTLBuffer> _squareIndexBuffer;
    
};

