1. std::shared_ptr is too long to type, consider using Macro.

2. 延迟BeginRenderPass直到开始Draw。

3. 尝试将RenderPass和Renderer像Unity一样规范化。但是目前RenderPass和VkRenderPass绑定，影响了RenderTarget的设置。因此采用Unity的架构，RenderGraph用来存储每一帧的资源。可以存储当前的VkRenderPass，方便后续设置Framebuffer等。
并且对Render和Pass隐藏底层的细节。

4. CommandBuffer submit时进行调用End，同时获取新的CommandBuffer。一旦获取到，即调用Begin。
获取新CommandBuffer时，可以重置已提交的CommandBuffer状态。

5. AcquireNextImage需要fence同步。保证semaphore被触发后再调用。否则会产生"If semaphore is not VK_NULL_HANDLE, it must not have any uncompleted signal or wait operations pending"错误。

6. Vulkan对象的生命周期还是需要手动控制，所以不再使用shared_ptr。

7. CommandBuffer需要fence同步，但是fence想使用manager管理，只能在CommandBuffer创建后再传入fence。Unity使用FrameTracking管理fence和semaphore，unreal则是直接使用。