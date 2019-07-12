### 首先列举最常见的和View的区别

- View的绘图效率低，主要用于动画变化较少的程序，必须在主线程更新
- SurfaceView绘图效率高，用于界面更新频繁的程序，一般在子线程更新
- SurfaceView拥有独立的Surface(绘图表面)，即它不与其宿主窗口共享同一个Surface
- SurfaceView使用双缓冲机制，播放视频时画面更流畅
- 每个窗口在SurfaceFlinger服务中都对应有一个Layer，用它来描述它的绘制表面。对于那些具有SurfaceView的窗口来说，每一个SurfaceView在SurfaceFlinger服务中还对应于一个独立的Layer或者LayerBuffer，用来单独描述它的绘图表面，以区别于它的宿主窗口的绘图表面。所以SurfaceView的UI就可以在一个独立的线程中进行绘制，可以不占用主线程资源，它产生原因也是为了应对耗时的操作，例如Camera X。

### 部分概念

- Canvas是Java层构建的数据结构，是给View用的画布,ViewGroup会将Canvas拆分给子View，在onDraw方法里将图形数据绘制在它获得的Canvas上

- Surface是Native层构建的数据结构，是给SurfaceFlinger用的画布，它是直接被用来绘制到屏幕上的数据结构

- 开发者一般所用到的View都是在Canvas进行绘制，然后最顶层的View的Canvas的数据信息会转换到一个Surface上，SurfaceFlinger会将各个应用窗口的Surface进行合成，然后绘制到屏幕上。

  ```java
  // ViewRootImpl#drawSoftware
  final Canvas canvas;
  canvas = mSurface.lockCanvas(dirty);
  mView.draw(canvas);
  surface.unlockCanvasAndPost(canvas);
  ```

  


### 双缓冲机制

- SurfaceView在更新视图时用到了两张Canvas，一张frontCanvas和一张backCanvas，每次实际显示的是frontCanvas，backCanvas存储的是上一次更改的视图。当你在播放这一帧的时候，它已经提前帮你加载好后面一帧了，所以播放起来流畅。
- 当使用lockCanvas()获取画布时，得到的实际是backCanvas而不是正在显示的frontCanvas，之后我们再在backCanvas上绘制新的视图，再通过unlockCanvasAndPost(canvas)此视图，然后上传的这张Canvas将替换原来的frontCanvas作为新的frontCanvas，原来的frontCanvas将切换到后台作为backCanvas。相当于多线程交替解析和渲染每一帧视频数据

**SurfaceView是Zorder排序的，默认在宿主Window的后面，SurfaceView通过在Window上面”挖洞“(设置透明区域进行显示)**

- SurfaceView是一个有自己的Surface的View，它的渲染可以放到单独线程而不是主线程中，其缺点是不能做变形和动画
- - SurfaceTexture可以用作非直接输出的内容流，这样就提供二次处理的机会，与SurfaceView直接输出相比，这样会有若干帧的延迟，内存消耗也会大一些
- TextureView是在View hierachy中做绘制，因此它一般是在主线程做的

 

链接：https://juejin.im/post/5d260b1c6fb9a07ef4442fb9 