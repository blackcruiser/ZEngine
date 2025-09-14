1. std::shared_ptr is too long to type, consider using Macro.

2. 延迟BeginRenderPass直到开始Draw。

3. 尝试将RenderPass和Renderer像Unity一样规范化。但是目前RenderPass和VkRenderPass绑定，影响了RenderTarget的设置。因此采用Unity的架构，RenderGraph用来存储每一帧的资源。可以存储当前的VkRenderPass，方便后续设置Framebuffer等。
并且对Render和Pass隐藏底层的细节。

4. CommandBuffer submit时进行调用End，同时获取新的CommandBuffer。一旦获取到，即调用Begin。
获取新CommandBuffer时，可以重置已提交的CommandBuffer状态。

5. AcquireNextImage需要fence同步。保证semaphore被触发后再调用。否则会产生"If semaphore is not VK_NULL_HANDLE, it must not have any uncompleted signal or wait operations pending"错误。

6. Vulkan对象的生命周期还是需要手动控制，所以不再使用shared_ptr。

7. CommandBuffer需要fence同步，但是fence想使用manager管理，只能在CommandBuffer创建后再传入fence。Unity使用FrameTracking管理fence和semaphore，unreal则是直接使用。
目前什么都没渲染，执行速度非常快，导致AcquireNextImage的signalSemaphore还没有被QueueSumbit wait，就被下一次的AcquireNextImage signal了。出现了"Semaphore must not have any pending operations."错误。
Unity使用FrameTracking管理semaphore，只有CPU端fence signal后，semaphore才能重新使用。因此Unity并没有直接wait fence。
UE不同，UE每一帧会等QueueSubmit fence wait后再继续执行。

8. Unity中TaskExecutor与UE中FRHICommandList相似。

9. Unity的渲染数据都在GameThread管理，Mesh与Material等。当需要渲染时，将这些数据组织成indexBuffer、vertexbuffer等再通过m_CommandQueue传递至RenderThread。

10. 原本想分离GameThread数据和RenderThread数据。例如MeshResource(GameThread)创建Mesh(RenderThread),Mesh在RenderThread调用InitGraphic初始化数据。MeshComponent(GameThread)创建MeshProxy(RenderThread)。但有个问题是，Mesh初始化GpuVertexBuffer需要顶点数据，如果MeshRenderThread初始化，则需要创建时从MeshResource中把顶点数据复制过来，开销过大。
最终还是学习UE的处理方法，UMesh同时持有GameThread数据和RenderThread数据。