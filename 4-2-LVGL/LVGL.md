<font size = 6>LVGL</font>

[toc]

# 导引

## FrameBuffer

### 简介

FrameBuffer实际上就是嵌入式系统中专门为GPU所保留的一块连续的物理内存， LCD通过专门的总线从framebuffer读取数据，显示到屏幕上。

FrameBuffer 也称帧缓存，是 Linux 内核将显存进行抽象后的一块缓存(内存)，目的是为了给用户态进程提供一套直接写屏的接口，而不用关心物理显存的位置、换页机制等具体细节。

<img src=".\img\image-20250520123133691.png" alt="image-20250520123133691" style="zoom:70%;" />

### 双framebuffer

让我们把计算机想象成一个画图比较快的人，假如他直接在屏幕上画图，而图形比较复杂， 则有可能在他只画了某幅图的一半的时候就被观众看到。而后面虽然他把画补全了， 但观众的眼睛却又没有反应过来，还停留在原来那个残缺的画面上。 也就是说，有时候观众看到完整的图象，有时却又只看到残缺的图象，这样就造成了屏幕的闪烁。

如何解决这一问题呢？我们设想有两块画板，画图的人在旁边画，画好以后把他手里的画板与挂在屏幕上的画板相交换。 这样以来，观众就不会看到残缺的画了。这一技术被应用到计算机图形中，称为双缓冲技术。 即：在存储器（很有可能是显存）中开辟两块区域，一块作为发送到显示器的数据，一块作为绘画的区域， 在适当的时候交换它们。由于交换两块内存区域实际上只需要交换两个指针，这一方法效率非常高，所以被广泛的采用。

## DMA2D

DMA2D是一个针对图像处理功能强化过的DMA，DMA2D可以在很多2D绘图的场合提供加速，完美嵌合了现代显卡中“GPU”的功能。虽然这个“GPU”只能提供2D加速，而且功能非常简单，与PC中的GPU不可同日而语，但是它已经可以满足大多数嵌入式开发中的图形显示加速需求。

### 简介

**功能**

- 颜色填充（矩形区域）
- 图像（内存）复制
- 颜色格式转换（如YCbCr转RGB或RGB888转RGB565）
- 透明度混合（Alpha Blend）

**工作模式**

- 寄存器到存储器
- 存储器到存储器
- 存储器到存储器并执行像素颜色格式转换
- 存储器到存储器且支持像素颜色格式转换和透明度混合

注

1. 大多数情况下，使用HAL库可以简化代码编写，提高可移植性。但对于DMA2D这种以计算和加速为目的的外设，考虑到相关的操作会在一个屏幕的绘制周期内被多次调用，此时再使用HAL库就会导致DAM2D的加速效率严重下降。所以，大多时候都不会用HAL库中的相关函数来对DMA2D进行操作。为了效率，我们会直接操作寄存器，这样才能起到最大化的加速效果。

**优势**

1. 速度更快：在部分项目中，DMA2D实现的速度相比纯软件实现最高可以达到30倍的差距！这是在主频高达400MHz还带L1-Cache的STM32H750平台上测试的结果，如果是在无cache且主频较低的STM32F4平台上进行测试，差距会进一步拉大
2. 性能更加稳定：由测试结果可以看出，DMA2D实现的方式受编译器优化等级的影响非常小，几乎可以忽略不计，这意味着，无论你使用IAR，GCC或是MDK，使用DMA2D都可以达到相同的性能表现。不会出现同一段代码移植后性能相差很大的情况
3. 代码编写更加简单：DMA2D的寄存器不多，而且比较直观。在某些场合，使用起来要比软件实现方便的多

### 应用举例

**颜色填充**

蓝色区域是我们要填充的矩形。可以看出矩形区域的内存地址是不连续的。使得我们不能简单使用memset这类高效的操作来实现矩形区域的填充。

<img src=".\img\image-20250520120509469.png" alt="image-20250520120509469" style="zoom:80%;" />

通常情况下，我们会使用以下方式的双重循环来填充任意矩形。

```c
for(int y = ys; y < ys + height; y++){
    for(int x = xs; x < xs + width; x++){
        framebuffer[y][x] = color;        
    }
}
```

代码虽然简单，但实际执行时，大量的CPU周期浪费在了判断、寻址、自增等的操作，实际写内存的时间占比很少。这样一来，效率就会下降。

这时候DMA2D的寄存器到存储器工作模式就可以发挥用场了，DAM2D可以以极高的速度填充矩形的内存区域，即使这些区域在内存中实际是不连续的。

<img src=".\img\image-20250520120849455.png" alt="image-20250520120849455" style="zoom:80%;" />

首先，因为我们只是进行内存填充，而不需要进行内存拷贝，所以我们要让DAM2D工作在寄存器到存储器模式。

```c
DMA2D->CR = 0x00030000UL;
```

然后，我们要告诉DAM2D要填充的矩形的属性，比如区域的起始地址在哪里，矩形的宽度有多少像素，矩形的高度有多少。区域起始地址是矩形区域左上角第一个像素的内存地址（图中红色像素的地址），这个地址由DAM2D的OMAR寄存器管理。而矩形的宽度和高度都是以像素为单位的，分别由NLR寄存器的高16位（宽度）和低16位（高度）来进行管理，具体的代码如下

```c
DMA2D->OMAR = (uint32_t)(&framebuffer[y][x]); // 设置填充区域的起始像素内存地址
DMA2D->NLR  = (uint32_t)(width << 16) | (uint16_t)height; // 设置矩形区域的宽高
```

接着，因为矩形在内存中的地址不连续，所以我们要告诉DMA2D在填充完一行的数据后，需要跳过多少个像素（即图中黄色区域的长度）。这个值由OOR寄存器管理。计算跳过的像素数量有一个简单的方法，即显示区域的宽度减去矩形的宽度即可。具体实现代码如下

```c
1DMA2D->OOR = screenWidthPx - width; // 设置行偏移，即跳过的像素
```

最后，我们需要告知DAM2D，你将使用什么颜色来进行填充，颜色的格式是什么。这分别由OCOLR和OPFCCR寄存器来管理，其中颜色格式由LTDC_PIXEL_FORMAT_XXX宏来定义，具体代码如下

```c
DMA2D->OCOLR   = color; // 设置填充使用的颜色
DMA2D->OPFCCR  = pixelFormat; // 设置颜色格式，比如想设置成RGB565，就可以使用宏LTDC_PIXEL_FORMAT_RGB565
```

一切都设置完毕，DMA2D已经获取到了填充这个矩形所需要的全部信息，接下来，我们要开启DMA2D的传输，这通过将DMA2D的CR寄存器的第0位设置为1来实现

```c
DMA2D->CR |= DMA2D_CR_START; // 开启DMA2D的数据传输，DMA2D_CR_START是一个宏，其值为0x01
```

等DMA2D传输开始后，我们只需要等待它传输完毕即可。DAM2D传输完成后，会自动把CR寄存器的第0位设置为0，所以我们可以通过以下代码来等待DAM2D传输完成

```c
while (DMA2D->CR & DMA2D_CR_START) {} // 等待DMA2D传输完成
```

注

1. 如果你使用了OS，则可以使能DMA2D的传输完毕中断。然后我们可以创建一个信号量并且在开启传输后等待它，随后在DMA2D的传输完毕中断服务函数中释放该信号量。这样的话CPU就可以在DMA2D工作的时候去干点别的事儿而不是在此处傻等
2. 当然，由于实际执行时，DMA2D进行内存填充的速度实在是太快了，以至于OS切换任务的开销都比这个时间要长，所以即便使用了OS，我们还是会选择死等 

配置函数

```c
 1static inline void DMA2D_Fill( void * pDst, uint32_t width, uint32_t height, uint32_t lineOff, uint32_t pixelFormat,  uint32_t color) {
 
     /* DMA2D配置 */  
     DMA2D->CR      = 0x00030000UL;                                  // 配置为寄存器到储存器模式
     DMA2D->OCOLR   = color;                                         // 设置填充使用的颜色，格式应该与设置的颜色格式相同
     DMA2D->OMAR    = (uint32_t)pDst;                                // 填充区域的起始内存地址
     DMA2D->OOR     = lineOff;                                       // 行偏移，即跳过的像素，注意是以像素为单位
     DMA2D->OPFCCR  = pixelFormat;                                   // 设置颜色格式
     DMA2D->NLR     = (uint32_t)(width << 16) | (uint16_t)height;    // 设置填充区域的宽和高，单位是像素

    /* 启动传输 */
    DMA2D->CR   |= DMA2D_CR_START;   

    /* 等待DMA2D传输完成 */
    while (DMA2D->CR & DMA2D_CR_START) {} 
}
```

矩阵填充函数

```c
void FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color){
    void* pDist = &(((uint16_t*)framebuffer)[y*320 + x]);
    DMA2D_Fill(pDist, w, h, 320 - w, LTDC_PIXEL_FORMAT_RGB565, color);
}

  // 填充背景色
  FillRect(0,   0,   320, 240,  0xFFFF);
  // 绘制数据条
  FillRect(80,  80,  20,  120,  0x001f);
  FillRect(120, 100, 20,  100,  0x001f);
  FillRect(160, 40,  20,  160,  0x001f);
  FillRect(200, 60,  20,  140,  0x001f);
  // 绘制X轴
  FillRect(40,  200, 240, 1,    0x0000);
```

**图片显示(内存复制)**

如果想在屏幕上显示一团跳动的火焰。一般是由美工先把火焰的每一帧都画出来，然后放到同一张图片素材里面，如下图所示

<img src=".\img\image-20250520122419666.png" alt="image-20250520122419666" style="zoom:50%;" />

然后我们以一定的间隔轮流显示每一帧图像，就可以在屏幕上实现“跳动的火焰”这个效果了。

假设这张素材图片已经在内存中了。然后我们来考虑如何将其中的一帧图片显示到屏幕上。通常情况下，我们会这样实现：先计算得出每一帧的数据在内存中的地址，然后将这一帧图片的数据复制到framebuffer中相应的位置即可。

显而易见，要实现这个效果需要大量的内存复制操作。在嵌入式系统中，需要大量数据复制的时候，硬件DMA的效率是最高的。但是硬件DMA只能搬运地址连续的数据，而这里，需要复制的数据在源图片和frambuffer中的地址都是不连续的，这引来了额外的开销，也导致我们无法使用硬件DMA来进行高效的数据复制。

为了以最快的速度把素材图片中的某一块数据搬运到帧缓冲中，我们来看如何使用DMA2D来实现。

首先，因为这次是要在存储器中进行数据复制，所以我们要把DMA2D的工作模式设定为“存储器到存储器模式”，这通过设置DMA2D的CR寄存器的[17:16]位为00来实现，代码如下

```c
DMA2D->CR      = 0x00000000UL;
```

然后我们要分别设置源和目标的内存地址，因为数据源也存在内存偏移，所以我们要同时设定源和目标位置的数据偏移

```c
DMA2D->FGMAR   = (uint32_t)pSrc; // 源地址
DMA2D->OMAR    = (uint32_t)pDst; // 目标地址
DMA2D->FGOR    = OffLineSrc;     // 源数据偏移（像素）
DMA2D->OOR     = OffLineDst;     // 目标地址偏移（像素）
```

然后依然是设置要复制的图像的宽和高、颜色格式、开启传输以及等待传输完成

```c
DMA2D->FGPFCCR = pixelFormat;
DMA2D->NLR     = (uint32_t)(xSize << 16) | (uint16_t)ySize;
/* 启动传输 */
DMA2D->CR   |= DMA2D_CR_START;
/* 等待DMA2D传输完成 */
while (DMA2D->CR & DMA2D_CR_START) {}
```

 配置函数

```c
static void DMA2D_MemCopy(uint32_t pixelFormat, void * pSrc, void * pDst, int xSize, int ySize, int OffLineSrc, int OffLineDst)
 {
     /* DMA2D配置 */
     DMA2D->CR      = 0x00000000UL;
     DMA2D->FGMAR   = (uint32_t)pSrc;
     DMA2D->OMAR    = (uint32_t)pDst;
     DMA2D->FGOR    = OffLineSrc;
     DMA2D->OOR     = OffLineDst;
     DMA2D->FGPFCCR = pixelFormat;
    DMA2D->NLR     = (uint32_t)(xSize << 16) | (uint16_t)ySize;

    /* 启动传输 */
    DMA2D->CR   |= DMA2D_CR_START;

    /* 等待DMA2D传输完成 */
    while (DMA2D->CR & DMA2D_CR_START) {}
}
```

火焰燃烧函数

```c
static void DMA2D_DisplayFrameAt(uint16_t index){

    uint16_t *pStart = (uint16_t *)img_fireSequenceFrame;
    pStart += (index / TILE_COUNT_ROW) * (TILE_WIDTH_PIXEL * TILE_WIDTH_PIXEL * TILE_COUNT_ROW);
    pStart += (index % TILE_COUNT_ROW) * TILE_WIDTH_PIXEL;
    uint32_t offlineSrc = (TILE_COUNT_ROW - 1) * TILE_WIDTH_PIXEL;

    DMA2D_MemCopy(LTDC_PIXEL_FORMAT_RGB565, (void*) pStart, pDist, TILE_WIDTH_PIXEL, TILE_WIDTH_PIXEL, offlineSrc, offlineDist);
}

while(1){
    for(int i = 0; i < FRAME_COUNTS; i++){
        DMA2D_DisplayFrameAt(i);
        HAL_Delay(FRAME_TIME_INTERVAL);
    }
}
```

**图片渐变切换**

假设我们要开发一个看图应用，在两张图片进行切换时，直接进行切换会显得比较生硬，所以我们要加入切换时的动态效果，而渐变切换（淡入淡出）是一个很经常使用的，而且看起来还不错的效果。

这里我们需要先了解一下透明度混合的基本概念。首先透明度混合需要有一个前景，一个背景。而混合的结果就相当于透过前景看背景时的效果。如果前景完全不透明，那么就完全看不到背景，反之如果前景完全透明，那么就只能看到背景。而如果前景是半透明的，则结果就是两者根据前景色的透明度按照一定的规则进行混合。

如果1表示完全透明，0表示不透明，则透明度的混合公式如下，其中A是背景色，B是前景色

```c
X(C)=(1-alpha)*X(B) + alpha*X(A)
```

因为颜色有RGB三个通道，所以我们需要对三通道都进行计算，计算完成后在进行组合

```c
R(C)=(1-alpha)*R(B) + alpha*R(A)
G(C)=(1-alpha)*G(B) + alpha*G(A)
B(C)=(1-alpha)*B(B) + alpha*B(A)
```

而在程序中为了效率起见（CPU对于浮点的运算速度很慢），我们并不用0\~1这个范围的值。通常情况下我们一般会使用一个8bit的数值来表示透明度，范围从0~255。需要注意的是，这个数值越大表示越不透明，也就是说255是完全不透明，而0表示完全透明（所以也叫不透明度），然后我们可以得到最终的公式

```c
outColor = ((int) (fgColor * alpha) + (int) (bgColor) * (256 - alpha)) >> 8;
```

假设整个渐变在30帧内完成，我们需要在内存中开辟一块儿大小等于图片的缓冲区。然后我们以第一张图片（当前显示的图片）为背景，第二张图片（接下来显示的图片）为前景，然后为前景设置一个透明度，对每个像素进行透明度混合，并且将混合结果暂存至缓冲区中。待混合结束后，将缓冲区中的数据复制到framebuffer中即完成了一帧的显示。接下来继续进行第二帧、第三帧……逐渐增大前景的不透明度，直到前景色的变为不透明，即完成了图片的渐变切换。

因为每一帧都需要对两张图片中的每一个像素都进行混合运算，这带了来巨大的运算量。交给CPU实现是很不明智的行为，所以我们还是把这些工作交给DMA2D。

这次用到了DMA2D的混合功能，所以我们要使能DAM2D的带颜色混合的存储器到存储器模式，对应CR寄存器[17:16]位的值为10

```c
DMA2D->CR    = 0x00020000UL;                // 设置工作模式为存储器到存储器并带颜色混合
```

然后分别设置前景、背景和输出数据的内存地址和数据传输偏移、传输图像的宽和高

```c
DMA2D->FGMAR = (uint32_t)pFg;               // 设置前景数据内存地址
DMA2D->BGMAR = (uint32_t)pBg;               // 设置背景数据内存地址
DMA2D->OMAR  = (uint32_t)pDst;              // 设置数据输出内存地址

DMA2D->FGOR  = offlineFg;                   // 设置前景数据传输偏移
DMA2D->BGOR  = offlineBg;                   // 设置背景数据传输偏移
DMA2D->OOR   = offlineDist;                 // 设置数据输出传输偏移

DMA2D->NLR = (uint32_t)(xSize << 16) | (uint16_t)ySize; // 设置图像数据宽高（像素）
```

设置颜色格式。这里设置前景色的颜色格式时需要注意，因为如果使用的是ARGB这样的颜色格式，那么我们进行透明度混合时，颜色数据中本身的alpha通道就会对混合结果产生影响，所以我们这里要设定在进行混合操作时，忽略前景色自身的alpha通道。并强制设定混合时的透明度

```c
DMA2D->FGPFCCR = pixelFormat                // 设置前景色颜色格式
        | (1UL << 16)                       // 忽略前景颜色数据中的Alpha通道
        | ((uint32_t)opa << 24);            // 设置前景色不透明度

DMA2D->BGPFCCR = pixelFormat;               // 设置背景颜色格式
DMA2D->OPFCCR = pixelFormat;                // 设置输出颜色格式
```

最后，启动传输即可

```c
/* 启动传输 */
DMA2D->CR   |= DMA2D_CR_START;

/* 等待DMA2D传输完成 */
while (DMA2D->CR & DMA2D_CR_START) {}
```

注

1. 有时我们会遇到一张带有透明通道的图片与背景叠加显示的情况，此时就不应该禁用颜色本身的alpha通道
2. 这个模式下，我们不仅可以进行颜色混合，还可以同时转换颜色格式，可以根据需要设置前景和背景以及输出的颜色格式

 配置函数

```c
 void _DMA2D_MixColors(void* pFg, void* pBg, void* pDst,
         uint32_t offlineFg, uint32_t offlineBg, uint32_t offlineDist,
         uint16_t xSize, uint16_t ySize,
         uint32_t pixelFormat, uint8_t opa) {
 
     DMA2D->CR    = 0x00020000UL;                // 设置工作模式为存储器到存储器并带颜色混合
 
     DMA2D->FGMAR = (uint32_t)pFg;               // 设置前景数据内存地址
     DMA2D->BGMAR = (uint32_t)pBg;               // 设置背景数据内存地址
     DMA2D->OMAR  = (uint32_t)pDst;              // 设置数据输出内存地址

     DMA2D->FGOR  = offlineFg;                   // 设置前景数据传输偏移
     DMA2D->BGOR  = offlineBg;                   // 设置背景数据传输偏移
     DMA2D->OOR   = offlineDist;                 // 设置数据输出传输偏移

     DMA2D->NLR = (uint32_t)(xSize << 16) | (uint16_t)ySize; // 设置图像数据宽高（像素）

     DMA2D->FGPFCCR = pixelFormat                // 设置前景色颜色格式
            | (1UL << 16)                       // 忽略前景颜色数据中的Alpha通道
            | ((uint32_t)opa << 24);            // 设置前景色不透明度


     DMA2D->BGPFCCR = pixelFormat;               // 设置背景颜色格式
     DMA2D->OPFCCR  = pixelFormat;                // 设置输出颜色格式

     /* 启动传输 */
     DMA2D->CR   |= DMA2D_CR_START;

     /* 等待DMA2D传输完成 */
     while (DMA2D->CR & DMA2D_CR_START) {}
}
```

图片渐变切换函数

```c
 void DMA2D_AlphaBlendDemo(){
 
     const uint16_t lcdXSize = 320, lcdYSize = 240;
     const uint8_t cnvFrames = 60; // 60帧完成切换
     const uint32_t interval = 33; // 每秒30帧
     uint32_t time = 0;
 
     // 计算输出位置的内存地址
     uint16_t distX = (lcdXSize - DEMO_IMG_WIDTH) / 2;
    uint16_t distY = (lcdYSize - DEMO_IMG_HEIGHT) / 2;
    uint16_t* pFb = (uint16_t*) framebuffer;
    uint16_t* pDist = pFb + distX + distY * lcdYSize;
    uint16_t offlineDist = lcdXSize - DEMO_IMG_WIDTH;

    uint8_t nextImg = 1;
    uint16_t opa = 0;
    void* pFg = 0;
    void* pBg = 0;
    while(1){
        // 切换前景/背景图片
        if(nextImg){
            pFg = (void*)img_cat;
            pBg = (void*)img_fox;
        }
        else{
            pFg = (void*)img_fox;
            pBg = (void*)img_cat;
        }

        // 完成切换
        for(int i = 0; i < cnvFrames; i++){
            time = HAL_GetTick();
            opa = 255 * i / (cnvFrames-1);
            _DMA2D_MixColors(pFg, pBg, pDist,
                    0,0,offlineDist,
                    DEMO_IMG_WIDTH, DEMO_IMG_HEIGHT,
                    LTDC_PIXEL_FORMAT_RGB565, opa);
            time = HAL_GetTick() - time;
            if(time < interval){
                HAL_Delay(interval - time);
            }
        }
        nextImg = !nextImg;
        HAL_Delay(5000);
    }
}
```

## 内存池

内存池管理是一种优化内存分配和释放的技术，特别适用于需要频繁分配和释放内存的场景。内存池通过预先分配一大块内存，并将其划分为多个小块，从而减少了频繁调用系统内存分配函数的开销。

### 固定大小内存块分配

固定大小内存块分配是一种简单而高效的内存池管理方法。它将内存池划分为若干个相同大小的内存块，每个内存块都有一个对应的内存管理表项。内存管理表项的值为0表示该内存块未被占用，非零表示该内存块已被占用。

**初始化**

在初始化时，需要预先定义待分配的内存物理地址和内存块大小。例如，在STM32F4 MCU中，可以将内部SRAM、内部CCRAM和外部SRAM分别设置为32字节的内存块。

**分配和释放**

分配内存时，从内存池中找到连续的空闲内存块，并更新内存管理表。释放内存时，将对应的内存管理表项清零。

### 可变大小内存块分配

可变大小内存块分配是一种更灵活的内存池管理方法。它允许内存块的大小根据需求动态调整，从而提高了内存利用率。

**初始化**

在初始化时，需要定义内存池的总大小和最小内存块大小。然后，根据需求动态分配内存块。

**分配和释放**

分配内存时，根据需求找到合适大小的空闲内存块，并更新内存管理表。释放内存时，将对应的内存块合并回内存池。

# 简介

LVGL（ Light and Versatile Graphics Library ）是一个轻量、多功能的开源图形库。

<img src=".\img\image-20250604164759166.png" alt="image-20250604164759166" style="zoom:60%;" />

**特点**

- 支持多种输入设备
- 支持操作系统以及GPU
- 部件、功能丰富
- 具备高级图形特征
- 支持多种样式属性
- C语言编写（兼容C++）

**LVGL模拟器**

使用PC端软件模拟LVGL运行，而不需要任何嵌入式硬件，便于学习、跨平台协同开发。

注

1. 根据板子需求配置屏幕分辨率以及配置文件

**编程思想**

LVGL采用的是面向对象的编程思想，以抽象的类来实例化不同的对象（部件）。

C语言中没有“类”的概念，LVGL以结构体的形式来实现“类”的思维。

# 移植

## 概要

**移植要求**

- 16、32 或 64 位微控制器或处理器
- 主控频率：>16Mhz
- Flash/ROM：> 64kb，建议180kb以上 
- RAM：>8kb，建议24kb以上
- 图形缓冲区（开辟于RAM上）：大于水平分辨率像素，建议大于1/10屏幕总像素对应所占空间
- C99或更新的版本的编译器

注

1. 仅针对V8版本的LVGL

**优化LVGL运行效果的方法**

- 提高芯片主频
- 增大SRAM容量、提高读写速度
- 增大图形缓冲区、使用双缓冲
- 减小需要刷新的总像素
- 提高图像数据的传输速度
- 工程允许的情况下，优化编译等级

注

1. 关键点：缩短图像刷新所需要的时间

**内存不够时的处理**

- 修改lv_conf.h，适当减小分配给LVGL管理的内存`#define LV_MEM_SIZE(20U * 1024)  /bytes`
- lv_port_disp_timplate.c，适当减小图形缓冲区的大小，同时需要兼顾运行效果
- FreeRTOSConfig.h，适当减小分配给FreeRTOS的内存，简单的工程，一般10~20k就够用了

## 移植流程

1. 确定输入、输出设备：输入触摸屏、鼠标、键盘以及编码器；输出显示屏
2. 确定所需功能：屏幕数据传输方式、系统、SRAM、内存管理算法
3. 准备LVGL库、例程：准备对应版本的LVGL头文件、源文件，还有支持所需功能的例程源码
4. 添加LVGL库到工程：按需裁剪、修改LVGL库头文件，添加库文件到`../LVGL/GUI/lvgl`文件夹，演示文件可以添加到`../LVGL/GUI_APP`文件夹

<img src=".\img\image-20250520183446350.png" alt="image-20250520183446350" style="zoom:40%;" />

5. 为工程创建合适的分组，添加lvgl源文件到工程中，并添加对应头文件路径，同时开启C99模式

<img src=".\img\image-20250520184317806.png" alt="image-20250520184317806" style="zoom:40%;" />

6. 配置输出：把 `lv_port_disp_template.c/h` 的条件编译指令 #if 0 修改成 #if 1，在 `disp_init `函数中初始化屏幕设备，配置图形数据缓冲模式（三种），设置屏幕尺寸，在 `disp_flush` 函数中配置打点输出
7. 配置输入：把 `lv_port_indev_template.c/h` 的条件编译指令 #if 0 修改成 #if 1，按需裁剪输入设备，在 `touchpad_init`函数中初始化输入设备，配置输入检测函数，配置坐标获取函数
8. 提供心跳：在定时器驱动`Timer.c`文件中包含`#include "lvgl.h"`，在定时器中断函数中调用`lv_tick_inc(x);`，初始化定时器时，需保证进入中断的时间间隔 = x 毫秒
9. main函数部分：在main.c文件中包含对应头文件，初始化定时器、LVGL库、输入输出设备，在while中每隔5ms调用一次`lv_timer_handler();`，并编写测试代码

```c
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/SDRAM/sdram.h"
#include "./BSP/TOUCH/touch.h"
#include "./BSP/TIMER/btim.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "lv_demo_stress.h"
#include "lv_demo_music.h"

int main(void)
{
    HAL_Init();                              /* 初始化HAL库 */
    sys_stm32_clock_init(360, 25, 2, 8);     /* 设置时钟,180Mhz */
    delay_init(180);                         /* 延时初始化 */
    usart_init(115200);                      /* 初始化USART */
    led_init();                              /* 初始化LED */
    key_init();                              /* 初始化按键 */
    sdram_init();                            /* 初始化SDRAM */

    timer_init(10-1, 9000-1);

    /* 初始化LVGL 本质为链表初始化（存放控件）*/
    lv_init(); 

    /* 输入设备、输出设备初始化 */
    lv_port_disp_init(); 
    lv_port_indev_init();

    /* 用户自定义部分*/
  //  lv_obj_t* switch_obj = lv_switch_create(lv_scr_act());
  //  lv_obj_set_size(switch_obj, 120, 60);
  //  lv_obj_align(switch_obj, LV_ALIGN_CENTER, 0, 0);
  //  lv_demo_stress();
     lv_demo_music();
    
    while(1)
    {

        /* LVGL心跳节拍， 驱动 LVGL 的异步逻辑，建议5ms调用一次
        - 作用
        - 1.执行注册的定时器（lv_timer_t）的回调函数
        - 2.处理屏幕刷新，将脏区内容更新到物理屏幕
        - 3.管理动画，逐帧更新动画状态
        - 4.处理输入设备事件（如触摸、按键），分发事件到目标控件
        - 5.执行延迟任务，优化性能
        */
        delay_ms(5);
        lv_timer_handler();
    }
} 
```

<img src=".\img\image-20250520182247200.png" alt="image-20250520182247200" style="zoom:40%;" />

注

1. 单缓冲模式：一个默认大小为水平分辨率像素×10的缓冲区，具体大小于颜色深度有关，在保证有足够空间的前提下，提高缓冲区大小可使得界面更流畅
2. 双缓冲模式：两个默认大小为水平分辨率像素×10的缓冲区
3. 全屏幕双缓冲模式：两个默认大小为屏幕总像素个数的缓冲区
4. lv_conf.h中可以通过宏`LV_INDEV_DEF_READ_PERLOD`来设置输入设备扫描间隔
5. RTOS时，需要在lv_conf.h文件中配置自定义的时钟源，删除定时器提供时基的部分代码`lv_tick_inc(x);`

```c
/* 自定义的时钟源 */
#if LV_TICK_CUSTOM 1
	#define LV_TICK_CUSTOM_INCLUDE "FreeRTOS.h"
	#define LV_TICK_CUSTOM_SYS_TIME_EXPR(xTaskGetTickCount())
#endif
```

## LVGL库文件

<img src=".\img\image-20250519213227356.png" alt="image-20250519213227356" style="zoom:50%;" />

<img src=".\img\image-20250520172009671.png" alt="image-20250520172009671" style="zoom:50%;" />

**lv_conf.h文件内容**

用户级文件，不同需求文件内容不同。

| 序号 | 板块介绍 |             功能描述             |
| :--: | :------: | :------------------------------: |
|  ①   |   颜色   |  颜色深度、字节交换、屏幕透明等  |
|  ②   |   内存   | 内存管理算法选择、内存分配大小等 |
|  ③   |  硬件层  | 显示刷新周期、输入设备读取周期等 |
|  ④   |   特征   |      绘图、日志、帧率显示等      |
|  ⑤   |  编译器  |      大数组前缀、内存对齐等      |
|  ⑥   |   字体   |  开启系统字体、配置自定义字体等  |
|  ⑦   |   文本   |        字符编码、文本特性        |
|  ⑧   | 核心部件 |        使能/失能核心部件         |
|  ⑨   | 拓展功能 |  使能/失能拓展部件、第三方库等   |
|  ⑩   |   实例   |     使能/失能LVGL官方的实例      |

## 外部SRAM

**移植过程**

1. 添加SRAM驱动相关文件
2. 包含外部SRAM驱动头文件
3. 初始化外部SRAM：sdram_init();或 sram_init();

**外部SRAM使用**

- 场景1：把LVGL管理的内存空间放到外部SRAM（非常不推荐）

1. 确定外部SRAM首地址，根据需求确定地址偏移（加上LCD帧缓存数组大小）
2. 在lv_conf.h中将LV_MEM_ADR定义到外部SRAM的地址（不一定是首地址）

- 场景2：把绘图缓冲区放到外部SRAM（内部空间匮乏时可取）

1. 确定外部SRAM首地址，根据需求确定地址偏移（加上LCD帧缓存数组大小）
2. 在`lv_port_disp_template.c`中创建全屏分辨率大小的数组，并将其定位到外部SRAM的地址

```c
static lv_color_t buf_1[800 * 480] __attribute__(at(0XC0000000 + 1280 * 800 * 2))
```

## 内存管理算法

分配给LVGL管理的内存不能太多或者太少，需要兼顾图形缓冲区的内存。

**LVGL中消耗内存的地方**

<img src=".\img\image-20250520170932154.png" alt="image-20250520170932154" style="zoom:50%;" />

注

1. 不同的项目，内存分配的比例不同，一般分配给LVGL管理的内存为15~40k字节

**自研内存管理配置流程**

1. 添加内存管理相关文件
2. 包含内存管理算法头文件
3. 配置需要管理的内存池大小：在malloc.h文件中通过相关的宏来定义，不同板子代码的宏定义不同
4. 初始化需要管理的内存池：调用my_mem_init函数对所需要管理的内存池进行初始化即可
5. 适配自研的内存管理算法：编写LVGL内存分配、内存释放、内存重新分配这三个函数，配置相关宏定义

**LVGL自带内存管理配置流程**

1. 添加内存管理相关文件
2. 包含内存管理算法头文件
3. 配置需要管理的内存池大小：通过lv_conf.h中的LV_MEM_SIZE来定义，以字节为单位
4. 创建需要管理的内存池：有3种方式：①大数组、②地址段、③内存分配函数

## DMA2D

**配置流程**

1. 修改lv_conf.h中相关的宏定义，开启DMA2D
2. 在MDK配置中添加对应芯片的定义
3. 配置DMA2D外设

# 基础

## 基础对象

### 简介

基础对象（lv_obj）可以作为父对象，来创建其他对象，同时它也可作为部件使用。

<img src=".\img\image-20250524174031178.png" alt="image-20250524174031178" style="zoom:40%;" />

```c
#include "lvgl.h"
#include "my_gui.h"

void my_gui(void)
{
  lv_obj_t *obj1 = lv_obj_create(lv_scr_act());  // 以默认活动窗口为父对象创建基础对象1，实则为一矩形框
  lv_obj_set_size(switch1,200,100);

  lv_obj_t *obj2 = lv_obj_create(switch1);       // 以基础对象1为父对象创建基础对象2，默认位于父对象左上角
}
```

### 父和子对象的关系

- 子对象会随着父对象移动

<img src=".\img\image-20250524174133528.png" alt="image-20250524174133528" style="zoom:50%;" />

- 子对象的位置超出父对象的范围，则超出的部分不显示（尽量避免）

<img src=".\img\image-20250524174151748.png" alt="image-20250524174151748" style="zoom:50%;" />

## 部件基本属性

<img src=".\img\image-20250604173112468.png" alt="image-20250604173112468" style="zoom:50%;" />

- 大小（size）
- 位置（position）
- 对齐（alignment）
- 组成（part）
- 状态（states）
- 样式（styles）
- 标志（flag）
- 图层（layers）
- 事件（events）

### 大小

<img src=".\img\image-20250524174542670.png" alt="image-20250524174542670" style="zoom:33%;" />

**相关API函数**

- 设置宽度：lv_obj_set_width(obj, new_width);
- 设置高度：lv_obj_set_height(obj, new_height);
- 同时设置宽度、高度：lv_obj_set_size(obj, new_width, new_height);

### 位置

需要注意的是，在设置部件位置时，是相对于父对象而言，即坐标原点为父对象的左上角。

<img src=".\img\image-20250524174651150.png" alt="image-20250524174651150" style="zoom:35%;" />

**相关API函数**

- 设置X轴坐标：lv_obj_set_x(obj, new_x);
- 设置Y轴坐标：lv_obj_set_y(obj, new_y);
- 同时设置X、Y轴坐标：lv_obj_set_pos(obj, new_x, new_y);

### 对齐

- 参照父对象对齐（只能位于父对象内部）

<img src=".\img\image-20250524174920606.png" alt="image-20250524174920606" style="zoom:45%;" />

- 参照其他对象对齐（无父子关系）

<img src=".\img\image-20250524174932546.png" alt="image-20250524174932546" style="zoom:45%;" />

**相关API函数**

- 参照父对象对齐：lv_obj_set_align(obj, LV_ALIGN_...);
- 参照父对象对齐，再进行偏移：lv_obj_align(obj, LV_ALIGN_..., x, y);
- 参照其他对象对齐（无父子关系），再进行偏移：lv_obj_align_to(obj_to_align, obj_referece, LV_ALIGN_..., x, y);

**对齐的模式**

<img src=".\img\image-20250524175131596.png" alt="image-20250524175131596" style="zoom:40%;" />

### 组成

小部件由一个或多个部件构建而成，通过使用部件，可以对小部件的子元素应用不同的样式。

```c
enum {

    LV_PART_MAIN		= 0x000000, 	 	/* 主体，像矩形一样的背景 */
    LV_PART_SCROLLBAR	= 0x010000, 	 	/* 滚动条 */
    LV_PART_INDICATOR	= 0x020000, 	 	/* 指示器，指示当前值 */
    LV_PART_KNOB		= 0x030000,	 		/* 手柄或旋钮，用于调整参数值 */
    LV_PART_SELECTED	= 0x040000, 	 	/* 选项框，指示当前选择的选项 */
    LV_PART_ITEMS		= 0x050000, 	 	/* 相似的元素，例如单元格 */
    LV_PART_TICKS		= 0x060000, 	 	/* 刻度 */
    LV_PART_CURSOR		= 0x070000, 	 	/* 光标 */
};
```

### 状态

部件可以是以下几种状态的组合

```c
enum {
    
    LV_STATE_DEFAULT     	=  0x0000,		 /* 默认状态 */
    LV_STATE_CHECKED     	=  0x0001,		 /* 切换或选中状态 */
    LV_STATE_FOCUSED     	=  0x0002,		 /* 通过键盘、编码器聚焦或通过触摸板、鼠标单击 */
    LV_STATE_FOCUS_KEY   	=  0x0004,		 /* 通过键盘、编码器聚焦 */
    LV_STATE_EDITED      	=  0x0008,		 /* 由编码器编辑 */
    LV_STATE_HOVERED     	=  0x0010,		 /* 鼠标悬停（现在不支持）*/
    LV_STATE_PRESSED     	=  0x0020,		 /* 已按下 */
    LV_STATE_SCROLLED    	=  0x0040,		 /* 滚动状态 */
    LV_STATE_DISABLED    	=  0x0080,		 /* 禁用状态 */
    …
};
```

### 样式

样式用于设置部件的外观，以优化显示界面和实现用户交互。

**可设置属性**

- 大小Size
- 位置Position
- 背景Background
- 轮廓Outline
- 边框Border
- 阴影Shadow
- 其它Others

**如何给部件添加样式**

- 添加普通样式——实现共享

```c
static lv_style_t style; 											/* 定义样式变量 必须为全局、静态变量或动态分配 否则会出现问题 */
lv_style_init(&style); 												/* 初始化样式 */
lv_style_set_bg_color(&style, lv_color_hex(0xf4b183)); 				/* 设置背景颜色 */

lv_obj_t * obj = lv_obj_create(lv_scr_act()); 						/* 创建一个部件 */
lv_obj_add_style(obj, & style, LV_STATE_DEFAULT); 					/* 设置部件的样式以及触发方式 */
// style 的指针（&style）会被 LVGL 的控件 obj 保存，但 style 的内存会在函数退出后失效
// 当LVGL需要重绘按钮（例如屏幕刷新、状态变化）时，会通过&style读取样式数据，但此时内存已失效
```

- 添加本地样式——无法共享

```c
lv_obj_t * obj = lv_obj_create(lv_scr_act()); 								/* 创建一个部件 */
lv_obj_set_style_bg_color(obj, lv_color_hex(0xf4b183),LV_STATE_DEFAULT);	/* 设置部件的样式以及触发方式 */
```

注

1. LVGL 不会 将 `style` 的内容复制到控件内部，控件会直接保存 `&style` 这个指针，并在后续渲染时通过该指针访问样式数据

**单独设置部件中某个部分样式**

<img src=".\img\image-20250524175814358.png" alt="image-20250524175814358" style="zoom:35%;" />

```c
#include "lvgl.h"
#include "my_gui.h"

void my_gui(void)
{
  lv_obj_t *slider = lv_slider_create(lv_scr_act());  
  lv_obj_set_align(slider,LV_ALIGN_CENTER);

  // 默认修改主题样式
  lv_obj_set_style_bg_color(slider,lv_color_hex(0x123456),LV_STATE_DEFAULT);

  // 修改其他部分样式
  lv_obj_set_style_bg_color(slider,lv_color_hex(0x123456),LV_STATE_DEFAULT | LV_PART_INDICATOR);
}
```

注

1. 无法一次修改多个样式，需多次调用函数
1. 可以调用`lv_obj_remove_style_all(obj);`或`lv_obj_remove_style(obj, style, selector);`来删除样式（注意顺序），仅当 selector 与 lv_obj_add_style 中使用的 selector 匹配时，此函数才会删除 style
1. 调用`lv_obj_remove_style(widget, NULL, LV_PART_...);`，可以用于删除部件的组成部分

### 标志

有一些部件的属性可以通过`lv_obj_add_flag(Widget, lv_obj_flag…)`和 `lv_obj_remove_flag(Widget, lv_obj_flag…)`来启用/禁用。

```c
enum {
    
    LV_OBJ_FLAG_HIDDEN          = (1L << 0),  /**< Make the object hidden. (Like it wasn't there at all)*/
    LV_OBJ_FLAG_CLICKABLE       = (1L << 1),  /**< Make the object clickable by the input devices*/
    LV_OBJ_FLAG_CLICK_FOCUSABLE = (1L << 2),  /**< Add focused state to the object when clicked*/
    LV_OBJ_FLAG_CHECKABLE       = (1L << 3),  /**< Toggle checked state when the object is clicked*/
    LV_OBJ_FLAG_SCROLLABLE      = (1L << 4),  /**< Make the object scrollable*/
    LV_OBJ_FLAG_SCROLL_ELASTIC  = (1L << 5),  /**< Allow scrolling inside but with slower speed*/
    LV_OBJ_FLAG_SCROLL_MOMENTUM = (1L << 6),  /**< Make the object scroll further when "thrown"*/
    LV_OBJ_FLAG_SCROLL_ONE      = (1L << 7),  /**< Allow scrolling only one snappable children*/
    LV_OBJ_FLAG_SCROLL_CHAIN_HOR = (1L << 8), /**< Allow propagating the horizontal scroll to a parent*/
    LV_OBJ_FLAG_SCROLL_CHAIN_VER = (1L << 9), /**< Allow propagating the vertical scroll to a parent*/
    LV_OBJ_FLAG_SCROLL_CHAIN     = (LV_OBJ_FLAG_SCROLL_CHAIN_HOR | LV_OBJ_FLAG_SCROLL_CHAIN_VER),
    LV_OBJ_FLAG_SCROLL_ON_FOCUS = (1L << 10),  /**< Automatically scroll object to make it visible when focused*/
    LV_OBJ_FLAG_SCROLL_WITH_ARROW  = (1L << 11), /**< Allow scrolling the focused object with arrow keys*/
    LV_OBJ_FLAG_SNAPPABLE       = (1L << 12), /**< If scroll snap is enabled on the parent it can snap to this object*/
    LV_OBJ_FLAG_PRESS_LOCK      = (1L << 13), /**< Keep the object pressed even if the press slid from the object*/
    LV_OBJ_FLAG_EVENT_BUBBLE    = (1L << 14), /**< Propagate the events to the parent too*/
    LV_OBJ_FLAG_GESTURE_BUBBLE  = (1L << 15), /**< Propagate the gestures to the parent*/
    LV_OBJ_FLAG_ADV_HITTEST     = (1L << 16), /**< Allow performing more accurate hit (click) test. E.g. consider rounded corners.*/
    LV_OBJ_FLAG_IGNORE_LAYOUT   = (1L << 17), /**< Make the object position-able by the layouts*/
    LV_OBJ_FLAG_FLOATING        = (1L << 18), /**< Do not scroll the object when the parent scrolls and ignore layout*/
    LV_OBJ_FLAG_OVERFLOW_VISIBLE = (1L << 19), /**< Do not clip the children's content to the parent's boundary*/

    LV_OBJ_FLAG_LAYOUT_1        = (1L << 23), /**< Custom flag, free to use by layouts*/
    LV_OBJ_FLAG_LAYOUT_2        = (1L << 24), /**< Custom flag, free to use by layouts*/

    LV_OBJ_FLAG_WIDGET_1        = (1L << 25), /**< Custom flag, free to use by widget*/
    LV_OBJ_FLAG_WIDGET_2        = (1L << 26), /**< Custom flag, free to use by widget*/
    LV_OBJ_FLAG_USER_1          = (1L << 27), /**< Custom flag, free to use by user*/
    LV_OBJ_FLAG_USER_2          = (1L << 28), /**< Custom flag, free to use by user*/
    LV_OBJ_FLAG_USER_3          = (1L << 29), /**< Custom flag, free to use by user*/
    LV_OBJ_FLAG_USER_4          = (1L << 30), /**< Custom flag, free to use by user*/
};
```

### 图层

默认情况下，LVGL绘制的部件图层按照先后顺序进行绘制。

<img src=".\img\image-20250604175140946.png" alt="image-20250604175140946" style="zoom:50%;" />

**相关API函数**

- 移动到前景：lv_obj_move_foreground(widget);
- 移动到背景：lv_obj_move_background(widget);
- 设置图层：lv_obj_move_to_index(widget, idx);  				/* 0: 背景 child_num - 1: 前景 */
- 交换图层：lv_obj_swap(widget1, widget2) ；
- 重新设置父子关系：lv_obj_set_parent(widget, new_parent);

### 事件

LVGL中，当发生用户感兴趣的事情时，可以触发回调事件，以执行相关的操作。

**相关API函数**

- 添加事件：lv_obj_add_event_cb(obj, event_cb, event_code, user_data);    /* 其中event_cb为事件回调函数，event_code为事件类型 */
- 删除事件：lv_obj_remove_event_cb(obj, event_cb);

**事件类型**

每个事件类型都有各自具体的触发方式，如对象被按下、删除对象等。

- 输入设备事件 Input device events
- 绘图事件 Drawing events
- 其他事件 Other events
- 特殊事件 Special events
- 自定义事件 Custom events

```c
/**
 * Type of event being sent to the object.
 */

typedef enum {
    LV_EVENT_ALL = 0,

    /** Input device events*/
    LV_EVENT_PRESSED,             /**< The object has been pressed*/
    LV_EVENT_PRESSING,            /**< The object is being pressed (called continuously while pressing)*/
    LV_EVENT_PRESS_LOST,          /**< The object is still being pressed but slid cursor/finger off of the object */
    LV_EVENT_SHORT_CLICKED,       /**< The object was pressed for a short period of time, then released it. Not called if scrolled.*/
    LV_EVENT_LONG_PRESSED,        /**< Object has been pressed for at least `long_press_time`.  Not called if scrolled.*/
    LV_EVENT_LONG_PRESSED_REPEAT, /**< Called after `long_press_time` in every `long_press_repeat_time` ms.  Not called if scrolled.*/
    LV_EVENT_CLICKED,             /**< Called on release if not scrolled (regardless to long press)*/
    LV_EVENT_RELEASED,            /**< Called in every cases when the object has been released*/
    LV_EVENT_SCROLL_BEGIN,        /**< Scrolling begins*/
    LV_EVENT_SCROLL_END,          /**< Scrolling ends*/
    LV_EVENT_SCROLL,              /**< Scrolling*/
    LV_EVENT_GESTURE,             /**< A gesture is detected. Get the gesture with `lv_indev_get_gesture_dir(lv_indev_get_act());` */
    LV_EVENT_KEY,                 /**< A key is sent to the object. Get the key with `lv_indev_get_key(lv_indev_get_act());`*/
    LV_EVENT_FOCUSED,             /**< The object is focused*/
    LV_EVENT_DEFOCUSED,           /**< The object is defocused*/
    LV_EVENT_LEAVE,               /**< The object is defocused but still selected*/
    LV_EVENT_HIT_TEST,            /**< Perform advanced hit-testing*/

    /** Drawing events*/
    LV_EVENT_COVER_CHECK,        /**< Check if the object fully covers an area. The event parameter is `lv_cover_check_info_t *`.*/
    LV_EVENT_REFR_EXT_DRAW_SIZE, /**< Get the required extra draw area around the object (e.g. for shadow). The event parameter is `lv_coord_t *` to store the size.*/
    LV_EVENT_DRAW_MAIN_BEGIN,    /**< Starting the main drawing phase*/
    LV_EVENT_DRAW_MAIN,          /**< Perform the main drawing*/
    LV_EVENT_DRAW_MAIN_END,      /**< Finishing the main drawing phase*/
    LV_EVENT_DRAW_POST_BEGIN,    /**< Starting the post draw phase (when all children are drawn)*/
    LV_EVENT_DRAW_POST,          /**< Perform the post draw phase (when all children are drawn)*/
    LV_EVENT_DRAW_POST_END,      /**< Finishing the post draw phase (when all children are drawn)*/
    LV_EVENT_DRAW_PART_BEGIN,    /**< Starting to draw a part. The event parameter is `lv_obj_draw_dsc_t *`. */
    LV_EVENT_DRAW_PART_END,      /**< Finishing to draw a part. The event parameter is `lv_obj_draw_dsc_t *`. */

    /** Special events*/
    LV_EVENT_VALUE_CHANGED,       /**< The object's value has changed (i.e. slider moved)*/
    LV_EVENT_INSERT,              /**< A text is inserted to the object. The event data is `char *` being inserted.*/
    LV_EVENT_REFRESH,             /**< Notify the object to refresh something on it (for the user)*/
    LV_EVENT_READY,               /**< A process has finished*/
    LV_EVENT_CANCEL,              /**< A process has been cancelled */

    /** Other events*/
    LV_EVENT_DELETE,              /**< Object is being deleted*/
    LV_EVENT_CHILD_CHANGED,       /**< Child was removed, added, or its size, position were changed */
    LV_EVENT_CHILD_CREATED,       /**< Child was created, always bubbles up to all parents*/
    LV_EVENT_CHILD_DELETED,       /**< Child was deleted, always bubbles up to all parents*/
    LV_EVENT_SCREEN_UNLOAD_START, /**< A screen unload started, fired immediately when scr_load is called*/
    LV_EVENT_SCREEN_LOAD_START,   /**< A screen load started, fired when the screen change delay is expired*/
    LV_EVENT_SCREEN_LOADED,       /**< A screen was loaded*/
    LV_EVENT_SCREEN_UNLOADED,     /**< A screen was unloaded*/
    LV_EVENT_SIZE_CHANGED,        /**< Object coordinates/size have changed*/
    LV_EVENT_STYLE_CHANGED,       /**< Object's style has changed*/
    LV_EVENT_LAYOUT_CHANGED,      /**< The children position has changed due to a layout recalculation*/
    LV_EVENT_GET_SELF_SIZE,       /**< Get the internal size of a widget*/

    _LV_EVENT_LAST,               /** Number of default events*/


    LV_EVENT_PREPROCESS = 0x80,   /** This is a flag that can be set with an event so it's processed
                                      before the class default event processing */
} lv_event_code_t;
```

注

1. 所有对象（例如按钮/标签/滑块等），无论其类型如何，都会接收 Input device、Drawing 和 Other 事件
2. 特殊事件只存在与某些部件类型
3. 自定义事件由用户添加，LVGL不会产生此事件

**不同的事件类型共用一个事件回调函数**

```c
static void event_cb( lv_event_t *e )
{
	lv_event_code_t   code = lv_event_get_code(e);			  /* 第一步：获取事件类型 */
	if ( code == LV_EVENT_CLICKED )							  /* 第二步：判断事件类型 */
	{	
        printf(“事件类型: 按下后释放\r\n”);	 		 			/* 第三步：执行相应操作 */
	}
	else if ( code == LV_EVENT_LONG_PRESSED)
	{
        printf(“事件类型:按下（长按）\r\n”);	
	}
}
```

**不同的部件共用一个事件回调函数**

```c
static void event_cb( lv_event_t *e )
{
	lv_obj_t   *target = lv_event_get_target(e); 		/* 第一步：获取触发事件的部件 */
	if ( target == parent_obj )				 			/* 第二步：判断触发事件的部件 */
	{
        printf(“父对象触发事件 \r\n”);	 		 			/* 第三步：执行相应操作 */
	}
	else if ( target == child_obj )
	{
        printf(“子对象触发事件 \r\n”);	
	}
}
```

**传递用户数据**

```c
void my_btn_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        int *num = e->user_data; 						// 通过此把要传递的数据重新恢复
        LV_LOG_USER("\n BTN: %d", *num + 1);
    }
}

void btn_menu(void)
{
    int i = 3;
    static int num[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    static lv_obj_t *my_btn[8];

    for (i = 0; i < 8; i++)
    {
        my_btn[i] = lv_btn_create(lv_scr_act());
        lv_obj_set_size(my_btn[i], 50, 50);

        lv_obj_align(my_btn[i], LV_ALIGN_DEFAULT, 0, i * 100);
        void *temp = &num[i]; 							// 通过此转换传递地址
        lv_obj_add_event_cb(my_btn[i], my_btn_event, LV_EVENT_ALL, temp);
    }
}
```

# 控件

## 标签部件

在 LVGL 中，标签部件（lv_label）主要用于文本显示，例如标题、提示信息等。

<img src=".\img\image-20250526174603606.png" alt="image-20250526174603606" style="zoom:43%;" />

**组成部分**

- 主体（LV_PART_MAIN）
- 滚动条（LV_PART_SCROLLBAR）
- 选中的文本（LV_PART_SELECTED）

注

1. 默认情况下，文本内容为`TEXT`
2. 默认情况下，标签背景透明度为255（完全不透明）
3. 使用其他字体时，需要在`lv_conf.h`开启对应宏
4. 默认情况下，如果没有限定标签部件大小，那它的大小自动扩展为文本大小

**相关API函数**

- 创建标签部件：lv_obj_t  *label = lv_label_create(parent);
- 设置文本
  - 直接设置文本，存储文本的内存动态分配 ：lv_label_set_text( label, "hallo \n lvgl");
  - 文本不存储在动态内存，而是在指定的缓冲区中（慎用）：lv_label_set_text_static( label,"hallo" );
  - 格式化显示文本，类似printf ：lv_label_set_text_fmt( label, “Value: %d”, 50 ) ;
- 设置文本样式
  - 背景颜色：lv_obj_set_style_bg_color( label, lv_color_hex(0xffe1d4), LV_STATE_DEFAULT );
  - 字体大小：lv_obj_set_style_text_font( label, &lv_font_montserrat_30, LV_STATE_DEFAULT );
  - 文本颜色：lv_obj_set_style_text_color( label, lv_color_hex(0xf7b37b), LV_STATE_DEFAULT );
- 设置个别文本的字体颜色
  1. lv_label_set_recolor( label, true );  			                                   /* 开启重新着色功能 */
  2. lv_label_set_text( label, "hallo #ff0000 lvgl# " );                                     /* 单独设置颜色 */
- 长文本模式：lv_label_set_long_mode(label, LV_LABEL_LONG_...);

```c
enum {
    LV_LABEL_LONG_WRAP,             		/* 默认模式, 如果部件大小已固定，超出的文本将被剪切 */
    LV_LABEL_LONG_DOT,              		/* 将 label 右下角的最后 3 个字符替换为点… */
    LV_LABEL_LONG_SCROLL,           		/* 来回滚动 */
    LV_LABEL_LONG_SCROLL_CIRCULAR,  		/* 循环滚动 类似广告牌 */
    LV_LABEL_LONG_CLIP,             		/* 直接剪切掉部件外面的文本部分 */
};
```

注

1. 设置文本函数，同样可以用来设置图标

**实现阴影效果**

```c
/**
 * @brief  例3 实现标签阴影
 * @param  无
 * @return 无
 */
static void lv_example_label_3(void)
{
    lv_obj_t* label = lv_label_create(lv_scr_act());                                /* 定义并创建标签 */
    lv_label_set_text_fmt(label, "Label can set text like %s", "printf");           /* 设置标签文本 */
    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);                          /* 设置标签文本字体 */
    lv_obj_set_width(label, scr_act_width() / 3);                                   /* 设置标签宽度 */
    lv_obj_align(label, LV_ALIGN_CENTER, scr_act_width() / 3, 0);                   /* 设置标签位置 */
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);         /* 设置标签文本对齐方式 */

    lv_obj_t* label_shadow = lv_label_create(lv_scr_act());                         /* 定义并创建阴影标签 */
    lv_label_set_text(label_shadow, lv_label_get_text(label));                      /* 设置标签文本 */
    lv_obj_set_style_text_font(label_shadow, font, LV_PART_MAIN);                   /* 设置标签文本字体 */
    lv_obj_set_width(label_shadow, scr_act_width() / 3);                            /* 设置标签宽度 */
    lv_obj_set_style_text_opa(label_shadow, LV_OPA_30, LV_PART_MAIN);               /* 设置标签文本透明度 */
    lv_obj_set_style_text_color(label_shadow, lv_color_black(), LV_PART_MAIN);      /* 设置标签文本颜色 */
    lv_obj_set_style_text_align(label_shadow, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);  /* 设置标签文本对齐方式 */
    lv_obj_align_to(label_shadow, label, LV_ALIGN_TOP_LEFT, 3, 3);                  /* 设置标签位置 */
}
```

**实例**

```c
#include "lvgl.h"
#include "my_gui.h"

void my_gui(void)
{
    lv_obj_t *label = lv_label_create(lv_scr_act());

    lv_label_set_text(label, "hello hello \n hello hello hello"); // 使用 \n 进行换行
    // lv_label_set_text_static(label, "hello");                  // 不可进行写操作
    // lv_label_set_text_fmt(label, "hello %d", 5);

    lv_obj_set_size(label, 100, 50);                    // 超出部件区域部分的文本将被裁剪

    lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);   // 会修改文本内容
}
```

## 按钮部件

在LVGL中，按钮部件（lv_btn）与基础对象相比，没有新增任何功能。

<img src=".\img\image-20250526174622164.png" alt="image-20250526174622164" style="zoom:43%;" />

**组成部分**

- 主体（LV_PART_MAIN）

注

1. 按钮部件不自带文本，需要用标签进行文本显示
2. 按钮部件按下后，大小会自动变大

**相关API函数**

- 创建按钮部件：lv_obj_t  *btn = lv_btn_create( parent );
- 设置样式
  - lv_obj_set_size( btn, 100, 50 );                                                                                            /* 大小 */ 
  - lv_obj_set_align( btn, LV_ALIGN_CENTER );                                                                       /* 对齐 */
  - lv_obj_set_style_bg_color( btn, lv_color_hex(0xffe1d4), LV_STATE_PRESSED );          /* 背景颜色 */
- 添加事件
  1. lv_obj_add_flag( btn, LV_OBJ_FLAG_CHECKABLE );					     	    /* 开启状态切换 */*
  2. lv_obj_add_event_cb( btn, event_cb, LV_EVENT_VALUE_CHANGED,NULL );	      /* 添加事件 */

注

1. 须开启状态切换，才会检测LV_EVENT_VALUE_CHANGED
2. 开启状态切换后，默认点击按钮后，会切换成红色

**实例**

<img src=".\img\image-20250528155029033.png" alt="image-20250528155029033" style="zoom:50%;" />

```c
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>

#if (1 != LV_FONT_MONTSERRAT_10 || 1 != LV_FONT_MONTSERRAT_14 || 1 != LV_FONT_MONTSERRAT_20)
    #error Please Make Sure Enable LV_FONT_MONTSERRAT_10 & LV_FONT_MONTSERRAT_14 & LV_FONT_MONTSERRAT_20
#endif

static const lv_font_t* font;       /* 定义字体 */

static lv_obj_t *label_speed;       /* 速度提示文本 */
static lv_obj_t *btn_speed_up;      /* 加速按钮 */
static lv_obj_t *btn_speed_down;    /* 减速按钮 */
static lv_obj_t *btn_stop;          /* 急停按钮 */

static int32_t speed_val = 0;       /* 速度值 */

/* 获取当前活动屏幕的宽高 */
#define scr_act_width() lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())


/**
 * @brief  按钮回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void btn_event_cb(lv_event_t * e)
{
    lv_obj_t *target = lv_event_get_target(e);      /* 获取触发源 */

    if(target == btn_speed_up)                      /* 加速按钮 */
    {
        speed_val += 30;
    }
    else if(target == btn_speed_down)               /* 减速按钮 */
    {
        speed_val -= 30;
    }
    else if(target == btn_stop)                     /* 急停按钮 */
    {
        speed_val = 0;
    }
    lv_label_set_text_fmt(label_speed, "Speed : %d RPM", speed_val);    /* 更新速度值 */
}


/**
 * @brief  速度值提示标签
 * @param  无
 * @return 无
 */
static void lv_example_label(void)
{
    /* 根据活动屏幕宽度选择字体 */
    if (scr_act_width() <= 320)
    {
        font = &lv_font_montserrat_10;
    }
    else if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
    }
    else
    {
        font = &lv_font_montserrat_20;
    }

    label_speed = lv_label_create(lv_scr_act());                                    /* 创建速度显示标签 */
    lv_obj_set_style_text_font(label_speed, font, LV_PART_MAIN);                    /* 设置字体 */
    lv_label_set_text(label_speed, "Speed : 0 RPM");                                /* 设置文本 */
    lv_obj_align(label_speed, LV_ALIGN_CENTER, 0, -scr_act_height() / 3);           /* 设置标签位置 */
}
/**
 * @brief  加速按钮
 * @param  无
 * @return 无
 */
static void lv_example_btn_up(void)
{
    btn_speed_up = lv_btn_create(lv_scr_act());                                     /* 创建加速按钮 */
    lv_obj_set_size(btn_speed_up, scr_act_width() / 4, scr_act_height() / 6);       /* 设置按钮大小 */
    lv_obj_align(btn_speed_up, LV_ALIGN_CENTER, -scr_act_width() / 3, 0);           /* 设置按钮位置 */
    lv_obj_add_event_cb(btn_speed_up, btn_event_cb, LV_EVENT_CLICKED, NULL);        /* 设置按钮事件 */

    lv_obj_t* label = lv_label_create(btn_speed_up);                                /* 创建加速按钮标签 */
    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);                          /* 设置字体 */
    lv_label_set_text(label, "Speed +");                                            /* 设置标签文本 */
    lv_obj_set_align(label,LV_ALIGN_CENTER);                                        /* 设置标签位置 */
}

/**
 * @brief  减速按钮
 * @param  无
 * @return 无
 */
static void lv_example_btn_down(void)
{
    btn_speed_down = lv_btn_create(lv_scr_act());                                   /* 创建加速按钮 */
    lv_obj_set_size(btn_speed_down, scr_act_width() / 4, scr_act_height() / 6);     /* 设置按钮大小 */
    lv_obj_align(btn_speed_down, LV_ALIGN_CENTER, 0, 0);                            /* 设置按钮位置 */
    lv_obj_add_event_cb(btn_speed_down, btn_event_cb, LV_EVENT_CLICKED, NULL);      /* 设置按钮事件 */

    lv_obj_t* label = lv_label_create(btn_speed_down);                              /* 创建减速按钮标签 */
    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);                          /* 设置字体 */
    lv_label_set_text(label, "Speed -");                                            /* 设置标签文本 */
    lv_obj_set_align(label,LV_ALIGN_CENTER);                                        /* 设置标签位置 */
}

/**
 * @brief  急停按钮
 * @param  无
 * @return 无
 */
static void lv_example_btn_stop(void)
{
    btn_stop = lv_btn_create(lv_scr_act());                                         /* 创建急停按钮 */
    lv_obj_set_size(btn_stop, scr_act_width() / 4, scr_act_height() / 6);           /* 设置按钮大小 */
    lv_obj_align(btn_stop, LV_ALIGN_CENTER, scr_act_width() / 3, 0);                /* 设置按钮位置 */
    lv_obj_set_style_bg_color(btn_stop, lv_color_hex(0xef5f60), LV_STATE_DEFAULT);  /* 设置按钮背景颜色（默认） */
    lv_obj_set_style_bg_color(btn_stop, lv_color_hex(0xff0000), LV_STATE_PRESSED);  /* 设置按钮背景颜色（按下） */
    lv_obj_add_event_cb(btn_stop, btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 设置按钮事件 */

    lv_obj_t* label = lv_label_create(btn_stop);                                    /* 创建急停按钮标签 */
    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);                          /* 设置字体 */
    lv_label_set_text(label, "Stop");                                               /* 设置标签文本 */
    lv_obj_set_align(label,LV_ALIGN_CENTER);                                        /* 设置标签位置 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_label();             /* 速度提示标签 */
    lv_example_btn_up();            /* 加速按钮 */
    lv_example_btn_down();          /* 减速按钮 */
    lv_example_btn_stop();          /* 急停按钮 */
}
```

## 按钮矩阵部件

按钮矩阵部件（lv_btnmatrix）可以在不同的行和列中显示多个轻量级按钮。

<img src=".\img\image-20250528212154079.png" alt="image-20250528212154079" style="zoom:40%;" />

**组成部分**

- 主体(LV_PART_MAIN）
- 按钮(LV_PART_ITEMS)

**相关API函数**

- 创建按钮矩阵部件 ：lv_obj_t  *btnm = lv_btnmatrix_create(parent);
- 设置按钮数量、文本
  1. static const char *map[] = { "btn1", "\n", "btn2", "btn3", "" };	 /\* 定义按钮数组，换行使用 \n 最后一个元素必须为空 \*/
  2. lv_btnmatrix_set_map(btnm, map);					            /* 设置按钮 */
- 设置按钮相对宽度：lv_btnmatrix_set_btn_width(btnm, id, width);	 /* 索引 (id) 从0开始，宽度1~7 ( 默认为1 ) */
- 获取按钮索引、文本
  - lv_btnmatrix_get_selected_btn(btnm);					      /* 获取索引 */
  - lv_btnmatrix_get_btn_text(btnm, id);					        /* 获取文本 */
- 设置、清除按钮属性
  - lv_btnmatrix_set_btn_ctrl(btnm, id, LV_BTNMATRIX_CTRL_...);		     /* 设置单个按钮属性 */
  - lv_btnmatrix_clear_btn_ctrl(btnm, id, LV_BTNMATRIX_CTRL_...);		 /* 清除单个按钮属性 */
  - lv_btnmatrix_set_btn_ctrl_all(btnm, LV_BTNMATRIX_CTRL_...);	 	   /* 设置所有按钮属性 */

```c
enum {
    LV_BTNMATRIX_CTRL_HIDDEN, 						/* 隐藏 */
    LV_BTNMATRIX_CTRL_DISABLED,						/* 失能 */
    LV_BTNMATRIX_CTRL_CHECKABLE, 					/* 允许状态切换 */
    LV_BTNMATRIX_CTRL_RECOLOR, 						/* 允许文本重新着色 */
};
```

- 设置单次选中属性：lv_btnmatrix_set_one_checked(btnm, true);		        /* 注意：需要先设置允许状态切换属性 */

注

1. 设置按钮属性时，可以通过`|`来实现多个属性设置
2. 关闭文本重着色时，需要还原文本内容

**实例**

<img src=".\img\image-20250530114324185.png" alt="image-20250530114324185" style="zoom:40%;" />

```c
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>


/* 获取当前活动屏幕的宽高 */
#define scr_act_width()  lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

/* 按钮矩阵数组 */
static const char *num_map[] = { "1", "2", "3", "\n",
                                 "4", "5", "6", "\n",
                                 "7", "8", "9", "\n",
                                 "#", "0", "%", "" };

static uint16_t zoom_val;                           /* 图片缩放系数 */
static lv_point_t points[] = {{0,0},{0,200}};       /* 线条坐标点数组 */

static const lv_font_t *font;                       /* 定义字体 */
static lv_obj_t *label_input;                       /* 输入框标签 */
static lv_obj_t *obj_input;                         /* 输入框背景 */

LV_IMG_DECLARE(img_user);                           /* 声明图片 */

/**
 * @brief  按钮矩阵事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void btnm_event_cb(lv_event_t* e)
{
    uint8_t id;

    lv_event_code_t code = lv_event_get_code(e);                                /* 获取事件类型 */
    lv_obj_t *target = lv_event_get_target(e);                                  /* 获取触发源 */

    if (code == LV_EVENT_VALUE_CHANGED)
    {
        id = lv_btnmatrix_get_selected_btn(target);                             /* 获取按键索引 */
        lv_label_set_text(label_input, lv_btnmatrix_get_btn_text(target, id));  /* 更新输入框标签文本 */
        lv_obj_align_to(label_input, obj_input, LV_ALIGN_CENTER, 0, 0);         /* 设置标签位置 */
    }
}

/**
 * @brief  密码输入界面
 * @param  无
 * @return 无
 */
static void lv_example_btnmatrix(void)
{

    /* 根据屏幕宽度选择字体和图片缩放系数 */
    if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
        zoom_val = 128;
    }
    else
    {
        font = &lv_font_montserrat_30;
        zoom_val = 256;
    }

    /* 图片显示 */
    lv_obj_t *img = lv_img_create(lv_scr_act());                                                /* 创建图片部件 */
    lv_img_set_src(img, &img_user);                                                             /* 设置图片源 */
    lv_img_set_zoom(img, zoom_val);                                                             /* 设置图片缩放 */
    lv_obj_align(img, LV_ALIGN_CENTER, -scr_act_width()/4, -scr_act_height()/7);                /* 设置位置 */
    lv_obj_set_style_img_recolor(img, lv_color_hex(0xf2f2f2),0);                                /* 设置重新着色 */
    lv_obj_set_style_img_recolor_opa(img,100,0);                                                /* 设置着色透明度 */

    /* 用户标签 */
    lv_obj_t *label_user = lv_label_create(lv_scr_act());                                       /* 创建标签 */
    lv_label_set_text(label_user, "USER");                                                      /* 设置文本 */
    lv_obj_set_style_text_font(label_user, font, LV_PART_MAIN);                                 /* 设置字体 */
    lv_obj_set_style_text_align(label_user, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);                /* 设置文本居中 */
    lv_obj_align_to(label_user, img, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);                           /* 设置位置 */

    /* 输入框背景 */
    obj_input = lv_obj_create(lv_scr_act());                                                    /* 创建基础对象 */
    lv_obj_set_size(obj_input, scr_act_width()/4, scr_act_height()/12);                         /* 设置大小 */
    lv_obj_align_to(obj_input, label_user, LV_ALIGN_OUT_BOTTOM_MID, 0, scr_act_height()/20);    /* 设置位置 */
    lv_obj_set_style_bg_color(obj_input, lv_color_hex(0xcccccc),0);                             /* 设置背景颜色 */
    lv_obj_set_style_bg_opa(obj_input, 150,0);                                                  /* 设置透明度 */
    lv_obj_set_style_border_width(obj_input, 0 , 0);                                            /* 去除边框 */
    lv_obj_set_style_radius(obj_input, 20, 0);                                                  /* 设置圆角 */
    lv_obj_remove_style(obj_input, NULL, LV_PART_SCROLLBAR);                                    /* 移除滚动条 */

    /* 输入框文本标签 */
    label_input = lv_label_create(lv_scr_act());                                                /* 创建标签 */
    lv_label_set_text(label_input, "");                                                         /* 设置文本 */
    lv_obj_set_style_text_font(label_input, font, LV_PART_MAIN);                                /* 设置字体 */
    lv_obj_set_style_text_align(label_input, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);               /* 设置文本居中 */
    lv_obj_align_to(label_input, obj_input, LV_ALIGN_CENTER, 0, 0);                             /* 设置位置 */

    /* 分隔线 */
    lv_obj_t *line = lv_line_create(lv_scr_act());                                              /* 创建线条 */
    lv_line_set_points(line, points, 2);                                                        /* 设置线条坐标点 */
    lv_obj_align(line, LV_ALIGN_CENTER, 0, 0);                                                  /* 设置位置 */
    lv_obj_set_style_line_color(line, lv_color_hex(0xcdcdcd),0);                                /* 设置线条颜色 */

    /* 按钮矩阵（创建） */
    lv_obj_t *btnm = lv_btnmatrix_create(lv_scr_act());                                         /* 创建按钮矩阵 */
    lv_obj_set_size(btnm, scr_act_width()* 2/5, scr_act_width()* 2/5);                          /* 设置大小 */
    lv_btnmatrix_set_map(btnm, num_map);                                                        /* 设置按钮 */
    lv_obj_align(btnm, LV_ALIGN_RIGHT_MID, -scr_act_width()/16, 0);                             /* 设置位置 */
    lv_obj_set_style_text_font(btnm, font, LV_PART_ITEMS);                                      /* 设置字体 */

    /* 按钮矩阵（优化界面） */
    lv_obj_set_style_border_width(btnm, 0, LV_PART_MAIN);                                       /* 去除主体边框 */
    lv_obj_set_style_bg_opa(btnm, 0, LV_PART_MAIN);                                             /* 设置主体背景透明度 */
    lv_obj_set_style_bg_opa(btnm, 0, LV_PART_ITEMS);                                            /* 设置按钮背景透明度 */
    lv_obj_set_style_shadow_width(btnm, 0, LV_PART_ITEMS);                                      /* 去除按钮阴影 */
    lv_obj_add_event_cb(btnm, btnm_event_cb, LV_EVENT_VALUE_CHANGED, NULL);                     /* 设置按钮矩阵回调 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_btnmatrix();
}
```

## 开关部件

开关部件（lv_switch）常用于控制某个功能的开启和关闭，它可以直接显示被控对象的状态。

<img src=".\img\image-20250526174643481.png" alt="image-20250526174643481" style="zoom:40%;" />

**组成部分**

- 主体（LV_PART_MAIN）
- 手柄（LV_PART_KNOB）
- 指示器（LV_PART_INDICATOR）

注

1. 针对于指示器而言，在默认条件下设置背景颜色无用，因为此时指示器被主体遮挡，而点击开关后状态就不是默认状态了，所以应该在开启状态`LV_STATE_CHECKED `下设置指示器颜色
2. 大小比例最好是2：1

**相关API函数**

- 创建开关部件：lv_obj_t  *switch1 = lv_switch_create( parent );
- 添加开关状态：lv_obj_add_state(switch1, LV_STATE_CHECKED | LV_STATE_DISABLED);       /* 打开并不可修改 */
- 清除开关状态：lv_obj_clear_state(switch1, LV_STATE_CHECKED | LV_STATE_DISABLED);     /* 清除开关的状态及不可修改 */ 
- 获取（判断）开关状态：lv_obj_has_state(switch1, LV_STATE_CHECKED) 				/* 返回值：bool类型，开 :1；关: 0 */

**实例**

<img src=".\img\image-20250528154944058.png" alt="image-20250528154944058" style="zoom:50%;" />

```c
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>


/* 获取当前活动屏幕的宽高 */
#define scr_act_width() lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

static lv_obj_t *switch_cool;            /* 制冷模式开关 */
static lv_obj_t *switch_heat;            /* 制暖模式开关 */
static lv_obj_t *switch_dry;             /* 干燥模式开关 */
static const lv_font_t *font;            /* 定义字体 */

/**
 * @brief  回调事件
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void switch_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);                      /* 获取触发源 */

    if(target == switch_cool)                                       /* 制冷开关触发 */
    {
        if(lv_obj_has_state(switch_cool, LV_STATE_CHECKED))         /* 判断开关状态 */
        {
            lv_obj_clear_state(switch_heat, LV_STATE_CHECKED);      /* 制冷模式已打开，关闭制暖模式 */
        }
    }
    else if(target == switch_heat)                                  /* 制暖开关触发 */
    {
        if(lv_obj_has_state(switch_heat, LV_STATE_CHECKED))         /* 判断开关状态 */
        {
            lv_obj_clear_state(switch_cool, LV_STATE_CHECKED);      /* 制暖模式已打开，关闭制冷模式 */
        }
    }

}

/**
 * @brief  功能文本标签
 * @param  无
 * @return 无
 */
static void lv_example_label(void)
{
    /* 根据活动屏幕宽度选择字体 */
    if (scr_act_width() <= 320)
    {
        font = &lv_font_montserrat_10;
    }
    else if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
    }
    else
    {
        font = &lv_font_montserrat_20;
    }
    lv_obj_t *label = lv_label_create(lv_scr_act());                            /* 创建标签 */
    lv_label_set_text(label, "Control  Center");                                /* 设置文本内容 */
    lv_obj_set_style_text_font(label, font, LV_STATE_DEFAULT);                  /* 设置字体 */
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -scr_act_height() / 3 );            /* 设置位置 */
}

/**
 * @brief  制冷功能开关
 * @param  无
 * @return 无
 */
static void lv_example_switch1(void)
{
    /* 制冷模式基础对象（矩形背景） */
    lv_obj_t *obj_cool = lv_obj_create(lv_scr_act());                               /* 创建基础对象 */
    lv_obj_set_size(obj_cool,scr_act_height() / 3, scr_act_height() / 3 );          /* 设置大小 */
    lv_obj_align(obj_cool, LV_ALIGN_CENTER, -scr_act_width() / 4, 0 );              /* 设置位置 */

    /* 制冷模式开关标签 */
    lv_obj_t *label_cool = lv_label_create(obj_cool);                               /* 创建标签 */
    lv_label_set_text(label_cool, "Cool");                                          /* 设置文本内容 */
    lv_obj_set_style_text_font(label_cool, font, LV_STATE_DEFAULT);                 /* 设置字体 */
    lv_obj_align(label_cool, LV_ALIGN_CENTER, 0, -scr_act_height() / 16 );          /* 设置位置 */

    /* 制冷模式开关 */
    switch_cool = lv_switch_create(obj_cool);                                       /* 创建开关 */
    lv_obj_set_size(switch_cool,scr_act_height() / 6, scr_act_height() / 12 );      /* 设置大小 */
    lv_obj_align(switch_cool, LV_ALIGN_CENTER, 0, scr_act_height() / 16 );          /* 设置位置 */
    lv_obj_add_event_cb(switch_cool, switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);/* 添加事件 */
}

/**
 * @brief  制暖功能开关
 * @param  无
 * @return 无
 */
static void lv_example_switch2(void)
{
    /* 制暖模式基础对象（矩形背景） */
    lv_obj_t *obj_heat = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj_heat,scr_act_height() / 3, scr_act_height() / 3 );
    lv_obj_align(obj_heat, LV_ALIGN_CENTER, 0, 0 );

    /* 制暖模式开关标签 */
    lv_obj_t *label_heat = lv_label_create(obj_heat);
    lv_label_set_text(label_heat, "Heat");
    lv_obj_set_style_text_font(label_heat, font, LV_STATE_DEFAULT);
    lv_obj_align(label_heat, LV_ALIGN_CENTER, 0, -scr_act_height() / 16 );

    /* 制暖模式开关 */
    switch_heat = lv_switch_create(obj_heat);
    lv_obj_set_size(switch_heat,scr_act_height() / 6, scr_act_height() / 12 );
    lv_obj_align(switch_heat, LV_ALIGN_CENTER, 0, scr_act_height() / 16 );
    lv_obj_add_event_cb(switch_heat, switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
}

/**
 * @brief  干燥功能开关
 * @param  无
 * @return 无
 */
static void lv_example_switch3(void)
{
    /* 干燥模式基础对象（矩形背景） */
    lv_obj_t *obj_dry = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj_dry,scr_act_height() / 3, scr_act_height() / 3 );
    lv_obj_align(obj_dry, LV_ALIGN_CENTER, scr_act_width() / 4, 0 );

    /* 干燥模式开关标签 */
    lv_obj_t *label_dry = lv_label_create(obj_dry);
    lv_label_set_text(label_dry, "Dry");
    lv_obj_set_style_text_font(label_dry, font, LV_STATE_DEFAULT);
    lv_obj_align(label_dry, LV_ALIGN_CENTER, 0, -scr_act_height() / 16 );

    /* 干燥模式开关 */
    switch_dry = lv_switch_create(obj_dry);
    lv_obj_set_size(switch_dry,scr_act_height() / 6, scr_act_height() / 12 );
    lv_obj_align(switch_dry, LV_ALIGN_CENTER, 0, scr_act_height() / 16 );
    lv_obj_add_state(switch_dry, LV_STATE_CHECKED|LV_STATE_DISABLED);
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_label();         /* 功能标题 */
    lv_example_switch1();       /* 制冷模式开关 */
    lv_example_switch2();       /* 制暖模式开关 */
    lv_example_switch3();       /* 干燥模式开关 */
}
```

## LED部件

LED部件（lv_led）常用于指示控制对象的状态。

<img src=".\img\image-20250528205203869.png" alt="image-20250528205203869" style="zoom:40%;" />

注

1. 更高版本LVGL中，LED打开样式为第三种

**组成部分**

- 主体（LV_PART_MAIN）

**相关API函数**

- 创建LED部件 ：lv_obj_t  *led = lv_led_create( parent );
- 设置LED颜色、亮度
  - lv_led_set_color( led, lv_color_hex(0xff0000) );         /* 设置LED颜色 */
  - lv_led_set_brightness( led, 0 );                                    /* 设置LED亮度，范围0~255，值越大越亮 */
- 设置LED状态
  - lv_led_on( led );            /* 打开LED（设置亮度为255）*/
  - lv_led_off( led );            /* 关闭LED */
  - lv_led_toggle( led );      /* 翻转LED状态 */

注

1. LED创建时默认打开，即亮度为255

**实例**

<img src=".\img\image-20250529171201874.png" alt="image-20250529171201874" style="zoom:50%;" />

```c
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>

/* 获取当前活动屏幕的宽高 */
#define scr_act_width() lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

static const lv_font_t *font;                   /* 定义字体 */
static lv_obj_t *obj;                           /* 基础对象 */

/**
 * @brief  LED事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void led_event_cb(lv_event_t* e)
{
    lv_obj_t* led = lv_event_get_target(e);     /* 获取触发源 */
    lv_led_toggle(led);                         /* 翻转LED状态 */
}

/**
 * @brief  LED1
 * @param  无
 * @return 无
 */
static void lv_example_led_1(void)
{
    /* 根据活动屏幕宽度选择字体 */
    if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
    }
    else
    {
        font = &lv_font_montserrat_20;
    }

    /* 创建基础对象作为背景 */
    obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj, scr_act_width() * 5 /6 , scr_act_height() * 3 /5);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0 , 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xefefef), LV_STATE_DEFAULT);

    lv_obj_t* led = lv_led_create(obj);                                                     /* 创建LED */
    lv_obj_set_size(led, scr_act_height() /5 , scr_act_height() /5);                        /* 设置LED大小 */
    lv_obj_align(led, LV_ALIGN_CENTER, -scr_act_width() * 4/ 15, -scr_act_height() /15);    /* 设置LED位置 */
    lv_led_off(led);                                                                        /* 关闭LED */
    lv_obj_add_event_cb(led, led_event_cb, LV_EVENT_CLICKED, NULL);                         /* 设置LED事件回调 */

    lv_obj_t *label = lv_label_create(lv_scr_act());                                        /* 创建LED功能标签 */
    lv_label_set_text(label, "ROOM 1");                                                     /* 设置文本 */
    lv_obj_set_style_text_font(label, font, LV_STATE_DEFAULT);                              /* 设置字体 */
    lv_obj_align_to(label, led, LV_ALIGN_OUT_BOTTOM_MID, 0, scr_act_height() /15 );         /* 设置位置 */
}

/**
 * @brief  LED2
 * @param  无
 * @return 无
 */
static void lv_example_led_2(void)
{
    lv_obj_t* led = lv_led_create(obj);                                                     /* 创建LED */
    lv_obj_set_size(led, scr_act_height() /5 , scr_act_height() /5);                        /* 设置LED大小 */
    lv_obj_align(led, LV_ALIGN_CENTER, 0, -scr_act_height() /15);                           /* 设置LED位置 */
    lv_led_set_color(led, lv_color_hex(0xff0000));                                          /* 设置LED颜色 */
    lv_led_on(led);                                                                         /* 打开LED */
    lv_obj_add_event_cb(led, led_event_cb, LV_EVENT_CLICKED, NULL);                         /* 设置LED事件回调 */

    lv_obj_t *label = lv_label_create(lv_scr_act());                                        /* 创建LED功能标签 */
    lv_label_set_text(label, "ROOM 2");                                                     /* 设置文本 */
    lv_obj_set_style_text_font(label, font, LV_STATE_DEFAULT);                              /* 设置字体 */
    lv_obj_align_to(label, led, LV_ALIGN_OUT_BOTTOM_MID, 0, scr_act_height() /15 );         /* 设置位置 */
}

/**
 * @brief  LED3
 * @param  无
 * @return 无
 */
static void lv_example_led_3(void)
{
    lv_obj_t* led = lv_led_create(obj);                                                     /* 创建LED */
    lv_obj_set_size(led, scr_act_height() /5 , scr_act_height() /5);                        /* 设置LED大小 */
    lv_obj_align(led, LV_ALIGN_CENTER, scr_act_width() * 4/ 15, -scr_act_height() /15);     /* 设置LED位置 */
    lv_led_set_color(led, lv_color_hex(0x2fc827));                                          /* 设置LED颜色 */
    lv_led_off(led);                                                                        /* 关闭LED */
    lv_obj_add_event_cb(led, led_event_cb, LV_EVENT_CLICKED, NULL);                         /* 设置LED事件回调 */

    lv_obj_t *label = lv_label_create(lv_scr_act());                                        /* 创建LED功能标签 */
    lv_label_set_text(label, "ROOM 3");                                                     /* 设置文本 */
    lv_obj_set_style_text_font(label, font, LV_STATE_DEFAULT);                              /* 设置字体 */
    lv_obj_align_to(label, led, LV_ALIGN_OUT_BOTTOM_MID, 0, scr_act_height() /15 );         /* 设置位置 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_led_1();         /* LED1 */
    lv_example_led_2();         /* LED2 */
    lv_example_led_3();         /* LED3 */
}
```

## 复选框部件

复选框部件（lv_checkbox）常用于选择某个内容的开启和关闭，可以理解为自带标签的开关。

<img src=".\img\image-20250526174712697.png" alt="image-20250526174712697" style="zoom:43%;" />

**组成部分**

- 主体（LV_PART_MAIN）：包含标签
- 勾选框（LV_PART_INDICATOR）

**相关API函数**

- 创建复选框部件：lv_obj_t  *checkbox = lv_checkbox_create (parent );
- 文本设置
  - lv_checkbox_set_text( checkbox, "remember the password" );                                                      /* 设置文本内容*/
  - lv_obj_set_style_pad_column( checkbox, 20, LV_STATE_DEFAULT );                                              /* 设置文本和勾选框的间距 */
- 添加复选框状态：lv_obj_add_state(checkbox, LV_STATE_CHECKED | LV_STATE_DISABLED);            /* 添加状态：默认选中且不可修改 */
- 清除复选框状态：lv_obj_clear_state(checkbox, LV_STATE_CHECKED | LV_STATE_DISABLED);          /* 清除复选框的状态 */ 
- 获取（判断）复选框状态：lv_obj_has_state(checkbox, LV_STATE_CHECKED);                                     /* 返回值：1，选中； 0，非选中 */

**实例**

<img src=".\img\image-20250528154743443.png" alt="image-20250528154743443" style="zoom:50%;" />

```c
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>

/* 获取当前活动屏幕的宽高 */
#define scr_act_width() lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

static int16_t aggregate = 0;                   /* 定义总价格 */

static const lv_font_t *font;                   /* 定义字体 */

static lv_obj_t *label_menu;                    /* 菜单标题标签 */
static lv_obj_t *label_aggregate;               /* 总价格标签 */

/* 定义复选框 */
static lv_obj_t *checkbox1;
static lv_obj_t *checkbox2;
static lv_obj_t *checkbox3;
static lv_obj_t *checkbox4;

/**
 * @brief  回调事件
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void checkbox_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);                              /* 获取触发源 */

    if(target == checkbox1)                                                 /* 复选框1触发 */
    {
        lv_obj_has_state(checkbox1, LV_STATE_CHECKED) ? (aggregate += 19) : (aggregate -= 19);
    }
    else if(target == checkbox2)                                            /* 复选框2触发 */
    {
        lv_obj_has_state(checkbox2, LV_STATE_CHECKED) ? (aggregate += 29) : (aggregate -= 29);
    }

    lv_label_set_text_fmt(label_aggregate, "Aggregate  :  $%d", aggregate); /* 更新总价格标签 */
}

/**
 * @brief  标题、总价格标签
 * @param  无
 * @return 无
 */
static void lv_example_label(void)
{
    /* 根据活动屏幕宽度选择字体 */
    if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
    }
    else
    {
        font = &lv_font_montserrat_20;
    }
    
    /* 菜单标题标签 */
    label_menu = lv_label_create(lv_scr_act());
    lv_label_set_text(label_menu, "MENU");
    lv_obj_set_style_text_font(label_menu, font, LV_STATE_DEFAULT);
    lv_obj_align(label_menu, LV_ALIGN_CENTER, 0, -scr_act_height() * 2 / 5 );

    /* 总价格标签 */
    label_aggregate = lv_label_create(lv_scr_act());
    lv_label_set_text(label_aggregate, "Aggregate  :  $0");
    lv_obj_set_style_text_font(label_aggregate, font, LV_STATE_DEFAULT);
    lv_obj_align(label_aggregate, LV_ALIGN_CENTER, 0, scr_act_height() * 2 / 5 );
}

/**
 * @brief  菜品复选框
 * @param  无
 * @return 无
 */
static void lv_example_checkbox(void)
{
    /* 创建基础对象作为背景 */
    lv_obj_t *obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj, scr_act_width() * 4 /5 , scr_act_height() * 3 /5);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0 , 0);

    /* 菜品1复选框 */
    checkbox1 = lv_checkbox_create(obj);
    lv_checkbox_set_text(checkbox1, "Roast chicken     $19");
    lv_obj_set_style_text_font(checkbox1, font, LV_STATE_DEFAULT);
    lv_obj_align(checkbox1, LV_ALIGN_LEFT_MID, 0, -scr_act_height() / 5 );
    lv_obj_add_event_cb(checkbox1, checkbox_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    /* 菜品2复选框 */
    checkbox2 = lv_checkbox_create(obj);
    lv_checkbox_set_text(checkbox2, "Roast duck     $29");
    lv_obj_set_style_text_font(checkbox2, font, LV_STATE_DEFAULT);
    lv_obj_align_to(checkbox2, checkbox1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 16);
    lv_obj_add_event_cb(checkbox2, checkbox_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    /* 菜品3复选框 */
    checkbox3 = lv_checkbox_create(obj);
    lv_checkbox_set_text(checkbox3, "Roast fish     $39");
    lv_obj_set_style_text_font(checkbox3, font, LV_STATE_DEFAULT);
    lv_obj_align_to(checkbox3, checkbox2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 16);
    lv_obj_add_state(checkbox3, LV_STATE_DISABLED);

    /* 菜品4复选框 */
    checkbox4 = lv_checkbox_create(obj);
    lv_checkbox_set_text(checkbox4, "Roast lamb     $69");
    lv_obj_set_style_text_font(checkbox4, font, LV_STATE_DEFAULT);
    lv_obj_align_to(checkbox4, checkbox3, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 16);
    lv_obj_add_state(checkbox4, LV_STATE_DISABLED);
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_label();     /* 菜单标题、总价标签 */
    lv_example_checkbox();  /* 菜品复选框 */
}
```

## 列表部件

列表部件（lv_list）常用于多选一的场景，默认会展现多个选项。

<img src=".\img\image-20250528205455525.png" alt="image-20250528205455525" style="zoom:50%;" />

**组成部分**

- 主体(LV_PART_MAIN）
- 滚动条(LV_PART_SCROLLBAR)

**相关API函数**

- 创建列表部件：lv_obj_t  *list = lv_list_create( parent );
- 添加列表文本：lv_list_add_text( list, “Settings" );
- 添加列表按钮：lv_obj_t *btn= lv_list_add_btn( list, LV_SYMBOL_WIFI, “WLAN"); 
- 获取列表按钮文本：lv_list_get_btn_text( list, list_btn );

<img src=".\img\image-20250529172255684.png" alt="image-20250529172255684" style="zoom:50%;" />

**实例**

<img src=".\img\image-20250529173501741.png" alt="image-20250529173501741" style="zoom:50%;" />

```c
 #include "LVGL/GUI_APP/lv_mainstart.h"
 #include "lvgl.h"
 #include <stdio.h>
 
 
 /* 获取当前活动屏幕的宽高 */
 #define scr_act_width() lv_obj_get_width(lv_scr_act())
 #define scr_act_height() lv_obj_get_height(lv_scr_act())
 
 static lv_obj_t *list;              /* 定义列表 */
 static lv_obj_t *list_label;        /* 定义标签 */
 static const lv_font_t *font;       /* 定义字体 */
 
 /**
  * @brief  列表按钮事件回调
  * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
  * @return 无
  */
 static void list_btn_event_cb(lv_event_t *e)
 {
     lv_obj_t *list_btn = lv_event_get_target(e);                                   /* 获取触发源 */
 
     lv_label_set_text(list_label, lv_list_get_btn_text(list, list_btn));           /* 获取按钮文本并显示 */
     
     lv_obj_add_state(list_btn, LV_STATE_FOCUS_KEY);                                /* 添加状态（聚焦） */
 }
 
 /**
  * @brief  例
  * @param  无
  * @return 无
  */
 static void lv_example_list(void)
 {
     /* 根据屏幕大小设置字体 */
     if (scr_act_width() <= 320)
     {
         font = &lv_font_montserrat_14;
     }
     else if (scr_act_width() <= 480)
     {
         font = &lv_font_montserrat_16;
     }
     else
     {
         font = &lv_font_montserrat_18;
     }
 
     /* 创建左侧矩形背景 */
     lv_obj_t* obj_left = lv_obj_create(lv_scr_act());                               /* 创建一个基础对象 */
     lv_obj_set_width(obj_left, scr_act_width() * 0.7);                              /* 设置宽度 */
     lv_obj_set_height(obj_left, scr_act_height() * 0.9);                            /* 设置高度 */
     lv_obj_align(obj_left, LV_ALIGN_LEFT_MID, 5, 0);                                /* 设置位置 */
     lv_obj_update_layout(obj_left);                                                 /* 手动更新物体的参数 防止后续使用宽高不准确 */
 
     /* 创建右侧矩形背景 */
     lv_obj_t* obj_right = lv_obj_create(lv_scr_act());                              /* 创建一个基础对象 */
     lv_obj_set_width(obj_right, scr_act_width() - lv_obj_get_width(obj_left) - 15); /* 设置宽度 */
     lv_obj_set_height(obj_right, lv_obj_get_height(obj_left));                      /* 设置高度 */
     lv_obj_align_to(obj_right, obj_left, LV_ALIGN_OUT_RIGHT_MID, 5, 0);             /* 设置位置 */
     lv_obj_update_layout(obj_right);                                                /* 手动更新物体的参数 */
 
     /* 显示当前选项的文本内容 */
     list_label = lv_label_create(obj_right);                                        /* 创建标签 */
     lv_obj_set_width(list_label, lv_obj_get_width(obj_right) - 13);                 /* 设置标签的宽度 */
     lv_obj_align(list_label, LV_ALIGN_TOP_MID, 0, 5);                               /* 设置标签位置 */
     lv_obj_update_layout(list_label);                                               /* 手动更新标签的参数 */
     lv_obj_set_style_text_align(list_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);    /* 设置标签文本对齐方式 */
     lv_label_set_text(list_label, "New");                                           /* 设置标签文本 */
     lv_obj_set_style_text_font(list_label, font, LV_PART_MAIN);                     /* 设置标签文本字体 */
 
     /* 创建列表 */
     list = lv_list_create(obj_left);                                                /* 创建列表 */
     lv_obj_set_width(list, lv_obj_get_width(obj_left) * 0.8);                       /* 设置列表宽度 */
     lv_obj_set_height(list, lv_obj_get_height(obj_left) * 0.9);                     /* 设置列表高度 */
     lv_obj_center(list);                                                            /* 设置列表的位置 */
     lv_obj_set_style_text_font(list, font, LV_PART_MAIN);                           /* 设置字体 */
 
     /* 为列表添加按钮 */
     lv_obj_t* btn;
     lv_list_add_text(list, "File");                                                 /* 添加列表文本 */
     btn = lv_list_add_btn(list, LV_SYMBOL_FILE, "New");                             /* 添加按钮 */
     lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */
     btn = lv_list_add_btn(list, LV_SYMBOL_DIRECTORY, "Open");                       /* 添加按钮 */
     lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */
     btn = lv_list_add_btn(list, LV_SYMBOL_SAVE, "Save");                            /* 添加按钮 */
     lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */
     btn = lv_list_add_btn(list, LV_SYMBOL_CLOSE, "Delete");                         /* 添加按钮 */
     lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */
     btn = lv_list_add_btn(list, LV_SYMBOL_EDIT, "Edit");                            /* 添加按钮 */
     lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */
     lv_list_add_text(list, "Connectivity");                                         /* 添加列表文本 */
     btn = lv_list_add_btn(list, LV_SYMBOL_BLUETOOTH, "Bluetooth");                  /* 添加按钮 */
     lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */
     btn = lv_list_add_btn(list, LV_SYMBOL_GPS, "Navigation");                       /* 添加按钮 */
     lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */
     btn = lv_list_add_btn(list, LV_SYMBOL_USB, "USB");                              /* 添加按钮 */
     lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */
     btn = lv_list_add_btn(list, LV_SYMBOL_BATTERY_FULL, "Battery");                 /* 添加按钮 */
     lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */
     lv_list_add_text(list, "Exit");                                                 /* 添加列表文本 */
     btn = lv_list_add_btn(list, LV_SYMBOL_OK, "Apply");                             /* 添加按钮 */
     lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */
     btn = lv_list_add_btn(list, LV_SYMBOL_CLOSE, "Close");                          /* 添加按钮 */
     lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, NULL);            /* 添加按钮回调 */
 }
 
 /**
  * @brief  LVGL演示
  * @param  无
  * @return 无
  */
 void lv_mainstart(void)
 {
     lv_example_list();
 }
```

## 下拉列表部件 

下拉列表部件（lv_dropdown）常用于多选一的场景，点击后可展现多个选项。

<img src=".\img\image-20250528205838920.png" alt="image-20250528205838920" style="zoom:50%;" />

**组成部分**

- 按钮（BUTTON）
- 列表（LIST)

**相关API函数**

- 创建下拉列表部件：lv_obj_t   *dd = lv_dropdown_create( parent );
- 设置选项内容
  - lv_dropdown_set_options( dd, "a\nb\nc\nd");                   /* 动态设置选项  通过\n进行区分*/
  - lv_dropdown_set_options_static( dd, "a\nb\nc\nd ");      /* 设置选项 静态 不可修改*/
  - lv_dropdown_add_option( dd, "e", 4);                                /* 添加选项，索引从0开始 ，不可使用静态*/
- 设置当前所选项：lv_dropdown_set_selected( dd, 1);               /* 索引从0开始，不可使用静态 */
- 获取选项内容
  - lv_dropdown_get_selected( dd);					/* 获取索引 */
  - lv_dropdown_get_selected_str( dd, buf, sizeof(buf));     /* 获取选项文本 */
- 设置方向和图标
  - lv_dropdown_set_dir(dd, LV_DIR_RIGHT);                        /* 设置列表展开方向 */
  - lv_dropdown_set_symbol(dd, LV_SYMBOL_RIGHT);      /* 设置图标 */

**实例**

<img src=".\img\image-20250529182726607.png" alt="image-20250529182726607" style="zoom:50%;" />

```c
 #include "LVGL/GUI_APP/lv_mainstart.h"
 #include "lvgl.h"
 #include <stdio.h>
 
 /* 获取当前活动屏幕的宽高 */
 #define scr_act_width() lv_obj_get_width(lv_scr_act())
 #define scr_act_height() lv_obj_get_height(lv_scr_act())
 
 static lv_obj_t *label;                                             /* 当前选项标签 */
 static const lv_font_t *dropdown_font;                              /* 定义字体 */
 static int16_t dropdown_width;                                      /* 列表宽度 */
 
 /* 下拉列表的选项 */
 static const char *options =    "option 1\n"
                                 "option 2\n"
                                 "option 3\n"
                                 "option 4\n"
                                 "option 5\n"
                                 "option 6\n"
                                 "option 7\n"
                                 "option 8\n";
 
 /**
  * @brief  下拉列表事件回调
  * @param  无
  * @return 无
  */
 static void dropdown_event_cb(lv_event_t* e)
 {
     lv_event_code_t code = lv_event_get_code(e);                    /* 获取事件类型 */
     lv_obj_t *dropdown = lv_event_get_target(e);                    /* 获取触发源 */
 
     if (LV_EVENT_VALUE_CHANGED == code)                             /* 判断事件类型 */
     {
         char buf[10];
         lv_dropdown_get_selected_str(dropdown, buf, sizeof(buf));   /* 获取当前选项文本 */
         lv_label_set_text(label, buf);                              /* 显示当前选项文本 */
     }
 }
 
 /**
  * @brief  例1
  * @param  无
  * @return 无
  */
 static void lv_example_dropdown_1(void)
 {
     /* 根据屏幕宽度选择字体和列表宽度 */
     if (scr_act_width() <= 320) 
     {
         dropdown_font = &lv_font_montserrat_14;
         dropdown_width = 90;
     }
     else if (scr_act_width() <= 480) 
     {
         dropdown_font = &lv_font_montserrat_18;
         dropdown_width = 120;
     }
     else 
     {
         dropdown_font = &lv_font_montserrat_22;
         dropdown_width = 150;
     }
 
     lv_obj_t* dropdown = lv_dropdown_create(lv_scr_act());                                      /* 定义并创建下拉列表 */
     lv_dropdown_set_options_static(dropdown, options);                                          /* 添加下拉列表选项 */
     lv_obj_set_style_text_font(dropdown, dropdown_font, LV_PART_MAIN);                          /* 设置下拉列表字体 */
     lv_obj_set_width(dropdown, dropdown_width);                                                 /* 设置下拉列表宽度 */
     lv_obj_align(dropdown, LV_ALIGN_CENTER, -scr_act_width() / 3, 0);                           /* 设置下拉列表位置 */
 
     label = lv_label_create(lv_scr_act());                                                      /* 定义并创建标签 */
     lv_obj_set_style_text_font(label, dropdown_font, LV_PART_MAIN);                             /* 设置标签表字体 */
     lv_obj_set_width(label, dropdown_width);                                                    /* 设置标签宽度 */
     lv_obj_align_to(label, dropdown, LV_ALIGN_OUT_TOP_MID, 15, -scr_act_height() / 8);          /* 设置标签位置 */
     lv_label_set_text(label, "option 1");                                                       /* 设置标签文本 */
 
     lv_obj_add_event_cb(dropdown, dropdown_event_cb, LV_EVENT_VALUE_CHANGED, NULL);             /* 添加下拉列表回调 */
 }
 
 /**
  * @brief  例2
  * @param  无
  * @return 无
  */
 static void lv_example_dropdown_2(void)
 {
     lv_obj_t* dropdown;                                                                         /* 定义下拉列表 */
     
     dropdown = lv_dropdown_create(lv_scr_act());                                                /* 创建下拉列表 */
     lv_dropdown_set_options_static(dropdown, options);                                          /* 添加下拉列表选项 */
     lv_obj_set_style_text_font(dropdown, dropdown_font, LV_PART_MAIN);                          /* 设置下拉列表字体 */
     lv_obj_set_width(dropdown, dropdown_width);                                                 /* 设置下拉列表宽度 */
     lv_dropdown_set_dir(dropdown, LV_DIR_BOTTOM);                                               /* 设置下拉列表方向 */
     lv_dropdown_set_symbol(dropdown, LV_SYMBOL_DOWN);                                           /* 设置下拉列表符号 */
     lv_obj_align(dropdown, LV_ALIGN_CENTER, scr_act_width() / 8, -3 * scr_act_height() / 8);    /* 设置下拉列表位置 */
 
     dropdown = lv_dropdown_create(lv_scr_act());                                                /* 创建下拉列表 */
     lv_dropdown_set_options_static(dropdown, options);                                          /* 添加下拉列表选项 */
     lv_obj_set_style_text_font(dropdown, dropdown_font, LV_PART_MAIN);                          /* 设置下拉列表字体 */
     lv_obj_set_width(dropdown, dropdown_width);                                                 /* 设置下拉列表宽度 */
     lv_dropdown_set_dir(dropdown, LV_DIR_LEFT);                                                 /* 设置下拉列表方向 */
     lv_dropdown_set_symbol(dropdown, LV_SYMBOL_LEFT);                                           /* 设置下拉列表符号 */
     lv_obj_align(dropdown, LV_ALIGN_CENTER, scr_act_width() / 8, -1 * scr_act_height() / 8);    /* 设置下拉列表位置 */
 
     dropdown = lv_dropdown_create(lv_scr_act());                                                /* 创建下拉列表 */
     lv_dropdown_set_options_static(dropdown, options);                                          /* 添加下拉列表选项 */
     lv_obj_set_style_text_font(dropdown, dropdown_font, LV_PART_MAIN);                          /* 设置下拉列表字体 */
     lv_obj_set_width(dropdown, dropdown_width);                                                 /* 设置下拉列表宽度 */
     lv_dropdown_set_dir(dropdown, LV_DIR_RIGHT);                                                /* 设置下拉列表方向 */
     lv_dropdown_set_symbol(dropdown, LV_SYMBOL_RIGHT);                                          /* 设置下拉列表符号 */
     lv_obj_align(dropdown, LV_ALIGN_CENTER, scr_act_width() / 8, 1 * scr_act_height() / 8);     /* 设置下拉列表位置 */
 
     dropdown = lv_dropdown_create(lv_scr_act());                                                /* 创建下拉列表 */
     lv_dropdown_set_options_static(dropdown, options);                                          /* 添加下拉列表选项 */
     lv_obj_set_style_text_font(dropdown, dropdown_font, LV_PART_MAIN);                          /* 设置下拉列表字体 */
     lv_obj_set_width(dropdown, dropdown_width);                                                 /* 设置下拉列表宽度 */
     lv_dropdown_set_dir(dropdown, LV_DIR_TOP);                                                  /* 设置下拉列表方向 */
     lv_dropdown_set_symbol(dropdown, LV_SYMBOL_UP);                                             /* 设置下拉列表符号 */
     lv_obj_align(dropdown, LV_ALIGN_CENTER, scr_act_width() / 8, 3 * scr_act_height() / 8);     /* 设置下拉列表位置 */
 }
 
 /**
  * @brief  LVGL演示
  * @param  无
  * @return 无
  */
 void lv_mainstart(void)
 {
     lv_example_dropdown_1();
     lv_example_dropdown_2();
 }
```

## 滚轮部件

滚轮部件（lv_roller）常用于多选一的场景，它以滚轮的形式来展现多个选项。

<img src=".\img\image-20250528210319340.png" alt="image-20250528210319340" style="zoom:50%;" />

**组成部分**

- 主体（LV_PART_MAIN）
- 选项框（LV_PART_SELECTED)

**相关API函数**

- 创建滚轮部件：lv_obj_t  *roller= lv_roller_create( parent );
- 设置选项间隔：lv_obj_set_style_text_line_space(roller, 30, LV_STATE_DEFAULT);   
- 设置选项内容、滚动模式：lv_roller_set_options(roller, “a\nb\nc\nd”, LV_ROLLER_MODE_NORMAL);   /* 正常模式 */
- 设置当前所选项：lv_roller_set_selected(roller, 3, LV_ANIM_ON);                                                                 /* 索引从0开始 ，开启动画 */
- 设置可见行数：lv_roller_set_visible_row_count(roller, 2);
- 获取选项内容
  - lv_roller_get_selected(roller);                                        /* 获取索引 */
  - lv_roller_get_selected_str(roller, buf, sizeof(buf));     /* 获取选项文本 */

注

1. 循环模式`LV_ROLLER_MODE_INFINITE`对内存占用较大

**实例**

<img src=".\img\image-20250529184741008.png" alt="image-20250529184741008" style="zoom:50%;" />

```c
 #include "LVGL/GUI_APP/lv_mainstart.h"
 #include "lvgl.h"
 #include <stdio.h>
 
 
 /* 获取当前活动屏幕的宽高 */
 #define scr_act_width() lv_obj_get_width(lv_scr_act())
 #define scr_act_height() lv_obj_get_height(lv_scr_act())
 
 /* 模式选项 */
 static const char *mode_options =  "Auto\n"
                                    "Heat\n"
                                    "Cool\n"
                                    "Fan";
 /* 温度选项 */
 static const char *temp_options =  "23\n"
                                    "24\n"
                                    "25\n"
                                    "26\n"
                                    "27\n"
                                    "28\n"
                                    "29";
 /* 湿度选项 */
 static const char *hum_options  =  "30\n"
                                    "40\n"
                                    "50\n"
                                    "60\n"
                                    "70\n"
                                    "80\n"
                                    "90";
 
 static const lv_font_t *font;                   /* 定义字体 */
 static lv_obj_t *temp_roller;                   /* 温度滚轮 */
 static lv_obj_t *hum_roller;                    /* 湿度滚轮 */
 
 
 /**
  * @brief  滚轮事件回调
  * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
  * @return 无
  */
 static void roller_event_cb(lv_event_t* e)
 {
     lv_obj_t *target = lv_event_get_target(e);                  /* 获取触发源 */
 
     if(lv_roller_get_selected(target) == 0)                     /* 获取索引，判断是否为Auto选项 */
     {
         lv_obj_add_state(temp_roller, LV_STATE_DISABLED);       /* 设置温度滚轮为不可选状态 */
         lv_obj_add_state(hum_roller, LV_STATE_DISABLED);        /* 设置湿度滚轮为不可选状态 */
     }
     else
     {
         lv_obj_clear_state(temp_roller, LV_STATE_DISABLED);     /* 解除温度滚轮不可选状态 */
         lv_obj_clear_state(hum_roller, LV_STATE_DISABLED);      /* 解除湿度滚轮不可选状态 */
     }
 }
 
 /**
  * @brief  模式设置
  * @param  无
  * @return 无
  */
 static void lv_example_roller1(void)
 {
     /* 根据活动屏幕宽度选择字体 */
     if (scr_act_width() <= 480)
     {
         font = &lv_font_montserrat_14;
     }
     else
     {
         font = &lv_font_montserrat_20;
     }
 
     lv_obj_t *mode_roller = lv_roller_create(lv_scr_act());                                     /* 创建模式设置滚轮 */
     lv_roller_set_options(mode_roller, mode_options, LV_ROLLER_MODE_NORMAL);                    /* 滚轮添加选项、设置正常模式 */
     lv_obj_align(mode_roller, LV_ALIGN_CENTER, -scr_act_width() / 4, 0);                        /* 设置滚轮位置 */
     lv_obj_set_width(mode_roller, scr_act_width() / 6);                                         /* 设置滚轮宽度 */
     lv_obj_set_style_text_font(mode_roller, font, LV_STATE_DEFAULT);                            /* 设置滚轮字体 */
     lv_roller_set_visible_row_count(mode_roller, 3);                                            /* 设置滚轮可见选项个数 */
     lv_roller_set_selected(mode_roller, 2, LV_ANIM_OFF);                                        /* 设置滚轮当前所选项 */
     lv_obj_add_event_cb(mode_roller, roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);            /* 添加事件回调 */
 
     lv_obj_t *label = lv_label_create(lv_scr_act());                                            /* 创建标签 */
     lv_label_set_text(label, "MODE");                                                           /* 设置文本内容 */
     lv_obj_set_style_text_font(label, font, LV_STATE_DEFAULT);                                  /* 设置字体 */
     lv_obj_align_to(label, mode_roller, LV_ALIGN_OUT_TOP_MID, 0, -15 );                         /* 设置位置 */
 }
 
 /**
  * @brief  温度设置
  * @param  无
  * @return 无
  */
 static void lv_example_roller2(void)
 {
     temp_roller = lv_roller_create(lv_scr_act());                                               /* 创建温度设置滚轮 */
     lv_roller_set_options(temp_roller, temp_options, LV_ROLLER_MODE_NORMAL);                    /* 滚轮添加选项、设置正常模式 */
     lv_obj_align(temp_roller, LV_ALIGN_CENTER, 0, 0);                                           /* 设置滚轮位置 */
     lv_obj_set_width(temp_roller, scr_act_width() / 6);                                         /* 设置滚轮宽度 */
     lv_obj_set_style_text_font(temp_roller, font, LV_STATE_DEFAULT);                            /* 设置滚轮字体 */
     lv_roller_set_visible_row_count(temp_roller, 3);                                            /* 设置滚轮可见选项个数 */
     lv_roller_set_selected(temp_roller, 2, LV_ANIM_OFF);                                        /* 设置滚轮当前所选项 */
 
     lv_obj_t *label = lv_label_create(lv_scr_act());                                            /* 创建标签 */
     lv_label_set_text(label, "TEMP");                                                           /* 设置文本内容 */
     lv_obj_set_style_text_font(label, font, LV_STATE_DEFAULT);                                  /* 设置字体 */
     lv_obj_align_to(label, temp_roller, LV_ALIGN_OUT_TOP_MID, 0, -15 );                         /* 设置位置 */
 }
 
 /**
  * @brief  湿度设置
  * @param  无
  * @return 无
  */
 static void lv_example_roller3(void)
 {
     hum_roller = lv_roller_create(lv_scr_act());                                                /* 创建湿度设置滚轮 */
     lv_roller_set_options(hum_roller, hum_options, LV_ROLLER_MODE_NORMAL);                      /* 滚轮添加选项、设置正常模式 */
     lv_obj_align(hum_roller, LV_ALIGN_CENTER, scr_act_width() / 4, 0);                          /* 设置滚轮位置 */
     lv_obj_set_width(hum_roller, scr_act_width() / 6);                                          /* 设置滚轮宽度 */
     lv_obj_set_style_text_font(hum_roller, font, LV_STATE_DEFAULT);                             /* 设置滚轮字体 */
     lv_roller_set_visible_row_count(hum_roller, 3);                                             /* 设置滚轮可见选项个数 */
     lv_roller_set_selected(hum_roller, 2, LV_ANIM_OFF);                                         /* 设置滚轮当前所选项 */
 
     lv_obj_t *label = lv_label_create(lv_scr_act());                                            /* 创建标签 */
     lv_label_set_text(label, "HUM");                                                            /* 设置文本内容 */
     lv_obj_set_style_text_font(label, font, LV_STATE_DEFAULT);                                  /* 设置字体 */
     lv_obj_align_to(label, hum_roller, LV_ALIGN_OUT_TOP_MID, 0, -15 );                          /* 设置位置 */
 }
 
 
 /**
  * @brief  LVGL演示
  * @param  无
  * @return 无
  */
 void lv_mainstart(void)
 {
     lv_example_roller1();               /* 模式设置滚轮 */
     lv_example_roller2();               /* 温度设置滚轮 */
     lv_example_roller3();               /* 湿度设置滚轮 */
 } 
```

## 微调器部件

微调器部件(lv_spinbox)本质上就是一个数字文本，常用于精确调节某个参数的值。

<img src=".\img\image-20250528215802563.png" alt="image-20250528215802563" style="zoom:50%;" />

**组成部分**

- 主体(LV_PART_MAIN）
- 光标(LV_PART_CURSOR)

**相关API函数**

- 创建微调器部件 ：lv_obj_t *spinbox = lv_spinbox_create(parent);
- 数值递增、递减
  - lv_spinbox_increment(spinbox);							     /* 递增 */
  - lv_spinbox_decrement(spinbox);							   /* 递减 */
- 设置步进值、范围值
  - lv_spinbox_set_step(spinbox, 200);						      /* 设置步进值，默认为1*/
  - lv_spinbox_set_range(spinbox, -1000, 1000);				      /* 设置范围值，默认±99999*/
- 设置当前值：lv_spinbox_set_value(spinbox, 400);
- 设置数字格式、光标位置
  - lv_spinbox_set_digit_format(spinbox, 4, 2); 					/* 设置数字位数、小数点位置 */
  - lv_spinbox_set_pos(spinbox, 3);							    /* 设置光标位置，最左索引为0，不建议与设置步进值同时使用*/

<img src=".\img\image-20250528220005220.png" alt="image-20250528220005220" style="zoom:50%;" />

- 获取当前值：lv_spinbox_get_value(spinbox);					      /* 返回值为整数，而非小数 */

注

1. 设置小数时，小数点只起到装饰作用，实际值仍为整数

**实例**

<img src=".\img\image-20250531220954529.png" alt="image-20250531220954529" style="zoom:40%;" />

```c
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>


static lv_obj_t *spinbox;                          /* 微调器 */
static lv_obj_t *btn_up;                           /* 递增按钮 */
static lv_obj_t *btn_down;                         /* 递减按钮 */

/**
 * @brief  按钮事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void btn_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);     /* 获取触发源 */

    if (target == btn_up)                          /* 递增按钮按下 */
    {
        lv_spinbox_increment(spinbox);             /* 数值递增 */
    }
    else if (target == btn_down)                   /* 递减按钮按下 */
    {
        lv_spinbox_decrement(spinbox);             /* 数值递减 */
    }
}

/**
 * @brief  微调器事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void spinbox_event_cb(lv_event_t *e)
{
    float spinbox_value;                           /* 微调器当前值 */

    lv_event_code_t code = lv_event_get_code(e);   /* 获取事件类型 */
    
    if(code == LV_EVENT_VALUE_CHANGED)
    {
        spinbox_value= (float)lv_spinbox_get_value(spinbox);

        printf("%.1f\r\n", spinbox_value/10);
    }
}

/**
 * @brief  微调器实例
 * @param  无
 * @return 无
 */
static void lv_example_spinbox(void)
{
    /* 微调器 */
    spinbox = lv_spinbox_create(lv_scr_act());                                          /* 创建微调器 */
    lv_spinbox_set_range(spinbox, -10000, 10000);                                       /* 设置范围值 */
    lv_spinbox_set_digit_format(spinbox, 5, 4);                                         /* 设置数字格式 */
    lv_obj_set_size(spinbox, 150, 47);                                                  /* 设置大小 */
    lv_obj_center(spinbox);                                                             /* 设置位置 */
    lv_obj_update_layout(spinbox);                                                      /* 更新布局 */
    lv_obj_set_style_text_font(spinbox, &lv_font_montserrat_18, LV_PART_MAIN);          /* 设置字体 */
    lv_obj_set_style_text_align(spinbox, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);           /* 设置文本对齐方式 */
    lv_obj_add_event_cb(spinbox, spinbox_event_cb, LV_EVENT_VALUE_CHANGED, NULL);       /* 添加事件回调 */

    /* 递增按钮 */
    btn_up = lv_btn_create(lv_scr_act());                                               /* 创建按钮 */
    lv_obj_set_size(btn_up, 38, 38);                                                    /* 设置大小 */
    lv_obj_align_to(btn_up, spinbox, LV_ALIGN_OUT_LEFT_MID, -10, 0);                    /* 设置位置 */
    lv_obj_set_style_bg_img_src(btn_up, LV_SYMBOL_PLUS, LV_PART_MAIN);                  /* 设置背景图标 */
    lv_obj_add_event_cb(btn_up, btn_event_cb, LV_EVENT_CLICKED, NULL);                  /* 添加事件回调 */

    /* 递减按钮 */
    btn_down = lv_btn_create(lv_scr_act());                                             /* 创建按钮 */
    lv_obj_set_size(btn_down, 38, 38);                                                  /* 设置大小 */
    lv_obj_align_to(btn_down, spinbox, LV_ALIGN_OUT_RIGHT_MID, 10, 0);                  /* 设置位置 */
    lv_obj_set_style_bg_img_src(btn_down, LV_SYMBOL_MINUS, LV_PART_MAIN);               /* 设置背景图标 */
    lv_obj_add_event_cb(btn_down, btn_event_cb, LV_EVENT_CLICKED, NULL);                /* 添加事件回调 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_spinbox();
}
```

## 进度条部件

进度条部件（lv_bar）常用于显示当前任务的完成进度。

<img src=".\img\image-20250528155252432.png" alt="image-20250528155252432" style="zoom:40%;" />

**组成部分**

- 主体(LV_PART_MAIN）
- 指示器(LV_PART_INDICATOR)

**相关API函数**

- 创建进度条部件：lv_obj_t  *bar = lv_bar_create ( parent );
- 设置大小：lv_obj_set_size( bar, 400, 20 );				                                /* 设置大小 */
- 设置当前值：lv_bar_set_value( bar, 50, LV_ANIM_ON ); 	                               /* 设置当前值并开启动画 */
- 设置范围值：lv_bar_set_range( bar, -100, 100 ); 			                            /* 设置范围值 需放在设置当前值之前 */
- 设置动画时间：lv_obj_set_style_anim_time( bar, 500, LV_STATE_DEFAULT );    /* 动画设置必须放在当前值设置之前  单位：ms*/
- 设置起始值：lv_bar_set_start_value( bar, -50, LV_ANIM_OFF );                             /* 设置起始值 */
- 设置模式：lv_bar_set_mode( bar, LV_BAR_MODE_RANGE );                                 /* 设置模式 */

```c
enum {
    LV_BAR_MODE_NORMAL,				/* 默认模式 从起始值开始绘制 */
    LV_BAR_MODE_SYMMETRICAL,		/* 从零值绘制到当前值（当前值可以小于0） */
    LV_BAR_MODE_RANGE				/* 允许设置起始值，但起始值必须小于当前值 */
};
```

注

1. 当进度条高度大于宽度时，其方向变为垂直
2. 默认范围值为0~100
3. 开启动画后，进度条会绘制到当前值为止
4. 动画时间设置需要放在当前值设置之前 ，否则看不到动画效果。动画时间指的是旧值刷新到当前值的绘图时间

**实例**

<img src=".\img\image-20250528161922943.png" alt="image-20250528161922943" style="zoom:50%;" />

```c
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>


/* 获取当前活动屏幕的宽高 */
#define scr_act_width() lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

static int16_t val = 0;                         /* 当前值 */

static const lv_font_t *font;                   /* 定义字体 */

static lv_obj_t *label_load;                    /* 加载标题标签 */
static lv_obj_t *label_per;                     /* 百分比标签 */
static lv_obj_t *bar;                           /* 进度条 */

/**
 * @brief  定时器回调
 * @param  *timer ：该定时器相关的数据
 * @return 无
 */
static void timer_cb(lv_timer_t *timer)
{
    if(val < 100)                                                           /* 当前值小于100 */
    {
        val ++;
        lv_bar_set_value(bar, val, LV_ANIM_ON);                             /* 设置当前值 */
        lv_label_set_text_fmt(label_per, "%d %%", lv_bar_get_value(bar));   /* 获取当前值，更新显示 */
    }
    else                                                                    /* 当前值大于等于100 */
    {
        lv_label_set_text(label_per, "finished!");                          /* 加载完成 */
    }
}

/**
 * @brief  加载提示标签
 * @param  无
 * @return 无
 */
static void lv_example_label(void)
{
    /* 根据活动屏幕宽度选择字体 */
    if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
    }
    else
    {
        font = &lv_font_montserrat_20;
    }

    /* 加载标题标签 */
    label_load = lv_label_create(lv_scr_act());
    lv_label_set_text(label_load, "LOADING...");
    lv_obj_set_style_text_font(label_load, font, LV_STATE_DEFAULT);
    lv_obj_align(label_load, LV_ALIGN_CENTER, 0, -scr_act_height() / 10 );

    /* 百分比标签 */
    label_per = lv_label_create(lv_scr_act());
    lv_label_set_text(label_per, "%0");
    lv_obj_set_style_text_font(label_per, font, LV_STATE_DEFAULT);
    lv_obj_align(label_per, LV_ALIGN_CENTER, 0, scr_act_height() / 10 );
}

/**
 * @brief  加载进度条
 * @param  无
 * @return 无
 */
static void lv_example_bar(void)
{
    bar = lv_bar_create(lv_scr_act());                            /* 创建进度条 */
    lv_obj_set_align(bar, LV_ALIGN_CENTER);                       /* 设置位置 */
    lv_obj_set_size(bar, scr_act_width() * 3 / 5, 20);            /* 设置大小 */
    lv_obj_set_style_anim_time(bar, 100, LV_STATE_DEFAULT);       /* 设置动画时间 */
    lv_timer_create(timer_cb, 100, NULL);                         /* 初始化定时器 单位：ms 用户数据：无 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_label();                                           /* 加载提示标签 */
    lv_example_bar();                                             /* 加载进度条 */
}
```

## 加载器部件

加载器部件（lv_spinner）常用于提示当前任务正在加载。

<img src=".\img\image-20250528204938768.png" alt="image-20250528204938768" style="zoom:40%;" />

**组成部分**

- 主体(LV_PART_MAIN）
- 指示器(LV_PART_INDICATOR)
- 手柄(LV_PART_KNOB)

**相关API函数**

- 创建加载器部件：lv_obj_t  *spinner = lv_spinner_create( parent, spin_time, arc_length );	 /\* 父对象、画一圈所需时间 单位：ms、指示器圆弧长度\*/
- 设置圆弧颜色
  - lv_obj_set_style_arc_color( spinner, lv_color_hex(0x4a9f00), LV_PART_MAIN );                   /* 设置主体圆弧颜色 */
  - lv_obj_set_style_arc_color( spinner, lv_color_hex(0x83bd55), LV_PART_INDICATOR );       /* 设置指示器圆弧颜色 */
- 设置圆弧宽度
  - lv_obj_set_style_arc_width( spinner, 30, LV_PART_MAIN );               /* 设置主体圆弧宽度 */
  - lv_obj_set_style_arc_width( spinner, 30, LV_PART_INDICATOR );    /* 设置指示器圆弧宽度 */

**实例**

```c
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>


/* 获取当前活动屏幕的宽高 */
#define scr_act_width() lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

static const lv_font_t *font;                   /* 定义字体 */
static lv_obj_t *spinner;                       /* 加载器 */
static lv_obj_t *label_load;                    /* 加载标题标签 */

/**
 * @brief  加载提示标签
 * @param  无
 * @return 无
 */
static void lv_example_label(void)
{
    /* 根据活动屏幕宽度选择字体 */
    if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
    }
    else
    {
        font = &lv_font_montserrat_20;
    }
    
    /* 加载标题标签 */
    label_load = lv_label_create(lv_scr_act());
    lv_label_set_text(label_load, "LOADING...");
    lv_obj_set_style_text_font(label_load, font, LV_STATE_DEFAULT);
    lv_obj_align(label_load, LV_ALIGN_CENTER, 0, scr_act_height() / 10 );
}

/**
 * @brief  加载器显示
 * @param  无
 * @return 无
 */
static void lv_example_spinner(void)
{
    spinner = lv_spinner_create(lv_scr_act(), 1000, 60);                            /* 创建加载器 */
    lv_obj_align(spinner, LV_ALIGN_CENTER, 0, -scr_act_height() / 15 );             /* 设置位置 */
    lv_obj_set_size(spinner, scr_act_height() / 5, scr_act_height() / 5);           /* 设置大小 */
    lv_obj_set_style_arc_width(spinner, scr_act_height() / 35, LV_PART_MAIN);       /* 设置主体圆弧宽度 */
    lv_obj_set_style_arc_width(spinner, scr_act_height() / 35, LV_PART_INDICATOR);  /* 设置指示器圆弧宽度 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_label();            /* 加载提示标签 */
    lv_example_spinner();          /* 加载器显示 */
}
```

## 滑块部件

滑块部件（lv_slider）常用于调节某个参数的值，它以直线滑动的形式来修改数值。

<img src=".\img\image-20250528210655109.png" alt="image-20250528210655109" style="zoom:40%;" />

**组成部分**

- 主体(LV_PART_MAIN）
- 指示器(LV_PART_INDICATOR)
- 旋钮(LV_PART_KNOB)

**相关API函数**

- 创建滑块部件：lv_obj_t  *slider = lv_slider_create( parent );
- 设置大小、当前值、范围值
  - lv_obj_set_size(slider, 20, 200);                                 /* 高度>宽度时，滑块为纵向 */
  - lv_slider_set_value(slider, 50, LV_ANIM_OFF);        /* 设置当前值 */
  - lv_slider_set_range(slider, -100, 100);                      /* 设置范围值 */
- 获取当前值：lv_slider_get_value(slider);
- 模式设置：lv_slider_set_mode(slider, LV_SLIDER_MODE_...);

<img src=".\img\image-20250528211826975.png" alt="image-20250528211826975" style="zoom:40%;" />

- 设置、获取左值
  - lv_slider_set_left_value(slider, 20, LV_ANIM_OFF);     /* 设置左值 */
  - lv_slider_get_left_value(slider);                                      /* 获取左值 */

注

1. 默认范围值为0~100

**实例**

<img src=".\img\image-20250529191140205.png" alt="image-20250529191140205" style="zoom:50%;" />

```c
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>

/* 获取当前活动屏幕的宽高 */
#define scr_act_width() lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

static const lv_font_t *font;                                                       /* 定义字体 */
static lv_obj_t *slider_label;                                                      /* 进度条百分比标签 */


/**
 * @brief  滑块事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void slider_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);                                      /* 获取触发源 */
    lv_event_code_t code = lv_event_get_code(e);                                    /* 获取事件类型 */

    if(code == LV_EVENT_VALUE_CHANGED)
    {
        lv_label_set_text_fmt(slider_label, "%d%%", lv_slider_get_value(target));   /* 获取当前值，更新进度条百分比 */
    }
}

/**
 * @brief  音量调节滑块
 * @param  无
 * @return 无
 */
static void lv_example_slider(void)
{
    /* 根据活动屏幕宽度选择字体 */
    if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
    }
    else
    {
        font = &lv_font_montserrat_20;
    }

    /* 滑块 */
    lv_obj_t * slider = lv_slider_create(lv_scr_act());                             /* 创建滑块 */
    lv_obj_set_size(slider, scr_act_width() / 2, 20);                               /* 设置大小 */
    lv_obj_center(slider);                                                          /* 设置位置 */
    lv_slider_set_value(slider, 50, LV_ANIM_OFF);                                   /* 设置当前值 */
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);     /* 添加事件 */

    /* 百分比标签 */
    slider_label = lv_label_create(lv_scr_act());                                   /* 创建百分比标签 */
    lv_label_set_text(slider_label, "50%");                                         /* 设置文本内容 */
    lv_obj_set_style_text_font(slider_label, font, LV_STATE_DEFAULT);               /* 设置字体 */
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_RIGHT_MID, 20, 0);           /* 设置位置 */

    /* 音量图标 */
    lv_obj_t *sound_label = lv_label_create(lv_scr_act());                          /* 创建音量标签 */
    lv_label_set_text(sound_label, LV_SYMBOL_VOLUME_MAX);                           /* 设置文本内容：音量图标 */
    lv_obj_set_style_text_font(sound_label, font, LV_STATE_DEFAULT);                /* 设置字体 */
    lv_obj_align_to(sound_label, slider, LV_ALIGN_OUT_LEFT_MID, -20, 0);            /* 设置位置 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_slider();
}
```

## 圆弧部件

圆弧部件（lv_arc）以弧形滑动的形式来调节、显示某个参数的值。

**组成部分**

- 背景弧(LV_PART_MAIN）
- 前景弧(LV_PART_INDICATOR)
- 旋钮(LV_PART_KNOB)

**相关API函数**

- 创建圆弧部件：lv_obj_t  *arc = lv_arc_create( parent );
- 设置当前值、范围值
  - lv_arc_set_value(arc, 80);           /* 设置当前值（需要在范围值之内）*/
  - lv_arc_set_range(arc, 0, 200);    /* 设置范围值 需放在设置当前值之前 */
- 设置圆弧角度
  - lv_arc_set_angles(arc, 135, 270);					    /* 设置前景弧角度 ，需包含在背景弧中*/
  - lv_arc_set_bg_angles(arc, 135, 45);					/* 设置背景弧角度，需位于设置前景弧角度之前*/

<img src=".\img\image-20250528211754530.png" alt="image-20250528211754530" style="zoom:40%;" />

- 设置旋转角度：lv_arc_set_rotation(arc, 180);
- 获取当前值：lv_arc_get_value(arc);
- 设置模式、圆弧绘制速率
  - lv_arc_set_mode(arc, LV_ARC_MODE_REVERSE);				/* 设置模式 */
  - lv_arc_set_change_rate(arc, 90);							   /* 绘制速率：90°/秒 */

**实例**

<img src=".\img\image-20250529230202649.png" alt="image-20250529230202649" style="zoom:50%;" />

```c
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>


/* 获取当前活动屏幕的宽高 */
#define scr_act_width() lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

static lv_obj_t *label_left;            /* 左侧百分比标签 */
static lv_obj_t *label_right;           /* 右侧百分比标签 */
static lv_obj_t *arc_left;              /* 左侧圆弧 */
static lv_obj_t *arc_right;             /* 右侧圆弧 */

static const lv_font_t *font;           /* 定义字体 */
static uint8_t arc_width;               /* 圆弧宽度 */

/**
 * @brief  圆弧事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void arc_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);                                  /* 获取触发源 */
    lv_event_code_t code = lv_event_get_code(e);                                /* 获取事件类型 */

    if(code == LV_EVENT_VALUE_CHANGED)
    {
        lv_label_set_text_fmt(label_left, "%d%%", lv_arc_get_value(target));    /* 获取当前值，更新左侧百分比标签 */
        lv_label_set_text_fmt(label_right, "%d%%", lv_arc_get_value(target));   /* 获取当前值，更新右侧百分比标签 */
        lv_arc_set_value(arc_right, lv_arc_get_value(target));                  /* 获取左侧圆弧当前值，更新右侧圆弧当前值 */
    }
}

/**
 * @brief  左侧圆弧
 * @param  无
 * @return 无
 */
void lv_example_arc1(void)
{
    /* 根据活动屏幕宽度选择字体 */
    if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
        arc_width = 10;
    }
    else
    {
        font = &lv_font_montserrat_30;
        arc_width = 20;
    }

    /* 左侧圆弧 */
    arc_left = lv_arc_create(lv_scr_act());                                     /* 创建圆弧 */
    lv_obj_set_size(arc_left, scr_act_height() * 3/8, scr_act_height() * 3/8);  /* 设置大小 */
    lv_obj_align(arc_left, LV_ALIGN_CENTER, -scr_act_width()/5, 0);             /* 设置位置 */
    lv_arc_set_value(arc_left, 0);                                              /* 设置当前值 */
    lv_obj_set_style_arc_width(arc_left, arc_width, LV_PART_MAIN);              /* 设置背景弧宽度 */
    lv_obj_set_style_arc_width(arc_left, arc_width, LV_PART_INDICATOR);         /* 设置前景弧宽度 */
    lv_obj_add_event_cb(arc_left, arc_event_cb, LV_EVENT_VALUE_CHANGED, NULL);  /* 添加事件 */

    /* 左侧百分比标签 */
    label_left = lv_label_create(lv_scr_act());                                 /* 创建百分比标签 */
    lv_obj_align(label_left, LV_ALIGN_CENTER, -scr_act_width()/5, 0);           /* 设置位置 */
    lv_label_set_text(label_left, "0%");                                        /* 设置文本 */
    lv_obj_set_style_text_font(label_left, font, LV_STATE_DEFAULT);             /* 设置字体 */
}

/**
 * @brief  右侧圆弧
 * @param  无
 * @return 无
 */
void lv_example_arc2(void)
{
    /* 右侧圆弧 */
    arc_right = lv_arc_create(lv_scr_act());                                    /* 创建圆弧 */
    lv_obj_set_size(arc_right, scr_act_height() * 3/8, scr_act_height() * 3/8); /* 设置大小 */
    lv_obj_align(arc_right, LV_ALIGN_CENTER, scr_act_width()/5, 0);             /* 设置位置 */
    lv_arc_set_value(arc_right, 0);                                             /* 设置当前值 */
    lv_arc_set_bg_angles(arc_right, 0, 360);                                    /* 设置背景弧角度 */
    lv_arc_set_rotation(arc_right, 270);                                        /* 设置旋转角度 */
    lv_obj_remove_style(arc_right, NULL, LV_PART_KNOB);                         /* 去除旋钮 */
    lv_obj_clear_flag(arc_right, LV_OBJ_FLAG_CLICKABLE);                        /* 去除可点击属性 */
    lv_obj_set_style_arc_width(arc_right, arc_width, LV_PART_MAIN);             /* 设置背景弧宽度 */
    lv_obj_set_style_arc_width(arc_right, arc_width, LV_PART_INDICATOR);        /* 设置前景弧宽度 */

    /* 右侧百分比标签 */
    label_right = lv_label_create(lv_scr_act());                                /* 创建百分比标签 */
    lv_obj_align(label_right, LV_ALIGN_CENTER, scr_act_width()/5, 0);           /* 设置位置 */
    lv_label_set_text(label_right, "0%");                                       /* 设置文本 */
    lv_obj_set_style_text_font(label_right, font, LV_STATE_DEFAULT);            /* 设置字体 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_arc1();       /* 左侧圆弧 */
    lv_example_arc2();       /* 右侧圆弧 */
}
```

## 线条部件

线条部件（lv_line）能够在一组坐标点之间依次绘制直线。

<img src=".\img\image-20250528211200042.png" alt="image-20250528211200042" style="zoom:50%;" />

**组成部分**

- 主体(LV_PART_MAIN）

**相关API函数**

- 创建线条部件 ：lv_obj_t   *line = lv_line_create( parent );
- 设置线条坐标点
	1. static lv_point_t  line_points[] = { {15, 5}, {25, 20}, {5, 20}, {15, 5} };
	2. lv_line_set_points(line, line_points, 4);
- 设置线条样式
	- lv_obj_set_style_line_width(line, 8, LV_PART_MAIN);           	          /* 设置宽度 */
	- lv_obj_set_style_line_rounded(line, true, LV_PART_MAIN);	   	/* 设置圆角 */
- 设置y轴反转：lv_line_set_y_invert(line, true);

**实例**

<img src=".\img\image-20250529232200223.png" alt="image-20250529232200223" style="zoom:33%;" />

```c
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>
#include "math.h"


#define scr_act_width() lv_obj_get_width(lv_scr_act())          /* 获取当前活动屏幕的宽度 */
#define scr_act_height() lv_obj_get_height(lv_scr_act())        /* 获取当前活动屏幕的高度 */

#define SIN_POINTS_NUM   (200)                                  /* 正弦波坐标点数量 */

static lv_point_t sin_line_points[SIN_POINTS_NUM];              /* 正弦波坐标数组 */
static lv_point_t straigh_line_points[] = {{0,0},{100,0}};      /* 直线坐标数组 */
static const lv_font_t *font;                                   /* 定义字体 */

/**
 * @brief       产生正弦波坐标点
 * @param       maxval : 峰值
 * @param       samples: 坐标点的个数
 * @retval      无
 */
static void create_sin_buf(uint16_t maxval, uint16_t samples)
{
    uint16_t i;
    float y = 0;

    /*
     * 正弦波最小正周期为2π，约等于2 * 3.1415926
     * 曲线上相邻的两个点在x轴上的间隔 = 2 * 3.1415926 / 采样点数量
     */
    float inc = (2 * 3.1415926) / samples;                      /* 计算相邻两个点的x轴间隔 */

    for (i = 0; i < samples; i++)                               /* 连续产生samples个点 */
    {
        /*
         * 正弦波函数解析式：y = Asin(wx + φ）+ b
         * 计算每个点的y值，将峰值放大maxval倍，并将曲线向上偏移maxval到正数区域
         */
        y = maxval * sin(inc * i) + maxval;

        sin_line_points[i].x = 2 * i;                           /* 存入x轴坐标 */
        sin_line_points[i].y = y;                               /* 存入y轴坐标 */
    }
}

/**
 * @brief  正弦波实例
 * @param  无
 * @return 无
 */
static void lv_example_line(void)
{
    /* 根据活动屏幕宽度选择字体 */
    if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
    }
    else
    {
        font = &lv_font_montserrat_30;
    }

    /* 标题 */
    lv_obj_t *label = lv_label_create(lv_scr_act());                                        /* 创建标签 */
    lv_label_set_text(label, "Line");                                                       /* 设置文本内容 */
    lv_obj_set_style_text_font(label, font, LV_STATE_DEFAULT);                              /* 设置字体 */
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, scr_act_width() / 20, scr_act_height()/16);      /* 设置位置 */

    /* 直线（用作下划线）*/
    lv_obj_t *straigh_line = lv_line_create(lv_scr_act());                                  /* 创建线条 */
    lv_line_set_points(straigh_line, straigh_line_points, 2);                               /* 设置线条坐标点 */
    lv_obj_align_to(straigh_line, label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);                   /* 设置位置 */

    /* 正弦波 */
    lv_obj_t *sin_line = lv_line_create(lv_scr_act());                                      /* 创建线条 */
    create_sin_buf(scr_act_height()/4, SIN_POINTS_NUM);                                     /* 产生正弦波坐标点 */
    lv_line_set_points(sin_line, sin_line_points, SIN_POINTS_NUM);                          /* 设置线条坐标点 */
    lv_obj_center(sin_line);                                                                /* 设置位置 */
    lv_obj_set_style_line_width(sin_line, 8, LV_PART_MAIN);                                 /* 设置线的宽度 */
    lv_obj_set_style_line_color(sin_line, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);  /* 设置线的颜色 */
    lv_obj_set_style_line_rounded(sin_line, true, LV_PART_MAIN);                            /* 设置线条圆角 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_line();         /* 正弦波实例 */
}
```

## 图片部件

图片部件（lv_img）可用于图片显示、功能界面优化、背景优化等。

<img src=".\img\image-20250528211357908.png" alt="image-20250528211357908" style="zoom:40%;" />

**组成部分**

- 主体(LV_PART_MAIN）

**图片转换**

图片转化可以通过LVGL官网实现，将原图转为对应文件，就可以使用。

<img src=".\img\image-20250530124648357.png" alt="image-20250530124648357" style="zoom:50%;" />

转换出来的图片文件，有两种保存方式，一种是C数组，文件为.c后缀，随着程序一起编译保存在NOR Flash中；另外一种则是bin文件，可以保存在Fatfs文件系统中。

- C数组：将.c文件添加到工程中就可以在源文件中使用

```c
/* DECLARE的参数为C文件最下面的LV_ATTRIBUTE_LARGE_CONST类型的结构体变量名 */
LV_IMG_DECLARE(_1_111x174);
lv_img_set_src(ui->screen_img_1,&_1_111x174);
```

- BIN文件：将bin文件拷贝到Fatfs文件系统中，就可以直接输入文件的路径显示

```c
/* 需要初始化好Fatfs文件系统，然后将宏定义LV_USE_FS_FATFS 置1 */
lv_img_set_src(ui->screen_img_1,"3:/_1_111x174.bin");
```

注

1. 如何显示带有透明度通道的图片：准备一张带有透明度通道的图片，官方图片转换工具中，色彩格式选择`CF_TRUE_COLOR_ALPHA`
1. LVGL中的图标本质为文字`TEXT`

**相关API函数**

- 创建图片部件 ：lv_obj_t  *img = lv_img_create( parent );
- 设置图片源
  1. LV_IMG_DECLARE(img_bird);				    /* 声明图片，需要将生成图片文件添加到项目中 */
  2. lv_img_set_src(img, &img_bird);			      /* 设置图片源 */
- 设置图片偏移
  - lv_img_set_offset_x(img, 100);				/* x轴偏移100 */
  - .lv_img_set_offset_y(img, 20); 				/* y轴偏移20 */

<img src=".\img\image-20250529235535927.png" alt="image-20250529235535927" style="zoom:50%;" />

- 图片重新着色
	1. lv_obj_set_style_img_recolor_opa(img, 150, LV_PART_MAIN); 				 /* 设置图片透明度 不设置则无法重新着色 */
	1. lv_obj_set_style_img_recolor(img, lv_color_hex(0xffe1d2), LV_PART_MAIN); 
- 设置图片缩放、旋转
  - llv_img_set_zoom(img, 512);		/* 放大2倍 */
  - lv_img_set_angle(img, 900);		/* 顺时针方向旋转90° */

<img src=".\img\image-20250529235745906.png" alt="image-20250529235745906" style="zoom:50%;" />

- 设置中心点
  1. lv_obj_update_layout(img);		/* 更新图片布局信息 */
  2. lv_img_set_pivot(img, 0, 0);		/* 设置中心点为图片左上角，相对于原照片而言，而非坐标系 */

<img src=".\img\image-20250528211906193.png" alt="image-20250528211906193" style="zoom:50%;" />

注

1. 图片重新着色可以让带透明通道的图片只着色图片主体区域，而不包含背景

**实例**

<img src=".\img\image-20250530001400738.png" alt="image-20250530001400738" style="zoom:50%;" />

```c
 #include "LVGL/GUI_APP/lv_mainstart.h"
 #include "lvgl.h"
 #include <stdio.h>
 
 
 /* 获取当前活动屏幕的宽高 */
 #define scr_act_width()  lv_obj_get_width(lv_scr_act())
 #define scr_act_height() lv_obj_get_height(lv_scr_act())
 
 /* 定义图片部件、滑块部件 */
 static lv_obj_t *img;
 static lv_obj_t *slider_zoom, *slider_angle, *slider_r, *slider_g, *slider_b, *slider_opa;
 
 LV_IMG_DECLARE(img_gear);                     /* 声明图片 */
 
 
 /**
  * @brief  滑块事件回调
  * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
  * @return 无
  */
 static void slider_event_cb(lv_event_t *e)
 {
     lv_img_set_zoom(img, lv_slider_get_value(slider_zoom));                                         /* 设置图片缩放 */
     lv_img_set_angle(img, lv_slider_get_value(slider_angle));                                       /* 设置图片旋转角度 */
 
     /* 设置图片重新着色 */
     lv_obj_set_style_img_recolor(img,
                                  lv_color_make(lv_slider_get_value(slider_r), lv_slider_get_value(slider_g), lv_slider_get_value(slider_b)),
                                  LV_PART_MAIN);
 
     lv_obj_set_style_img_recolor_opa(img, lv_slider_get_value(slider_opa), LV_PART_MAIN);           /* 设置重新着色透明度 */
 }
 
 /**
  * @brief  创建滑块
  * @param  color:颜色值
  * @return *slider：创建成功的滑块部件
  */
 static lv_obj_t *my_slider_create(lv_color_t color)
 {
     lv_obj_t *slider = lv_slider_create(lv_scr_act());                                              /* 创建滑块 */
     lv_obj_set_height(slider, scr_act_height() / 20);                                               /* 设置高度 */
     lv_obj_set_width(slider, scr_act_width() / 3);                                                  /* 设置宽度 */
     lv_obj_remove_style(slider, NULL, LV_PART_KNOB);                                                /* 移除旋钮 */
     lv_obj_set_style_bg_color(slider,color,LV_PART_INDICATOR);                                      /* 设置滑块指示器颜色 */
     lv_obj_set_style_bg_color(slider,lv_color_darken(color, 100),LV_PART_MAIN);                     /* 设置滑块主体颜色、透明度 */
     lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);                     /* 设置滑块回调 */
     return slider;
 }
 
 /**
  * @brief  图片部件实例
  * @param  无
  * @return 无
  */
 static void lv_example_img(void)
 {
     img = lv_img_create(lv_scr_act());                                                              /* 创建图片部件 */
     lv_img_set_src(img, &img_gear);                                                                 /* 设置图片源 */
     lv_obj_align(img, LV_ALIGN_CENTER, -scr_act_width() / 5, 0);                                    /* 设置图片位置 */
     lv_obj_update_layout(img);                                                                      /* 更新图片参数 */
 
     /* 图片缩放控制滑块 */
     slider_zoom = my_slider_create(lv_color_hex(0x989c98));                                         /* 创建滑块 */
     lv_slider_set_range(slider_zoom, 128, 512);                                                     /* 设置滑块的范围 */
     lv_slider_set_value(slider_zoom, 256, LV_ANIM_OFF);                                             /* 设置滑块的值 */
     lv_obj_align(slider_zoom, LV_ALIGN_CENTER, scr_act_width() / 4, -scr_act_height() / 4);         /* 设置滑块位置 */
 
     /* 旋转角度控制滑块 */
     slider_angle = my_slider_create(lv_color_hex(0x989c98));                                        /* 创建滑块 */
     lv_slider_set_range(slider_angle, 0, 3600);                                                     /* 设置滑块的范围 */
     lv_obj_align_to(slider_angle, slider_zoom, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 20); /* 设置滑块位置 */
 
     /* 红色通道控制滑块 */
     slider_r = my_slider_create(lv_color_hex(0xff0000));                                            /* 创建滑块 */
     lv_slider_set_range(slider_r, 0, 255);                                                          /* 设置滑块的范围 */
     lv_obj_align_to(slider_r, slider_angle, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 20);    /* 设置滑块位置 */
 
     /* 绿色通道控制滑块 */
     slider_g = my_slider_create(lv_color_hex(0x00ff00));                                            /* 创建滑块 */
     lv_slider_set_range(slider_g, 0, 255);                                                          /* 设置滑块的范围 */
     lv_obj_align_to(slider_g, slider_r, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 20);        /* 设置滑块位置 */
 
     /* 蓝色通道控制滑块 */
     slider_b = my_slider_create(lv_color_hex(0x0000ff));                                            /* 创建滑块 */
     lv_slider_set_range(slider_b, 0, 255);                                                          /* 设置滑块的范围 */
     lv_obj_align_to(slider_b, slider_g, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 20);        /* 设置滑块位置 */
 
     /* 着色透明度控制滑块 */
     slider_opa = my_slider_create(lv_color_hex(0x000000));                                          /* 创建滑块 */
     lv_slider_set_range(slider_opa, 0, 255);                                                        /* 设置滑块的范围 */
     lv_slider_set_value(slider_opa, 150, LV_ANIM_OFF);                                              /* 设置滑块的值 */
     lv_obj_align_to(slider_opa, slider_b, LV_ALIGN_OUT_BOTTOM_LEFT, 0, scr_act_height() / 20);      /* 设置滑块位置 */
 }
 
 /**
  * @brief  LVGL演示
  * @param  无
  * @return 无
  */
 void lv_mainstart(void)
 {
     lv_example_img();
 }
```

## 图片按钮部件

图片按钮部件（lv_imgbtn）类似于按钮部件，不同的是，用户可以在其中设置图片。

<img src=".\img\image-20250528214055374.png" alt="image-20250528214055374" style="zoom:50%;" />

**组成部分**

- 主体(LV_PART_MAIN）

**相关API函数**

- 创建图片按钮部件 ：lv_obj_t  *imgbtn = lv_imgbtn_create(parent);
- 设置图片源、按钮大小
  - lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_..., src_left, src_mid, src_right);    /* 设置某个状态某个位置的图片源 */
  - lv_obj_set_size(imgbtn, 64 * 3, 64);								               /* 设置图片按钮大小 像素，要根据图片大小进行设置 */

<img src=".\img\image-20250528214248712.png" alt="image-20250528214248712" style="zoom:50%;" />

- 设置按钮状态：lv_imgbtn_set_state(imgbtn, LV_IMGBTN_STATE_PRESSED);		/* 设置按下状态 */

**实例**

<img src=".\img\image-20250530155648363.png" alt="image-20250530155648363" style="zoom:40%;" />

```c
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>


/* 获取当前活动屏幕的宽高 */
#define scr_act_width()  lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

static lv_point_t line_points[] = {{0,0},{0,120}};          /* 直线坐标数组 */

static const lv_font_t *font;                               /* 定义字体 */
static uint8_t cool_mode_state = 0;                         /* 制冷模式状态：0，关闭；1，开启 */
static uint8_t warm_mode_state = 0;                         /* 制暖模式状态：0，关闭；1，开启 */
static uint8_t dry_mode_state = 0;                          /* 干燥模式状态：0，关闭；1，开启 */

LV_IMG_DECLARE(img_cool);                                   /* 声明制冷按钮图片 */
LV_IMG_DECLARE(img_warm);                                   /* 声明制暖按钮图片 */
LV_IMG_DECLARE(img_dry);                                    /* 声明干燥按钮图片 */

/**
 * @brief  制冷按钮事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void cool_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);                              /* 获取触发源 */

    if(cool_mode_state == 0)                                                /* 判断按钮状态，如果为0，即关闭状态 */
    {
        cool_mode_state = 1;                                                /* 切换按钮状态 */
        lv_obj_set_style_img_recolor_opa(target, 255, 0);                   /* 设置图片重新着色透明度 */
        lv_obj_set_style_img_recolor(target, lv_color_hex(0x00a9ff), 0);    /* 设置图片重新着色：蓝色 */
    }
    else                                                                    /* 按钮为开启状态 */
    {
        cool_mode_state = 0;                                                /* 切换按钮状态 */
        lv_obj_set_style_img_recolor_opa(target, 255, 0);                   /* 设置图片重新着色透明度 */
        lv_obj_set_style_img_recolor(target, lv_color_hex(0x8a8a8a), 0);    /* 设置图片重新着色：灰色 */
    }
}

/**
 * @brief  制暖按钮事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void warm_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);                              /* 获取触发源 */

    if(warm_mode_state == 0)                                                /* 判断按钮状态，如果为0，即关闭状态 */
    {
        warm_mode_state = 1;                                                /* 切换按钮状态 */
        lv_obj_set_style_img_recolor_opa(target, 255, 0);                   /* 设置图片重新着色透明度 */
        lv_obj_set_style_img_recolor(target, lv_color_hex(0xff0000), 0);    /* 设置图片重新着色：红色 */
    }
    else
    {
        warm_mode_state = 0;                                                /* 切换按钮状态 */
        lv_obj_set_style_img_recolor_opa(target, 255, 0);                   /* 设置图片重新着色透明度 */
        lv_obj_set_style_img_recolor(target, lv_color_hex(0x8a8a8a), 0);    /* 设置图片重新着色：灰色 */
    }
}

/**
 * @brief  干燥按钮事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void dry_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);                              /* 获取触发源 */

    if(dry_mode_state == 0)                                                 /* 判断按钮状态，如果为0，即关闭状态 */
    {
        dry_mode_state = 1;                                                 /* 切换按钮状态 */
        lv_obj_set_style_img_recolor_opa(target, 255, 0);                   /* 设置图片重新着色透明度 */
        lv_obj_set_style_img_recolor(target, lv_color_hex(0x00a9ff), 0);    /* 设置图片重新着色：蓝色 */
    }
    else                                                                    /* 按钮为开启状态 */
    {
        dry_mode_state = 0;                                                 /* 切换按钮状态 */
        lv_obj_set_style_img_recolor_opa(target, 255, 0);                   /* 设置图片重新着色透明度 */
        lv_obj_set_style_img_recolor(target, lv_color_hex(0x8a8a8a), 0);    /* 设置图片重新着色：灰色 */
    }
}

/**
 * @brief  图片按钮实例
 * @param  无
 * @return 无
 */
static void lv_example_imgbtn(void)
{
    /* 根据活动屏幕宽度选择字体 */
    if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
    }
    else
    {
        font = &lv_font_montserrat_22;
    }

    /* 基础对象（用作背景） */
    lv_obj_t *obj = lv_obj_create(lv_scr_act());                                        /* 创建基础对象 */
    lv_obj_set_size(obj, scr_act_width()-10, 200);                                      /* 设置大小 */
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);                                           /* 设置位置 */

    /* 制冷模式（图片按钮） */
    lv_obj_t *imgbtn_cool = lv_imgbtn_create(obj);                                      /* 创建图片按钮 */
    lv_imgbtn_set_src(imgbtn_cool, LV_IMGBTN_STATE_RELEASED, NULL, &img_cool, NULL);    /* 设置图片源 */
    lv_obj_set_size(imgbtn_cool, 64, 64);                                               /* 设置大小 */
    lv_obj_align(imgbtn_cool, LV_ALIGN_CENTER, -scr_act_width()/3, -15);                /* 设置位置 */
    lv_obj_add_event_cb(imgbtn_cool, cool_event_cb, LV_EVENT_PRESSED, NULL);            /* 添加事件 */

    /* 制冷模式（标签） */
    lv_obj_t *label_cool = lv_label_create(obj);                                        /* 创建标签 */
    lv_label_set_text(label_cool, "Cool");                                              /* 设置文本 */
    lv_obj_set_style_text_font(label_cool, font, 0);                                    /* 设置字体 */
    lv_obj_align_to(label_cool, imgbtn_cool, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);           /* 设置位置 */

    /* 制暖模式（图片按钮） */
    lv_obj_t *imgbtn_warm = lv_imgbtn_create(obj);                                      /* 创建图片按钮 */
    lv_imgbtn_set_src(imgbtn_warm, LV_IMGBTN_STATE_RELEASED, NULL, &img_warm, NULL);    /* 设置图片源 */
    lv_obj_set_size(imgbtn_warm, 64, 64);                                               /* 设置大小 */
    lv_obj_align(imgbtn_warm, LV_ALIGN_CENTER, 0, -15);                                 /* 设置位置 */
    lv_obj_add_event_cb(imgbtn_warm, warm_event_cb, LV_EVENT_PRESSED, NULL);            /* 添加事件 */

    /* 制暖模式（标签） */
    lv_obj_t *label_warm = lv_label_create(obj);                                        /* 创建标签 */
    lv_label_set_text(label_warm, "Warm");                                              /* 设置文本 */
    lv_obj_set_style_text_font(label_warm, font, 0);                                    /* 设置字体 */
    lv_obj_align_to(label_warm, imgbtn_warm, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);           /* 设置位置 */

    /* 干燥模式（图片按钮） */
    lv_obj_t *imgbtn_dry = lv_imgbtn_create(obj);                                       /* 创建图片按钮 */
    lv_imgbtn_set_src(imgbtn_dry, LV_IMGBTN_STATE_RELEASED, NULL, &img_dry, NULL);      /* 设置图片源 */
    lv_obj_set_size(imgbtn_dry, 64, 64);                                                /* 设置大小 */
    lv_obj_align(imgbtn_dry, LV_ALIGN_CENTER, scr_act_width()/3, -15);                  /* 设置位置 */
    lv_obj_add_event_cb(imgbtn_dry, dry_event_cb, LV_EVENT_PRESSED, NULL);              /* 添加事件 */

    /* 干燥模式（标签） */
    lv_obj_t *label_dry = lv_label_create(obj);                                         /* 创建标签 */
    lv_label_set_text(label_dry, "Dry");                                                /* 设置文本 */
    lv_obj_set_style_text_font(label_dry, font, 0);                                     /* 设置字体 */
    lv_obj_align_to(label_dry, imgbtn_dry, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);             /* 设置位置 */
    
    /* 线条（分割线） */
    lv_obj_t *line_left = lv_line_create(obj);                                          /* 创建线条 */
    lv_line_set_points(line_left, line_points, 2);                                      /* 设置线条坐标点 */
    lv_obj_set_style_line_color(line_left, lv_color_hex(0xc6c6c6),LV_STATE_DEFAULT);    /* 设置线条颜色 */
    lv_obj_align(line_left, LV_ALIGN_CENTER, -scr_act_width()/6, 0);                    /* 设置位置 */

    lv_obj_t *line_right = lv_line_create(obj);                                         /* 创建线条 */
    lv_line_set_points(line_right, line_points, 2);                                     /* 设置线条坐标点 */
    lv_obj_set_style_line_color(line_right, lv_color_hex(0xc6c6c6),LV_STATE_DEFAULT);   /* 设置线条颜色 */
    lv_obj_align(line_right, LV_ALIGN_CENTER, scr_act_width()/6, 0);                    /* 设置位置 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_imgbtn();
}
```

## 文本区域部件

文本区域部件（lv_textarea）即文本输入框，用户可以在其中输入文本内容。

<img src=".\img\image-20250528212505715.png" alt="image-20250528212505715" style="zoom:50%;" />

**组成部分**

- 主体(LV_PART_MAIN）
- 滚动条(LV_PART_SCROLLBAR)
- 所选文本(LV_PART_SELECTED）
- 光标(LV_PART_CURSOR)
- 占位符(TEXTAREA_PLACEHOLDER)

**相关API函数**

- 创建文本区域部件：lv_obj_t  *ta = lv_textarea_create(parent);
- 添加文本
	- lv_textarea_add_char(ta, 'A');			        /* 添加一个字符到当前光标处 */
	- lv_textarea_add_text(ta, "BCDEF"); 		     /* 添加字符串到当前光标处 */
	- lv_keyboard_set_textarea(keyboard, ta);	   /* 关联键盘和文本区域部件 */
- 设置光标位置：lv_textarea_set_cursor_pos(ta, 0);    /* 0：最左侧， LV_TEXTAREA_CURSOR_LAST：最右侧 */
- 删除文本
	- lv_textarea_del_char(ta);							/* 删除光标左侧的一个字符 */
	- lv_textarea_del_char_forward(ta);					/* 删除光标右侧的一个字符 */
- 设置模式
	- lv_textarea_set_one_line(ta, true);					/* 单行模式 */
	- lv_textarea_set_password_mode(ta, true);			 /* 密码模式（隐藏输入的内容） */
	- lv_textarea_set_password_show_time(ta, 100);		 /* 密码显示时间 单位：ms */
- 限制字符输入
	- lv_textarea_set_accepted_chars(ta,"0123456789");	 	  /* 限制接收的字符 */
	- lv_textarea_set_max_length(ta,6);			 		        /* 限制字符长度 */
- 设置占位符：lv_textarea_set_placeholder_text(ta, "password");      /* 提示输入密码 */
- 获取文本：const char *txt = lv_textarea_get_text(ta);		         /\* 获取文本框文本 */ 
- 比对文本内容：strcmp(const char *s1,const char *s2);		      /\* 当s1=s2时，返回0 */

**实例**

<img src=".\img\image-20250530140253662.png" alt="image-20250530140253662" style="zoom:40%;" />

```c
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>


/* 获取当前活动屏幕的宽高 */
#define scr_act_width()  lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

static const lv_font_t *font;       /* 定义字体 */

static lv_obj_t *keyboard;          /* 键盘 */
static lv_obj_t *label_name;        /* 用户名正误提示标签 */
static lv_obj_t *label_pass;        /* 密码正误提示标签 */

/**
 * @brief  用户名文本框事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void name_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);            /* 获取事件类型 */
    lv_obj_t *target = lv_event_get_target(e);              /* 获取触发源 */

    if(code == LV_EVENT_FOCUSED)                            /* 事件类型：被聚焦 */
    {
        lv_keyboard_set_textarea(keyboard, target);         /* 关联用户名文本框和键盘 */
    }
    else if(code == LV_EVENT_VALUE_CHANGED)                 /* 事件类型：文本框的内容发生变化 */
    {
        const char *txt = lv_textarea_get_text(target);     /* 获取文本框的文本 */

        if(strcmp(txt,"admin") == 0)                        /* 判断用户名是否正确 */
        {
            lv_label_set_text(label_name, LV_SYMBOL_OK);    /* 用户名正确，显示√ */
        }
        else
        {
            lv_label_set_text(label_name, "");              /* 用户名错误，不提示 */
        }
    }
}

/**
 * @brief  密码文本框事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void pass_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);            /* 获取事件类型 */
    lv_obj_t *target = lv_event_get_target(e);              /* 获取触发源 */

    if(code == LV_EVENT_FOCUSED)                            /* 事件类型：被聚焦 */
    {
        lv_keyboard_set_textarea(keyboard, target);         /* 关联用户名文本框和键盘 */
    }
    else if(code == LV_EVENT_VALUE_CHANGED)                 /* 事件类型：文本框的内容发生变化 */
    {
        const char *txt = lv_textarea_get_text(target);     /* 获取文本框的文本 */

        if(strcmp(txt,"123456") == 0)                       /* 判断密码是否正确 */
        {
            lv_label_set_text(label_pass, LV_SYMBOL_OK);    /* 密码正确，显示√ */
        }
        else
        {
            lv_label_set_text(label_pass, "");              /* 密码错误，不提示 */
        }
    }
}

/**
 * @brief  用户登录实例
 * @param  无
 * @return 无
 */
static void lv_example_textarea(void)
{

    /* 根据屏幕大小设置字体 */
    if (scr_act_width() <= 320)
    {
        font = &lv_font_montserrat_12;
    }
    else if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
    }
    else
    {
        font = &lv_font_montserrat_22;
    }

    /* 用户名文本框 */
    lv_obj_t *textarea_name = lv_textarea_create(lv_scr_act());                                     /* 创建文本框 */
    lv_obj_set_width(textarea_name, scr_act_width()/2);                                             /* 设置宽度 */
    lv_obj_set_style_text_font(textarea_name, font, LV_PART_MAIN);                                  /* 设置字体 */
    lv_obj_align(textarea_name, LV_ALIGN_CENTER, 0, -scr_act_height()/3 );                          /* 设置位置 */
    lv_textarea_set_one_line(textarea_name, true);                                                  /* 设置单行模式 */
    lv_textarea_set_max_length(textarea_name, 6);                                                   /* 设置输入字符的最大长度 */
    lv_textarea_set_placeholder_text(textarea_name, "user name");                                   /* 设置占位符 */
    lv_obj_add_event_cb(textarea_name, name_event_cb, LV_EVENT_ALL, NULL);                          /* 添加文本框事件回调 */

    /* 用户名正误提示标签 */
    label_name = lv_label_create(lv_scr_act());                                                     /* 创建标签 */
    lv_label_set_text(label_name, "");                                                              /* 默认不提示 */
    lv_obj_set_style_text_font(label_name, font, LV_PART_MAIN);                                     /* 设置字体 */
    lv_obj_align_to(label_name, textarea_name, LV_ALIGN_OUT_RIGHT_MID, 5, 0);                       /* 设置位置 */

    /* 密码文本框 */
    lv_obj_t *textarea_pass = lv_textarea_create(lv_scr_act());                                     /* 创建文本框 */
    lv_obj_set_width(textarea_pass, scr_act_width()/2);                                             /* 设置宽度 */
    lv_obj_set_style_text_font(textarea_pass, font, LV_PART_MAIN);                                  /* 设置字体 */
    lv_obj_align_to(textarea_pass, textarea_name, LV_ALIGN_OUT_BOTTOM_MID, 0, scr_act_height()/20); /* 设置位置 */
    lv_textarea_set_one_line(textarea_pass, true);                                                  /* 设置单行模式 */
    lv_textarea_set_password_mode(textarea_pass, true);                                             /* 设置密码模式 */
    lv_textarea_set_password_show_time(textarea_pass, 1000);                                        /* 设置密码显示时间 */
    lv_textarea_set_max_length(textarea_pass, 8);                                                   /* 设置输入字符的最大长度 */
    lv_textarea_set_placeholder_text(textarea_pass, "password");                                    /* 设置占位符 */
    lv_obj_add_event_cb(textarea_pass, pass_event_cb, LV_EVENT_ALL, NULL);                          /* 添加文本框事件回调 */

    /* 密码正误提示标签 */
    label_pass = lv_label_create(lv_scr_act());                                                     /* 创建标签 */
    lv_label_set_text(label_pass, "");                                                              /* 默认不提示 */
    lv_obj_set_style_text_font(label_pass, font, LV_PART_MAIN);                                     /* 设置字体 */
    lv_obj_align_to(label_pass, textarea_pass, LV_ALIGN_OUT_RIGHT_MID, 5, 0);                       /* 设置位置 */

    /* 键盘 */
    keyboard = lv_keyboard_create(lv_scr_act());                                                    /* 创建键盘 */
    lv_obj_set_size(keyboard, scr_act_width(), scr_act_height()/2);                                 /* 设置大小 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_textarea();
}
```

## 键盘部件

键盘部件（lv_keyboard）可用于输入文本内容，其本质上就是一个特殊的按钮矩阵。

<img src=".\img\image-20250528212905868.png" alt="image-20250528212905868" style="zoom:40%;" />

**组成部分**

- 主体(LV_PART_MAIN）
- 按钮(LV_PART_ITEMS)

**相关API函数**

- 创建键盘部件：lv_obj_t *kb = lv_keyboard_create(parent);
- 关联文本框
	1. lv_obj_t *ta = lv_textarea_create(lv_scr_act());                                                    /\* 创建文本区域部件 */
	2. lv_keyboard_set_textarea(kb, ta);                                                                         /* 关联键盘和文本区域部件 */
- 设置按键弹窗：lv_keyboard_set_popovers(kb, true);						/* 允许按键弹窗提示 */
- 设置键盘模式：lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_NUMBER);     /* 数字键盘模式 */

**实例**

<img src=".\img\image-20250530152723412.png" alt="image-20250530152723412" style="zoom:50%;" />

```c
 #include "LVGL/GUI_APP/lv_mainstart.h"
 #include "lvgl.h"
 #include <stdio.h>
 
 /* 获取当前活动屏幕的宽高 */
 #define scr_act_width()  lv_obj_get_width(lv_scr_act())
 #define scr_act_height() lv_obj_get_height(lv_scr_act())
 
 /**
  * @brief  键盘事件回调
  * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
  * @return 无
  */
 static void keyboard_event_cb(lv_event_t *e)
 {
     lv_event_code_t code = lv_event_get_code(e);                            /* 获取事件类型 */
     lv_obj_t *target = lv_event_get_target(e);                              /* 获取触发源 */
 
     if(code == LV_EVENT_VALUE_CHANGED)
     {
         uint16_t id = lv_btnmatrix_get_selected_btn(target);                /* 获取键盘按钮索引 */
 
         const char *txt = lv_btnmatrix_get_btn_text(target, id);            /* 获取按钮文本 */
 
         if(strcmp(txt, LV_SYMBOL_KEYBOARD) == 0)                            /* 判断是不是键盘图标被按下 */
         {
             if(lv_keyboard_get_mode(target) == LV_KEYBOARD_MODE_NUMBER)     /* 获取当前键盘模式，判断是否为数字模式 */
             {
                 lv_keyboard_set_mode(target, LV_KEYBOARD_MODE_TEXT_LOWER);  /* 如果是数字模式，则切换为小写字母模式 */
             }
             else
             {
                 lv_keyboard_set_mode(target, LV_KEYBOARD_MODE_NUMBER);      /* 不是数字模式，则切换为数字模式 */
             }
         }
     }
 }
 
 /**
  * @brief  键盘实例
  * @param  无
  * @return 无
  */
 static void lv_example_keyboard(void)
 {
     /* 文本框 */
     lv_obj_t *textarea = lv_textarea_create(lv_scr_act());                              /* 创建文本框 */
     lv_obj_set_size(textarea, scr_act_width() - 10, scr_act_height() / 2 - 10);         /* 设置大小 */
     lv_obj_align(textarea, LV_ALIGN_TOP_MID, 0, 0);                                     /* 设置位置 */
     
     /* 键盘 */
     lv_obj_t *keyboard = lv_keyboard_create(lv_scr_act());                              /* 创建键盘 */
     lv_keyboard_set_textarea(keyboard, textarea);                                       /* 关联键盘和文本框 */
     lv_obj_add_event_cb(keyboard, keyboard_event_cb, LV_EVENT_VALUE_CHANGED, NULL);     /* 设置键盘事件回调 */
 }
 
 /**
  * @brief  LVGL演示
  * @param  无
  * @return 无
  */
 void lv_mainstart(void)
 {
     lv_example_keyboard();
 }
```

## 选项卡部件

选项卡部件(lv_tabview)可以实现多页面切换，用户可以在不同的页面中添加内容。

<img src=".\img\image-20250528214506326.png" alt="image-20250528214506326" style="zoom:40%;" />

**组成部分**

- 主体（lv_obj）
- 按钮（lv_btnmatrix)

**相关API函数**

<img src=".\img\image-20250528214750237.png" alt="image-20250528214750237" style="zoom:50%;" />

- 创建选项卡部件 ：lv_obj_t  *tabview =  lv_tabview_create(parent, LV_DIR_..., tab_size);
- 添加选项卡：lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "Tab 1"); 
- 设置当前选中的选项卡：lv_tabview_set_act(tabview, 1, LV_ANIM_OFF); 			       /* 索引从0开始 */

**实例**

<img src=".\img\image-20250530210656927.png" alt="image-20250530210656927" style="zoom:40%;" />

```c
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>


/* 获取当前活动屏幕的宽高 */
#define scr_act_width()  lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

static const lv_font_t *font;           /* 定义字体 */

/**
 * @brief  选项卡实例
 * @param  无
 * @return 无
 */
static void lv_example_tabview(void)
{
    /* 根据屏幕大小设置字体 */
    if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
    }
    else
    {
        font = &lv_font_montserrat_20;
    }

    /*************************   第一部分：选项卡（基础）   ****************************/

    lv_obj_t *tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, scr_act_height()/6);        /* 创建选项卡 */
    lv_obj_set_style_text_font(tabview, font, LV_STATE_DEFAULT);                                /* 设置字体 */

    lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "Message");                                    /* 添加选项卡1 */
    lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "Schedule");                                   /* 添加选项卡2 */
    lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "Meeting");                                    /* 添加选项卡3 */

    lv_obj_t *label1 = lv_label_create(tab1);                                                   /* 创建标签（在选项卡1内） */
    lv_label_set_text(label1, "Tonight's meeting cancelled.");                                  /* 设置文本内容 */
    lv_obj_center(label1);                                                                      /* 设置位置 */

    lv_obj_t *label2 = lv_label_create(tab2);                                                   /* 创建标签（在选项卡2内） */
    lv_label_set_text(label2, "AM   8:30   meet the client\n\n" "PM   13:30   factory tour");   /* 设置文本内容 */
    lv_obj_center(label2);                                                                      /* 设置位置 */

    lv_obj_t *label3 = lv_label_create(tab3);                                                   /* 创建标签（在选项卡3内） */
    lv_label_set_text(label3, "None");                                                          /* 设置文本内容 */
    lv_obj_center(label3);                                                                      /* 设置位置 */

    /*************************   第二部分：选项卡（界面优化）   ************************/

    /* 1、按钮 */
    lv_obj_t *btn = lv_tabview_get_tab_btns(tabview);                                           /* 获取按钮部分 */

    /* 未选中的按钮 */
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xb7472a), LV_PART_ITEMS|LV_STATE_DEFAULT);     /* 设置按钮背景颜色：橙色 */
    lv_obj_set_style_bg_opa(btn, 200, LV_PART_ITEMS|LV_STATE_DEFAULT);                          /* 设置按钮背景透明度 */
    lv_obj_set_style_text_color(btn, lv_color_hex(0xf3f3f3), LV_PART_ITEMS|LV_STATE_DEFAULT);   /* 设置按钮文本颜色：白色 */

    /* 选中的按钮 */
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xe1e1e1), LV_PART_ITEMS|LV_STATE_CHECKED);     /* 设置按钮背景颜色：灰色 */
    lv_obj_set_style_bg_opa(btn, 200, LV_PART_ITEMS|LV_STATE_CHECKED);                          /* 设置按钮背景透明度 */
    lv_obj_set_style_text_color(btn, lv_color_hex(0xb7472a), LV_PART_ITEMS|LV_STATE_CHECKED);   /* 设置按钮文本颜色：橙色 */
    lv_obj_set_style_border_width(btn, 0, LV_PART_ITEMS| LV_STATE_CHECKED);                     /* 设置按钮边框宽度为0 */

    /* 2、主体 */
    lv_obj_t *obj = lv_tabview_get_content(tabview);                                            /* 获取主体部分 */
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_STATE_DEFAULT);                   /* 设置背景颜色：白色 */
    lv_obj_set_style_bg_opa(obj, 255, LV_STATE_DEFAULT);                                        /* 设置背景透明度 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_tabview();
}
```

## 平铺视图部件

平铺视图部件（lv_tileview）可以实现不同方向的页面切换，用户可以页面中添加内容。

<img src=".\img\image-20250528214817182.png" alt="image-20250528214817182" style="zoom:50%;" />

注

1. 可以创建多个，不一定为四个

**组成部分**

- 主体(LV_PART_MAIN）
- 滚动条(LV_PART_SCROLLBAR)

**相关API函数**

<img src=".\img\image-20250528214903572.png" alt="image-20250528214903572" style="zoom:50%;" />

- 创建平铺视图部件 ：lv_obj_t  *tileview = lv_tileview_create( parent );
- 添加页面
  1. lv_obj_t *tile1 = lv_tileview_add_tile( tileview, 0, 0, LV_DIR_RIGHT );         /\* 添加页面以及允许的滑动方向\*/
  2. lv_obj_t *tile2 = lv_tileview_add_tile( tileview, 1, 0, LV_DIR_LEFT );
- 设置当前显示页面
  - lv_obj_update_layout( tileview );								     /* 更新参数 需先更新 */
  - lv_obj_set_tile( tileview, tile2, LV_ANIM_OFF );					      /* 根据页面对象切换 */
  - lv_obj_set_tile_id( tileview, 1, 0, LV_ANIM_OFF );				          /* 根据页面行列切换 */

**实例**

<img src=".\img\image-20250530212713950.png" alt="image-20250530212713950" style="zoom:43%;" />

```c
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>


/* 获取当前活动屏幕的宽高 */
#define scr_act_width()  lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

static const lv_font_t *font;           /* 定义字体 */

/**
 * @brief  平铺视图实例
 * @param  无
 * @return 无
 */
static void lv_example_tileview(void)
{
    /* 根据屏幕宽度设置字体 */
    if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_14;
    }
    else
    {
        font = &lv_font_montserrat_20;
    }

    /* 创建平铺视图页面 */
    lv_obj_t *tileview = lv_tileview_create(lv_scr_act());                                  /* 创建平铺视图 */

    lv_obj_t *tile_1 = lv_tileview_add_tile( tileview, 0, 0, LV_DIR_RIGHT );                /* 添加页面1 */
    lv_obj_t *tile_2 = lv_tileview_add_tile( tileview, 1, 0, LV_DIR_LEFT|LV_DIR_RIGHT );    /* 添加页面2 */
    lv_obj_t *tile_3 = lv_tileview_add_tile( tileview, 2, 0, LV_DIR_LEFT );                 /* 添加页面3 */

    /* 设置页面内容 */
    lv_obj_t *label_1 = lv_label_create(tile_1);                                            /* 创建标签 */
    lv_label_set_text(label_1, "Page_1");                                                   /* 设置文本内容 */
    lv_obj_set_style_text_font(label_1, font, LV_STATE_DEFAULT);                            /* 设置字体 */
    lv_obj_center(label_1);                                                                 /* 设置位置 */

    lv_obj_t *label_2 = lv_label_create(tile_2);                                            /* 创建标签 */
    lv_label_set_text(label_2, "Page_2");                                                   /* 设置文本内容 */
    lv_obj_set_style_text_font(label_2, font, LV_STATE_DEFAULT);                            /* 设置字体 */
    lv_obj_center(label_2);                                                                 /* 设置位置 */

    lv_obj_t *label_3 = lv_label_create(tile_3);                                            /* 创建标签 */
    lv_label_set_text(label_3, "Page_3");                                                   /* 设置文本内容 */
    lv_obj_set_style_text_font(label_3, font, LV_STATE_DEFAULT);                            /* 设置字体 */
    lv_obj_center(label_3);                                                                 /* 设置位置 */
    lv_obj_remove_style(tileview, NULL, LV_PART_SCROLLBAR);                                 /* 移除滚动条 */

    /* 左侧状态栏 */
    lv_obj_t *label_left = lv_label_create(lv_scr_act());                                   /* 创建标签 */
    lv_label_set_text(label_left, "AM 8:30" );                                              /* 设置文本内容 */
    lv_obj_set_style_text_font(label_left, font, LV_STATE_DEFAULT);                         /* 设置字体 */
    lv_obj_align(label_left, LV_ALIGN_TOP_LEFT, 10, 10);                                    /* 设置位置 */

    /* 右侧状态栏 */
    lv_obj_t *label_right = lv_label_create(lv_scr_act());                                  /* 创建标签 */
    lv_label_set_text(label_right, LV_SYMBOL_WIFI "   80% " LV_SYMBOL_BATTERY_3);           /* 设置文本内容 */
    lv_obj_set_style_text_font(label_right, font, LV_STATE_DEFAULT);                        /* 设置字体 */
    lv_obj_align(label_right, LV_ALIGN_TOP_RIGHT, -10, 10);                                 /* 设置位置 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_tileview();
}
```

## 窗口部件

窗口部件（lv_win)可以作为一个容器，展现不同功能的页面。

<img src=".\img\image-20250528215121587.png" alt="image-20250528215121587" style="zoom:50%;" />

**组成部分**

- 头部（Header）
- 主体（Content）

**相关API函数**

- 创建窗口部件 ：lv_obj_t *win = lv_win_create(parent, header_height);
- 添加标题、按钮
  - lv_obj_t *title = lv_win_add_title(win, "Setting");
  - lv_obj_t *btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE, 20);
- 添加主体内容
  1. lv_obj_t *content = lv_win_get_content(win); 				   /\* 获取主体 */
  2. lv_obj_t *label = lv_label_create(content);					 /\* 添加内容 */

注

1. 如果按钮创建在标题创建之前，则按钮左对齐，相反右对齐

**实例**

<img src=".\img\image-20250530214619368.png" alt="image-20250530214619368" style="zoom:43%;" />

```c
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>


/* 获取当前活动屏幕的宽高 */
#define scr_act_width()  lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

static const lv_font_t *font;                                       /* 定义字体 */
static lv_obj_t *win;                                               /* 定义窗口 */

/**
 * @brief  按钮事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void btn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);                    /* 获取事件类型 */

    if(code == LV_EVENT_CLICKED)                                    /* 按钮按下 */
    {
        lv_obj_add_flag(win, LV_OBJ_FLAG_HIDDEN);                   /* 隐藏窗口 */
    }
}

/**
 * @brief  窗口实例
 * @param  无
 * @return 无
 */
static void lv_example_win(void)
{
    /* 根据屏幕宽度选择字体 */
    if (scr_act_width() <= 480)
    {
        font = &lv_font_montserrat_12;
    }
    else
    {
        font = &lv_font_montserrat_20;
    }

    /* 窗口整体 */
    win = lv_win_create(lv_scr_act(), scr_act_height()/12);                                     /* 创建窗口 */
    lv_obj_set_size(win, scr_act_width()* 5/8, scr_act_height()* 4/7);                          /* 设置大小 */
    lv_obj_center(win);                                                                         /* 设置位置 */
    lv_obj_set_style_border_width(win, 1, LV_STATE_DEFAULT);                                    /* 设置边框宽度 */
    lv_obj_set_style_border_color(win, lv_color_hex(0x8a8a8a), LV_STATE_DEFAULT);               /* 设置边框颜色 */
    lv_obj_set_style_border_opa(win, 100, LV_STATE_DEFAULT);                                    /* 设置边框透明度 */
    lv_obj_set_style_radius(win, 10, LV_STATE_DEFAULT);                                         /* 设置圆角 */

    /*************************   第一部分：头部   ****************************/

    /* 左侧按钮 */
    lv_obj_t *btn_setting = lv_win_add_btn(win, LV_SYMBOL_SETTINGS,30);                         /* 添加按钮 */
    lv_obj_set_style_bg_opa(btn_setting, 0, LV_STATE_DEFAULT);                                  /* 设置背景透明度 */
    lv_obj_set_style_shadow_width(btn_setting, 0, LV_STATE_DEFAULT);                            /* 设置阴影宽度 */
    lv_obj_set_style_text_color(btn_setting, lv_color_hex(0x000000), LV_STATE_DEFAULT);         /* 设置文本颜色 */

    /* 标题 */
    lv_obj_t *title = lv_win_add_title(win, "Setting");                                         /* 添加标题 */
    lv_obj_set_style_text_font(title, font, LV_STATE_DEFAULT);                                  /* 设置字体 */

    /* 右侧按钮 */
    lv_obj_t *btn_close = lv_win_add_btn(win, LV_SYMBOL_CLOSE,30);                              /* 添加按钮 */
    lv_obj_set_style_bg_opa(btn_close, 0, LV_STATE_DEFAULT);                                    /* 设置背景透明度 */
    lv_obj_set_style_shadow_width(btn_close, 0, LV_STATE_DEFAULT);                              /* 设置阴影宽度 */
    lv_obj_set_style_text_color(btn_close, lv_color_hex(0x000000), LV_STATE_DEFAULT);           /* 设置文本颜色（未按下） */
    lv_obj_set_style_text_color(btn_close, lv_color_hex(0xff0000), LV_STATE_PRESSED);           /* 设置文本颜色（已按下） */
    lv_obj_add_event_cb(btn_close, btn_event_cb, LV_EVENT_CLICKED, NULL);                       /* 添加事件 */

    /*************************   第二部分：主体   ****************************/

    /* 主体背景 */
    lv_obj_t *content = lv_win_get_content(win);                                                /* 获取主体 */
    lv_obj_set_style_bg_color(content, lv_color_hex(0xffffff),LV_STATE_DEFAULT);                /* 设置背景颜色 */

    /* 音乐音量滑块 */
    lv_obj_t *slider_audio = lv_slider_create(content);                                         /* 创建滑块 */
    lv_obj_set_size(slider_audio, scr_act_width()/3, scr_act_height()/30);                      /* 设置大小 */
    lv_obj_align(slider_audio, LV_ALIGN_CENTER, 15, -scr_act_height()/14);                      /* 设置位置 */
    lv_slider_set_value(slider_audio, 50, LV_ANIM_OFF);                                         /* 设置当前值 */
    lv_obj_set_style_bg_color(slider_audio, lv_color_hex(0x787c78), LV_PART_MAIN);              /* 设置主体颜色 */
    lv_obj_set_style_bg_color(slider_audio, lv_color_hex(0xc3c3c3), LV_PART_INDICATOR);         /* 设置指示器颜色 */
    lv_obj_remove_style(slider_audio, NULL, LV_PART_KNOB);                                      /* 移除旋钮 */

    /* 音乐音量图标 */
    lv_obj_t *label_audio = lv_label_create(content);                                           /* 创建音量标签 */
    lv_label_set_text(label_audio, LV_SYMBOL_AUDIO);                                            /* 设置文本内容：音乐图标 */
    lv_obj_set_style_text_font(label_audio, font, LV_STATE_DEFAULT);                            /* 设置字体 */
    lv_obj_align_to(label_audio, slider_audio, LV_ALIGN_OUT_LEFT_MID, -scr_act_width()/40, 0);  /* 设置位置 */

    /* 闹钟音量滑块 */
    lv_obj_t *slider_bell = lv_slider_create(content);                                          /* 创建滑块 */
    lv_obj_set_size(slider_bell, scr_act_width()/3, scr_act_height()/30);                       /* 设置大小 */
    lv_obj_align(slider_bell, LV_ALIGN_CENTER, 15, scr_act_height()/14);                        /* 设置位置 */
    lv_slider_set_value(slider_bell, 50, LV_ANIM_OFF);                                          /* 设置当前值 */
    lv_obj_set_style_bg_color(slider_bell, lv_color_hex(0x787c78), LV_PART_MAIN);               /* 设置主体颜色 */
    lv_obj_set_style_bg_color(slider_bell, lv_color_hex(0xc3c3c3), LV_PART_INDICATOR);          /* 设置指示器颜色 */
    lv_obj_remove_style(slider_bell, NULL, LV_PART_KNOB);                                       /* 移除旋钮 */

    /* 闹钟音量图标 */
    lv_obj_t *label_bell = lv_label_create(content);                                            /* 创建音量标签 */
    lv_label_set_text(label_bell, LV_SYMBOL_BELL);                                              /* 设置文本内容：闹钟图标 */
    lv_obj_set_style_text_font(label_bell, font, LV_STATE_DEFAULT);                             /* 设置字体 */
    lv_obj_align_to(label_bell, slider_bell, LV_ALIGN_OUT_LEFT_MID, -scr_act_width()/40, 0);    /* 设置位置 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_win();
}
```

## 消息框部件

消息框部件（(lv_msgbox)）可以实现弹窗提示，常用于消息通知、确定操作等。

<img src=".\img\image-20250528215330241.png" alt="image-20250528215330241" style="zoom:50%;" />

**组成部分**

- 主体（obj）
- 标题（title）
- 关闭按钮（close_btn）
- 内容（content）
- 按钮矩阵（btnmatrix）

**相关API函数**

- 创建消息框部件 
  1. static const char *btns[] = { "Continue", "Close", "" };                       /\* 最后一个元素为空 \*/
  1. lv_obj_t *msgbox = lv_msgbox_create( lv_scr_act(), "Notice", "Do you want to continue?", btns, true );		/\* 参数依次为：父对象、标题、主题文本、按钮矩阵、关闭按钮 */
- 关闭消息框：lv_msgbox_close(msgbox);
- 获取按钮索引、文本
  - lv_obj_t *target = lv_event_get_current_target(e);			      /\* 获取当前触发源 */
  - lv_msgbox_get_active_btn(target);							/* 获取按钮索引 */
  - lv_msgbox_get_active_btn_text(target);					       /* 获取按钮文本 */

注

1. 创建消息框时，如果父对象为空，则只能操作消息框，其他部件不能操作；反之，其他部件仍可操作

**实例**

<img src=".\img\image-20250530221340205.png" alt="image-20250530221340205" style="zoom:50%;" />

```c
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>


/* 获取当前活动屏幕的宽高 */
#define scr_act_width()  lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

static lv_obj_t *msgbox;                                                            /* 消息框 */
static lv_obj_t *slider_label;                                                      /* 音量百分比标签 */


/*************************  第一部分 音量调节滑块  ******************************/

/**
 * @brief  滑块事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void slider_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);                                      /* 获取触发源 */
    lv_event_code_t code = lv_event_get_code(e);                                    /* 获取事件类型 */

    if(code == LV_EVENT_VALUE_CHANGED)
    {
        lv_label_set_text_fmt(slider_label, "%d%%", lv_slider_get_value(target));   /* 获取当前值，更新音量百分比 */

        if(lv_slider_get_value(target) > 80)                                        /* 音量大于80% */
        {
            lv_obj_clear_flag(msgbox, LV_OBJ_FLAG_HIDDEN);                          /* 清除消息框隐藏属性，出现弹窗提示 */
        }
    }
}

/**
 * @brief  音量调节滑块
 * @param  无
 * @return 无
 */
static void lv_example_slider(void)
{
    /* 滑块 */
    lv_obj_t * slider = lv_slider_create(lv_scr_act());                                 /* 创建滑块 */
    lv_obj_set_size(slider, scr_act_width() / 2, 20);                                   /* 设置大小 */
    lv_obj_center(slider);                                                              /* 设置位置 */
    lv_slider_set_value(slider, 50, LV_ANIM_OFF);                                       /* 设置当前值 */
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);         /* 添加事件 */

    /* 百分比标签 */
    slider_label = lv_label_create(lv_scr_act());                                       /* 创建百分比标签 */
    lv_label_set_text(slider_label, "50%");                                             /* 设置文本内容 */
    lv_obj_set_style_text_font(slider_label, &lv_font_montserrat_20, LV_STATE_DEFAULT); /* 设置字体 */
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_RIGHT_MID, 20, 0);               /* 设置位置 */

    /* 音量图标 */
    lv_obj_t *sound_label = lv_label_create(lv_scr_act());                              /* 创建音量标签 */
    lv_label_set_text(sound_label, LV_SYMBOL_VOLUME_MAX);                               /* 设置文本内容：音量图标 */
    lv_obj_set_style_text_font(sound_label, &lv_font_montserrat_20, LV_STATE_DEFAULT);  /* 设置字体 */
    lv_obj_align_to(sound_label, slider, LV_ALIGN_OUT_LEFT_MID, -20, 0);                /* 设置位置 */
}


/*************************  第二部分 消息框  ******************************/

/**
 * @brief  消息框事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void msgbox_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_current_target(e);                 /* 获取当前触发源 */

    if(lv_msgbox_get_active_btn(target) == 2)                          /* 获取按钮索引 */
    {
        lv_obj_add_flag(msgbox, LV_OBJ_FLAG_HIDDEN);                   /* 隐藏消息框 */
    }
}

/**
 * @brief  消息框实例
 * @param  无
 * @return 无
 */
static void lv_example_msgbox(void)
{
    static const char *btns[] = { " ", " ", "OK", "" };

    /* 消息框整体 */
    msgbox = lv_msgbox_create(lv_scr_act(),LV_SYMBOL_WARNING "Notice","Excessive volume may damage hearing.", btns,false);
    lv_obj_set_size(msgbox, 300, 170);                                                       /* 设置大小 */
    lv_obj_center(msgbox);                                                                   /* 设置位置 */
    lv_obj_set_style_border_width(msgbox, 0, LV_STATE_DEFAULT);                              /* 去除边框 */
    lv_obj_set_style_shadow_width(msgbox, 20, LV_STATE_DEFAULT);                             /* 设置阴影宽度 */
    lv_obj_set_style_shadow_color(msgbox, lv_color_hex(0xa9a9a9), LV_STATE_DEFAULT);         /* 设置阴影颜色 */
    lv_obj_set_style_pad_top(msgbox,18,LV_STATE_DEFAULT);                                    /* 设置顶部内边距 */
    lv_obj_set_style_pad_left(msgbox,20,LV_STATE_DEFAULT);                                   /* 设置左侧内边距 */
    lv_obj_add_event_cb(msgbox, msgbox_event_cb, LV_EVENT_VALUE_CHANGED, NULL);              /* 添加事件 */

    /* 消息框标题 */
    lv_obj_t *title = lv_msgbox_get_title(msgbox);                                           /* 获取标题部分 */
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, LV_STATE_DEFAULT);             /* 设置字体 */
    lv_obj_set_style_text_color(title, lv_color_hex(0xff0000),LV_STATE_DEFAULT);             /* 设置文本颜色：红色 */

    /* 消息框主体 */
    lv_obj_t *content = lv_msgbox_get_content(msgbox);                                       /* 获取主体部分 */
    lv_obj_set_style_text_font(content, &lv_font_montserrat_20, LV_STATE_DEFAULT);           /* 设置字体 */
    lv_obj_set_style_text_color(content, lv_color_hex(0x6c6c6c), LV_STATE_DEFAULT);          /* 设置文本颜色：灰色 */
    lv_obj_set_style_pad_top(content,15,LV_STATE_DEFAULT);                                   /* 设置顶部内边距 */

    /* 消息框按钮 */
    lv_obj_t *btn = lv_msgbox_get_btns(msgbox);                                              /* 获取按钮矩阵部分 */
    lv_obj_set_style_bg_opa(btn, 0, LV_PART_ITEMS);                                          /* 设置按钮背景透明度 */
    lv_obj_set_style_shadow_width(btn, 0, LV_PART_ITEMS);                                    /* 去除按钮阴影 */
    lv_obj_set_style_text_font(btn, &lv_font_montserrat_20, LV_PART_ITEMS);                  /* 设置字体 */
    lv_obj_set_style_text_color(btn, lv_color_hex(0x2271df),LV_PART_ITEMS);                  /* 设置文本颜色（未按下）：蓝色 */
    lv_obj_set_style_text_color(btn, lv_color_hex(0xff0000),LV_PART_ITEMS|LV_STATE_PRESSED); /* 设置文本颜色（已按下）：红色 */

    lv_obj_add_flag(msgbox, LV_OBJ_FLAG_HIDDEN);                                             /* 隐藏消息框 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_slider();
    lv_example_msgbox();
}
```

## 表格部件

表格部件(lv_table)由一个个单元格组成，它的单元格中只能存放文本形式的内容。

<img src=".\img\image-20250528220030772.png" alt="image-20250528220030772" style="zoom:50%;" />

**组成部分**

- 主体(LV_PART_MAIN）
- 单元格(LV_PART_ITEMS)

**相关API函数**

- 创建表格部件 ：lv_obj_t *table = lv_table_create(parent);
- 设置行数、列数
	- lv_table_set_row_cnt(table,2); 							     /* 设置行数 */
	- lv_table_set_col_cnt(table,2); 							       /* 设置列数 */
	- lv_table_set_col_width(table, 1, 200); 						 /* 设置列的宽度，从左至右 0~n */
- 设置单元格的内容
	- lv_table_set_cell_value(table, 0, 0, "123");					   /* 行、列*/
	- lv_table_set_cell_value_fmt(table, 1, 0, "%d", 100 );			  /* 格式化输入，类似printf */

**实例**

<img src=".\img\image-20250531222127337.png" alt="image-20250531222127337" style="zoom:50%;" />

```c
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>


/* 获取当前活动屏幕的宽高 */
#define scr_act_width() lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

/**
 * @brief  表格实例
 * @param  无
 * @return 无
 */
static void lv_example_table(void)
{
    /* 标题 */
    lv_obj_t *label_title = lv_label_create(lv_scr_act());                                  /* 创建标题 */
    lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, scr_act_height()/8);                     /* 设置位置 */
    lv_obj_set_style_text_font(label_title, &lv_font_montserrat_20, LV_STATE_DEFAULT);      /* 设置字体 */
    lv_label_set_text(label_title, "Today's prices");                                       /* 设置文本内容 */

    /* 表格 */
    lv_obj_t *table = lv_table_create(lv_scr_act());                                        /* 创建表格 */
    lv_obj_set_height(table, scr_act_height()/2);                                           /* 设置表格总的高度 */
    lv_obj_center(table);                                                                   /* 设置位置 */

    /* 设置第1列单元格内容（名称） */
    lv_table_set_cell_value(table, 0, 0, "Name");
    lv_table_set_cell_value(table, 1, 0, "Apple");
    lv_table_set_cell_value(table, 2, 0, "Banana");
    lv_table_set_cell_value(table, 3, 0, "Lemon");
    lv_table_set_cell_value(table, 4, 0, "Grape");
    lv_table_set_cell_value(table, 5, 0, "Melon");
    lv_table_set_cell_value(table, 6, 0, "Peach");
    lv_table_set_cell_value(table, 7, 0, "Nuts");

    /* 设置第2列单元格内容（价格） */
    lv_table_set_cell_value(table, 0, 1, "Price");
    lv_table_set_cell_value(table, 1, 1, "$7");
    lv_table_set_cell_value(table, 2, 1, "$4");
    lv_table_set_cell_value(table, 3, 1, "$6");
    lv_table_set_cell_value(table, 4, 1, "$2");
    lv_table_set_cell_value(table, 5, 1, "$5");
    lv_table_set_cell_value(table, 6, 1, "$1");
    lv_table_set_cell_value(table, 7, 1, "$9");

    /* 单元格宽度 */
    lv_table_set_col_width(table, 0, scr_act_width()/3);
    lv_table_set_col_width(table, 1, scr_act_width()/3);
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_table();
}
```

## 色环部件

在UI设计中，色环部件（lv_colorwheel）一般用作颜色选择器。

<img src=".\img\image-20250528211936204.png" alt="image-20250528211936204" style="zoom:50%;" />

**组成部分**

- 主体(LV_PART_MAIN）
- 旋钮(LV_PART_KNOB)

**相关API函数**

- 创建色环部件 ：lv_obj_t  *cw = lv_colorwheel_create( parent, knob_recolor );                          /\* 创建并是否开启重着色 \*/
- 设置当前选中颜色：lv_colorwheel_set_rgb(cw, lv_color_hex(0xff0000));
- 获取当前选中颜色：lv_colorwheel_get_rgb(cw);
- 设置色环模式
  1. lv_colorwheel_set_mode(cw, LV_COLORWHEEL_MODE_HUE/SATURATION/VALUE);     /* 色相、饱和度、明度 */
  2. lv_colorwheel_set_mode_fixed(cw, true);							                        /* 固定色环模式，防止长按导致色环模式切换 */

<img src=".\img\image-20250528214143627.png" alt="image-20250528214143627" style="zoom:50%;" />

**实例**

<img src=".\img\image-20250530003929709.png" alt="image-20250530003929709" style="zoom:40%;" />

```c
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>


/* 获取当前活动屏幕的宽高 */
#define scr_act_width() lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

static lv_obj_t* obj;

/**
 * @brief  色环事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void colorwheel_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);                                          /* 获取触发源 */

    lv_obj_set_style_bg_color(obj, lv_colorwheel_get_rgb(target), LV_PART_MAIN);        /* 设置基础对象背景颜色 */
}

/**
 * @brief  色环部件实例
 * @param  无
 * @return 无
 */
static void lv_example_colorwheel(void)
{
    /* 色环（用于选择颜色） */
    lv_obj_t *colorwheel = lv_colorwheel_create(lv_scr_act(), true);                    /* 创建色环 */
    lv_obj_set_size(colorwheel, scr_act_height() * 2 / 3, scr_act_height()* 2 / 3);     /* 设置大小 */
    lv_obj_center(colorwheel);                                                          /* 设置位置 */
    lv_obj_set_style_arc_width(colorwheel, scr_act_height() * 0.1, LV_PART_MAIN);       /* 设置色环圆弧宽度 */
    lv_colorwheel_set_mode_fixed(colorwheel, true);                                     /* 固定色环模式 */

    /* 基础对象（用于显示所选颜色） */
    obj = lv_obj_create(lv_scr_act());                                                  /* 创建基础对象 */
    lv_obj_set_size(obj, scr_act_height() / 3, scr_act_height() / 3);                   /* 设置大小 */
    lv_obj_align_to(obj, colorwheel, LV_ALIGN_CENTER, 0, 0);                            /* 设置位置 */
    lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, LV_PART_MAIN);                       /* 设置圆角 */
    lv_obj_set_style_bg_color(obj, lv_colorwheel_get_rgb(colorwheel), LV_PART_MAIN);    /* 设置背景颜色 */
    lv_obj_add_event_cb(colorwheel, colorwheel_event_cb, LV_EVENT_VALUE_CHANGED, NULL); /* 设置色环事件回调 */
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_example_colorwheel();
}
```

# 扩展

## 实体按键控制

使用按键控制用户界面，需要将所控制的部件添加到组中，并将按键和组关联起来。

组（group）：用于连接按键（编码器）和部件的桥梁。

<img src=".\img\image-20250531223027116.png" alt="image-20250531223027116" style="zoom:50%;" />

注

1. 某些部件默认加入到组（默认组），无需再单独添加

**配置步骤**

- 配置输入
1. 在lv_port_indev_template.c中包含按键头文件
2. 取消“键盘”相关代码的注释，在`keypad_init`函数中初始化按键
3. 配置按键扫描函数，如key_scan(0)
4. 根据部件设置按键值对应的LVGL控制字符

- 配置组
1. 创建组：lv_group_t *g = lv_group_create();
2. 设置默认组：lv_group_set_default(g);       	     	    /\* 如果部件已加入默认组，使用这种配置
    \*/
3. 添加部件到组：lv_group_add_obj(g, obj);			  /\* 如果部件未加入默认组，使用这种配置
    \*/
4. 关联按键和组：lv_indev_set_group(indev_keypad, g);    /* 需要外部声明 extern lv_indev_t  \* indev_keypad */

注

1. 不同的部件，所能配置的控制字符不同

## 文件系统移植

**步骤**

1. 初始化文件系统
2. 打开 lv_conf.h 文件，将宏定义LV_USE_FS_FATFS修改为1 ，LV_FS_FATFS_LETTER修改为'0'
3. 打开 lv_fs_fatfs.c 文件，包含头文件，修改fs_init函数，具体如下

```c
#include "./BSP/SDIO/sdio_sdcard.h"
#include "./FATFS/exfuns/exfuns.h"

while(sd_init())               	  /* 初始化 SD 卡 */    
{        
     printf("SD Card Error, Please Check!\r\n");       
     HAL_Delay(200);    
}         
exfuns_init();                   /* 为 fatfs 相关变量申请内存 */
f_mount(fs[0], "0:", 1);    	 /* 挂载 SD 卡 */
```

5. 打开 lv_demo_stress.c 文件，包含头文件，编写测试代码，具体如下

```c
#include "lvgl.h"
#include <stdio.h>

void lv_demo_stress(void)
{
    uint32_t read_num = 0;
    uint8_t buf[30] = {0};
    lv_fs_file_t f;
    lv_fs_res_t res;
    res = lv_fs_open(&f, "0:/SYSTEM/LV_FATFS/Fatfs_test.txt", LV_FS_MODE_RD);
    if(res != LV_FS_RES_OK)    printf("Open failed");
    res = lv_fs_read(&f, buf, 30, &read_num);				/* 读取30个字节 */
    if(res != LV_FS_RES_OK)    printf("Read failed");
    printf("实际读取长度：%d字节， %s",read_num , buf);
    lv_fs_close(&f);
}
```

## 中文及其他字库

**步骤**

1. 根据需求将字体文件转换为C语言数组源文件

<img src=".\img\image-20250601213931270.png" alt="image-20250601213931270" style="zoom:50%;" />

|          字符集          | Unicode编码范围 |
| :----------------------: | :-------------: |
|         基本汉字         |  0x4E00-0x9FA5  |
| 数字、拉丁字母、标点符号 |    0x20-0x7E    |

2. 添加字库文件到LVGL工程中，声明字体，编写测试代码

```c
LV_FONT_DECLARE(Font30)       /* 使用前需要声明字体 */

void my_gui(void)
{
    lv_obj_t *font_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(font_label, &Font30, LV_STATE_DEFAULT);
    lv_label_set_text(font_label, "你好，正点原子");
    lv_obj_center(font_label);
}
```

注

1. 转换的范围越大，占用的内存就越多
2. 加入字库后，必须让工程代码处于UTF-8编码模式下，否则文字显示会出问题
3. 既可以通过Unicode编码范围添加，也可以直接输入文字进行自定义字库
4. 添加特殊符号时，通过Unicode（四字节）范围进行添加，使用`#define MY_SYMBOL_Wet "\xEE\xA2\xAE"`，其中`\xEE\xA2\xAE`为对应特殊符号的 unicode 编码通过转换得到的 utf-8 编码



