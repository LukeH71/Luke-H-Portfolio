#include <iostream>

// FILE STILL BEING WORKED ON

// Aligned data type
static const size_t kAlignedVertexSize = (sizeof(Vertex2de));

// General class for handling UI
class UIController {
public:
    

#pragma mark -
#pragma mark Initiator
    
    
    
    //
    UIController(id<MTLDevice> device) : _device(device), _state(0) {
        
        static const GUIVertex UIMesh[] = { { { -1, 0 }, 0 } };
        static const GUIVertexUniforms UIUniforms[] = { { {0.5, 1} } };
        static const GUITextureUniforms UITexUniforms[] = { { {0, 0}, {1, 1} } };
        
        static const uint16_t indices[] = { 0, 1, 2, 1, 3, 2 };
        
        id<MTLBuffer> defaultUIBuffer = [_device newBufferWithBytes:UIMesh
                                                             length:sizeof(UIMesh)
                                                            options:MTLResourceStorageModeShared];
        defaultUIBuffer.label = @"UI Mesh";
        
        id<MTLBuffer> uniformBuffer = [_device newBufferWithBytes:UIUniforms
                                                           length:sizeof(UIUniforms)
                                                          options:MTLResourceStorageModeShared];
        uniformBuffer.label = @"UI Uniforms";
        
        id<MTLBuffer> uniformTexBuffer = [_device newBufferWithBytes:UITexUniforms
                                                           length:sizeof(UITexUniforms)
                                                          options:MTLResourceStorageModeShared];
        uniformTexBuffer.label = @"UI Uniforms";
        
        
        for (uint8_t i = 0; i < MaxBuffersInFlight; ++i) {
            _defaultUIBuffers[i] = defaultUIBuffer;
            _uniformBuffers[i] = uniformBuffer;
            _uniformTexBuffer[i] = uniformTexBuffer;
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
        float defaultVal = 1.33333;
        _aspect = [_device newBufferWithBytes:&defaultVal
                                       length:sizeof(float)
                                      options:MTLResourceStorageModeShared];
        
        
        
        float letterSizes[] = {1};
        textPerameters params = { 90, -0.75, -0.75, 1.5, 1 };
        bezierCurve curve = { 0, 0, 0, 0, 0, 0 };
        
        for (int i = 0; i<3; ++i){
            _letterSizesBuffer[i] = [_device newBufferWithBytes:&letterSizes
                                                         length:sizeof(letterSizes)
                                                        options:MTLResourceStorageModeShared];
            
            _textPerameterBuffer[i] = [_device newBufferWithBytes:&params
                                                           length:sizeof(params)
                                                          options:MTLResourceStorageModeShared];
            
            _bezierCurvesBuffer[i] = [_device newBufferWithBytes:&curve
                                                          length:sizeof(curve)
                                                         options:MTLResourceStorageModeShared];
            
            
            
        }
        
        
        
    }
    
    void setAspect (float newAspect) {
        
        float *buffer = (float *)_aspect.contents;
        
        *buffer = newAspect;
        aspect = newAspect;

    }
        
    
    
#pragma mark -
#pragma mark GUI State inits
    
    
    
    
    
    
    void loadFontDesignerUI (int numDots) {
        
        _numElementsStatic = 1;
        _numElementsButtons = _numElementsStatic + 2;
        _numElementsDraggable = _numElementsButtons + numDots;
        
        
        static const GUIVertex UIMesh[] = {
            { { -1.6, -1 }, 0 },
            { {0.1, -0.5}, 2 },
            { {0.5, -0.5}, 2 }
            //{ { -0.75, -0.75 }, 2 }
        };
        
        // 19 -- nice blue
        
        int UIMeshSize = (sizeof(UIMesh))/sizeof(GUIVertex) ;
        
        static const GUIVertexUniforms UIUniforms[] = {
            { {3.2, 2} },
            { {0.05, 0.05} },
            { {0.25, 0.25} },
        };
        
        static const GUITextureUniforms UITexUniforms[] = {
            { {31/32.f, 27/31.f}, {1/32.f, 1/32.f} },
            { {0, 0}, {31/32.f, 1} },
            { {31/32.f, 19/31.f}, {1/32.f, 1/32.f} },
            { {31/32.f, 16/31.f}, {1/32.f, 1/32.f} },
            { {31/32.f, 15/31.f}, {1/32.f, 1/32.f} }
        };
        
        
        
        for(uint8_t j = 0; j<MaxBuffersInFlight; ++j){
            
            _uniformDetailedBuffers[j] = [_device newBufferWithBytes:UIUniforms
                                                                 length:sizeof(UIUniforms)
                                                                options:MTLResourceStorageModeShared];
            
            _uniformDetailedTexBuffers[j] = [_device newBufferWithBytes:UITexUniforms
                                                                 length:sizeof(UITexUniforms)
                                                                options:MTLResourceStorageModeShared];
            
            _detailedBuffers[j] = [_device newBufferWithLength:(numDots+UIMeshSize)*sizeof(GUIVertex)
                                                       options:MTLResourceStorageModeShared];
            
            
            
            
            GUIVertex *bufferAddress = ((GUIVertex*)_detailedBuffers[j].contents);
            
            for(uint8_t i = 0; i<UIMeshSize; ++i){
                (bufferAddress+i)->position = UIMesh[i].position;
                (bufferAddress+i)->uniformVertexIndex = UIMesh[i].uniformVertexIndex;
                (bufferAddress+i)->uniformTextureIndex = UIMesh[i].uniformTextureIndex;
            }
            
            const float scalar = 0.1;
            for(int i = UIMeshSize; i<(numDots+UIMeshSize); ++i){
                (bufferAddress+i)->position = {i*scalar, 0};
                (bufferAddress+i)->uniformVertexIndex = 1;
                (bufferAddress+i)->uniformTextureIndex = 1;
            }
            
            
            //std::terminate();
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
    
    
    void setDraggableDotNumber (int numNewDots, uint8_t currentBufferIndex) {
        
        int numOldDots = _numElementsDraggable - _numElementsButtons;

        _numElementsDraggable += numNewDots - numOldDots;
        
        if(numNewDots == 0) numNewDots = 3;
        
        //for (int i = 0; i<_detailedBuffers[0].length/sizeof(GUIVertex); ++i){
        GUIVertex *detailedBuffers = (GUIVertex *)_detailedBuffers[currentBufferIndex].contents;
        bezierCurve *curvesBuffers = (bezierCurve *)_bezierCurvesBuffer[currentBufferIndex].contents;
        //}
        
        for(uint8_t j = 0; j<MaxBuffersInFlight; ++j){
            
            _detailedBuffers[j] = [_device newBufferWithBytes:detailedBuffers
                                                       length:(_numElementsDraggable)*sizeof(GUIVertex)
                                                      options:MTLResourceStorageModeShared];
            
            if(numOldDots < numNewDots) {
                
                for(int i = _numElementsButtons+numOldDots; i<(_numElementsButtons+numNewDots); ++i){
                    GUIVertex *buffer = (GUIVertex *)_detailedBuffers[j].contents + i;
                    
                    (buffer)->position = {0.8, 0};
                    (buffer)->uniformVertexIndex = 1;
                    (buffer)->uniformTextureIndex = 1;
                }
                
            }
            
            int originalCurves = static_cast<int>(_bezierCurvesBuffer[j].length) / sizeof(bezierCurve);
            
            _bezierCurvesBuffer[j] = [_device newBufferWithBytes:curvesBuffers
                                                          length:numNewDots/3*sizeof(bezierCurve)
                                                         options:MTLResourceStorageModeShared];
            
            if (originalCurves < numNewDots/3 || _numElementsDraggable == _numElementsButtons) {
                
                bezierCurve *buffer = (bezierCurve *)_bezierCurvesBuffer[j].contents;
                
                for(int i = originalCurves - (_numElementsDraggable == _numElementsButtons); i<numNewDots/3; ++i){
                    
                    for (int k = 0; k<6; ++k)
                        (buffer+i)->coords[k] = 0;
            
                }
            }
            
            textPerameters *buffer = (textPerameters *)_textPerameterBuffer[j].contents;
            
            buffer->numCurves = numNewDots/3;
        }
        
        
        
    }
    
    
    
 
#pragma mark -
#pragma mark UI Element Logic
    
    bool insideBounds(GUIVertex* tri, uint8_t currentBufferIndex, CGPoint mousePos){
        
        vector_float2 pos = tri->position;
        vector_float2 size = (((GUIVertexUniforms*)_uniformDetailedBuffers[currentBufferIndex].contents)+tri->uniformVertexIndex)->sizes;
        
        return (mousePos.x>pos.x&&mousePos.x<pos.x+size.x && mousePos.y>pos.y&&mousePos.y<pos.y+size.y);
    }
    
    
    void checkIfInBounds(uint8_t currentBufferIndex, CGPoint mousePos) {
        
        if(_selectedElement == 255) {
            
            for (int i = _numElementsStatic; i< _numElementsDraggable; ++i) {
                
                GUIVertex* tri = ((GUIVertex*)_detailedBuffers[currentBufferIndex].contents)+i;
        
                if (insideBounds(tri, currentBufferIndex, mousePos)){
                    _selectedElement = i;
                    return;
                }
            }
            
            _selectedElement = 254;
        }
    }
    
    
#pragma mark draggable logic
    
    // A function called when the mouse is clicked
    void moveDot(GUIVertex* tri, vector_float2 size, CGPoint mousePos){
        
        if (_selectedElement >= _numElementsButtons && _selectedElement < _numElementsDraggable) {
            
            tri->position = {static_cast<float>(mousePos.x)-size.x/2, static_cast<float>(mousePos.y)-size.y/2};
            
            return;
        }
    }
    
    // A function called when the mouse is unclicked
    void deselectDot(uint8_t currentBufferIndex){
        
        if (_selectedElement >= _numElementsButtons && _selectedElement < _numElementsDraggable) {
            
            GUIVertex* pastDot = (GUIVertex*)_detailedBuffers[(currentBufferIndex + 2) % 3].contents;
            
            for (int k = 0; k < 3; ++k) {
                
                GUIVertex* presentDot = (GUIVertex*)_detailedBuffers[k].contents;
                
                (presentDot+_selectedElement)->position = (pastDot+_selectedElement)->position;
            }
        }
    }
    
#pragma mark button logic
    
    // A function called when the mouse is clicked
    void clickButton(GUIVertex* tri, uint8_t currentBufferIndex, CGPoint mousePos){
        if (_selectedElement >= _numElementsStatic && _selectedElement < _numElementsButtons) {
            if(insideBounds(tri, currentBufferIndex, mousePos)) {
                tri->uniformTextureIndex = 3;
            } else {
                tri->uniformTextureIndex = 2;
            }
        }
    }
    
    // A function called when the mouse is unclicked
    uint8_t deselectButton(uint8_t currentBufferIndex, CGPoint mousePos){
        
        mousePos.x /= ((aspect < 1.6f) ? 1/1.6 : (1/aspect));
        mousePos.y /= ((aspect > 1.6f) ? 1 : aspect/1.6);
        
        if (_selectedElement >= _numElementsStatic && _selectedElement < _numElementsButtons) {

            GUIVertex* tri = ((GUIVertex*)_detailedBuffers[currentBufferIndex].contents)+_selectedElement;
            
            if (insideBounds(tri, currentBufferIndex, mousePos)){
                
                for (int k = 0; k < 3; ++k) {
                    GUIVertex* presentDot = (GUIVertex*)_detailedBuffers[k].contents;
                    (presentDot+_selectedElement)->uniformTextureIndex = 4;
                }
                
                return _selectedElement - _numElementsStatic;
            }
            
            for (int k = 0; k < 3; ++k) {
                GUIVertex* presentDot = (GUIVertex*)_detailedBuffers[k].contents;
                (presentDot+_selectedElement)->uniformTextureIndex = 2;
            }
            
        }
        
        for (int i = _numElementsStatic; i<_numElementsButtons; ++i) {
            
            GUIVertex* tri = ((GUIVertex*)_detailedBuffers[currentBufferIndex].contents)+i;
            
            if (insideBounds(tri, currentBufferIndex, mousePos)){
                tri->uniformTextureIndex = 4;
            } else {
                tri->uniformTextureIndex = 2;
            }
        }
        
        return 255;
    }
    
    
#pragma mark logic function
    
    // A general function that can be called to handle all GUI related logic
    void GUILogic(KeyboardMTKView *mtkView, uint8_t currentBufferIndex){
        
        // Handle switching between states
        if([mtkView getLastKey] == 9) {
            if(_state == 1){
                _state = 0;
            } else {
                _state = 1;
                loadFontDesignerUI(0);
            }
            [mtkView resetLastKey];
        }
        
        if (_state == 1) {
            
            if([mtkView getKey:6] && _selectedElement != 254) {
                
                
                CGPoint mousePos = [mtkView getMousePos];
                
                mousePos.x /= ((aspect < 1.6f) ? 1/1.6 : (1/aspect));
                mousePos.y /= ((aspect > 1.6f) ? 1 : aspect/1.6);
                
                checkIfInBounds(currentBufferIndex, mousePos);
                
                GUIVertex* tri = ((GUIVertex*)_detailedBuffers[currentBufferIndex].contents)+_selectedElement;
                
                vector_float2 size = (((GUIVertexUniforms*)_uniformDetailedBuffers[currentBufferIndex].contents)+tri->uniformVertexIndex)->sizes;
                
                clickButton(tri, currentBufferIndex, mousePos);
                
                // setting logic
                moveDot(tri, size, mousePos);
                
                if (_selectedElement >= _numElementsButtons && _selectedElement < _numElementsDraggable && _selectedElement) {
                    
                    bezierCurve *curve = (bezierCurve *) _bezierCurvesBuffer[currentBufferIndex].contents + ((_selectedElement - _numElementsButtons) / 3);
                    
                    curve->coords[((_selectedElement - _numElementsButtons) % 3) * 2] = (tri->position.x+0.75)/1.5;
                    
                    curve->coords[((_selectedElement - _numElementsButtons) % 3) * 2 + 1] = (tri->position.y+0.75)/1.5;
                    
                }
                
            } else if (![mtkView getKey:6]) {
                // unsetting logic
                
                
                switch(deselectButton(currentBufferIndex, [mtkView getMousePos])){
                    case 255:
                        break;
                    case 0:
                        std::cout << "Button 0!\n";
                        
                        setDraggableDotNumber(_numElementsDraggable-_numElementsButtons+3, currentBufferIndex);
                        
                        break;
                    case 1:
                        std::cout << "Button 1!\n";

                        if(_numElementsDraggable-_numElementsButtons != 0){
                            setDraggableDotNumber(_numElementsDraggable-_numElementsButtons-3, currentBufferIndex);
                        }
                        
                        break;
                }
            
                if (_selectedElement != 255) {
                    
                    deselectDot(currentBufferIndex); // snapping to a grid logic
                    
                    GUIVertex* tri = ((GUIVertex*)_detailedBuffers[currentBufferIndex].contents)+_selectedElement;
                    
                    if (_selectedElement >= _numElementsButtons && _selectedElement < _numElementsDraggable) {
                        
                        
                        float x = (tri->position.x+0.75)/1.5;
                        float y = (tri->position.y+0.75)/1.5;
                        
                        for (int p = 0; p<3; ++p){
                            
                            bezierCurve *curve = (bezierCurve *) _bezierCurvesBuffer[p].contents + ((_selectedElement - _numElementsButtons) / 3);
                            
                            curve->coords[(_selectedElement - _numElementsButtons) % 3 * 2] = x;
                            
                            curve->coords[(_selectedElement - _numElementsButtons) % 3 * 2 + 1] = y;
                        }
                        
                    }
                    
                    _selectedElement = 255;
                }
            }
        }
    }
    
    

    
#pragma mark -
#pragma mark Drawing functions
    
    
    
    
    
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
        
        [renderEncoder setVertexBuffer:_uniformTexBuffer[currentBufferIndex]
                                offset:0
                               atIndex:3];
        
        [renderEncoder setFragmentTexture:_defaultUITextureMap atIndex:0];
        
        [renderEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                  indexCount:6
                                   indexType:MTLIndexTypeUInt16
                                 indexBuffer:_squareIndexBuffer
                           indexBufferOffset:0
                               instanceCount:_defaultUIBuffers[0].length/sizeof(GUIVertex)];
        
        [renderEncoder popDebugGroup];
    }

    // Draws all GUI related
    void drawFontRenderer (id<MTLRenderCommandEncoder> renderEncoder, uint8_t currentBufferIndex, bool backFront)
    {
        [renderEncoder pushDebugGroup:@"Font Designer UI"];
        
        [renderEncoder setVertexBuffer:_detailedBuffers[currentBufferIndex]
                                offset:backFront*sizeof(GUIVertex)
                               atIndex:0];
        
        [renderEncoder setVertexBuffer:_uniformDetailedBuffers[currentBufferIndex]
                                offset:0
                               atIndex:1];
        
        [renderEncoder setVertexBuffer:_uniformDetailedTexBuffers[currentBufferIndex]
                                offset:0
                               atIndex:3];
        
        [renderEncoder setFragmentTexture:_screenTextureMap atIndex:0];
        
        // Draw multiple instances of the same square
        [renderEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                  indexCount:6
                                   indexType:MTLIndexTypeUInt16
                                 indexBuffer:_squareIndexBuffer
                           indexBufferOffset:0
                               instanceCount:_detailedBuffers[0].length/sizeof(GUIVertex)-backFront];
        
        [renderEncoder popDebugGroup];
    }

    
    void drawText (id<MTLRenderCommandEncoder> renderEncoder, uint8_t currentBufferIndex)
    {
        
        //std::cout << static_cast<float>(((presentDot+2)->position.y+0.75)/1.5) << "\n";
        
        [renderEncoder setVertexBuffer:_letterSizesBuffer[currentBufferIndex]
                                offset:0
                               atIndex:0];
        
        [renderEncoder setVertexBuffer:_textPerameterBuffer[currentBufferIndex]
                                offset:0
                               atIndex:1];
        
        [renderEncoder setFragmentBuffer:_bezierCurvesBuffer[currentBufferIndex]
                                  offset:0
                                 atIndex:0];
        
        
        // Draw multiple instances of the same square
        [renderEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                  indexCount:6
                                   indexType:MTLIndexTypeUInt16
                                 indexBuffer:_squareIndexBuffer
                           indexBufferOffset:0
                               instanceCount:1];
        
    }
    
    
    // A function used to draw the UI from one call
    void drawUI (id<MTLRenderCommandEncoder> renderEncoder, uint8_t currentBufferIndex,id<MTLRenderPipelineState> _textPipelineState,id<MTLRenderPipelineState> _GUIPipelineState)
    {
        
        [renderEncoder setVertexBuffer:_aspect
                                offset:0
                               atIndex:2];
        
        // Draw the UI that is always present (like health bar)
        drawDefaultUI(renderEncoder, currentBufferIndex);
        
        // Check for exclusive GUI states (like inventory)
        if (_state == 1) { // DEBUG - font designer
            drawFontRenderer(renderEncoder, currentBufferIndex, false);
            
            [renderEncoder setRenderPipelineState:_textPipelineState];
            
            drawText (renderEncoder, currentBufferIndex);
            
            [renderEncoder setRenderPipelineState:_GUIPipelineState];
            
            drawFontRenderer(renderEncoder, currentBufferIndex, true);
            
        }
    }
    
    
    
private:
    
    // Variables for all UI states
    uint8_t _state;
    id<MTLDevice> _device;
    id <MTLBuffer> _aspect;
    float aspect;
    
    // The UI that is constantly on the screen (i.e. MC hotbar)
    id <MTLBuffer> _defaultUIBuffers[MaxBuffersInFlight];
    id <MTLBuffer> _uniformBuffers[MaxBuffersInFlight];
    id <MTLBuffer> _uniformTexBuffer[MaxBuffersInFlight];
    id <MTLTexture> _defaultUITextureMap;
    
    // Re-initiate this buffer whenever it is needed to be larger
    // Otherwise, keep it the same and reuse it for each different
    // UI states
    id <MTLBuffer> _detailedBuffers[MaxBuffersInFlight];
    id <MTLBuffer> _uniformDetailedBuffers[MaxBuffersInFlight];
    id <MTLBuffer> _uniformDetailedTexBuffers[MaxBuffersInFlight];
    id <MTLTexture> _screenTextureMap;
    
    // The values below are used to describe how to add logic to a U.I. screen
    uint8_t _selectedElement;
    uint8_t _numElementsStatic;
    uint8_t _numElementsButtons;
    uint8_t _numElementsDraggable;
    
    id <MTLBuffer> _letterSizesBuffer[MaxBuffersInFlight];
    id <MTLBuffer> _textPerameterBuffer[MaxBuffersInFlight];
    id <MTLBuffer> _bezierCurvesBuffer[MaxBuffersInFlight];
    
    
    
    // Index wrapping buffers for more efficient rendering in inctancing
    id <MTLBuffer> _squareIndexBuffer;
    
};

// int numBackgroundUnnefected things = static: no input
// int draggable = draggable things
// int buttons
