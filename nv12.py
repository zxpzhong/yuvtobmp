import cv2
import numpy as np
import argparse

def yuv2bgr(args, height, width, startfrm):
    """
    :param filename: the input YUV video name | 待处理 YUV 视频的名字
    :param height: the height of the YUV video | YUV 视频中图像的高
    :param width: the width of the YUV video | YUV 视频中图像的宽
    :param startfrm: start frame | 起始帧
    :return: None
    """
    fp = open(args.input, 'rb')

    framesize = height * width * 3 // 2  # the all pixel number of one frame | 一帧图像所含的像素个数
    h_h = height // 2
    h_w = width // 2

    fp.seek(0, 2)  # set the file point to the end of the file stream | 设置文件指针到文件流的尾部
    ps = fp.tell()  # current location of the file point | 当前文件指针位置
    numfrm = ps // framesize  # calculate the output frames | 计算输出帧数
    fp.seek(framesize * startfrm, 0)
    print('numfrm - startfrm is ',numfrm - startfrm)
    for i in range(numfrm - startfrm):
        print('i = ',i)
        Yt = np.zeros(shape=(height, width), dtype='uint8', order='C')
        Ut = np.zeros(shape=(h_h, h_w), dtype='uint8', order='C')
        Vt = np.zeros(shape=(h_h, h_w), dtype='uint8', order='C')

        for m in range(height):
            for n in range(width):
                Yt[m, n] = ord(fp.read(1))
        for m in range(h_h):
            for n in range(h_w):
                Ut[m, n] = ord(fp.read(1))
        for m in range(h_h):
            for n in range(h_w):
                Vt[m, n] = ord(fp.read(1))

        img = np.concatenate((Yt.reshape(-1), Ut.reshape(-1), Vt.reshape(-1)))
        img = img.reshape((height * 3 // 2, width)).astype('uint8')  # YUV sotrage format:NV12(YYYY UV) | YUV 的存储格式为：NV12（YYYY UV）

        # transform the format because opencv can not read the YUV data directly | 由于 opencv 不能直接读取 YUV 格式的文件, 所以要转换一下格式
        bgr_img = cv2.cvtColor(img, cv2.COLOR_YUV2BGR_NV12)  # be catious about the YUV storage format | 注意 YUV 的存储格式
        print(bgr_img.shape)
        cv2.imwrite(args.output, bgr_img)
        print("Extract frame %d " % (i + 1))

    fp.close()
    print("job done!")
    return None


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--input', dest='input', type=str, default=None,
                        help='Input the location of YUV file(*.yuv or *.nv12) that need to be handled')
    parser.add_argument('--output', dest='output', type=str, default=None,
                        help='Input the location of file name of the transformed img, the output format jpg | jpeg | bmp | png is all available')
    parser.add_argument('--height', dest='height', type=int, default=1080,
                        help='The img height, default 1080')
    parser.add_argument('--width', dest='width', type=int, default=1920,
                        help='The img height, default 1920')
    args = parser.parse_args()
    if args.input == None or args.output == None:
        print('input arg is wrong')
        exit()
    _ = yuv2bgr(args, args.height, args.width, startfrm=0)
