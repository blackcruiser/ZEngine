1. std::shared_ptr is too long to type, consider using Macro.

2. 延迟BeginRenderPass直到开始Draw。

3. RenderGraph用来存储每一帧的资源。可以存储当前的VkRenderPass，方便后续设置Framebuffer等。
并且对Render和Pass隐藏底层的细节。


4. CommandBuffer submit时进行调用End，同时获取新的CommandBuffer。一旦获取到，即调用Begin。
获取新CommandBuffer时，可以重置已提交的CommandBuffer状态。