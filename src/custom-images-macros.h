/*
 * custom-images-macros.h
 *
 *  Created on: 2015-05-21
 *      Author: Moustafa S. Ibrahim
 */

#ifndef CUSTOM_IMAGES_MACROS_H_
#define CUSTOM_IMAGES_MACROS_H_

namespace MostCV {

#define REPIMG2(y, x, img)       for(int y=0;y<(int)(img.rows);++y) for(int x=0;x<(int)(img.cols);++x)
#define REPIMG3(y, x, c, img)    for(int y=0;y<(int)(img.rows);++y) for(int x=0;x<(int)(img.cols);++x) for(int c=0;c<(int)(img.channels());++c)
#define REPIMG_JUMP(y, x, dy, dx, img)       for(int y=0;y<(int)(img.rows);y+=dy) for(int x=0;x<(int)(img.cols);x+=dx)
}

#endif /* CUSTOM_IMAGES_MACROS_H_ */
