#pragma once
#include "pix_color.h"
#include "pix_image.h"
#include "pix_sampling.h"
namespace pix
{
    inline array<double> gaussian (double radius)
    {
        array<double> kernel;

        using std::abs;
        using std::exp;
        using std::sqrt;

        const double pi = 3.14159265358979323846;

        radius = abs(radius);
        
        if (radius < 0.2) return kernel;

        int dim = 1 + 2 * int(radius + 0.5);

        kernel.resize(dim);

        double denom = 0;

        for (int x=0; x<dim; x++)
        {
            double r = x - dim/2.0 + 0.5;
            double g = 1.0 / sqrt(2*pi*radius)
                * exp(-r*r / (2*radius*radius));

            kernel[x] = g; denom += g;
        }
        for (int x=0; x<dim; x++) kernel[x] /= denom;

        return kernel;
    }

    template<class color> void convolution
    (
        frame<color> src,
        frame<color> dst,
        array<double> kernel,
        double k1, double k2,
        double threshold = 0.0
    )
    {
        if (src.size != dst.size)
            throw std::runtime_error
                ("pix::convolution: sizes mismatch");

        if (kernel.size() <= 0) { dst.copy_from(src); return; }

        using COLOR = aux::vector<color::color_channels>;
        image<COLOR> intermediate (src.size);
        const int d = kernel.size() / 2;

        for (int y=0; y<src.size.y; y++)
        for (int x=0; x<src.size.x; x++)
        {
            COLOR c;

            for (int xx=0; xx<kernel.size(); xx++) {
                int X = clamp(x + xx - d, 0, src.size.x-1);
                for (int i=0; i<color::color_channels; i++)
                    c(i) += float(kernel[xx] * src(X, y).channels[i]);
            }

            intermediate(x, y) = c;
        }

        for( int y=0; y<src.size.y; y++ )
        for( int x=0; x<src.size.x; x++ )
        {
            COLOR c0, c1;

            for (int i=0; i<color::color_channels; i++) 
                c0(i) = src(x, y).channels[i];

            for (int yy=0; yy<kernel.size(); yy++) {
                int Y = clamp(y + yy - d, 0, src.size.y-1);
                for (int i=0; i<color::color_channels; i++) 
                    c1(i) += float(kernel[yy] * intermediate(x, Y)(i));
            }

            if ((c1-c0).norm() < threshold) { dst(x, y) = src(x, y); continue; }

            for (int i=0; i<color::color_channels; i++) 
                dst(x, y).channels[i] = clamp<uint8_t>(
                    k1 * c0(i) + k2 * c1(i));
        }
    }

    template<class color> void blur (
        frame<color> src, frame<color> dst,
        double radius = 2.0, double k = 1.0)
    {
        int mul = int(radius) / 5;
        if (mul <= 2)
        {
            convolution(src, dst, gaussian(radius), 0, k);
        }
        else
        {
            image<color> img = resized(src, src.size/mul);

            convolution(img.crop(), img.crop(), gaussian(radius/mul), 0, k);

            dst.copy_from(resized(img.crop(), dst.size).crop());
        }
    }

    template<class color> void sharpen (
        frame<color> src, frame<color> dst,
        double radius = 2.0, double k = 1.0,
        double threshold = 5.0/255.0)
    {
        convolution(src, dst, gaussian(radius), 1+k, -k, threshold);
    }

    template<class color> void glow (
        frame<color> src, frame<color> dst,
        double radius = 2.0, double k = 1.0)
    {
        convolution(src, dst, gaussian(radius), 1/(1+k), k/(1+k));
    }

    template<class color> void defocus (
        frame<color> src, frame<color> dst,
        double radius = 2.0, double k = 1.0)
    {
        convolution(src, dst, gaussian(radius), -k, 1+k);
    }

    template<class c> void blur    (image<c> & i, double r = 2.0) { blur   (i.crop(), i.crop(), r); }
    template<class c> void sharpen (image<c> & i, double r = 2.0) { sharpen(i.crop(), i.crop(), r); }
    template<class c> void glow    (image<c> & i, double r = 2.0) { glow   (i.crop(), i.crop(), r); }
    template<class c> void defocus (image<c> & i, double r = 2.0) { defocus(i.crop(), i.crop(), r); }
}

