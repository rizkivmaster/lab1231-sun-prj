
#ifndef SSVM_SS_IMAGE_CONSTRAINT_H
#define SSVM_SS_IMAGE_CONSTRAINT_H

#include <shotton/shotton.h>
#include <util/util.h>
#include <QVector>
#include <QString>
#include <QRgb>
#include <QDataStream>
#include <QFile>
#include <QString>
#include <QImage>
#include <QtGlobal>
#include <opengm/graphicalmodel/graphicalmodel.hxx>
#include "../textonboost/src/settings.h"

namespace ssvm_ss
{
namespace image_constraint
{
const size_t height = IMAGE_MAX_LENGTH;
const size_t width = IMAGE_MAX_LENGTH;
const size_t n_pixel = width *height;
const size_t n_label = 21;
const size_t neighbour_size = 2;
const size_t unary_size = n_pixel;
const size_t pair_size = (width - 1) *(height - 1);

const size_t feature_size = unary_size + neighbour_size*pair_size ;
const string inferenceMethod = "AlphaExpansion";//: "AlphaExpansion", "ICM"

}


}

#endif