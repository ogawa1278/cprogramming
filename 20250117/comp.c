// 	タイトル:
//
//	実装の方針:
//
//	工夫した点:
//
//	参考にした資料:
//
//

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/////////////////////////
//
//	ここから変更禁止
//
#define BUFFER_SIZE 1024
#define IMAGE_WIDTH 480
#define IMAGE_HEIGHT 320
#define CANARY_BYTES_SIZE 64

/// @brief メモリを確保します。
/// @param size_bytes 確保するメモリのサイズ（バイト）
/// @return 確保したメモリの先頭アドレス
void *Malloc(size_t size_bytes) {
    void *p = malloc(size_bytes);

    if (p == NULL) {
        printf("failed to allocate %zu bytes allocation.\n", size_bytes);
        exit(EXIT_FAILURE);
    }

    return p;
}

/// @brief 画像の色を表す構造体
typedef struct {
    /// @brief 赤成分（0～255）
    int r;

    /// @brief 緑成分（0～255）
    int g;

    /// @brief 青成分（0～255）
    int b;

} RGB;

/// @brief 画像データ
typedef struct {
    /// @brief 画素データ
    RGB *pixels;

    /// @brief 幅
    int width;

    /// @brief 高さ
    int height;

} Image;

/// @brief 画像データを作成します。
/// @param width 画像の幅
/// @param height 画像の高さ
/// @return 作成した画像データ
Image *CreateImage(int width, int height) {
    Image *image = (Image *)Malloc(sizeof(Image));
    image->pixels = (RGB *)Malloc(sizeof(RGB) * width * height);
    image->width = width;
    image->height = height;
    return image;
}

/// @brief ランダムな画像データを作成します。
/// @param width 画像の幅
/// @param height 画像の高さ
/// @return 作成した画像データ
Image *CreateRandomImage(int width, int height) {
    Image *image = CreateImage(width, height);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const double r
                = (sin(x * 0.02) * 0.5 + sin(x * 0.1) * 0.5) * 127 + 128;
            const double g
                = (sin(y * 0.04) * 0.5 + sin(y * 0.1) * 0.5) * 127 + 128;
            const double b
                = (sin(x * 0.1 + y * 0.1) * 0.5 + sin(x * 0.02 + y * 0.02) * 0.5
                  ) * 127
                + 128;
            const RGB color = {(int)r, (int)g, (int)b};
            image->pixels[(y * width) + x] = color;
        }
    }

    return image;
}

/// @brief 画像データを破棄します。
/// @param image 破棄する画像データ
void DestroyImage(Image *image) {
    free(image->pixels);
    free(image);
}

#pragma pack(push, 1)

/// @brief BMP ヘッダ
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BMPHeader;

#pragma pack(pop)

/// @brief BMP ヘッダを作成します。
/// @param width 画像の幅（ピクセル）
/// @param height 画像の高さ（ピクセル）
/// @param size_bytes 画像データのサイズ（バイト）
/// @return 作成した BMP ヘッダ
BMPHeader MakeBMPHeader(int width, int height, int size_bytes) {
    BMPHeader header
        = {.bfType = 0x4d42,
           .bfSize = (sizeof(BMPHeader) + size_bytes),
           .bfReserved1 = 0,
           .bfReserved2 = 0,
           .bfOffBits = sizeof(BMPHeader),
           .biSize = 40,
           .biWidth = width,
           .biHeight = height,
           .biPlanes = 1,
           .biBitCount = 24,
           .biCompression = 0,
           .biSizeImage = size_bytes,
           .biXPelsPerMeter = 0,
           .biYPelsPerMeter = 0,
           .biClrUsed = 0,
           .biClrImportant = 0};
    return header;
}

/// @brief BMP 形式の画像を読み込みます
/// @param filename 読み込むファイル名
/// @return 読み込んだ画像。読み込めなかった場合は NULL
Image *LoadBMP(const char *filename) {
    FILE *fp = fopen(filename, "rb");

    if (fp == NULL) {
        printf("failed to open %s.\n", filename);
        return NULL;
    }

    BMPHeader header;

    fread(&header, sizeof(BMPHeader), 1, fp);

    if (header.biSize != 40) {
        puts("invalid bmp format.");
        return NULL;
    }

    if (header.biCompression) {
        puts("cannot read compressed bmp.");
        return NULL;
    }

    if (header.biBitCount != 24) {
        puts("cannot read non 24-bit bmp.");
        return NULL;
    }

    const bool reversed = (header.biHeight > 0);
    const int32_t width = header.biWidth;
    const int32_t height = (reversed ? header.biHeight : -header.biHeight);

    Image *image = CreateImage(width, height);
    {
        const int32_t stride_bytes = (width * 3 + width % 4);
        const int32_t depthBytes = 3;
        const int32_t lineStep = (reversed ? -width : width);
        RGB *pDstLine = &image->pixels[reversed ? ((height - 1) * width) : 0];

        uint8_t *buffer = (uint8_t *)Malloc(stride_bytes);
        for (int32_t y = 0; y < height; ++y) {
            fread(buffer, stride_bytes, 1, fp);
            const RGB *const pDstEnd = (pDstLine + width);
            uint8_t *pSrc = buffer;

            for (RGB *pDst = pDstLine; pDst != pDstEnd; ++pDst) {
                pDst->r = pSrc[2];
                pDst->g = pSrc[1];
                pDst->b = pSrc[0];
                pSrc += depthBytes;
            }

            pDstLine += lineStep;
        }

        free(buffer);
    }

    fclose(fp);

    return image;
}

/// @brief BMP 形式で画像を保存します。
/// @param filename 保存するファイル名
/// @param image 保存する画像
void SaveBMP(const char *filename, const Image *image) {
    FILE *fp = fopen(filename, "wb");

    if (fp == NULL) {
        printf("failed to open %s.\n", filename);
        exit(EXIT_FAILURE);
    }

    const int32_t width = image->width;
    const int32_t height = image->height;
    const int32_t stride_bytes = (width * 3 + width % 4);
    const int32_t size_bytes = (stride_bytes * height);
    const RGB *pSrcLine = image->pixels + (width * (height - 1));
    BMPHeader header = MakeBMPHeader(width, height, size_bytes);
    uint8_t *line = (uint8_t *)Malloc(stride_bytes);
    for (int i = 0; i < stride_bytes; ++i) {
        line[i] = 0;
    }

    fwrite(&header, sizeof(BMPHeader), 1, fp);

    for (int y = 0; y < height; ++y) {
        const RGB *pSrc = pSrcLine;
        uint8_t *pDst = line;

        for (int x = 0; x < width; ++x) {
            *pDst++ = pSrc->b;
            *pDst++ = pSrc->g;
            *pDst++ = pSrc->r;
            ++pSrc;
        }

        fwrite(line, stride_bytes, 1, fp);
        pSrcLine -= width;
    }

    fclose(fp);
}

/// @brief 画像データを検証します。
/// @param image 検証する画像データ
void ValidateImage(const Image *image) {
    if ((image->width != IMAGE_WIDTH) || (image->height != IMAGE_HEIGHT)) {
        puts("[ERR ] invalid image size.");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < (IMAGE_WIDTH * IMAGE_HEIGHT); ++i) {
        const RGB pixel = image->pixels[i];
        if ((pixel.r < 0) || (pixel.r > 255) || (pixel.g < 0) || (pixel.g > 255)
            || (pixel.b < 0) || (pixel.b > 255)) {
            printf(
                "[ERR ] invalid pixel value (RGB){ %d, %d, %d }\n", pixel.r,
                pixel.g, pixel.b
            );
            exit(EXIT_FAILURE);
        }
    }
}

/// @brief 展開された画像の品質を計算します。
/// @param a オリジナル画像
/// @param b 展開された画像
/// @return 品質（0.0～1.0）
double CalculateQuality(const Image *a, const Image *b) {
    if ((a->width != IMAGE_WIDTH) || (a->height != IMAGE_HEIGHT)) {
        puts("invalid image size.");
        return -1.0;
    }

    if ((b->width != IMAGE_WIDTH) || (b->height != IMAGE_HEIGHT)) {
        puts("invalid image size.");
        return -1.0;
    }

    double error = 0.0;

    for (int y = 0; y < IMAGE_HEIGHT; ++y) {
        for (int x = 0; x < IMAGE_WIDTH; ++x) {
            const RGB pixelA = a->pixels[(y * IMAGE_WIDTH) + x];
            const RGB pixelB = b->pixels[(y * IMAGE_WIDTH) + x];
            const double dr = fabs((double)pixelA.r - pixelB.r);
            const double dg = fabs((double)pixelA.g - pixelB.g);
            const double db = fabs((double)pixelA.b - pixelB.b);
            error += (dr + dg + db);
        }
    }

    const double errorRate = (error / (IMAGE_WIDTH * IMAGE_HEIGHT * 3 * 255));
    return (1.0 - errorRate);
}
//
//	ここまで変更禁止
//
/////////////////////////
typedef struct {
    int r;
    int g;
    int b;
} RGB12;

void AddRGBtoSum(RGB *a, RGB *sum) {
    sum->r += a->r;
    sum->g += a->g;
    sum->b += a->b;
}
/// @brief 画像を圧縮します。
/// @param image 画像（480x320）
/// @param compressed 圧縮したデータを格納する配列（1024 バイト）
void CompressImage(const Image *image, unsigned char *compressed) {
    // 仮実装: 画像の内容を確認する
    /* for (int y = 0; y < IMAGE_HEIGHT; ++y) { */
    /*     for (int x = 0; x < IMAGE_WIDTH; ++x) { */
    /*         // const RGB pixel = image->pixels[(y * IMAGE_WIDTH) + x]; */
    /*     } */
    /* } */
    /**/
    /* // 仮実装: 適当なデータを格納する */
    /* for (int i = 0; i < BUFFER_SIZE; ++i) { */
    /*     compressed[i] = (i % 256); */
    /* } */
    RGB tilesum[30 * 20] = {0, 0, 0};
    // 16*16のタイルごとにRGBの平均をとる
    for (int y = 0; y < IMAGE_HEIGHT; y++) {
        for (int x = 0; x < IMAGE_WIDTH; x++) {
            RGB pixel = image->pixels[y * IMAGE_WIDTH + x];
            tilesum[(y / 16) * 30 + x / 16].r += pixel.r;
            tilesum[(y / 16) * 30 + x / 16].g += pixel.g;
            tilesum[(y / 16) * 30 + x / 16].b += pixel.b;
            /* AddRGBtoSum(pixel, &tilesum[]); */
        }
    }
    //
    for (int i = 0; i < 30 * 20; i++) {
        tilesum[i].r = tilesum[i].r / 256 / 16;
        tilesum[i].g = tilesum[i].g / 256 / 16;
        tilesum[i].b = tilesum[i].b / 256 / 16;
        /* printf("%d %d %d, ", tilesum[i].r, tilesum[i].g, tilesum[i].b); */
    }
    printf("\n");
    for (int i = 0; i < 300; i++) {
        compressed[i * 3] = tilesum[i * 2].r * 16 + tilesum[i * 2].g;
        compressed[i * 3 + 1] = tilesum[i * 2].b * 16 + tilesum[i * 2 + 1].r;
        compressed[i * 3 + 2]
            = tilesum[i * 2 + 1].g * 16 + tilesum[i * 2 + 1].b;
    }
    for (int i = 0; i < 900; i++) {
        /* compressed[i] = 12 * 16 + 2; */
    }
}

/// @brief 画像を展開します。
/// @param compressed 圧縮されたデータ（1024 バイト）
/// @param image 画像（480x320）
void DecompressImage(const unsigned char *compressed, Image *image) {
    RGB uncompressed[900];
    for (int i = 0; i < 300; i++) {
        uncompressed[i * 2].r = compressed[i * 3] / 16;
        uncompressed[i * 2].g = compressed[i * 3] % 16;
        uncompressed[i * 2].b = compressed[i * 3 + 1] / 16;
        uncompressed[i * 2 + 1].r = compressed[i * 3 + 1] % 16;
        uncompressed[i * 2 + 1].g = compressed[i * 3 + 2] / 16;
        uncompressed[i * 2 + 1].b = compressed[i * 3 + 2] % 16;
    }
    for (int y = 0; y < IMAGE_HEIGHT; ++y) {
        for (int x = 0; x < IMAGE_WIDTH; ++x) {
            RGB *pixel = &image->pixels[y * IMAGE_WIDTH + x];
            pixel->r = uncompressed[(y / 16) * 30 + x / 16].r * 16 + 8;
            pixel->g = uncompressed[(y / 16) * 30 + x / 16].g * 16 + 8;
            pixel->b = uncompressed[(y / 16) * 30 + x / 16].b * 16 + 8;
        }
    }
}

/////////////////////////
//
//	ここから変更禁止
//

int main(void) {
    puts("[INFO] 入力画像 `input.bmp` を読み込みます。");
    Image *input = LoadBMP("input.bmp");
    {
        if (input == NULL) {
            puts("[WARN] 入力画像 `input.bmp` "
                 "が見つからないため、ランダムな画像を生成します。");
            input = CreateRandomImage(IMAGE_WIDTH, IMAGE_HEIGHT);
            SaveBMP("input.bmp", input);
        }

        if ((input->width != IMAGE_WIDTH) || (input->height != IMAGE_HEIGHT)) {
            puts("[ERR ] 入力画像のサイズが 480x320 ではありません。");
            exit(EXIT_FAILURE);
        }
    }
    puts("[ OK ] 入力画像 `input.bmp` の読み込みが完了しました。");

    unsigned char compressed[BUFFER_SIZE + CANARY_BYTES_SIZE] = {0};
    Image *output = CreateImage(IMAGE_WIDTH, IMAGE_HEIGHT);
    for (int i = 0; i < (IMAGE_WIDTH * IMAGE_HEIGHT); ++i) {
        output->pixels[i] = (RGB){0, 0, 0};
    }

    // 画像を圧縮する
    puts("[INFO] 画像を圧縮します。");
    CompressImage(input, compressed);
    puts("[ OK ] 画像の圧縮が完了しました。");

    // 配列の範囲外に書き込んでいないかチェックする
    for (int i = BUFFER_SIZE; i < (BUFFER_SIZE + CANARY_BYTES_SIZE); ++i) {
        if (compressed[i] != 0) {
            puts("[ERROR] 配列範囲外へのアクセスが検出されました。");
            exit(EXIT_FAILURE);
        }
    }

    // 圧縮したデータを展開する
    puts("[INFO] 圧縮した画像を展開します。");
    DecompressImage(compressed, output);
    puts("[ OK ] 画像の展開が完了しました。");

    // 展開した画像を検証する
    puts("[INFO] 展開した画像を検証します。");
    ValidateImage(output);
    puts("[ OK ] 画像の検証が完了しました。");

    // 展開した画像を保存する
    puts("[INFO] 展開した画像を `output.bmp` に保存します。");
    SaveBMP("output.bmp", output);
    puts("[ OK ] 展開した画像を `output.bmp` に保存しました。");

    // 展開した画像の品質
    puts("[INFO] 展開した画像の品質を計算します。");
    const double quality = CalculateQuality(input, output);

    if (quality < 0.0) {
        puts("[ERR ] 不正な入力です。");
    } else {
        printf("[INFO] 品質（高いほど良い）: %.3f %%\n", (quality * 100));
    }

    DestroyImage(input);
    DestroyImage(output);
}
