// 	タイトル: 画像の圧縮
//
//	実装の方針:
// 実行時間にかなり余裕があったので複数の方法で圧縮し比較するようにした
//
//	工夫した点: できるだけ多くの設定を保存できるように範囲を分割した
//
//
//	参考にした資料: なし
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

// 色の変化のカーブ

// Arctan(x-8)
double curve0[] = {0,    0.01, 0.02, 0.03, 0.05, 0.09, 0.15, 0.26,
                   0.50, 0.74, 0.85, 0.91, 0.95, 0.97, 0.98, 0.99};
// x
double curve1[]
    = {0,         1.0 / 16,  2.0 / 16,  3.0 / 16, 4.0 / 16,  5.0 / 16,
       6.0 / 16,  7.0 / 16,  8.0 / 16,  9.0 / 16, 10.0 / 16, 11.0 / 16,
       12.0 / 16, 13.0 / 16, 14.0 / 16, 15.0 / 16};
// x+sinx
double curve2[] = {0,    -0.13, 0.37, 0.05, 0.18, 0.54, 0.15, 0.51,
                   0.64, 0.31,  0.81, 0.69, 0.56, 1.06, 0.74, 0.86};
// sin(x-8)
double curve3[] = {0,    0.01, 0.04, 0.08, 0.15, 0.22, 0.31, 0.40,
                   0.50, 0.60, 0.69, 0.78, 0.85, 0.92, 0.96, 0.99};
double *curves[] = {curve0, curve1, curve2, curve3};

// 16*16のタイルの描画
void FillTile(
    const Image *new, RGB *uncompressed, int n, int m, int curven, int dir
) {
    double *curve = curves[curven];
    RGB *pixels = new->pixels;
    if (dir == 1) {
        for (int y = 0; y < 16; y++) {
            for (int x = 0; x < y; x++) {
                RGB *pixel = &pixels[(n * 16 + y) * IMAGE_WIDTH + (m * 16 + x)];
                pixel->r
                    = (int)((uncompressed[n * 31 + m].r * 16 + 8)
                                * (1 - curve[y])
                            + curve[x]
                                  * (uncompressed[(n + 1) * 31 + m + 1].r * 16
                                     + 8)
                            + (-curve[x] + curve[y]
                              ) * (uncompressed[(n + 1) * 31 + m].r * 16 + 8));
                pixel->g
                    = ((uncompressed[n * 31 + m].g * 16 + 8) * (1 - curve[y])
                       + curve[x]
                             * (uncompressed[(n + 1) * 31 + m + 1].g * 16 + 8)
                       + (-curve[x] + curve[y])
                             * (uncompressed[(n + 1) * 31 + m].g * 16 + 8));
                pixel->b
                    = ((uncompressed[n * 31 + m].b * 16 + 8) * (1 - curve[y])
                       + curve[x]
                             * (uncompressed[(n + 1) * 31 + m + 1].b * 16 + 8)
                       + (-curve[x] + curve[y])
                             * (uncompressed[(n + 1) * 31 + m].b * 16 + 8));
            }
            for (int x = y; x < 16; x++) {
                RGB *pixel = &pixels[(n * 16 + y) * IMAGE_WIDTH + (m * 16 + x)];
                pixel->r
                    = (int)((uncompressed[n * 31 + m].r * 16 + 8)
                                * (1 - curve[x])
                            + curve[y]
                                  * (uncompressed[(n + 1) * 31 + m + 1].r * 16
                                     + 8)
                            + (curve[x] - curve[y])
                                  * (uncompressed[n * 31 + m + 1].r * 16 + 8));
                pixel->g
                    = (int)((uncompressed[n * 31 + m].g * 16 + 8)
                                * (1 - curve[x])
                            + curve[y]
                                  * (uncompressed[(n + 1) * 31 + m + 1].g * 16
                                     + 8)
                            + (curve[x] - curve[y])
                                  * (uncompressed[n * 31 + m + 1].g * 16 + 8));
                pixel->b
                    = (int)((uncompressed[n * 31 + m].b * 16 + 8)
                                * (1 - curve[x])
                            + curve[y]
                                  * (uncompressed[(n + 1) * 31 + m + 1].b * 16
                                     + 8)
                            + (curve[x] - curve[y])
                                  * (uncompressed[n * 31 + m + 1].b * 16 + 8));
            }
        }
    } else {
        for (int y = 0; y < 16; y++) {
            for (int x = 0; x < 16 - y; x++) {
                RGB *pixel = &pixels[(n * 16 + y) * IMAGE_WIDTH + (m * 16 + x)];
                pixel->r
                    = (int)((uncompressed[n * 31 + m].r * 16 + 8)
                                * (1 - curve[x] - curve[y])
                            + curve[y]
                                  * (uncompressed[(n + 1) * 31 + m].r * 16 + 8)
                            + curve[x]
                                  * (uncompressed[(n * 31 + m + 1)].r * 16 + 8)
                    );
                pixel->g
                    = (int)((uncompressed[n * 31 + m].g * 16 + 8)
                                * (1 - curve[x] - curve[y])
                            + curve[y]
                                  * (uncompressed[(n + 1) * 31 + m].g * 16 + 8)
                            + curve[x]
                                  * (uncompressed[(n * 31 + m + 1)].g * 16 + 8)
                    );
                pixel->b
                    = (int)((uncompressed[n * 31 + m].b * 16 + 8)
                                * (1 - curve[x] - curve[y])
                            + curve[y]
                                  * (uncompressed[(n + 1) * 31 + m].b * 16 + 8)
                            + curve[x]
                                  * (uncompressed[(n * 31 + m + 1)].b * 16 + 8)
                    );
            }
            for (int x = 16 - y; x < 16; x++) {
                RGB *pixel = &pixels[(n * 16 + y) * IMAGE_WIDTH + (m * 16 + x)];
                pixel->r
                    = (int)((uncompressed[(n + 1) * 31 + m + 1].r * 16 + 8)
                                * (-1 + curve[x] + curve[y])
                            + (1 - curve[x])
                                  * (uncompressed[(n + 1) * 31 + m].r * 16 + 8)
                            + (1 - curve[y]
                              ) * (uncompressed[(n * 31 + m + 1)].r * 16 + 8));
                pixel->g
                    = (int)((uncompressed[(n + 1) * 31 + m + 1].g * 16 + 8)
                                * (-1 + curve[x] + curve[y])
                            + (1 - curve[x])
                                  * (uncompressed[(n + 1) * 31 + m].g * 16 + 8)
                            + (1 - curve[y]
                              ) * (uncompressed[(n * 31 + m + 1)].g * 16 + 8));
                pixel->b
                    = (int)((uncompressed[(n + 1) * 31 + m + 1].b * 16 + 8)
                                * (-1 + curve[x] + curve[y])
                            + (1 - curve[x])
                                  * (uncompressed[(n + 1) * 31 + m].b * 16 + 8)
                            + (1 - curve[y]
                              ) * (uncompressed[(n * 31 + m + 1)].b * 16 + 8));
            }
        }
    }
}

// タイルの評価 小さいほど誤差が少ない
int TestTile(const Image *image, const Image *new, int n, int m) {
    int score = 0;
    for (int y = n * 16; y < (n + 1) * 16; y++) {
        for (int x = m * 16; x < (m + 1) * 16; x++) {
            RGB *ref = &image->pixels[y * IMAGE_WIDTH + x];
            RGB *pixel = &new->pixels[y * IMAGE_WIDTH + x];
            score += abs(ref->r - pixel->r);
            score += abs(ref->g - pixel->g);
            score += abs(ref->b - pixel->b);
        }
    }
    return score;
}

typedef struct {
    int Score;
    int color;
    int c[20];
    unsigned char dir[300];
} TestResult;

RGB ATileAverage[31 * 21] = {{0, 0, 0}};
RGB BTileAverage[31 * 21] = {{0, 0, 0}};
RGB *Colors[] = {ATileAverage, BTileAverage};

// 色が決まっているときの最適解
TestResult TestSetting(const Image *image, int color) {

    // テスト用に画像を作る
    Image *test = CreateImage(IMAGE_WIDTH, IMAGE_HEIGHT);

    TestResult result;
    result.color = color;
    result.Score = 0;

    // 96*80=(16*16)*6*5ごとにカーブを設定する
    for (int a = 0; a < 4; a++) {
        for (int b = 0; b < 5; b++) {
            int cbestscore = -1;
            int cbest = 0;
            for (int c = 0; c < 4; c++) {
                int cscore = 0;
                for (int n = a * 5; n < (a + 1) * 5; n++) {
                    for (int m = b * 3; m < (b + 1) * 3; m++) {
                        FillTile(test, Colors[color], n, m * 2, c, 0);
                        FillTile(test, Colors[color], n, m * 2 + 1, c, 0);
                        int score0 = TestTile(image, test, n, m * 2)
                                   + TestTile(image, test, n, m * 2 + 1);
                        FillTile(test, Colors[color], n, m * 2, c, 1);
                        FillTile(test, Colors[color], n, m * 2 + 1, c, 1);
                        int score1 = TestTile(image, test, n, m * 2)
                                   + TestTile(image, test, n, m * 2 + 1);
                        if (score0 < score1) {
                            FillTile(test, Colors[color], n, m * 2, c, 0);
                            FillTile(test, Colors[color], n, m * 2 + 1, c, 0);
                            cscore += score0;
                            result.dir[n * 15 + m] = 0;
                        } else {
                            cscore += score1;
                            result.dir[n * 15 + m] = 1;
                        }
                    }
                }
                if (cbestscore == -1 || cscore < cbestscore) {
                    cbestscore = cscore;
                    cbest = c;
                }
            }
            result.Score += cbestscore;
            result.c[a * 5 + b] = cbest;
        }
    }
    DestroyImage(test);
    return result;
}
/// @brief 画像を圧縮します。
/// @param image 画像（480x320）
/// @param compressed 圧縮したデータを格納する配列（1024 バイト）
void CompressImage(const Image *image, unsigned char *compressed) {
    // 2種類の平均値の配列を作る
    for (int y = 0; y < IMAGE_HEIGHT; y++) {
        for (int x = 0; x < IMAGE_WIDTH; x++) {
            RGB pixel = image->pixels[y * IMAGE_WIDTH + x];
            int i = (y + 8) / 16 * 31 + (x + 8) / 16;
            ATileAverage[i].r += pixel.r;
            ATileAverage[i].g += pixel.g;
            ATileAverage[i].b += pixel.b;

            if (((x + 4) / 8) % 2 == 0 && ((y + 4) / 8) % 2 == 0) {
                BTileAverage[i].r += pixel.r;
                BTileAverage[i].g += pixel.g;
                BTileAverage[i].b += pixel.b;
            }
        }
    }
    for (int i = 0; i < 31 * 21; i++) {
        int k = 256;
        if (i % 31 == 0 || i % 31 == 30) {
            k = k / 2;
        }
        if (i / 31 == 0 || i / 31 == 20) {
            k = k / 2;
        }
        ATileAverage[i].r = ATileAverage[i].r / 16 / k;
        ATileAverage[i].g = ATileAverage[i].g / 16 / k;
        ATileAverage[i].b = ATileAverage[i].b / 16 / k;

        BTileAverage[i].r = BTileAverage[i].r / 16 / (k / 4);
        BTileAverage[i].g = BTileAverage[i].g / 16 / (k / 4);
        BTileAverage[i].b = BTileAverage[i].b / 16 / (k / 4);
    }

    // 設定を変えながら比較
    TestResult result[2];
    int bestresult = 0;
    for (int color = 0; color < 2; color++) {
        result[color] = TestSetting(image, color);
        if (result[color].Score < result[bestresult].Score) {
            bestresult = color;
        }
    }

    // 圧縮
    // RGB*31*21, 方向*300, カーブ*20の順に配置
    compressed[0] = bestresult;
    RGB *color = Colors[result[bestresult].color];
    for (int i = 0; i < 325; i++) {
        compressed[i * 3 + 1] = color[i * 2].r * 16 + color[i * 2].g;
        compressed[i * 3 + 2] = color[i * 2].b * 16 + color[i * 2 + 1].r;
        compressed[i * 3 + 3] = color[i * 2 + 1].g * 16 + color[i * 2 + 1].b;
    }
    compressed[976] = color[650].r * 16 + color[650].g;
    compressed[977] = color[650].b * 16;

    unsigned char *dir = result[bestresult].dir;
    compressed[977] += dir[0] * 8 + dir[1] * 4 + dir[2] * 2 + dir[3];
    for (int i = 0; i < 37; i++) {
        compressed[i + 978] = dir[i * 8 + 4] * 128 + dir[i * 8 + 5] * 64
                            + dir[i * 8 + 6] * 32 + dir[i * 8 + 7] * 16
                            + dir[i * 8 + 8] * 8 + dir[i * 8 + 9] * 4
                            + dir[i * 8 + 10] * 2 + dir[i * 8 + 11];
    }

    int *c = result[bestresult].c;
    for (int i = 0; i < 5; i++) {
        compressed[i + 1015] = c[i * 4 + 0] * 64 + c[i * 4 + 1] * 16
                             + c[i * 4 + 2] * 4 + c[i * 4 + 3];
    }
}

/// @brief 画像を展開します。
/// @param compressed 圧縮されたデータ（1024 バイト）
/// @param image 画像（480x320）
void DecompressImage(const unsigned char *compressed, Image *image) {
    // 展開
    RGB color[31 * 21];
    for (int i = 0; i < 325; i++) {
        color[i * 2].r = compressed[i * 3 + 1] / 16;
        color[i * 2].g = compressed[i * 3 + 1] % 16;
        color[i * 2].b = compressed[i * 3 + 2] / 16;
        color[i * 2 + 1].r = compressed[i * 3 + 2] % 16;
        color[i * 2 + 1].g = compressed[i * 3 + 3] / 16;
        color[i * 2 + 1].b = compressed[i * 3 + 3] % 16;
    }
    color[650].r = compressed[976] / 16;
    color[650].g = compressed[976] % 16;
    color[650].b = compressed[977] / 16;

    unsigned char dir[300];
    dir[0] = compressed[977] % 16 / 8;
    dir[1] = compressed[977] % 8 / 4;
    dir[2] = compressed[977] % 4 / 2;
    dir[3] = compressed[977] % 2;
    for (int i = 0; i < 37; i++) {
        dir[i * 8 + 4] = compressed[i + 978] / 128;
        dir[i * 8 + 5] = compressed[i + 978] % 128 / 64;
        dir[i * 8 + 6] = compressed[i + 978] % 64 / 32;
        dir[i * 8 + 7] = compressed[i + 978] % 32 / 16;
        dir[i * 8 + 8] = compressed[i + 978] % 16 / 8;
        dir[i * 8 + 9] = compressed[i + 978] % 8 / 4;
        dir[i * 8 + 10] = compressed[i + 978] % 4 / 2;
        dir[i * 8 + 11] = compressed[i + 978] % 2;
    }
    unsigned char c[20];
    for (int i = 0; i < 5; i++) {
        c[i * 4] = compressed[1015 + i] / 64;
        c[i * 4 + 1] = compressed[1015 + i] % 64 / 16;
        c[i * 4 + 2] = compressed[1015 + i] % 16 / 4;
        c[i * 4 + 3] = compressed[1015 + i] % 4;
    }

    // 描画
    for (int n = 0; n < 20; n++) {
        for (int m = 0; m < 30; m++) {
            FillTile(
                image, color, n, m, c[n / 5 * 5 + m / 6], dir[n * 15 + m / 2]
            );
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
