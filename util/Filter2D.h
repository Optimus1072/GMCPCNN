//
// Created by wrede on 11.07.16.
//

#ifndef GBMOT_FILTER_H
#define GBMOT_FILTER_H


#include <vector>
#include <string>

namespace util
{
    class Filter2D
    {
    public:
        Filter2D();

        Filter2D(double multiplier, std::vector<double> mask);

        // Example:
        // mask_string = multiplier, m00, m01, m02, ..., m10, m11, m12, ..., mnn
        // delimiter = ,
        Filter2D(std::string const & mask_string, char delimiter);

        double Get(int x, int y) const;

        double Get(int i) const;

        double GetMultiplier() const;

        int GetVicinity() const;

        int GetDimension() const;

        void Normalize();

    private:
        int Index(int x, int y) const;

        int Vicinity(int size) const;

        int Dimension(int size) const;

        std::vector<double> mask_;
        double multiplier_;
        int dimension_;
        int vicinity_;
    };
}


#endif //GBMOT_FILTER_H
